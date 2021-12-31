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

#ifdef __cplusplus
extern "C" {
#endif
/*============================ MACROS ========================================*/

#ifndef I2C_TEMPLATE_USE_MODULAR_NAME
#   error "I2C_TEMPLATE_USE_MODULAR_NAME no defineded"
#endif

#define ____i2c_instance(__N, __NAME)               vsf_##__NAME##_i2c##__N
#define __i2c_instance(__N, __NAME)                 ____i2c_instance(__N, __NAME)
#define i2c_instance(__N)                           __i2c_instance(__N, I2C_TEMPLATE_USE_MODULAR_NAME)

#undef i2c_driver_init
#undef i2c_driver_fini
#undef i2c_driver_enable
#undef i2c_driver_disable
#undef i2c_driver_irq_enable
#undef i2c_driver_irq_disable
#undef i2c_driver_status
#undef i2c_driver_master_request
#undef i2c_type_ptr
#undef i2c_type_status
#undef i2c_type_isrhandler

#define ____i2c_type_ptr(__NAME)                    vsf_##__NAME##_i2c_t
#define ____i2c_type_status(__NAME)                 vsf_##__NAME##_i2c_status_t
#define ____i2c_type_isrhandler(__NAME)             vsf_##__NAME##_i2c_isrhandler_t
#define ____i2c_driver_init(__NAME)                 vsf_##__NAME##_i2c_init
#define ____i2c_driver_fini(__NAME)                 vsf_##__NAME##_i2c_fini
#define ____i2c_driver_enable(__NAME)               vsf_##__NAME##_i2c_enable
#define ____i2c_driver_disable(__NAME)              vsf_##__NAME##_i2c_disable
#define ____i2c_driver_irq_enable(__NAME)           vsf_##__NAME##_i2c_irq_enable
#define ____i2c_driver_irq_disable(__NAME)          vsf_##__NAME##_i2c_irq_disable
#define ____i2c_driver_status(__NAME)               vsf_##__NAME##_i2c_status
#define ____i2c_driver_master_request(__NAME)       vsf_##__NAME##_i2c_master_request

#define __i2c_type_ptr(__NAME)                      ____i2c_type_ptr(__NAME)
#define __i2c_type_status(__NAME)                   ____i2c_type_status(__NAME)
#define __i2c_type_isrhandler(__NAME)               ____i2c_type_isrhandler(__NAME)
#define __i2c_driver_init(__NAME)                   ____i2c_driver_init(__NAME)
#define __i2c_driver_fini(__NAME)                   ____i2c_driver_fini(__NAME)
#define __i2c_driver_enable(__NAME)                 ____i2c_driver_enable(__NAME)
#define __i2c_driver_disable(__NAME)                ____i2c_driver_disable(__NAME)
#define __i2c_driver_irq_enable(__NAME)             ____i2c_driver_irq_enable(__NAME)
#define __i2c_driver_irq_disable(__NAME)            ____i2c_driver_irq_disable(__NAME)
#define __i2c_driver_status(__NAME)                 ____i2c_driver_status(__NAME)
#define __i2c_driver_master_request(__NAME)         ____i2c_driver_master_request(__NAME)

#define i2c_type_ptr                                __i2c_type_ptr(I2C_TEMPLATE_USE_MODULAR_NAME)
#define i2c_type_status                             __i2c_type_status(I2C_TEMPLATE_USE_MODULAR_NAME)
#define i2c_type_isrhandler                         __i2c_type_isrhandler(I2C_TEMPLATE_USE_MODULAR_NAME)
#define i2c_driver_init                             __i2c_driver_init(I2C_TEMPLATE_USE_MODULAR_NAME)
#define i2c_driver_fini                             __i2c_driver_fini(I2C_TEMPLATE_USE_MODULAR_NAME)
#define i2c_driver_enable                           __i2c_driver_enable(I2C_TEMPLATE_USE_MODULAR_NAME)
#define i2c_driver_disable                          __i2c_driver_disable(I2C_TEMPLATE_USE_MODULAR_NAME)
#define i2c_driver_irq_enable                       __i2c_driver_irq_enable(I2C_TEMPLATE_USE_MODULAR_NAME)
#define i2c_driver_irq_disable                      __i2c_driver_irq_disable(I2C_TEMPLATE_USE_MODULAR_NAME)
#define i2c_driver_status                           __i2c_driver_status(I2C_TEMPLATE_USE_MODULAR_NAME)
#define i2c_driver_master_request                   __i2c_driver_master_request(I2C_TEMPLATE_USE_MODULAR_NAME)

/*============================ MACROFIED FUNCTIONS ===========================*/

#if I2C_TEMPLATE_COUNT > 1
#   define vsf_i2c_t                                void
#   define ____VSF_I2C_INTTERFACE_BODY(__NAME)                                  \
    /* I2C INIT */                                                              \
        vsf_err_t vsf_i2c_init(vsf_i2c_t *i2c_ptr,                              \
                                          i2c_cfg_t *cfg_ptr) {                 \
            return ((vsf_##__NAME##_i2c_t *)i2c_ptr)->Init(((vsf_##__NAME##_i2c_t *)i2c_ptr), cfg_ptr);\
        }                                                                       \
    /* I2C FINI */                                                              \
        void vsf_i2c_fini(vsf_i2c_t *i2c_ptr) {                                 \
            ((vsf_##__NAME##_i2c_t *)i2c_ptr)->Fini(((vsf_##__NAME##_i2c_t *)i2c_ptr));\
        }                                                                       \
    /* I2C ENABLED */                                                           \
        fsm_rt_t vsf_i2c_enable(vsf_i2c_t *i2c_ptr) {                           \
            return ((vsf_##__NAME##_i2c_t *)i2c_ptr)->Enable(((vsf_##__NAME##_i2c_t *)i2c_ptr));\
        }                                                                       \
    /* I2C DISABLED */                                                          \
        fsm_rt_t vsf_i2c_disable(vsf_i2c_t *i2c_ptr) {                          \
            return ((vsf_##__NAME##_i2c_t *)i2c_ptr)->Disable(((vsf_##__NAME##_i2c_t *)i2c_ptr));\
        }                                                                       \
    /* I2C IRQ ENABLED */                                                       \
        void vsf_i2c_irq_enable(vsf_i2c_t *i2c_ptr,                             \
                                em_i2c_irq_mask_t irq_mask) {                   \
            ((vsf_##__NAME##_i2c_t *)i2c_ptr)->Irq_Enable(((vsf_##__NAME##_i2c_t *)i2c_ptr),\
                                 irq_mask);                                     \
        }                                                                       \
    /* I2C IRQ DISABLED */                                                      \
        void vsf_i2c_irq_disable(vsf_i2c_t *i2c_ptr,                            \
                                      em_i2c_irq_mask_t irq_mask) {             \
            ((vsf_##__NAME##_i2c_t *)i2c_ptr)->Irq_Disable(((vsf_##__NAME##_i2c_t *)i2c_ptr),\
                                        irq_mask);                              \
        }                                                                       \
    /* I2C STATUS */                                                            \
        i2c_type_status vsf_i2c_status(vsf_i2c_t *i2c_ptr) {                    \
            return ((vsf_##__NAME##_i2c_t *)i2c_ptr)->Status(((vsf_##__NAME##_i2c_t *)i2c_ptr));\
        }                                                                       \
    /* I2C MASTER REQUEST */                                                    \
        vsf_err_t vsf_i2c_master_request(vsf_i2c_t *i2c_ptr,                    \
                                             uint16_t address,                  \
                                             em_i2c_cmd_t cmd,                  \
                                             uint16_t count,                    \
                                             uint8_t *buffer_ptr){              \
            return ((vsf_##__NAME##_i2c_t *)i2c_ptr)->Master_Request(((vsf_##__NAME##_i2c_t *)i2c_ptr),\
                                                                    address,    \
                                                                    cmd,        \
                                                                    count,      \
                                                                    buffer_ptr);\
        }
#   define ____VSF_I2C_INTTERFACE_TYPE_DEFINE(__NAME)                           \
    /* I2C INIT */                                                              \
        vsf_err_t       (*Init)             (vsf_##__NAME##_i2c_t *i2c_ptr,     \
                                             i2c_cfg_t *cfg_ptr);               \
    /* I2C FINI */                                                              \
        void            (*Fini)             (vsf_##__NAME##_i2c_t *i2c_ptr);    \
    /* I2C EENABLED */                                                          \
        fsm_rt_t        (*Enable)          (vsf_##__NAME##_i2c_t *i2c_ptr);     \
    /* I2C DISABLED */                                                          \
        fsm_rt_t        (*Disable)         (vsf_##__NAME##_i2c_t *i2c_ptr);     \
    /* I2C IRQ ENABLED */                                                       \
        void            (*Irq_Enable)      (vsf_##__NAME##_i2c_t *i2c_ptr,      \
                                             em_i2c_irq_mask_t irq_mask);       \
    /* I2C IRQ DISABLED */                                                      \
        void            (*Irq_Disable)     (vsf_##__NAME##_i2c_t *i2c_ptr,      \
                                             em_i2c_irq_mask_t irq_mask);       \
    /* I2C STATUS */                                                            \
        i2c_type_status (*Status)           (vsf_##__NAME##_i2c_t *i2c_ptr);    \
    /* I2C MASTER REQUEST */                                                    \
        vsf_err_t       (*Master_Request)   (vsf_##__NAME##_i2c_t *i2c_ptr,     \
                                             uint16_t address,                  \
                                             em_i2c_cmd_t cmd,                  \
                                             uint16_t count,                    \
                                             uint8_t *buffer_ptr);

#   define ____VSF_I2C_LV1_INTERFACE_INIT(__NAME)                               \
    .Init               = i2c_driver_init,                                      \
    .Fini               = i2c_driver_fini,                                      \
    .Enable             = i2c_driver_enable,                                    \
    .Disable            = i2c_driver_disable,                                   \
    .Irq_Enable         = i2c_driver_irq_enable,                                \
    .Irq_Disable        = i2c_driver_irq_disable,                               \
    .Status             = i2c_driver_status,                                    \
    .Master_Request     = i2c_driver_master_request,

#else
#   define ____VSF_I2C_INTTERFACE_BODY(__NAME)
#   define ____VSF_I2C_INTTERFACE_TYPE_DEFINE(__NAME)
#   define ____VSF_I2C_LV1_INTERFACE_INIT(__NAME)
#   define vsf_i2c_t               i2c_type_ptr
#   define vsf_i2c_init            i2c_driver_init
#   define vsf_i2c_fini            i2c_driver_fini
#   define vsf_i2c_enable          i2c_driver_enable
#   define vsf_i2c_disable         i2c_driver_disable
#   define vsf_i2c_irq_enable      i2c_driver_irq_enable
#   define vsf_i2c_irq_disable     i2c_driver_irq_disable
#   define vsf_i2c_status          i2c_driver_status
#   define vsf_i2c_master_request  i2c_driver_master_request
#endif

#define __VSF_I2C_INTTERFACE_BODY(__NAME)                                       \
    ____VSF_I2C_INTTERFACE_BODY(__NAME)

#define __VSF_I2C_INTTERFACE_TYPE_DEFINE(__NAME)                                \
    ____VSF_I2C_INTTERFACE_TYPE_DEFINE(__NAME)

#define __VSF_I2C_LV1_INTERFACE_INIT(__NAME)                                    \
    ____VSF_I2C_LV1_INTERFACE_INIT(__NAME)

#define VSF_I2C_INTTERFACE_BODY                                                 \
    __VSF_I2C_INTTERFACE_BODY(I2C_TEMPLATE_USE_MODULAR_NAME)

#define VSF_I2C_INTTERFACE_TYPE_DEFINE                                          \
    __VSF_I2C_INTTERFACE_TYPE_DEFINE(I2C_TEMPLATE_USE_MODULAR_NAME)

#define VSF_I2C_LV1_INTERFACE_INIT                                              \
    __VSF_I2C_LV1_INTERFACE_INIT(I2C_TEMPLATE_USE_MODULAR_NAME)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

