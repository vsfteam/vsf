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

#include "../../vsf_linux_cfg.h"

#if VSF_USE_LINUX == ENABLED

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../../include/simple_libc/string.h"
#   include "../../include/libgen.h"
#else
#   include <string.h>
#   include <libgen.h>
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

char * basename(char *path)
{
    if (!path || !*path) { return "."; }

    int pathlen = strlen(path);
    if (pathlen > 1 && path[pathlen - 1] == '/') {
        path[pathlen - 1] = '\0';
    }

    char *base = strrchr(path, '/');
    return base && base[1] ? &base[1] : path;
}

char * dirname(char *path)
{
    char *base = basename(path);
    VSF_LINUX_ASSERT(base != NULL);
    if (    (base[0] == '.' && base[1] == '\0')
        ||  (base[0] == '/' && base[1] == '\0')
        ||  (base[0] == '.' && base[1] == '.' && base[2] == '\0')) {
        return base;
    }
    if (base == path) {
        return ".";
    }
    base[-1] = '\0';
    return path;
}

#endif      // VSF_USE_LINUX
