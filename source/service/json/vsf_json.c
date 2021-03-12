/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "service/vsf_service_cfg.h"

#if VSF_USE_JSON == ENABLED

#define __VSF_JSON_CLASS_IMPLEMENT
#include "./vsf_json.h"
#include "utilities/vsf_utilities.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static bool vsf_json_token_match(const char *json, const char *value)
{
    int n = strlen(value);
    return !strncmp(json, value, n) && !isalnum(json[n]);
}

vsf_json_type_t vsf_json_get_type(const char *json)
{
    vsf_json_type_t type = VSF_JSON_TYPE_INVALID;
    char ch = *json;

    if (ch == '{') {
        type = VSF_JSON_TYPE_OBJECT;
    } else if (ch == '[') {
        type = VSF_JSON_TYPE_ARRAY;
    } else if (ch == '"') {
        type = VSF_JSON_TYPE_STRING;
    } else if ((ch >= '0') && (ch <= '9')) {
        type = VSF_JSON_TYPE_NUMBER;
    } else if (vsf_json_token_match(json, "true") || vsf_json_token_match(json, "false")) {
        type = VSF_JSON_TYPE_BOOLEAN;
    } else if (vsf_json_token_match(json, "null")) {
        type = VSF_JSON_TYPE_NULL;
    }
    return type;
}

static char * vsf_json_skip_space(const char *json)
{
    while ((*json != '\0') && isspace(*json)) {
        json++;
    }
    return *json == '\0' ? NULL : (char *)json;
}

vsf_json_type_t vsf_json_enumerate_start(vsf_json_enumerator_t *e, const char *json)
{
    vsf_json_type_t type = vsf_json_get_type(json);

    if ((type != VSF_JSON_TYPE_OBJECT) && (type != VSF_JSON_TYPE_ARRAY)) {
        type = VSF_JSON_TYPE_INVALID;
    } else {
        e->ptr = (char *)json;
        e->type = type;
        e->first = true;
        e->ptr = vsf_json_skip_space(++e->ptr);
        if (!e->ptr) {
            type = VSF_JSON_TYPE_INVALID;
        }
    }
    return type;
}

char * vsf_json_enumerate_next(vsf_json_enumerator_t *e)
{
    char *cur = e->ptr;
    bool str = false;
    int brace_level = 0, square_level = 0;

    if (e->first) {
        e->first = false;
        if (*cur == ',') {
            return NULL;
        }
        goto exit;
    } else {
        while (*cur != '\0') {
            switch (*cur) {
            case '\"':
                str = !str;
                break;
            case '\\':
                if (!str) {
                    return NULL;
                }
                cur++;
                break;
            case '{':
                if (!str) {
                    brace_level++;
                }
                break;
            case '[':
                if (!str) {
                    square_level++;
                }
                break;
            case ',':
                if (!str && !brace_level && !square_level) {
                    cur = vsf_json_skip_space(++cur);
                    goto exit;
                }
                break;
            case '}':
                if (!str) {
                    brace_level--;
                    if (brace_level < 0) {
                        return NULL;
                    }
                    goto check;
                }
                break;
            case ']':
                if (!str) {
                    square_level--;
                    if (square_level < 0) {
                        return NULL;
                    }
                check:
                    if (!brace_level && !square_level) {
                        cur = vsf_json_skip_space(++cur);
                        if (*cur == ',') {
                            cur = vsf_json_skip_space(++cur);
                            goto exit;
                        } else {
                            return NULL;
                        }
                    }
                }
                break;
            }
            cur++;
        }

    exit:
        if ((*cur == '}') || (*cur == ']')) {
            return NULL;
        }
    }
    e->ptr = cur;
    return cur;
}

static bool vsf_json_isdiv(char ch)
{
    return (ch == '/') || (ch == '\\');
}

int vsf_json_num_of_entry(const char *json)
{
    vsf_json_enumerator_t e;
    int num_of_entry = 0;

    if (vsf_json_enumerate_start(&e, json) != VSF_JSON_TYPE_INVALID) {
        while (vsf_json_enumerate_next(&e) != NULL) {
            num_of_entry++;
        }
    }
    return num_of_entry;
}

char *vsf_json_get(const char *json, const char *key)
{
    vsf_json_enumerator_t e;
    unsigned long idx, curidx = 0;
    char *cur;

    if (vsf_json_isdiv(*key)) {
        key++;
    }

    while (*key != '\0') {
        if (VSF_JSON_TYPE_INVALID == vsf_json_enumerate_start(&e, json)) {
            return NULL;
        }

        if (e.type == VSF_JSON_TYPE_ARRAY) {
            idx = strtoul(key, (char **)&key, 0);
            curidx = 0;
        } else {
            for (idx = 0; key[idx] && !vsf_json_isdiv(key[idx]); idx++);
        }

        while (1) {
            cur = vsf_json_enumerate_next(&e);
            if (!cur) {
                return NULL;
            }

            if (e.type == VSF_JSON_TYPE_ARRAY) {
                if (curidx++ == idx) {
                    json = cur;
                    break;
                }
            } else {
                if (*cur++ != '\"') {
                    return NULL;
                }
                if (!strncmp(key, cur, idx) && (cur[idx] == '\"')) {
                    key += idx;
                    if (*key) {
                        key++;
                    }
                    cur += idx + 1;
                    cur = vsf_json_skip_space(cur);
                    if (*cur++ != ':') {
                        return NULL;
                    }

                    json = vsf_json_skip_space(cur);
                    break;
                }
            }
        }
    }
    return (char *)json;
}

int vsf_json_get_string(const char *json, char *result, int len)
{
    char ch;
    int curlen = 0;

    if (*json++ != '"') {
        return -1;
    }

    do {
        ch = *json++;
        if (ch == '\\') {
            ch = *json++;
            if (result && (curlen >= len)) {
                VSF_SERVICE_ASSERT(false);
                return VSF_ERR_NOT_ENOUGH_RESOURCES;
            }
            switch (ch) {
            case '"':   if (result) result[curlen] = '\"';  curlen++;break;
            case '\\':  if (result) result[curlen] = '\\';  curlen++;break;
            case '/':   if (result) result[curlen] = '/';   curlen++;break;
            case 'b':   if (result) result[curlen] = '\b';  curlen++;break;
            case 'f':   if (result) result[curlen] = '\f';  curlen++;break;
            case 'n':   if (result) result[curlen] = '\n';  curlen++;break;
            case 'r':   if (result) result[curlen] = '\r';  curlen++;break;
            case 't':   if (result) result[curlen] = '\t';  curlen++;break;
            case 'u':
                // TODO:
                break;
            default:
                return -1;
            }
        } else if (ch != '\"') {
            if (result && (curlen >= len)) {
                VSF_SERVICE_ASSERT(false);
                return VSF_ERR_NOT_ENOUGH_RESOURCES;
            }
            if (result) {
                result[curlen] = ch;
            }
            curlen++;
        }
    } while (ch != '\"');

    if (result && (curlen >= len)) {
        VSF_SERVICE_ASSERT(false);
        return VSF_ERR_NOT_ENOUGH_RESOURCES;
    }
    if (result) {
        result[curlen] = '\0';
    }
    return ++curlen;
}

int vsf_json_get_number(const char *json, double *result)
{
    double value = strtod(json, NULL);
    if (result) {
        *result = value;
    }
    return sizeof(*result);
}

int vsf_json_get_boolean(const char *json, bool *result)
{
    bool value;
    if (vsf_json_token_match(json, "true")) {
        value = true;
    } else if (vsf_json_token_match(json, "false")) {
        value = false;
    } else {
        return -1;
    }
    if (result) {
        *result = value;
    }
    return sizeof(*result);
}

// constructor
void vsf_json_constructor_init(vsf_json_constructor_t *c, void *param,
        int (*write_str)(void *, char *, int))
{
    c->write_str = write_str;
    if (write_str != NULL) {
        c->param = param;
    } else {
        c->len = 0;
    }
    c->first = true;
    c->result = true;
}

int vsf_json_write_str(vsf_json_constructor_t *c, char *buf, int len)
{
    if (c->write_str != NULL) {
        int ret = c->write_str(c->param, buf, len);
        if (ret < 0) {
            c->result = false;
        }
        return ret;
    } else {
        c->len += len;
        return 0;
    }
}

static int vsf_json_set_token(vsf_json_constructor_t *c, char *token)
{
    int n = strlen(token);
    return vsf_json_write_str(c, token, n);
}

int vsf_json_set_key(vsf_json_constructor_t *c, char *key)
{
    int ret;

    if (c->first) {
        c->first = false;
    } else {
        ret = vsf_json_write_str(c, ",", 1);
        if (ret < 0) { return ret; }
    }

    if (key != NULL) {
        ret = vsf_json_write_str(c, "\"", 1);
        if (ret < 0) { return ret; }
        ret = vsf_json_set_token(c, key);
        if (ret < 0) { return ret; }
        ret = vsf_json_write_str(c, "\"", 1);
        if (ret < 0) { return ret; }

        ret = vsf_json_write_str(c, ":", 1);
        if (ret < 0) { return ret; }
    }
    return 0;
}

int vsf_json_set_string(vsf_json_constructor_t *c, char *key, char *value)
{
    int ret;
    char ch, buf[2];

    ret = vsf_json_set_key(c, key);
    if (ret < 0) { return ret; }
    ret = vsf_json_write_str(c, "\"", 1);
    if (ret < 0) { return ret; }

    do {
        ch = *value++;
        switch (ch) {
        case '\0':  break;
        case '\"':  buf[1] = '"';   goto set_esc;
        case '\\':  buf[1] = '\\';  goto set_esc;
        case '/':   buf[1] = '/';   goto set_esc;
        case '\b':  buf[1] = 'b';   goto set_esc;
        case '\f':  buf[1] = 'b';   goto set_esc;
        case '\n':  buf[1] = 'n';   goto set_esc;
        case '\r':  buf[1] = 'r';   goto set_esc;
        case '\t':  buf[1] = 't';
        set_esc:
            buf[0] = '\\';
            ret = vsf_json_write_str(c, buf, 2);
            if (ret < 0) { return ret; }
            break;
        default:
            buf[0] = ch;
            ret = vsf_json_write_str(c, buf, 1);
            if (ret < 0) { return ret; }
            break;
        }
    } while (ch != '\0');

    ret = vsf_json_write_str(c, "\"", 1);
    return (ret < 0) ? ret : 0;
}

int vsf_json_set_integer(vsf_json_constructor_t *c, char *key, int value)
{
    int ret;
    char buf[64];

    ret = vsf_json_set_key(c, key);
    if (ret < 0) { return ret; }

    ret = snprintf(buf, sizeof(buf), "%d", value);
    ret = vsf_json_write_str(c, buf, ret);
    return (ret < 0) ? ret : 0;
}

int vsf_json_set_double(vsf_json_constructor_t *c, char *key, double value)
{
    int ret;
    char buf[64];

    ret = vsf_json_set_key(c, key);
    if (ret < 0) { return ret; }

    ret = snprintf(buf, sizeof(buf), "%f", value);
    ret = vsf_json_write_str(c, buf, ret);
    return (ret < 0) ? ret : 0;
}

int vsf_json_set_boolean(vsf_json_constructor_t *c, char *key, bool value)
{
    if (vsf_json_set_key(c, key) < 0) { return -1; }
    return vsf_json_set_token(c, value ? "true" : "false");
}

int vsf_json_set_null(vsf_json_constructor_t *c, char *key)
{
    if (vsf_json_set_key(c, key) < 0) { return -1; }
    return vsf_json_set_token(c, "null");
}

int vsf_json_constructor_buffer_write_str(void *param, char *str, int len)
{
    vsf_mem_t *mem = param;

    if (mem->size < (len + 1)) {
        return -1;
    }
    memcpy(mem->buffer, str, len);
    mem->buffer[len] = '\0';
    mem->buffer += len;
    mem->size -= len;
    return len;
}

#endif      // VSF_USE_JSON
