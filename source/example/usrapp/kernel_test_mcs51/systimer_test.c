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
#include <stdio.h>
/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
declare_vsf_pt(timer_task_a_t)
def_vsf_pt(timer_task_a_t,
    def_params(
        uint_fast16_t hwCount;
    )
)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/


/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

implement_vsf_pt(timer_task_a_t)
{
    vsf_pt_begin();

    while(1) {
        vsf_pt_wait_until(vsf_delay_ms(1000));
        printf("task a delay 1000ms [%04x]...\r\n", this.hwCount);
        this.hwCount++;
    }

    vsf_pt_end();
}

void usrapp_timer_test_start(void)
{
    do {
        static NO_INIT timer_task_a_t __task_a;
        init_vsf_pt(timer_task_a_t, &__task_a, vsf_prio_0);
    } while(0);
}
