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

#ifndef __I_REG_PWM_H__
#define __I_REG_PWM_H__

/*============================ INCLUDES ======================================*/

/*! \note i_reg_xxxx.h header files must be standalong and assume following
 *!       conditions:
 *!       a. stdint.h exists
 *!       b. anonymouse structures and unions are supported
 */
#include "./utilities/compiler/__common/__type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef __AIC8800_PWM_USE_BIT_FIELD
#   define __AIC8800_PWM_USE_BIT_FIELD              DISABLED
#endif

/* Define structure member permissions : 'read only' */
#ifndef __IM
#   define __IM                                     const
#endif

/* Define structure member permissions : 'write only' */
#ifndef __OM
#   define __OM
#endif

/* Define structure member permissions : 'read or write' */
#ifndef __IOM
#   define __IOM
#endif

/* -------------------- tmr cfg register ------------------------------------ */

#define PWM_TMR_MODE                                0       /* 0x00000001 */
#define PWM_TMR_MODE_MASK                           ((reg32_t)(0x1 << PWM_TMR_MODE))

#define PWM_TMR_RUN                                 1       /* 0x00000002 */
#define PWM_TMR_RUN_MASK                            ((reg32_t)(0x1 << PWM_TMR_RUN))

/* -------------------- tmr int_ctrl register ------------------------------- */

#define PWM_TMR_INT_EN                              0       /* 0x00000001 */
#define PWM_TMR_INT_EN_MASK                         ((reg32_t)(0x1 << PWM_TMR_INT_EN))

#define PWM_TMR_INT_RAW                             1       /* 0x00000002 */
#define PWM_TMR_INT_RAW_MASK                        ((reg32_t)(0x1 << PWM_TMR_INT_RAW))

#define PWM_TMR_INT_STATUS                          2       /* 0x00000004 */
#define PWM_TMR_INT_STATUS_MASK                     ((reg32_t)(0x1 << PWM_TMR_INT_STATUS))

#define PWM_TMR_INT_CLR                             3       /* 0x00000008 */
#define PWM_TMR_INT_CLR_MASK                        ((reg32_t)(0x1 << PWM_TMR_INT_CLR))

/* -------------------- pwm_cfg --------------------------------------------- */

#define PWM_PWM_RUN                                 0
#define PWM_PWM_RUN_MASK                            ((reg32_t)(0x1 << PWM_PWM_RUN))

#define PWM_PWM_INC_MODE                            1
#define PWM_PWM_INC_MODE_MASK                       ((reg32_t)(0x1 << PWM_PWM_INC_MODE))

#define PWM_PWM_DEFAULT_VAL                         2
#define PWM_PWM_DEFAULT_VAL_MASK                    ((reg32_t)(0x1 << PWM_PWM_DEFAULT_VAL))

#define PWM_PWM_UPDATE                              31
#define PWM_PWM_UPDATE_MASK                         ((reg32_t)(0x1 << PWM_PWM_UPDATE))

/*============================ MACROFIED FUNCTIONS ===========================*/

#if __AIC8800_PWM_USE_BIT_FIELD == ENABLED
#   define DEF_PWM_REG(__NAME, __TOTAL_SIZE, ...)                               \
    union {                                                                     \
        reg##__TOTAL_SIZE##_t VALUE;                                            \
        struct {                                                                \
            __VA_ARGS__                                                         \
        };                                                                      \
    } __NAME
#else
#   define DEF_PWM_REG(__NAME, __TOTAL_SIZE, ...)                               \
        __VA_ARGS__ reg##__TOTAL_SIZE##_t __NAME
#endif

/*============================ TYPES =========================================*/

typedef struct __TMR_REG_T {
    DEF_PWM_REG(ld_value,       32, __IOM);
    REG_RSVD_U32
    DEF_PWM_REG(cnt_value,      32, __IOM);
    REG_RSVD_U32
    DEF_PWM_REG(cfg,            32, __IOM);
    DEF_PWM_REG(int_ctrl,       32, __IOM);
    DEF_PWM_REG(cnt_read,       32, __IOM);
    REG_RSVD_U32
} __TMR_REG_T;

typedef struct __PWM_REG_T {
    DEF_PWM_REG(sta_val,        32, __IOM);
    DEF_PWM_REG(end_val,        32, __IOM);
    DEF_PWM_REG(step_cfg0,      32, __IOM);
    DEF_PWM_REG(step_cfg1,      32, __IOM);
    DEF_PWM_REG(cfg,            32, __IOM);
    DEF_PWM_REG(int_ctrl,       32, __IOM);
    DEF_PWM_REG(end_val_use,    32, __IOM);
    DEF_PWM_REG(step_cnt,       32, __IOM);
} __PWM_REG_T;

typedef struct PWM_REG_T {
    __TMR_REG_T                 TMR[3];
    REG_RSVD_U32N(40)
    __PWM_REG_T                 PWM[6];
} PWM_REG_T;

#ifdef __cplusplus
}
#endif
#endif
/* EOF */
