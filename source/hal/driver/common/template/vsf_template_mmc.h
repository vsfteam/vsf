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

#ifndef __HAL_DRIVER_MMC_INTERFACE_H__
#define __HAL_DRIVER_MMC_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// application code can redefine it
#ifndef VSF_MMC_CFG_PREFIX
#   if defined(VSF_HW_MMC_COUNT) && (VSF_HW_MMC_COUNT != 0)
#       define VSF_MMC_CFG_PREFIX               vsf_hw
#   else
#       define VSF_MMC_CFG_PREFIX               vsf
#   endif
#endif

// multi-class support enabled by default for maximum availability.
#ifndef VSF_MMC_CFG_MULTI_CLASS
#   define VSF_MMC_CFG_MULTI_CLASS              ENABLED
#endif

#ifndef VSF_MMC_CFG_FUNCTION_RENAME
#   define VSF_MMC_CFG_FUNCTION_RENAME          ENABLED
#endif

#ifndef VSF_MMC_CFG_REIMPLEMENT_FEATURE
#   define VSF_MMC_CFG_REIMPLEMENT_FEATURE      DISABLED
#endif

#ifndef VSF_MMC_CFG_REIMPLEMENT_IRQ_MASK
#   define VSF_MMC_CFG_REIMPLEMENT_IRQ_MASK     DISABLED
#endif

#ifndef VSF_MMC_CFG_REIMPLEMENT_STATUS
#   define VSF_MMC_CFG_REIMPLEMENT_STATUS       DISABLED
#endif

#ifndef VSF_MMC_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_MMC_CFG_REIMPLEMENT_CAPABILITY   DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_MMC_APIS(__prefix)                                                                                                                                  \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,             mmc, init,                  VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, vsf_mmc_cfg_t *cfg_ptr)        \
    __VSF_HAL_TEMPLATE_API(__prefix, fsm_rt_t,              mmc, enable,                VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr)                                \
    __VSF_HAL_TEMPLATE_API(__prefix, fsm_rt_t,              mmc, disable,               VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr)                                \
    __VSF_HAL_TEMPLATE_API(__prefix, void,                  mmc, irq_enable,            VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)   \
    __VSF_HAL_TEMPLATE_API(__prefix, void,                  mmc, irq_disable,           VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, vsf_mmc_irq_mask_t irq_mask)   \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_mmc_status_t,      mmc, status,                VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr)                                \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_mmc_capability_t,  mmc, capability,            VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr)                                \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,             mmc, host_set_clock,        VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, uint32_t clock_hz)             \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,             mmc, host_set_buswidth,     VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, uint8_t buswidth)              \
    __VSF_HAL_TEMPLATE_API(__prefix, vsf_err_t,             mmc, host_transact_start,   VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr, vsf_mmc_trans_t *trans)        \
    __VSF_HAL_TEMPLATE_API(__prefix, void,                  mmc, host_transact_stop,    VSF_MCONNECT(__prefix, _mmc_t) *mmc_ptr)

/*============================ TYPES =========================================*/

#if VSF_MMC_CFG_REIMPLEMENT_FEATURE == DISABLED
typedef enum vsf_mmc_feature_t {
    MMC_MODE_HOST               = (0x1ul << 0), // select host mode
    MMC_MODE_SLAVE              = (0x0ul << 0), // select slave mode
    MMC_MODE_MASK               = (0x1ul << 0),
} vsf_mmc_feature_t;
#endif

/**
 \~english
 @brief flags of mmc tranfer operations
 @note vsf_mmc_transop_t is implemented by specific driver.

 \~chinese
 @brief mmc 传输操作的标志
 @note vsf_mmc_transop_t 由具体驱动实现。
 */
#if VSF_MMC_CFG_REIMPLEMENT_CMDOP == DISABLED
typedef enum vsf_mmc_transop_t {
    MMC_CMDOP_AUTOCMD12         = (1ul << 0),
    MMC_CMDOP_MULTIBLOCK        = (1ul << 1),
    MMC_CMDOP_WRITE             = (1ul << 2),
    MMC_CMDOP_RW                = (1ul << 3),
    MMC_CMDOP_RESP              = (1ul << 4),
    MMC_CMDOP_RESP_SHORT        = (1ul << 5),
    MMC_CMDOP_RESP_SHORT_CRC    = (2ul << 5),
    MMC_CMDOP_RESP_LONG_CRC     = (3ul << 5),
} vsf_mmc_transop_t;
#endif

typedef struct vsf_mmc_trans_t {
    uint8_t cmd;
    uint32_t arg;
    uint8_t *resp;
    vsf_mmc_transop_t op;

    uint8_t *buffer;
    uint32_t count;
} vsf_mmc_trans_t;

#if VSF_MMC_CFG_REIMPLEMENT_IRQ_MASK == DISABLED
typedef enum vsf_mmc_irq_mask_t {
    MMC_IRQ_MASK_HOST_TRANSACT_DONE             = (0x1ul <<  0),
    MMC_IRQ_MASK_MASTER_ALL                     =  MMC_IRQ_MASK_HOST_TRANSACT_DONE,
} vsf_mmc_irq_mask_t;
#endif

#if VSF_MMC_CFG_REIMPLEMENT_TRANSACT_RESULT == DISABLED
typedef enum vsf_mmc_transact_result_t {
    MMC_TRANSACT_RESULT_DONE                    = 0,
    MMC_TRANSACT_RESULT_ERR_RESP_NONE           = (0x1ul <<  0),
    MMC_TRANSACT_RESULT_ERR_RESP_CRC            = (0x1ul <<  1),
    MMC_TRANSACT_RESULT_ERR_DATA_CRC            = (0x1ul <<  2),
    MMC_TRANSACT_RESULT_ERR_TRANSACT            = (0x1ul <<  3),
    MMC_TRANSACT_RESULT_ERR_MASK                = MMC_TRANSACT_RESULT_ERR_RESP_NONE
                                                | MMC_TRANSACT_RESULT_ERR_RESP_CRC
                                                | MMC_TRANSACT_RESULT_ERR_DATA_CRC
                                                | MMC_TRANSACT_RESULT_ERR_TRANSACT,
} vsf_mmc_transact_result_t;
#endif

#if VSF_MMC_CFG_REIMPLEMENT_STATUS == DISABLED
typedef struct vsf_mmc_status_t {
    union {
        inherit(peripheral_status_t)
        vsf_mmc_transact_result_t value;
    };
} vsf_mmc_status_t;
#endif

#if VSF_MMC_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct vsf_mmc_capability_t {
    inherit(vsf_peripheral_capability_t)
} vsf_mmc_capability_t;
#endif

typedef struct vsf_mmc_t vsf_mmc_t;

/**
 \~english
 @brief mmc interrupt callback function prototype.
 @note It is called after interrupt occurs.

 @param target_ptr pointer of user.
 @param mmc_ptr pointer of mmc instance.
 @param irq_mask one or more value of enum vsf_mmc_irq_mask_t
 @return None.

 \~chinese
 @brief mmc 中断回调函数原型
 @note 这个回调函数在中断发生之后被调用

 @param target_ptr 用户指针
 @param mmc_ptr mmc实例的指针
 @param irq_mask 一个或者多个枚举 vsf_mmc_irq_mask_t 的值的按位或
 @return 无。

 \~
 \code {.c}
    static void __user_mmc_irchandler(void *target_ptr, vsf_mmc_t *mmc_ptr, enum irq_mask)
    {
        if (irq_mask & MMC_IRQ_MASK_HOST_TRANSACT_DONE) {
            // do something
        }
    }
 \endcode
 */
typedef void vsf_mmc_isr_handler_t(void *target_ptr,
                                   vsf_mmc_t *mmc_ptr,
                                   vsf_mmc_irq_mask_t irq_mask);

/**
 \~english
 @brief mmc interrupt configuration

 \~chinese
 @brief mmc 中断配置
 */
typedef struct vsf_mmc_isr_t {
    vsf_mmc_isr_handler_t *handler_fn;          //!< \~english TODO
                                                //!< \~chinese 中断回调函数
    void                  *target_ptr;          //!< \~english pointer of user target
                                                //!< \~chinese 用户传入的指针
    vsf_arch_prio_t        prio;                //!< \~english interrupt priority
                                                //!< \~chinesh 中断优先级
} vsf_mmc_isr_t;

/**
 \~english
 @brief mmc configuration

 \~chinese
 @brief mmc 配置
 */
typedef struct vsf_mmc_cfg_t {
    vsf_mmc_feature_t mode;                     //!< \~english mmc mode \ref vsf_mmc_feature_t
                                                //!< \~chinese mmc 模式 \ref vsf_mmc_feature_t

    uint32_t clock_hz;                          //!< \~english mmc clock (in Hz)
                                                //!< \~chinese mmc 时钟频率 (单位：Hz)

    vsf_mmc_isr_t isr;                          //!< \~english mmc interrupt
                                                //!< \~chinese mmc 中断
} vsf_mmc_cfg_t;

typedef struct vsf_mmc_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_MMC_APIS(vsf)
} vsf_mmc_op_t;

#if VSF_MMC_CFG_MULTI_CLASS == ENABLED
struct vsf_mmc_t  {
    const vsf_mmc_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_mmc_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if mmc was initialized, or a negative error code

 \~chinese
 @brief 初始化一个 mmc 实例
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] cfg_ptr: 结构体 vsf_mmc_cfg_t 的指针，参考 @ref vsf_mmc_cfg_t
 @return vsf_err_t: 如果 mmc 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_mmc_init(vsf_mmc_t *mmc_ptr, vsf_mmc_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @return none

 \~chinese
 @brief 结束一个 mmc 实例
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @return none
 */
extern void vsf_mmc_fini(vsf_mmc_t *mmc_ptr);

/**
 \~english
 @brief enable mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @return fsm_rt_t: fsm_rt_cpl if enable complete, else return fsm_rt_onging

 \~chinese
 @brief 使能 mmc 实例
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @return fsm_rt_t: 如果使能成功，返回 fsm_rt_cpl, 未完成初始化返回 fsm_rt_onging
 */
extern fsm_rt_t vsf_mmc_enable(vsf_mmc_t *mmc_ptr);

/**
 \~english
 @brief disable mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @return fsm_rt_t: fsm_rt_cpl if disable complete, else return fsm_rt_onging

 \~chinese
 @brief禁能 mmc 实例
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @return fsm_rt_t: 如果禁能成功，返回 fsm_rt_cpl, 未完成初始化返回 fsm_rt_onging
 */
extern fsm_rt_t vsf_mmc_disable(vsf_mmc_t *mmc_ptr);

/**
 \~english
 @brief enable interrupt masks of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] irq_mask: one or more value of enum vsf_mmc_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 mmc 实例的中断
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @return fsm_rt_t: 如果使能成功，返回 fsm_rt_cpl, 未完成初始化返回 fsm_rt_onging
 */
extern void vsf_mmc_irq_enable(vsf_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] irq_mask: one or more value of enum vsf_mmc_irq_mask_t, @ref vsf_mmc_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 mmc 实例的中断
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_mmc_irq_mask_t 的值的按位或，@ref vsf_mmc_irq_mask_t
 @return 无。
 */
extern void vsf_mmc_irq_disable(vsf_mmc_t *mmc_ptr, vsf_mmc_irq_mask_t irq_mask);

/**
 \~english
 @brief get the status of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @return vsf_mmc_status_t: return all status of current mmc

 \~chinese
 @brief 获取 mmc 实例状态
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @return vsf_mmc_status_t: 返回当前mmc的所有状态
 */
extern vsf_mmc_status_t vsf_mmc_status(vsf_mmc_t *mmc_ptr);

/**
 \~english
 @brief get the capability of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @return vsf_mmc_capability_t: return all capability of current mmc @ref vsf_mmc_capability_t

 \~chinese
 @brief 获取 mmc 实例能力
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @return vsf_mmc_capability_t: 返回当前mmc的所有能力 @ref vsf_mmc_capability_t
 */
extern vsf_mmc_capability_t vsf_mmc_capability(vsf_mmc_t *mmc_ptr);

/**
 \~english
 @brief set the clock of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] clock_hz: clock in Hz
 @return vsf_err_t: VSF_ERR_NONE if mmc was initialized, or a negative error code

 \~chinese
 @brief 设置 mmc 时钟
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] clock_hz: 时钟速度 (单位：赫兹)
 @return vsf_err_t: 如果 mmc 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_mmc_host_set_clock(vsf_mmc_t *mmc_ptr, uint32_t clock_hz);

/**
 \~english
 @brief set the bus width of mmc instance.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] buswidth: bus width in 1, 4, 8
 @return vsf_err_t: VSF_ERR_NONE if mmc was initialized, or a negative error code

 \~chinese
 @brief 设置 mmc 总线位宽
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] buswidth: 总线位宽，范围：1, 4, 8
 @return vsf_err_t: 如果 mmc 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_mmc_host_set_buswidth(vsf_mmc_t *mmc_ptr, uint8_t buswidth);

/**
 \~english
 @brief start mmc transaction.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] trans: a pointer to mmc transaction structure
 @return vsf_err_t: VSF_ERR_NONE if mmc was initialized, or a negative error code

 \~chinese
 @brief 启动 mmc 传输
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] trans: mmc 传输结构指针
 @return vsf_err_t: 如果 mmc 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_mmc_host_transact_start(vsf_mmc_t *mmc_ptr, vsf_mmc_trans_t *trans);

/**
 \~english
 @brief stop mmc transaction if not done.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @return none.

 \~chinese
 @brief mmc 传输未完成时，停止 mmc 传输
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @return 无。
 */
extern void vsf_mmc_host_transact_stop(vsf_mmc_t *mmc_ptr);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_MMC_CFG_FUNCTION_RENAME == ENABLED
#   define vsf_mmc_init(__MMC, ...)                                             \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_init)             ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_enable(__MMC)                                                \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_enable)           ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC)
#   define vsf_mmc_disable(__MMC)                                               \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_disable)          ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC)
#   define vsf_mmc_irq_enable(__MMC, ...)                                       \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_irq_enable)       ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_irq_disable(__MMC, ...)                                      \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_irq_disable)      ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_status(__MMC)                                                \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_status)           ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC)
#   define vsf_mmc_capability(__MMC)                                            \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_capability)       ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC)
#   define vsf_mmc_host_set_clock(__MMC, ...)                                   \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_host_set_clock)   ((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_host_set_buswidth(__MMC, ...)                                \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_host_set_buswidth)((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_host_transact_start(__MMC, ...)                              \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_host_transact_start)((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#   define vsf_mmc_host_transact_stop(__MMC, ...)                               \
        VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_host_transact_stop)((VSF_MCONNECT(VSF_MMC_CFG_PREFIX, _mmc_t) *)__MMC, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_MMC_INTERFACE_H__*/
