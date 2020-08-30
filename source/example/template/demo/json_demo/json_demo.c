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

#include "vsf.h"

#if APP_CFG_USE_JSON_DEMO == ENABLED && APP_CFG_USE_LINUX_DEMO == ENABLED

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static char __json[] = "\
{\
  \"Member\":\
  [\
    {\
      \"Name\":\"SimonQian\",\
      \"Age\":100,\
    },\
    {\
      \"Name\":\"Dino\",\
      \"Age\":200,\
    }\
  ]\
}";

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

int json_main(int argc, char *argv[])
{
    char *json_member, buffer[128];

    json_member = vsf_json_get(__json, "Member");
    if (json_member != NULL) {
        char id[2] = { '0', '\0' }, *json_persion;
        char *json_node;
        int len;
        double age;

        do {
            json_persion = vsf_json_get(json_member, id);
            if (json_persion != NULL) {
                vsf_trace(VSF_TRACE_INFO, "Member %s:" VSF_TRACE_CFG_LINEEND, id);
                json_node = vsf_json_get(json_persion, "Name");
                if (json_node != NULL) {
                    len = vsf_json_get_string(json_node, buffer, sizeof(buffer));
                    if (len > 0) {
                        buffer[len] = '\0';
                        vsf_trace(VSF_TRACE_INFO, "  Name: %s" VSF_TRACE_CFG_LINEEND, buffer);
                    }
                }
                json_node = vsf_json_get(json_persion, "Age");
                if (json_node != NULL) {
                    len = vsf_json_get_number(json_node, &age);
                    if (len > 0) {
                        vsf_trace(VSF_TRACE_INFO, "  Age: %d" VSF_TRACE_CFG_LINEEND, (int)age);
                    }
                }
            }
            id[0]++;
        } while (json_persion != NULL);
    }
    return 0;
}

#endif
