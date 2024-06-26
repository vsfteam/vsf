/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#define __VSF_LINUX_FS_CLASS_INHERIT__
#include "component/script/python/vsf_python.h"

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

#if !VSF_PYAL_FEATURE_MODULE_IS_DYN
vsf_pyal_class_declare_for_inherit(os, environ_dict, dict);
vsf_pyal_builtinclass_declare(dict);
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if !VSF_PYAL_FEATURE_MODULE_IS_DYN
vsf_pyal_static_dict_type(__os_environ, vsf_pyal_class_type(os, environ_dict));
#else
vsf_pyal_class_declare(os, environ_dict);
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#endif

vsf_pyal_module_func_fix_imp(os, __init_environ, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_func_void_return_t, vsf_pyal_obj_t environobj)
{
    char **__env = environ, *key, *value;

    while (*__env != NULL) {
        key = strdup(*__env);
        value = strstr(key, "=");
        if (value != NULL) {
            *value = '\0';
            value++;
        }

        vsf_pyal_arg_t arg = vsf_pyal_newarg_str(value);
        vsf_pyal_dictobj_strkey_set(environobj, key, arg);
        free(key);

        __env++;
    }
    vsf_pyal_func_void_return();
}

vsf_pyal_module_func_init_imp(os)
{
#if defined(vsf_pyal_class_begin) && defined(vsf_pyal_class_end)
#   ifdef vsf_pyal_static_dict_t
    vsf_pyal_class_inherit_func_call(os, environ_dict, dict);
#   endif
#endif

#if VSF_PYAL_FEATURE_MODULE_IS_DYN
    vsf_pyal_module_addstr(os, "sep", "/");
    vsf_pyal_module_addint(os, "O_RDONLY", O_RDONLY);
    vsf_pyal_module_addint(os, "O_WRONLY", O_WRONLY);
    vsf_pyal_module_addint(os, "O_RDWR", O_RDWR);
    vsf_pyal_module_addint(os, "O_APPEND", O_APPEND);
    vsf_pyal_module_addint(os, "O_CREAT", O_CREAT);

    struct utsname name;
    uname(&name);
    vsf_pyal_module_addstr(os, "name", name.sysname);

    // for dynamic implementation, __init_environ should be called in python code
#else
    vsf_pyal_obj_t environobj = &__os_environ;
    vsf_pyal_module_func_call(vsf_pyal_module_func_name(os, __init_environ), environobj);
    vsf_pyal_root_strkey_addobj("_os_environ", environobj);
#endif

    vsf_pyal_module_func_init_return();
}

#if VSF_USE_MICROPYTHON == ENABLED
vsf_pyal_module_func_fix_imp(os, qstr_hash, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_func_void_return_t, vsf_pyal_funcarg_str_t strarg)
{
    const char *str = vsf_pyal_funcarg_getstr(strarg);
    size_t hash = qstr_compute_hash(str, strlen(str));
    vsf_trace_info("hash of %s is %d" VSF_TRACE_CFG_LINEEND, str, hash);
    vsf_pyal_func_void_return();
}
#endif

vsf_pyal_module_func_fix_imp(os, stat, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_obj_t, vsf_pyal_funcarg_str_t patharg)
{
    const char *path = vsf_pyal_funcarg_getstr(patharg);
    struct stat st;
    if (stat(path, &st) < 0) {
        vsf_pyal_raise("fail to stat()\n");
        return VSF_PYAL_OBJ_NULL;
    }

    vsf_pyal_arg_t args[10] = {
        vsf_pyal_newarg_int(st.st_mode),
        vsf_pyal_newarg_int(st.st_ino),
        vsf_pyal_newarg_int(st.st_dev),
        vsf_pyal_newarg_int(st.st_nlink),
        vsf_pyal_newarg_int(st.st_uid),
        vsf_pyal_newarg_int(st.st_gid),
        vsf_pyal_newarg_int(st.st_size),
        vsf_pyal_newarg_int(st.st_atime),
        vsf_pyal_newarg_int(st.st_mtime),
        vsf_pyal_newarg_int(st.st_ctime),
    };
    return vsf_pyal_newobj_tuple(dimof(args), args);
}

vsf_pyal_module_func_var_imp(os, listdir, vsf_pyal_obj_t, 0, 1, arg)
{
    int argc = vsf_pyal_funcarg_var_num(arg);
#if VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
    if (argc > 1) {
        vsf_pyal_raise("invalid argument, format: lisdir(*path)\n");
        return VSF_PYAL_OBJ_NULL;
    }
#endif
    char __path[PATH_MAX];
    char *str_path = argc == 0 ? getcwd(__path, sizeof(__path)) : vsf_pyal_funcarg_var_getstr(arg, 0);
    struct dirent* dp;
    DIR* dir = opendir(str_path);

    vsf_pyal_obj_t list = vsf_pyal_newobj_list(0, NULL);

    if (dir != NULL) {
        while ((dp = readdir(dir)) != NULL) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                vsf_pyal_arg_t arg = vsf_pyal_newarg_str(dp->d_name);
                vsf_pyal_listobj_append(list, arg);
            }
        }

        closedir(dir);
    }

    return list;
}

vsf_pyal_module_func_fix_imp(os, getcwd, VSF_PYAL_MODULE_FUNCARG_OBJ_0, vsf_pyal_funcarg_str_t)
{
    char path[PATH_MAX];
    if (NULL == getcwd(path, sizeof(path))) {
        vsf_pyal_raise("fail to getcwd()\n");
        return VSF_PYAL_OBJ_NULL;
    }

    return vsf_pyal_newfuncarg_str(path);
}

vsf_pyal_module_func_var_imp(os, mkdir, vsf_pyal_func_void_return_t, 1, 2, arg)
{
    int argc = vsf_pyal_funcarg_var_num(arg);
#if VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
    if ((argc < 1) || (argc > 2)) {
        vsf_pyal_raise("invalid argument, format: mkdir(path, *mode)\n");
        return;
    }
#endif

    char *path_str = vsf_pyal_funcarg_var_getstr(arg, 0);
    int mode_int = 1 == argc ? 0777 : vsf_pyal_funcarg_var_getint(arg, 1);
    if (mkdir((const char *)path_str, mode_int) < 0) {
        vsf_pyal_raise("fail to mkdir(%s, %d)\n", path_str, mode_int);
    }
    vsf_pyal_func_void_return();
}

vsf_pyal_module_func_var_imp(os, makedirs, vsf_pyal_func_void_return_t, 1, 2, arg)
{
    int argc = vsf_pyal_funcarg_var_num(arg);
#if VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
    if ((argc < 1) || (argc > 2)) {
        vsf_pyal_raise("invalid argument, format: makedirs(path, *mode)\n");
        return;
    }
#endif

    char *path_str = vsf_pyal_funcarg_var_getstr(arg, 0);
    int mode_int = 1 == argc ? 0777 : vsf_pyal_funcarg_var_getint(arg, 1);
    if (mkdirs((const char *)path_str, mode_int) < 0) {
        vsf_pyal_raise("fail to mkdirs(%s, %d)\n", path_str, mode_int);
    }
    vsf_pyal_func_void_return();
}

vsf_pyal_module_func_fix_imp(os, rmdir, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_func_void_return_t, vsf_pyal_funcarg_str_t path)
{
    char *path_str = vsf_pyal_funcarg_getstr(path);
    if (rmdir((const char *)path_str) < 0) {
        vsf_pyal_raise("fail to rmdir(%s)\n", path_str);
    }
    vsf_pyal_func_void_return();
}

vsf_pyal_module_func_fix_imp(os, chdir, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_func_void_return_t, vsf_pyal_funcarg_str_t path)
{
    char *path_str = vsf_pyal_funcarg_getstr(path);
    if (chdir((const char *)path_str) < 0) {
        vsf_pyal_raise("fail to chdir(%s)\n", path_str);
    }
    vsf_pyal_func_void_return();
}

vsf_pyal_module_func_fix_imp(os, rename, VSF_PYAL_MODULE_FUNCARG_OBJ_2, vsf_pyal_func_void_return_t, vsf_pyal_funcarg_str_t old, vsf_pyal_funcarg_str_t new)
{
    char *path_old_str = vsf_pyal_funcarg_getstr(old);
    char *path_new_str = vsf_pyal_funcarg_getstr(new);
    if (rename((const char *)path_old_str, (const char *)path_new_str) < 0) {
        vsf_pyal_raise("fail to rename(%s, %s)\n", path_old_str, path_new_str);
    }
    vsf_pyal_func_void_return();
}

vsf_pyal_module_func_fix_imp(os, remove, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_func_void_return_t, vsf_pyal_funcarg_str_t path)
{
    char *path_str = vsf_pyal_funcarg_getstr(path);
    if (remove((const char *)path_str) < 0) {
        vsf_pyal_raise("fail to remove(%s)\n", path_str);
    }
    vsf_pyal_func_void_return();
}

vsf_pyal_module_func_var_imp(os, open, vsf_pyal_arg_t, 1, 3, arg)
{
    int argc = vsf_pyal_funcarg_var_num(arg);
#if VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
    if ((argc < 1) || (argc > 3)) {
        vsf_pyal_raise("invalid argument, format: fd_int open(path, flags_int, *mode) | file_obj open(path, mode_str)\n");
        return VSF_PYAL_ARG_NULL;
    }
#endif

    // if the first arg is int, it's fd_int
    // if the first arg is str, it's path_str
    if ((1 == argc) && vsf_pyal_funcarg_var_is_int(arg, 0)) {
        int fd = vsf_pyal_funcarg_var_getint(arg, 0);
        FILE *f = fdopen(fd, "r+");
        if (NULL == f) {
            vsf_pyal_raise("invalid fd %d\n", fd);
            return VSF_PYAL_ARG_NULL;
        }
        return vsf_pyal_newarg_file(f, true);
    }

    char *path_str = vsf_pyal_funcarg_var_getstr(arg, 0);
    // if the 2nd arg is int, it's flags of open
    // if the 2nd arg is str, it's mode of fopen
    if (vsf_pyal_funcarg_var_is_int(arg, 1)) {
        int flags_int = vsf_pyal_funcarg_var_getint(arg, 1);
        // the possible 3rd arg as mode of open, default is 0777
        int mode_int = 2 == argc ? 0777 : vsf_pyal_funcarg_var_getint(arg, 2);
        int fd = open((const char *)path_str, flags_int, mode_int);
        if (fd < 0) {
            vsf_pyal_raise("fail to open(%s, %d)\n", path_str, mode_int);
            return VSF_PYAL_ARG_NULL;
        }
        return vsf_pyal_newarg_int(fd);
    } else if (vsf_pyal_funcarg_var_is_str(arg, 1)) {
        char *mode_str = vsf_pyal_funcarg_var_getstr(arg, 1);
        FILE *f = fopen(path_str, mode_str);
        if (NULL == f) {
            vsf_pyal_raise("fail to open(%s, %s)\n", path_str, mode_str);
            return VSF_PYAL_ARG_NULL;
        }
        bool is_text = NULL == strchr(mode_str, 'b');
        return vsf_pyal_newarg_file(f, is_text);
    } else {
        vsf_pyal_raise("invalid argument, format: open(path, *mode)\n");
        return VSF_PYAL_ARG_NULL;
    }
}

vsf_pyal_module_func_var_imp(os, read, vsf_pyal_funcarg_bytes_t, 2, 2, arg)
{
#if VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
    int argc = vsf_pyal_funcarg_var_num(arg);
    if (argc != 2) {
        vsf_pyal_raise("invalid argument, format: bytes read(fd/file, length)\n");
        return VSF_PYAL_OBJ_NULL;
    }
#endif

    // the 2nd arg is length
    ssize_t length = vsf_pyal_funcarg_var_getint(arg, 1);
    int fd;

    // the first arg is fd_int/file_obj
    if (vsf_pyal_funcarg_var_is_int(arg, 0)) {
        fd = vsf_pyal_funcarg_var_getint(arg, 0);
    } else {
        vsf_pyal_arg_t filearg = vsf_pyal_funcarg_var_getarg(arg, 0);
        FILE *f = vsf_pyal_filearg_getfile(filearg);
        fd = ((vsf_linux_fd_t *)f)->fd;
    }

#ifdef vsf_pyal_buffer_t
    vsf_pyal_buffer_t buffer = vsf_pyal_new_buffer(length);
    length = read(fd, vsf_pyal_buffer_getptr(buffer), vsf_pyal_buffer_getlen(buffer));
    if (length <= 0) {
        vsf_pyal_raise("fail to read fd %d\n", fd);
        vsf_pyal_buffer_free(buffer);
        return VSF_PYAL_OBJ_NULL;
    }

    return vsf_pyal_newarg_bytes_ret_from_buffer(buffer, length);
#else
    char *buffer = malloc(length);
    if (NULL == buffer) {
        vsf_pyal_raise("fail to allocate buffer\n");
        return VSF_PYAL_OBJ_NULL;
    }

    length = read(fd, buffer, length);
    if (length <= 0) {
        vsf_pyal_raise("fail to read fd %d\n", fd);
        free(buffer);
        return VSF_PYAL_OBJ_NULL;
    }

    return vsf_pyal_newfuncarg_bytes_ret_and_free(buffer, length);
#endif
}

vsf_pyal_module_func_var_imp(os, write, vsf_pyal_funcarg_int_t, 2, 2, arg)
{
#if VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
    int argc = vsf_pyal_funcarg_var_num(arg);
    if (argc != 2) {
        vsf_pyal_raise("invalid argument, format: write(fd/file, bytes)\n");
        return vsf_pyal_newfuncarg_int(-1);
    }
#endif

    // the 2nd arg is bytesobj
    vsf_pyal_arg_t bytesarg = vsf_pyal_funcarg_var_getarg(arg, 1);
    size_t length;
    uint8_t *buffer = vsf_pyal_arg_getbytes(bytesarg, &length);
    int fd;

    // the first arg is fd_int/file_obj
    if (vsf_pyal_funcarg_var_is_int(arg, 0)) {
        fd = vsf_pyal_funcarg_var_getint(arg, 0);
    } else {
        vsf_pyal_arg_t filearg = vsf_pyal_funcarg_var_getarg(arg, 0);
        FILE *f = vsf_pyal_filearg_getfile(filearg);
        fd = ((vsf_linux_fd_t *)f)->fd;
    }

    length = write(fd, buffer, length);
    return vsf_pyal_newfuncarg_int(length);
}

vsf_pyal_module_func_var_imp(os, ioctl, vsf_pyal_funcarg_int_t, 2, 2, arg)
{
    int argc = vsf_pyal_funcarg_var_num(arg);
#if VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
    if ((argc < 2) || (argc > 3)) {
        vsf_pyal_raise("invalid argument, format: int ioctl(fd/file, request, *arg)\n");
        return vsf_pyal_newfuncarg_int(-1);
    }
#endif

    // the 2nd arg is request of ioctl
    unsigned long request = (unsigned long)vsf_pyal_funcarg_var_getint(arg, 1);
    int fd, result;

    // the first arg is fd_int/file_obj
    if (vsf_pyal_funcarg_var_is_int(arg, 0)) {
        fd = vsf_pyal_funcarg_var_getint(arg, 0);
    } else {
        vsf_pyal_arg_t filearg = vsf_pyal_funcarg_var_getarg(arg, 0);
        FILE *f = vsf_pyal_filearg_getfile(filearg);
        fd = ((vsf_linux_fd_t *)f)->fd;
    }

    // the possible 3rd arg is arg of ioctl
    if (3 == argc) {
        uintptr_t ioctl_arg = (uintptr_t)vsf_pyal_funcarg_var_getint(arg, 3);
        result = ioctl(fd, request, ioctl_arg);
    } else {
        result = ioctl(fd, request);
    }

    return vsf_pyal_newfuncarg_int(result);
}

vsf_pyal_module_func_var_imp(os, close, vsf_pyal_func_void_return_t, 1, 1, arg)
{
#if VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
    int argc = vsf_pyal_funcarg_var_num(arg);
    if (argc != 2) {
        vsf_pyal_raise("invalid argument, format: close(fd/file)\n");
        vsf_pyal_func_void_return();
    }
#endif

    // the first arg is fd_int/file_obj
    if (vsf_pyal_funcarg_var_is_int(arg, 0)) {
        int fd = vsf_pyal_funcarg_var_getint(arg, 0);
        close(fd);
    } else {
        vsf_pyal_arg_t filearg = vsf_pyal_funcarg_var_getarg(arg, 0);
        FILE *f = vsf_pyal_filearg_getfile(filearg);
        fclose(f);
        vsf_pyal_filearg_clear(filearg);
    }

    vsf_pyal_func_void_return();
}

vsf_pyal_module_func_fix_imp(os, system, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_func_void_return_t, vsf_pyal_funcarg_str_t cmd)
{
    char *cmd_str = vsf_pyal_funcarg_getstr(cmd);
    int result = system((const char *)cmd_str);
    if (result < 0) {
        vsf_pyal_raise("fail to call system(%s)\n", cmd_str);
        vsf_pyal_func_void_return();
    }
    vsf_pyal_func_void_return();
}

// os.environ_dict

vsf_pyal_class_func_fix_imp(os_environ_dict, __setitem__, VSF_PYAL_MODULE_FUNCARG_OBJ_3, vsf_pyal_func_void_return_t, vsf_pyal_arg_t keyarg, vsf_pyal_arg_t valuearg)
{
    char *key_str = vsf_pyal_arg_getstr(keyarg);
    char *value_str = vsf_pyal_arg_getstr(valuearg);
    setenv(key_str, value_str, true);
    vsf_pyal_func_void_return();
}

vsf_pyal_class_func_fix_imp(os_environ_dict, __delitem__, VSF_PYAL_MODULE_FUNCARG_OBJ_2, vsf_pyal_func_void_return_t, vsf_pyal_arg_t keyarg)
{
    char *key_str = vsf_pyal_arg_getstr(keyarg);
    unsetenv(key_str);
    vsf_pyal_func_void_return();
}

// os.path

vsf_pyal_module_func_fix_imp(os_path, abspath, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_funcarg_str_t, vsf_pyal_funcarg_str_t path)
{
    char *path_str = vsf_pyal_funcarg_getstr(path);
    char abspath_buffer[PATH_MAX];
    
    if (NULL == realpath((const char *)path_str, (char *)abspath_buffer)) {
        return VSF_PYAL_OBJ_NULL;
    }

    return vsf_pyal_newfuncarg_str(abspath_buffer);
}

vsf_pyal_module_func_fix_imp(os_path, exists, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_funcarg_bool_t, vsf_pyal_funcarg_str_t path)
{
    char *path_str = vsf_pyal_funcarg_getstr(path);
    struct stat st;
    return vsf_pyal_newfuncarg_bool(stat(path_str, &st) == 0);
}

vsf_pyal_module_func_fix_imp(os_path, isabs, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_funcarg_bool_t, vsf_pyal_funcarg_str_t path)
{
    char *path_str = vsf_pyal_funcarg_getstr(path);
    return vsf_pyal_newfuncarg_bool(path_str[0] == '/');
}

vsf_pyal_module_func_fix_imp(os_path, isdir, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_funcarg_bool_t, vsf_pyal_funcarg_str_t path)
{
    char *path_str = vsf_pyal_funcarg_getstr(path);
    struct stat st;
    return vsf_pyal_newfuncarg_bool((stat(path_str, &st) == 0) && S_ISDIR(st.st_mode));
}

vsf_pyal_module_func_fix_imp(os_path, isfile, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_funcarg_bool_t, vsf_pyal_funcarg_str_t path)
{
    char *path_str = vsf_pyal_funcarg_getstr(path);
    struct stat st;
    return vsf_pyal_newfuncarg_bool((stat(path_str, &st) == 0) && S_ISREG(st.st_mode));
}

vsf_pyal_module_func_var_imp(os_path, join, vsf_pyal_funcarg_str_t, 0, 255, arg)
{
    char path_str[PATH_MAX], *path_ptr = path_str, *path_cur;
    int argc = vsf_pyal_funcarg_var_num(arg);
    for (int i = 0; i < argc; i++) {
        path_cur = vsf_pyal_funcarg_var_getstr(arg, i);
        if (path_ptr - path_str + strlen(path_cur) >= sizeof(path_str)) {
            vsf_pyal_raise("path too long\n");
            return VSF_PYAL_OBJ_NULL;
        }
        strcpy(path_ptr, path_cur);
        path_ptr += strlen(path_cur);
    }
    return vsf_pyal_newfuncarg_str(path_str);
}

vsf_pyal_module_func_fix_imp(os_path, basename, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_funcarg_str_t, vsf_pyal_funcarg_str_t path)
{
    char *path_str = vsf_pyal_funcarg_getstr(path);
    char *basename_str = strrchr(path_str, '/');
    if (NULL == basename_str) {
        return vsf_pyal_newfuncarg_str(path_str);
    } else {
        return vsf_pyal_newfuncarg_str(basename_str + 1);
    }
}

vsf_pyal_module_func_fix_imp(os_path, dirname, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_funcarg_str_t, vsf_pyal_funcarg_str_t path)
{
    char *path_str = vsf_pyal_funcarg_getstr(path);
    char *basename_str = strrchr(path_str, '/');
    if (NULL == basename_str) {
        return vsf_pyal_newfuncarg_str("");
    } else {
        int dirname_len = basename_str - path_str;
        char dirname_str[dirname_len + 1];
        memcpy(dirname_str, path_str, dirname_len);
        dirname_str[dirname_len] = '\0';
        return vsf_pyal_newfuncarg_str(dirname_str);
    }
}

vsf_pyal_module_func_fix_imp(os_path, __split_by_char, VSF_PYAL_MODULE_FUNCARG_OBJ_2, vsf_pyal_obj_t, vsf_pyal_funcarg_str_t path, vsf_pyal_funcarg_int_t ch)
{
    char *path_str = vsf_pyal_funcarg_getstr(path);
    int ch_int = vsf_pyal_funcarg_getint(ch);
    char *basename_str = strrchr(path_str, ch_int);
    vsf_pyal_arg_t args[2];

    if (NULL == basename_str) {
        args[0] = vsf_pyal_newarg_str(path_str);
        args[1] = vsf_pyal_newarg_str("");
        return vsf_pyal_newobj_tuple(2, args);
    } else {
        int dirname_len = basename_str - path_str;
        char dirname_str[dirname_len + 1];
        memcpy(dirname_str, path_str, dirname_len);
        dirname_str[dirname_len] = '\0';

        args[0] = vsf_pyal_newarg_str(dirname_str);
        args[1] = vsf_pyal_newarg_str(basename_str + 1);
        return vsf_pyal_newobj_tuple(2, args);
    }
}

vsf_pyal_module_func_fix_imp(os_path, split, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_obj_t, vsf_pyal_funcarg_str_t path)
{
    return vsf_pyal_module_func_call(vsf_pyal_module_func_name(os_path, __split_by_char), path, vsf_pyal_newfuncarg_int('/'));
}

vsf_pyal_module_func_fix_imp(os_path, splitext, VSF_PYAL_MODULE_FUNCARG_OBJ_1, vsf_pyal_obj_t, vsf_pyal_funcarg_str_t path)
{
    return vsf_pyal_module_func_call(vsf_pyal_module_func_name(os_path, __split_by_char), path, vsf_pyal_newfuncarg_int('.'));
}

#if   __IS_COMPILER_LLVM__ || __IS_COMPILER_ARM_COMPILER_6__
#   pragma clang diagnostic pop
#endif

#ifdef vsf_pyal_module

// avoid conflict with environ MACRO
#ifdef environ
#   undef environ
#endif

#   if defined(vsf_pyal_class_begin) && defined(vsf_pyal_class_end)

#       ifdef vsf_pyal_static_dict_t
vsf_pyal_class_subscript_func(os, environ_dict, arg)
{
    vsf_pyal_arg_t indexarg = vsf_pyal_class_subscript_idxarg(arg);
    vsf_pyal_arg_t valuearg = vsf_pyal_class_subscript_valuearg(arg);
    char *key_str = (char *)vsf_pyal_arg_getstr(indexarg);

    if (vsf_pyal_class_subscript_is_delete(arg)) {
        unsetenv(key_str);
    } else if (vsf_pyal_class_subscript_is_store(arg)) {
        char *value_str = (char *)vsf_pyal_arg_getstr(valuearg);
        setenv(key_str, value_str, true);
    }

    return vsf_pyal_class_call_builtin_feature_subscript(dict, arg);
}

// placeholders
vsf_pyal_class_print_func(os, environ_dict) {  }
vsf_pyal_class_iterator_func(os, environ_dict) { return VSF_PYAL_OBJ_NULL; }
vsf_pyal_class_binary_func(os, environ_dict) { return VSF_PYAL_OBJ_NULL; }
vsf_pyal_class_unary_func(os, environ_dict) { return VSF_PYAL_OBJ_NULL; }
vsf_pyal_class_entry(os, environ_dict)

// inherit from builtin dict features
vsf_pyal_class_inherit_func(os, environ_dict, dict, print, unary, binary, iterator, entry)
vsf_pyal_class_begin_for_inherit(os, environ_dict, dict)
vsf_pyal_class_end_for_inherit(os, environ_dict, dict,
    vsf_pyal_class_feature_subscript(os, environ_dict),
    vsf_pyal_class_feature_print(os, environ_dict),
    vsf_pyal_class_feature_entry(os, environ_dict),
    vsf_pyal_class_feature_iterator(os, environ_dict),
    vsf_pyal_class_feature_unary(os, environ_dict),
    vsf_pyal_class_feature_binary(os, environ_dict)
)
#       endif
#   endif

vsf_pyal_submodule(os, path,
    vsf_pyal_module_func(os_path, abspath),
    vsf_pyal_module_func(os_path, exists),
    vsf_pyal_module_func(os_path, isabs),
    vsf_pyal_module_func(os_path, isdir),
    vsf_pyal_module_func(os_path, isfile),
    vsf_pyal_module_func(os_path, join),
    vsf_pyal_module_func(os_path, basename),
    vsf_pyal_module_func(os_path, dirname),
    vsf_pyal_module_func(os_path, splitext),
    vsf_pyal_module_func(os_path, split),
)
vsf_pyal_module(os,
    vsf_pyal_module_func(os, __init__),
    vsf_pyal_module_func(os, __init_environ),
    vsf_pyal_module_func(os, stat),
    vsf_pyal_module_func(os, listdir),
    vsf_pyal_module_func(os, getcwd),
    vsf_pyal_module_func(os, mkdir),
    vsf_pyal_module_func(os, makedirs),
    vsf_pyal_module_func(os, rmdir),
    vsf_pyal_module_func(os, chdir),
    vsf_pyal_module_func(os, rename),
    vsf_pyal_module_func(os, remove),
    vsf_pyal_module_func(os, open),
    vsf_pyal_module_func(os, read),
    vsf_pyal_module_func(os, write),
    vsf_pyal_module_func(os, ioctl),
    vsf_pyal_module_func(os, close),
#if VSF_USE_MICROPYTHON == ENABLED
    vsf_pyal_module_func(os, qstr_hash),
#endif
    vsf_pyal_module_func(os, system),
    vsf_pyal_module_int(os, O_RDONLY, O_RDONLY),
    vsf_pyal_module_int(os, O_WRONLY, O_WRONLY),
    vsf_pyal_module_int(os, O_RDWR, O_RDWR),
    vsf_pyal_module_int(os, O_APPEND, O_APPEND),
    vsf_pyal_module_int(os, O_CREAT, O_CREAT),
    vsf_pyal_module_str(os, name, linux),
    vsf_pyal_module_str(os, sep, _slash_),
    vsf_pyal_module_inst(os, environ, __os_environ),
    vsf_pyal_module_submod(os, path),
)
#endif
