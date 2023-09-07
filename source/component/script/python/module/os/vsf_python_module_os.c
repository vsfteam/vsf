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

#include "component/script/python/vsf_python.h"

#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

#if !VSF_PYAL_FEATURE_MODULE_IS_DYN
vsf_pyal_static_dict(__os_environ);
#endif

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void __vsf_pyal_os_environ_on_update(vsf_pyal_obj_t self, vsf_pyal_dict_evt_t evt,
        vsf_pyal_dict_key_t key, vsf_pyal_arg_t value)
{
    char *key_str = vsf_pyal_dictkey_get_str(key);
    char *value_str = vsf_pyal_strarg_get_str(value);
    switch (evt) {
    case VSF_PYAL_DICT_EVT_ON_UPDATE:
        setenv(key_str, value_str, true);
        break;
    case VSF_PYAL_DICT_EVT_ON_DELETE:
        unsetenv(key_str);
        break;
    }
}

vsf_pyal_module_func_init_imp(os)
{
    char **__env = environ, *key, *value;
    vsf_pyal_obj_t environ_dict;

#if VSF_PYAL_FEATURE_MODULE_IS_DYN
    vsf_pyal_module_add_str(os, "sep", "/");
    vsf_pyal_module_add_int(os, "O_RDONLY", O_RDONLY);
    vsf_pyal_module_add_int(os, "O_WRONLY", O_WRONLY);
    vsf_pyal_module_add_int(os, "O_RDWR", O_RDWR);
    vsf_pyal_module_add_int(os, "O_APPEND", O_APPEND);
    vsf_pyal_module_add_int(os, "O_CREAT", O_CREAT);

    environ_dict = vsf_pyal_newdict();
    vsf_pyal_module_add_obj(os, "environ", environ_dict);
#else
    environ_dict = &__os_environ;
#endif

    vsf_pyal_dictobj_set_on_update(environ_dict, NULL);
    while (*__env != NULL) {
        key = strdup(*__env);
        value = strstr(key, "=");
        if (value != NULL) {
            *value = '\0';
            value++;
        }

        vsf_pyal_arg_t arg = vsf_pyal_newarg_str(value);
        vsf_pyal_dictobj_set(environ_dict, key, arg, true);
        free(key);

        __env++;
    }
    vsf_pyal_dictobj_set_on_update(environ_dict, __vsf_pyal_os_environ_on_update);

    vsf_pyal_module_func_init_return();
}

vsf_pyal_module_func_var_imp(os, listdir, vsf_pyal_obj_t, 0, 1, vsf_pyal_funcarg_var(arg))
{
    int argc = vsf_pyal_funcarg_var_num(arg);
#if VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
    if (argc > 1) {
        vsf_pyal_raise("invalid argument, format: lisdir(*path)\n");
        return VSF_PYAL_OBJ_NULL;
    }
#endif
    char __path[PATH_MAX];
    char *str_path = argc == 0 ? getcwd(__path, sizeof(__path)) : vsf_pyal_funcarg_var_get_str(arg, 0);
    struct dirent* dp;
    DIR* dir = opendir(str_path);

    vsf_pyal_obj_t list = vsf_pyal_newlist(0, NULL);

    if (dir != NULL) {
        while ((dp = readdir(dir)) != NULL) {
            if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0) {
                vsf_pyal_arg_t arg = vsf_pyal_newarg_str(dp->d_name);
                vsf_pyal_listobj_append(list, arg, true);
            }
        }

        closedir(dir);
    }

    return list;
}

vsf_pyal_module_func_var_imp(os, mkdir, vsf_pyal_func_void_return_t, 1, 2, vsf_pyal_funcarg_var(arg))
{
    int argc = vsf_pyal_funcarg_var_num(arg);
#if VSF_PYAL_FEATURE_FUNCARG_NUM_CHECK
    if ((argc < 1) || (argc > 2)) {
        vsf_pyal_raise("invalid argument, format: mkdir(path, *mode)\n");
        return;
    }
#endif

    char *path_str = vsf_pyal_funcarg_var_get_str(arg, 0);
    int mode_int = 1 == argc ? 0511 : vsf_pyal_funcarg_var_get_int(arg, 1);
    mkdir((const char *)path_str, mode_int);
    vsf_pyal_func_void_return();
}

#ifdef vsf_pyal_module
vsf_pyal_module(os,
    vsf_pyal_module_func(os, __init__),
    vsf_pyal_module_func(os, listdir),
    vsf_pyal_module_func(os, mkdir),
    vsf_pyal_module_int(os, O_RDONLY, O_RDONLY),
    vsf_pyal_module_int(os, O_WRONLY, O_WRONLY),
    vsf_pyal_module_int(os, O_RDWR, O_RDWR),
    vsf_pyal_module_int(os, O_APPEND, O_APPEND),
    vsf_pyal_module_int(os, O_CREAT, O_CREAT),
    vsf_pyal_module_str(os, sep, _slash_),
    vsf_pyal_module_dict(os, environ, __os_environ),
)
#endif
