#include "../vsf_espidf_cfg.h"

#if VSF_USE_ESPIDF == ENABLED && VSF_ESPIDF_CFG_USE_CONSOLE == ENABLED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/param.h>
#include <fcntl.h>
#include "esp_console.h"
#include "esp_log.h"
#include "esp_system.h"
#include "linenoise.h"
#include "argtable3.h"
#include "esp_vfs_eventfd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#if VSF_USE_LINUX == ENABLED
#   include "shell/sys/linux/vsf_linux.h"
#endif

/* ============================ LOCAL TYPES ================================= */

#define ANSI_COLOR_DEFAULT      39

#define CONSOLE_PROMPT_MAX_LEN  32
#define CONSOLE_PATH_MAX_LEN    128

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

typedef enum {
    CONSOLE_REPL_STATE_DEINIT,
    CONSOLE_REPL_STATE_INIT,
    CONSOLE_REPL_STATE_START,
} repl_state_t;

typedef struct {
    esp_console_repl_t repl_core;
    char prompt[CONSOLE_PROMPT_MAX_LEN];
    repl_state_t state;
    void *state_mux;
    const char *history_save_path;
    void *task_hdl;
    size_t max_cmdline_length;
} esp_console_repl_com_t;

typedef struct {
    esp_console_repl_com_t repl_com;
    void *dev_config;
} esp_console_repl_universal_t;

/* Forward declarations for functions defined later */
esp_err_t esp_console_internal_set_event_fd(esp_console_repl_com_t *repl_com);

typedef struct cmd_item_ {
    const char *command;
    const char *help;
    char *hint;
    esp_console_cmd_func_t func;
    esp_console_cmd_func_with_context_t func_w_context;
    void *argtable;
    void *context;
    struct cmd_item_ *next;
} cmd_item_t;

/* ============================ STATIC STATE ================================ */

static cmd_item_t *s_cmd_list;
static esp_console_config_t s_config = {
    .heap_alloc_caps = MALLOC_CAP_DEFAULT
};
static char *s_tmp_line_buf;
static esp_console_help_verbose_level_e s_verbose_level = ESP_CONSOLE_HELP_VERBOSE_LEVEL_1;

static struct {
    struct arg_str *help_cmd;
    struct arg_int *verbose_level;
    struct arg_end *end;
} help_args;

static const char *TAG = "console";
static const char *TAG_REPL = "console.repl";

/* ============================ split_argv ================================== */

#define SS_FLAG_ESCAPE 0x8

typedef enum {
    SS_SPACE = 0x0,
    SS_ARG = 0x1,
    SS_QUOTED_ARG = 0x2,
    SS_ARG_ESCAPED = SS_ARG | SS_FLAG_ESCAPE,
    SS_QUOTED_ARG_ESCAPED = SS_QUOTED_ARG | SS_FLAG_ESCAPE,
} split_state_t;

#define END_ARG() do { \
    char_out = 0; \
    argv[argc++] = next_arg_start; \
    state = SS_SPACE; \
} while(0)

size_t esp_console_split_argv(char *line, char **argv, size_t argv_size)
{
    const int QUOTE = '"';
    const int ESCAPE = '\\';
    const int SPACE = ' ';
    split_state_t state = SS_SPACE;
    size_t argc = 0;
    char *next_arg_start = line;
    char *out_ptr = line;
    for (char *in_ptr = line; argc < argv_size - 1; ++in_ptr) {
        int char_in = (unsigned char) * in_ptr;
        if (char_in == 0) {
            break;
        }
        int char_out = -1;

        switch (state) {
        case SS_SPACE:
            if (char_in == SPACE) {
            } else if (char_in == QUOTE) {
                next_arg_start = out_ptr;
                state = SS_QUOTED_ARG;
            } else if (char_in == ESCAPE) {
                next_arg_start = out_ptr;
                state = SS_ARG_ESCAPED;
            } else {
                next_arg_start = out_ptr;
                state = SS_ARG;
                char_out = char_in;
            }
            break;

        case SS_QUOTED_ARG:
            if (char_in == QUOTE) {
                END_ARG();
            } else if (char_in == ESCAPE) {
                state = SS_QUOTED_ARG_ESCAPED;
            } else {
                char_out = char_in;
            }
            break;

        case SS_ARG_ESCAPED:
        case SS_QUOTED_ARG_ESCAPED:
            if (char_in == ESCAPE || char_in == QUOTE || char_in == SPACE) {
                char_out = char_in;
            }
            state = (split_state_t)(state & (~SS_FLAG_ESCAPE));
            break;

        case SS_ARG:
            if (char_in == SPACE) {
                END_ARG();
            } else if (char_in == ESCAPE) {
                state = SS_ARG_ESCAPED;
            } else {
                char_out = char_in;
            }
            break;
        }
        if (char_out >= 0) {
            *out_ptr = char_out;
            ++out_ptr;
        }
    }
    *out_ptr = 0;
    if (state != SS_SPACE && argc < argv_size - 1) {
        argv[argc++] = next_arg_start;
    }
    argv[argc] = NULL;

    return argc;
}

/* ============================ COMMAND MGMT ================================ */

static const cmd_item_t *find_command_by_name(const char *name)
{
    size_t len = strlen(name);
    for (cmd_item_t *it = s_cmd_list; it != NULL; it = it->next) {
        if (strlen(it->command) == len && strcmp(name, it->command) == 0) {
            return it;
        }
    }
    return NULL;
}

esp_err_t esp_console_init(const esp_console_config_t *config)
{
    if (!config) {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_tmp_line_buf) {
        return ESP_ERR_INVALID_STATE;
    }
    memcpy(&s_config, config, sizeof(s_config));
    if (s_config.hint_color == 0) {
        s_config.hint_color = ANSI_COLOR_DEFAULT;
    }
    if (s_config.heap_alloc_caps == 0) {
        s_config.heap_alloc_caps = MALLOC_CAP_DEFAULT;
    }
    s_tmp_line_buf = heap_caps_calloc(1, config->max_cmdline_length, s_config.heap_alloc_caps);
    if (s_tmp_line_buf == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t esp_console_deinit(void)
{
    if (!s_tmp_line_buf) {
        return ESP_ERR_INVALID_STATE;
    }
    heap_caps_free(s_tmp_line_buf);
    s_tmp_line_buf = NULL;
    cmd_item_t *it = s_cmd_list;
    while (it != NULL) {
        cmd_item_t *next = it->next;
        free(it->hint);
        heap_caps_free(it);
        it = next;
    }
    s_cmd_list = NULL;
    return ESP_OK;
}

static void cmd_list_remove(cmd_item_t *item)
{
    if (s_cmd_list == item) {
        s_cmd_list = item->next;
        item->next = NULL;
        return;
    }
    for (cmd_item_t *prev = s_cmd_list; prev != NULL; prev = prev->next) {
        if (prev->next == item) {
            prev->next = item->next;
            item->next = NULL;
            return;
        }
    }
}

esp_err_t esp_console_cmd_deregister(const char *cmd_name)
{
    cmd_item_t *item = (cmd_item_t *)find_command_by_name(cmd_name);
    if (item == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    cmd_list_remove(item);
    free(item->hint);
    heap_caps_free(item);
    return ESP_OK;
}

#if VSF_USE_LINUX == ENABLED
static int esp_console_cmd_stub(int argc, char *argv[])
{
    if (argc == 0 || argv == NULL || argv[0] == NULL) {
        return 1;
    }
    const cmd_item_t *cmd = find_command_by_name(argv[0]);
    if (cmd == NULL) {
        printf("%s: command not found\n", argv[0]);
        return 1;
    }
    if (cmd->func) {
        return cmd->func(argc, argv);
    }
    if (cmd->func_w_context) {
        return cmd->func_w_context(cmd->context, argc, argv);
    }
    return 1;
}
#endif

esp_err_t esp_console_cmd_register(const esp_console_cmd_t *cmd)
{
    cmd_item_t *item = NULL;
    if (!cmd || cmd->command == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if (strchr(cmd->command, ' ') != NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    if ((cmd->func == NULL && cmd->func_w_context == NULL)
            || (cmd->func != NULL && cmd->func_w_context != NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    item = (cmd_item_t *)find_command_by_name(cmd->command);
    if (!item) {
        item = heap_caps_calloc(1, sizeof(*item), s_config.heap_alloc_caps);
        if (item == NULL) {
            return ESP_ERR_NO_MEM;
        }
        item->next = NULL;
    } else {
        cmd_list_remove(item);
        free(item->hint);
    }
    item->command = cmd->command;
    item->help = cmd->help;
    if (cmd->hint) {
        asprintf(&item->hint, " %s", cmd->hint);
    } else if (cmd->argtable) {
        arg_dstr_t ds = arg_dstr_create();
        arg_print_syntax_ds(ds, cmd->argtable, NULL);
        item->hint = strdup(arg_dstr_cstr(ds));
        arg_dstr_destroy(ds);
    }
    item->argtable = cmd->argtable;

    if (cmd->func) {
        item->func = cmd->func;
    } else {
        item->func_w_context = cmd->func_w_context;
        item->context = cmd->context;
    }

    cmd_item_t *last = NULL;
    for (cmd_item_t *it = s_cmd_list; it != NULL; it = it->next) {
        if (strcmp(it->command, item->command) > 0) {
            break;
        }
        last = it;
    }
    if (last == NULL) {
        item->next = s_cmd_list;
        s_cmd_list = item;
    } else {
        item->next = last->next;
        last->next = item;
    }

#if VSF_USE_LINUX == ENABLED
    {
        size_t path_len = strlen(VSF_LINUX_CFG_BIN_PATH "/") + strlen(cmd->command) + 1;
        char path[path_len];
        snprintf(path, path_len, VSF_LINUX_CFG_BIN_PATH "/%s", cmd->command);
        vsf_linux_fs_bind_executable(path, esp_console_cmd_stub);
    }
#endif

    return ESP_OK;
}

void esp_console_get_completion(const char *buf, linenoiseCompletions *lc)
{
    size_t len = strlen(buf);
    if (len == 0) {
        return;
    }
    for (cmd_item_t *it = s_cmd_list; it != NULL; it = it->next) {
        if (strncmp(buf, it->command, len) == 0) {
            linenoiseAddCompletion(lc, it->command);
        }
    }
}

const char *esp_console_get_hint(const char *buf, int *color, int *bold)
{
    size_t len = strlen(buf);
    for (cmd_item_t *it = s_cmd_list; it != NULL; it = it->next) {
        if (strlen(it->command) == len &&
                strncmp(buf, it->command, len) == 0) {
            *color = s_config.hint_color;
            *bold = s_config.hint_bold;
            return it->hint;
        }
    }
    return NULL;
}

esp_err_t esp_console_run(const char *cmdline, int *cmd_ret)
{
    if (s_tmp_line_buf == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    char **argv = (char **) heap_caps_calloc(s_config.max_cmdline_args, sizeof(char *), s_config.heap_alloc_caps);
    if (argv == NULL) {
        return ESP_ERR_NO_MEM;
    }
    strlcpy(s_tmp_line_buf, cmdline, s_config.max_cmdline_length);

    size_t argc = esp_console_split_argv(s_tmp_line_buf, argv, s_config.max_cmdline_args);
    if (argc == 0) {
        free(argv);
        return ESP_ERR_INVALID_ARG;
    }
    const cmd_item_t *cmd = find_command_by_name(argv[0]);
    if (cmd == NULL) {
        free(argv);
        return ESP_ERR_NOT_FOUND;
    }
    if (cmd->func) {
        *cmd_ret = (*cmd->func)(argc, argv);
    }
    if (cmd->func_w_context) {
        *cmd_ret = (*cmd->func_w_context)(cmd->context, argc, argv);
    }
    free(argv);
    return ESP_OK;
}

/* ============================ HELP COMMAND ================================ */

typedef void (*const fn_print_arg_t)(cmd_item_t*);

static void print_arg_help(cmd_item_t *it)
{
    const char *hint = (it->hint) ? it->hint : "";
    printf("%-s %s\n", it->command, hint);
    printf("  ");
    arg_print_formatted(stdout, 2, 78, it->help);
    if (it->argtable) {
        arg_print_glossary(stdout, (void **) it->argtable, "  %12s  %s\n");
    }
    printf("\n");
}

static void print_arg_command(cmd_item_t *it)
{
    const char *hint = (it->hint) ? it->hint : "";
    printf("%-s %s\n\n", it->command, hint);
}

static fn_print_arg_t print_verbose_level_arr[ESP_CONSOLE_HELP_VERBOSE_LEVEL_MAX_NUM] = {
    print_arg_command,
    print_arg_help,
};

static int help_command(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **) &help_args);

    if (nerrors != 0) {
        arg_print_errors(stderr, help_args.end, argv[0]);
        return 1;
    }

    int ret_value = 1;

    if (help_args.help_cmd->count == 0) {
        esp_console_help_verbose_level_e verbose_level;
        if (help_args.verbose_level->count == 0) {
            verbose_level = s_verbose_level;
        } else {
            verbose_level = (esp_console_help_verbose_level_e)help_args.verbose_level->ival[0];
        }
        if (verbose_level >= ESP_CONSOLE_HELP_VERBOSE_LEVEL_MAX_NUM) {
            printf("help: invalid verbose level %d", (int)verbose_level);
            return 1;
        }
        for (cmd_item_t *it = s_cmd_list; it != NULL; it = it->next) {
            if (it->help == NULL) {
                continue;
            }
            print_verbose_level_arr[verbose_level](it);
        }
        ret_value = 0;
    } else {
        bool found_command = false;
        for (cmd_item_t *it = s_cmd_list; it != NULL; it = it->next) {
            if (it->help == NULL) {
                continue;
            }
            if (strcmp(help_args.help_cmd->sval[0], it->command) == 0) {
                print_arg_help(it);
                found_command = true;
                ret_value = 0;
                break;
            }
        }
        if (!found_command) {
            printf("help: Unrecognized option '%s'. Please use correct command as argument "
                   "or type 'help' only to print help for all commands\n", help_args.help_cmd->sval[0]);
        }
    }

    return ret_value;
}

esp_err_t esp_console_register_help_command(void)
{
    help_args.help_cmd = arg_str0(NULL, NULL, "<string>", "Name of command");
    help_args.verbose_level = arg_intn("v", "verbose", "<0|1>", 0, 1,
                                       "If specified, list console commands with given verbose level");
    help_args.end = arg_end(2);

    esp_console_cmd_t command = {
        .command = "help",
        .help = "Print the summary of all registered commands if no arguments "
        "are given, otherwise print summary of given command.",
        .func = &help_command,
        .argtable = &help_args
    };
    return esp_console_cmd_register(&command);
}

esp_err_t esp_console_deregister_help_command(void)
{
    free(help_args.help_cmd);
    free(help_args.verbose_level);
    free(help_args.end);

    return esp_console_cmd_deregister("help");
}

esp_err_t esp_console_set_help_verbose_level(esp_console_help_verbose_level_e verbose_level)
{
    if (verbose_level >= ESP_CONSOLE_HELP_VERBOSE_LEVEL_MAX_NUM) {
        return ESP_ERR_INVALID_ARG;
    }
    s_verbose_level = verbose_level;
    return ESP_OK;
}

/* ============================ stdio stub ================================== */

static esp_err_t esp_stdio_install_io_driver(void)
{
    return ESP_OK;
}

static esp_err_t esp_stdio_uninstall_io_driver(void)
{
    return ESP_OK;
}

/* ============================ REPL ======================================== */

static void esp_console_repl_task(void *args)
{
    esp_console_repl_universal_t *repl_conf = (esp_console_repl_universal_t *) args;
    esp_console_repl_com_t *repl_com = &repl_conf->repl_com;

    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    if (repl_com->state_mux != NULL) {
        xSemaphoreTake(repl_com->state_mux, portMAX_DELAY);
    }

    setvbuf(stdin, NULL, _IONBF, 0);

    printf("\r\n"
           "Type 'help' to get the list of commands.\r\n"
           "Use UP/DOWN arrows to navigate through command history.\r\n"
           "Press TAB when typing command name to auto-complete.\r\n");

    if (linenoiseIsDumbMode()) {
        printf("\r\n"
               "Your terminal application does not support escape sequences.\n\n"
               "Line editing and history features are disabled.\n\n"
               "On Windows, try using Windows Terminal or Putty instead.\r\n");
    }

    while (repl_com->state == CONSOLE_REPL_STATE_START) {
        char *line = linenoise(repl_com->prompt);
        if (line == NULL) {
            continue;
        }
        linenoiseHistoryAdd(line);
        if (repl_com->history_save_path) {
            linenoiseHistorySave(repl_com->history_save_path);
        }

        int ret;
        esp_err_t err = esp_console_run(line, &ret);
        if (err == ESP_ERR_NOT_FOUND) {
            printf("Unrecognized command\n");
        } else if (err == ESP_ERR_INVALID_ARG) {
        } else if (err == ESP_OK && ret != ESP_OK) {
            printf("Command returned non-zero error code: 0x%x\n", ret);
        } else if (err != ESP_OK) {
            printf("Internal error: %s\n", esp_err_to_name(err));
        }
        linenoiseFree(line);
    }

    if (repl_com->state_mux != NULL) {
        xSemaphoreGive(repl_com->state_mux);
    }
    vTaskDelete(NULL);
}

static esp_err_t esp_console_setup_prompt(const char *prompt, esp_console_repl_com_t *repl_com)
{
    const char *prompt_temp = "esp>";
    if (prompt) {
        prompt_temp = prompt;
    }
    snprintf(repl_com->prompt, CONSOLE_PROMPT_MAX_LEN - 1, "%s> ", prompt_temp);

    int probe_status = linenoiseProbe();
    if (probe_status) {
        linenoiseSetDumbMode(1);
        snprintf(repl_com->prompt, CONSOLE_PROMPT_MAX_LEN - 1, "%s> ", prompt_temp);
    }

    return ESP_OK;
}

static esp_err_t esp_console_setup_history(const char *history_path, uint32_t max_history_len,
                                           esp_console_repl_com_t *repl_com)
{
    repl_com->history_save_path = history_path;
    if (history_path) {
        linenoiseHistoryLoad(history_path);
    }

    if (linenoiseHistorySetMaxLen(max_history_len) != 1) {
        ESP_LOGE(TAG, "set max history length to %lu failed", (unsigned long)max_history_len);
        return ESP_FAIL;
    }
    return ESP_OK;
}

static esp_err_t esp_console_common_init(size_t max_cmdline_length, esp_console_repl_com_t *repl_com)
{
    esp_err_t ret;
    esp_console_config_t console_config = ESP_CONSOLE_CONFIG_DEFAULT();
    repl_com->max_cmdline_length = console_config.max_cmdline_length;
    if (max_cmdline_length != 0) {
        console_config.max_cmdline_length = max_cmdline_length;
        repl_com->max_cmdline_length = max_cmdline_length;
    }

    console_config.hint_color = 39;
    ret = esp_console_init(&console_config);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_console_register_help_command();
    if (ret != ESP_OK) {
        return ret;
    }

    linenoiseSetMultiLine(1);
    linenoiseSetCompletionCallback(&esp_console_get_completion);
    linenoiseSetHintsCallback((linenoiseHintsCallback *)&esp_console_get_hint);

    ret = linenoiseSetMaxLineLen(repl_com->max_cmdline_length);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_console_internal_set_event_fd(repl_com);
    if (ret != ESP_OK) {
        return ret;
    }

    return ESP_OK;
}

static esp_err_t esp_console_common_deinit(esp_console_repl_com_t *repl_com)
{
    repl_com->state = CONSOLE_REPL_STATE_DEINIT;

    esp_err_t ret = esp_console_deregister_help_command();
    if (ret != ESP_OK) {
        return ret;
    }

    linenoiseHistoryFree();

    return ESP_OK;
}

esp_err_t esp_console_delete_repl_stdio(esp_console_repl_t *repl)
{
    esp_console_repl_com_t *repl_com = (esp_console_repl_com_t*)repl;
    esp_console_repl_universal_t *universal_repl = (esp_console_repl_universal_t*)repl_com;

    const esp_err_t ret = esp_console_common_deinit(repl_com);
    if (ret != ESP_OK) {
        if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGE(TAG, "already de-initialized");
        }
        return ret;
    }

    esp_console_deinit();

    esp_stdio_uninstall_io_driver();

    free(universal_repl);

    return ESP_OK;
}

esp_err_t esp_console_new_repl_stdio(const esp_console_repl_config_t *repl_config, esp_console_repl_t **ret_repl)
{
    esp_err_t ret = ESP_OK;
    esp_console_repl_universal_t *universal_repl = NULL;
    if (!repl_config || !ret_repl) {
        ret = ESP_ERR_INVALID_ARG;
        goto _exit;
    }

    universal_repl = calloc(1, sizeof(esp_console_repl_universal_t));
    if (!universal_repl) {
        ret = ESP_ERR_NO_MEM;
        goto _exit;
    }

    fflush(stdout);
    fsync(fileno(stdout));

    ret = esp_console_common_init(repl_config->max_cmdline_length, &universal_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    ret = esp_console_setup_history(repl_config->history_save_path, repl_config->max_history_len,
                                    &universal_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    ret = esp_console_setup_prompt(repl_config->prompt, &universal_repl->repl_com);
    if (ret != ESP_OK) {
        goto _exit;
    }

    ret = esp_stdio_install_io_driver();
    if (ret != ESP_OK) {
        goto _exit;
    }

    universal_repl->repl_com.state = CONSOLE_REPL_STATE_INIT;
    universal_repl->repl_com.repl_core.del = esp_console_delete_repl_stdio;

    if (xTaskCreate(esp_console_repl_task, "console_repl", repl_config->task_stack_size,
                    universal_repl, repl_config->task_priority,
                    (TaskHandle_t *)&universal_repl->repl_com.task_hdl) != pdTRUE) {
        ret = ESP_FAIL;
        goto _exit;
    }

    *ret_repl = &universal_repl->repl_com.repl_core;
    return ESP_OK;
_exit:
    if (universal_repl) {
        esp_console_deinit();
        free(universal_repl);
    }
    if (ret_repl) {
        *ret_repl = NULL;
    }
    return ret;
}

esp_err_t esp_console_start_repl(esp_console_repl_t *repl)
{
    esp_console_repl_com_t *repl_com = (esp_console_repl_com_t*)repl;
    if (repl_com->state != CONSOLE_REPL_STATE_INIT) {
        return ESP_ERR_INVALID_STATE;
    }

    repl_com->state = CONSOLE_REPL_STATE_START;
    xTaskNotifyGive(repl_com->task_hdl);
    return ESP_OK;
}

/* ============================ STOP REPL =================================== */

static int s_interrupt_reading_fd = -1;
static uint64_t s_interrupt_reading_signal = 1;

static ssize_t read_bytes(int fd, void *buf, size_t max_bytes)
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    if (s_interrupt_reading_fd != -1) {
        FD_SET(s_interrupt_reading_fd, &read_fds);
    }
    int maxFd = MAX(fd, s_interrupt_reading_fd);
    int nread = select(maxFd + 1, &read_fds, NULL, NULL, NULL);
    if (nread < 0) {
        return -1;
    }

    if (s_interrupt_reading_fd != -1 && FD_ISSET(s_interrupt_reading_fd, &read_fds)) {
        int buf[sizeof(s_interrupt_reading_signal)];
        nread = read(s_interrupt_reading_fd, buf, sizeof(s_interrupt_reading_signal));
        if ((nread == sizeof(s_interrupt_reading_signal)) && (buf[0] == s_interrupt_reading_signal)) {
            return -1;
        }
    } else if (FD_ISSET(fd, &read_fds)) {
        nread = read(fd, buf, max_bytes);
    }
    return nread;
}

void linenoiseSetReadCharacteristics(void)
{
    int stdin_fileno = fileno(stdin);
    int flags = fcntl(stdin_fileno, F_GETFL);
    flags |= O_NONBLOCK;
    (void)fcntl(stdin_fileno, F_SETFL, flags);

    linenoiseSetReadFunction(read_bytes);
}

esp_err_t esp_console_internal_set_event_fd(esp_console_repl_com_t *repl_com)
{
    esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
    esp_err_t ret = esp_vfs_eventfd_register(&config);
    if (ret == ESP_OK) {
        s_interrupt_reading_fd = eventfd(0, 0);
    } else if (ret == ESP_ERR_INVALID_STATE) {
    } else {
        return ESP_FAIL;
    }

    repl_com->state_mux = xSemaphoreCreateMutex();
    if (repl_com->state_mux == NULL) {
        ESP_LOGE(TAG_REPL, "state_mux create error");
        return ESP_ERR_NO_MEM;
    }
    xSemaphoreGive(repl_com->state_mux);

    return ESP_OK;
}

static esp_err_t esp_console_common_deinit_full(esp_console_repl_com_t *repl_com)
{
    repl_com->state = CONSOLE_REPL_STATE_DEINIT;

    if (s_interrupt_reading_fd == -1) {
        return ESP_FAIL;
    }

    int nwrite = write(s_interrupt_reading_fd, &s_interrupt_reading_signal, sizeof(s_interrupt_reading_signal));
    if (nwrite != sizeof(s_interrupt_reading_signal)) {
        return ESP_FAIL;
    }

    xSemaphoreTake(repl_com->state_mux, portMAX_DELAY);

    vSemaphoreDelete(repl_com->state_mux);
    repl_com->state_mux = NULL;

    esp_err_t ret = esp_console_deregister_help_command();
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_vfs_eventfd_unregister();
    if (ret != ESP_OK) {
        return ret;
    }

    linenoiseHistoryFree();

    return ESP_OK;
}

esp_err_t esp_console_stop_repl(esp_console_repl_t *repl)
{
    esp_console_repl_com_t *repl_com = (esp_console_repl_com_t*)repl;
    esp_console_repl_universal_t *universal_repl = (esp_console_repl_universal_t*)repl_com;

    const esp_err_t ret = esp_console_common_deinit_full(repl_com);
    if (ret != ESP_OK) {
        return ret;
    }

    esp_console_deinit();

    esp_stdio_uninstall_io_driver();

    free(universal_repl);

    return ESP_OK;
}

#endif
