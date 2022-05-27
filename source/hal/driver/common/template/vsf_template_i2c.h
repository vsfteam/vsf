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

#ifndef __HAL_DRIVER_I2C_INTERFACE_H__
#define __HAL_DRIVER_I2C_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_I2C_CFG_MULTI_CLASS
#   define VSF_I2C_CFG_MULTI_CLASS              DISABLED
#endif

// Turn off multi class support for the current implementation
// when the VSF_I2C_CFG_MULTI_CLASS is enabled
#ifndef VSF_I2C_CFG_IMPLEMENT_OP
#   if VSF_I2C_CFG_MULTI_CLASS == ENABLED
#       define VSF_I2C_CFG_IMPLEMENT_OP         ENABLED
#   else
#       define VSF_I2C_CFG_IMPLEMENT_OP         DISABLED
#   endif
#endif

// VSF_I2C_CFG_PREFIX: use for macro vsf_i2c_{init, enable, ...}
#ifndef VSF_I2C_CFG_PREFIX
#   if VSF_I2C_CFG_MULTI_CLASS == ENABLED
#       define VSF_I2C_CFG_PREFIX               vsf
#   elif defined(VSF_HW_I2C_COUNT) && (VSF_HW_I2C_COUNT != 0)
#       define VSF_I2C_CFG_PREFIX               vsf_hw
#   elif VSF_HAL_USE_GPIO_I2C == ENABLED
#       define VSF_I2C_CFG_PREFIX               vsf_gpio
#   endif
#endif

#ifndef VSF_I2C_CFG_FUNCTION_RENAME
#   define VSF_I2C_CFG_FUNCTION_RENAME          ENABLED
#endif

#ifndef VSF_I2C_CFG_REQUEST_TEMPLATE
#   define VSF_I2C_CFG_REQUEST_TEMPLATE         DISABLED
#endif

#ifndef VSF_I2C_CFG_REIMPLEMENT_FEATURE
#   define VSF_I2C_CFG_REIMPLEMENT_FEATURE      DISABLED
#endif

#ifndef VSF_I2C_CFG_REIMPLEMENT_IRQ_MASK
#   define VSF_I2C_CFG_REIMPLEMENT_IRQ_MASK     DISABLED
#endif

#ifndef VSF_I2C_CFG_REIMPLEMENT_STATUS
#   define VSF_I2C_CFG_REIMPLEMENT_STATUS       DISABLED
#endif

#ifndef VSF_I2C_CFG_REIMPLEMENT_CAPABILITY
#   define VSF_I2C_CFG_REIMPLEMENT_CAPABILITY   DISABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_I2C_APIS(__prefix_name)                                                                                                                        \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        i2c, init,           VSF_MCONNECT(__prefix_name, _i2c_t) *i2c_ptr, i2c_cfg_t *cfg_ptr)              \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,         i2c, enable,         VSF_MCONNECT(__prefix_name, _i2c_t) *i2c_ptr)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,         i2c, disable,        VSF_MCONNECT(__prefix_name, _i2c_t) *i2c_ptr)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             i2c, irq_enable,     VSF_MCONNECT(__prefix_name, _i2c_t) *i2c_ptr, em_i2c_irq_mask_t irq_mask)      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,             i2c, irq_disable,    VSF_MCONNECT(__prefix_name, _i2c_t) *i2c_ptr, em_i2c_irq_mask_t irq_mask)      \
    __VSF_HAL_TEMPLATE_API(__prefix_name, i2c_status_t,     i2c, status,         VSF_MCONNECT(__prefix_name, _i2c_t) *i2c_ptr)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, i2c_capability_t, i2c, capability,     VSF_MCONNECT(__prefix_name, _i2c_t) *i2c_ptr)                                  \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,        i2c, master_request, VSF_MCONNECT(__prefix_name, _i2c_t) *i2c_ptr, uint16_t address, em_i2c_cmd_t cmd, uint16_t count, uint8_t* buffer_ptr)

/*============================ TYPES =========================================*/

#if VSF_I2C_CFG_REIMPLEMENT_FEATURE == DISABLED
typedef enum em_i2c_feature_t {
    I2C_MODE_MASTER           = (0x1ul << 0),  // select master mode
    I2C_MODE_SLAVE            = (0x0ul << 0),  // select slave mode
    I2C_MODE_MASK             = (0x1ul << 0),

    // TODO: Ultra Fast-mode I2C-bus protocol
    I2C_SPEED_STANDARD_MODE   = (0x0ul << 1),  // up to 100 kbit/s
    I2C_SPEED_FAST_MODE       = (0x1ul << 1),  // up to 400 kbit/s
    I2C_SPEED_FAST_MODE_PLUS  = (0x2ul << 1),  // up to 1 Mbit/s
    I2C_SPEED_HIGH_SPEED_MODE = (0x3ul << 1),  // up to 3.4 Mbit/s
    I2C_SPEED_MASK            = (0x3ul << 1),

    I2C_ADDR_7_BITS           = (0x0ul << 3),
    I2C_ADDR_10_BITS          = (0x1ul << 3),
    I2C_ADDR_MASK             = (0x1ul << 3),

    I2C_FEATURE_MASK          =  I2C_MODE_MASK
                               | I2C_SPEED_MASK
                               | I2C_ADDR_MASK,
} em_i2c_feature_t;
#endif

/**
 \~english
 @brief flags of i2c tranfer messages
 @note em_i2c_cmd_t is implemented by specific driver.

 \~chinese
 @brief i2c 传输的标志
 @note em_i2c_cmd_t 由具体驱动实现。
 */
#if VSF_I2C_CFG_REIMPLEMENT_CMD == DISABLED
typedef enum em_i2c_cmd_t {
    I2C_CMD_WRITE      = (0x00ul << 0),
    I2C_CMD_READ       = (0x01ul << 0),
    I2C_CMD_RW_MASK    = (0x01ul << 1),

    I2C_CMD_START      = (0x01ul << 1),
    I2C_CMD_STOP       = (0x01ul << 1),
    I2C_CMD_RESTAR     = (0x01ul << 1),
    I2C_CMD_MASK       =  I2C_CMD_START
                        | I2C_CMD_STOP
                        | I2C_CMD_RESTAR,

    I2C_CMD_7_BITS     = (0x00ul << 2),
    I2C_CMD_10_BITS    = (0x01ul << 2),
    I2C_CMD_BITS_MASK  =  I2C_CMD_7_BITS
                        | I2C_CMD_10_BITS,

    I2C_CMD_ALL_MASK   = I2C_CMD_RW_MASK | I2C_CMD_MASK | I2C_CMD_BITS_MASK,
} em_i2c_cmd_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_IRQ_MASK == DISABLED
typedef enum em_i2c_irq_mask_t {
    I2C_IRQ_MASK_MASTER_STARTED                 = (0x1ul <<  0),
    I2C_IRQ_MASK_MASTER_ADDRESS_SEND            = (0x1ul <<  1),
    I2C_IRQ_MASK_MASTER_10_BITS_ADDRESS_SEND    = (0x1ul <<  2),
    I2C_IRQ_MASK_MASTER_STOP_DETECT             = (0x1ul <<  3),
    I2C_IRQ_MASK_MASTER_NACK_DETECT             = (0x1ul <<  4),
    I2C_IRQ_MASK_MASTER_ARBITRATION_LOST        = (0x1ul <<  5),
    I2C_IRQ_MASK_MASTER_TX_EMPTY                = (0x1ul <<  6),
    I2C_IRQ_MASK_MASTER_ERROR                   = (0x1ul <<  7),

    I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE       = (0x1ul <<  8),
    I2C_IRQ_MASK_MASTER_ADDRESS_NACK            = (0x1ul <<  9),

    I2C_IRQ_MASK_MASTER_ALL                     =  I2C_IRQ_MASK_MASTER_STARTED
                                                 | I2C_IRQ_MASK_MASTER_ADDRESS_SEND
                                                 | I2C_IRQ_MASK_MASTER_10_BITS_ADDRESS_SEND
                                                 | I2C_IRQ_MASK_MASTER_STOP_DETECT
                                                 | I2C_IRQ_MASK_MASTER_NACK_DETECT
                                                 | I2C_IRQ_MASK_MASTER_ARBITRATION_LOST
                                                 | I2C_IRQ_MASK_MASTER_TX_EMPTY
                                                 | I2C_IRQ_MASK_MASTER_ERROR
                                                 | I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                                                 | I2C_IRQ_MASK_MASTER_ADDRESS_NACK,
} em_i2c_irq_mask_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_STATUS == DISABLED
typedef struct i2c_status_t {
    union {
        inherit(peripheral_status_t)
        uint32_t value;
    };
} i2c_status_t;
#endif

#if VSF_I2C_CFG_REIMPLEMENT_CAPABILITY == DISABLED
typedef struct i2c_capability_t {
    inherit(peripheral_capability_t)
} i2c_capability_t;
#endif

typedef struct vsf_i2c_t vsf_i2c_t;

/**
 \~english
 @brief i2c interrupt callback function prototype.
 @note It is called after interrupt occurs.

 @param target_ptr pointer of user.
 @param i2c_ptr pointer of i2c instance.
 @param irq_mask one or more value of enum em_i2c_irq_mask_t
 @return None.

 \~chinese
 @brief i2c 中断回调函数原型
 @note 这个回调函数在中断发生之后被调用

 @param target_ptr 用户指针
 @param i2c_ptr i2c实例的指针
 @param irq_mask 一个或者多个枚举 em_i2c_irq_mask_t 的值的按位或
 @return 无。

 \~
 \code {.c}
    static void __user_i2c_irchandler(void *target_ptr, vsf_i2c_t *i2c_ptr, enum irq_mask)
    {
        if (irq_mask & I2C_IRQ_MASK_MASTER_STARTED) {
            // do something
        }

        if (irq_mask & I2C_IRQ_MASK_MASTER_STOP) {
            // do something
        }
    }
 \endcode
 */
typedef void vsf_i2c_isr_handler_t(void *target_ptr,
                                   vsf_i2c_t *i2c_ptr,
                                   em_i2c_irq_mask_t irq_mask);

/**
 \~english
 @brief i2c configuration

 \~chinese
 @brief i2c 中断配置
 */
typedef struct vsf_i2c_isr_t {
    vsf_i2c_isr_handler_t *handler_fn;        //!< \~english TODO
                                              //!< \~chinese 中断回调函数
    void                  *target_ptr;        //!< \~english pointer of user target
                                              //!< \~chinese 用户传入的指针
    vsf_arch_prio_t        prio;              //!< \~english interrupt priority
                                              //!< \~chinesh 中断优先级
} vsf_i2c_isr_t;

/**
 \~english
 @brief i2c configuration

 \~chinese
 @brief i2c 配置
 */
typedef struct i2c_cfg_t {
    em_i2c_feature_t mode;      //!< \~english i2c mode \ref em_i2c_feature_t
                                //!< \~chinese i2c 模式 \ref em_i2c_feature_t

    uint32_t clock_hz;          //!< \~english i2c clock (in Hz)
                                //!< \~chinese i2c 时钟频率 (单位：Hz)

    vsf_i2c_isr_t isr;          //!< \~english i2c interrupt
                                //!< \~chinese i2c 中断
} i2c_cfg_t;

typedef struct vsf_i2c_op_t {
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_I2C_APIS(vsf)
} vsf_i2c_op_t;

#if VSF_I2C_CFG_MULTI_CLASS == ENABLED
struct vsf_i2c_t  {
    const vsf_i2c_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @param[in] cfg_ptr: a pointer to structure @ref i2c_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if i2c was initialized, or a negative error code

 \~chinese
 @brief 初始化一个 i2c 实例
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @param[in] cfg_ptr: 结构体 i2c_cfg_t 的指针，参考 @ref i2c_cfg_t
 @return vsf_err_t: 如果 i2c 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_i2c_init(vsf_i2c_t *i2c_ptr, i2c_cfg_t *cfg_ptr);

/**
 \~english
 @brief finalize a i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @return none

 \~chinese
 @brief 结束一个 i2c 实例
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return none
 */
extern void vsf_i2c_fini(vsf_i2c_t *i2c_ptr);

/**
 \~english
 @brief enable i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @return fsm_rt_t: fsm_rt_cpl if enable complete, else return fsm_rt_onging

 \~chinese
 @brief 使能 i2c 实例
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return fsm_rt_t: 如果使能成功，返回 fsm_rt_cpl, 未完成初始化返回 fsm_rt_onging
 */
extern fsm_rt_t vsf_i2c_enable(vsf_i2c_t *i2c_ptr);

/**
 \~english
 @brief disable i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @return fsm_rt_t: fsm_rt_cpl if disable complete, else return fsm_rt_onging

 \~chinese
 @brief禁能 i2c 实例
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return fsm_rt_t: 如果禁能成功，返回 fsm_rt_cpl, 未完成初始化返回 fsm_rt_onging
 */
extern fsm_rt_t vsf_i2c_disable(vsf_i2c_t *i2c_ptr);

/**
 \~english
 @brief enable interrupt masks of i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @param[in] irq_mask: one or more value of enum em_i2c_irq_mask_t
 @return none.

 \~chinese
 @brief 使能 i2c 实例的中断
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return fsm_rt_t: 如果使能成功，返回 fsm_rt_cpl, 未完成初始化返回 fsm_rt_onging
 */
extern void vsf_i2c_irq_enable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @param[in] irq_mask: one or more value of enum em_i2c_irq_mask_t, @ref em_i2c_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 i2c 实例的中断
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @param[in] irq_mask: 一个或者多个枚举 em_i2c_irq_mask_t 的值的按位或，@ref em_i2c_irq_mask_t
 @return 无。
 */
extern void vsf_i2c_irq_disable(vsf_i2c_t *i2c_ptr, em_i2c_irq_mask_t irq_mask);

/**
 \~english
 @brief get the status of i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @return i2c_status_t: return all status of current i2c

 \~chinese
 @brief 获取 i2c 实例状态
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return i2c_status_t: 返回当前i2c的所有状态
 */
extern i2c_status_t vsf_i2c_status(vsf_i2c_t *i2c_ptr);

/**
 \~english
 @brief get the capability of i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @return i2c_status_t: return all capability of current i2c

 \~chinese
 @brief 获取 i2c 实例能力
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return i2c_status_t: 返回当前i2c的所有能力
 */
extern i2c_capability_t vsf_i2c_capability(vsf_i2c_t *i2c_ptr);

/**
 \~english
 @brief i2c instance as master mode request a transfer
 @note i2c_msg_ptr can be a local variable.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @param[in] address: address of i2c transfer
 @param[in] cmd: i2c cmd
 @param[in] count: i2c transfer buffer count (in byte)
 @param[in] buffer_ptr: i2c transfer buffer
 @return vsf_err_t: VSF_ERR_NONE if i2c was initialized, or a negative error code

 \~chinese
 @brief i2c主机请求一次传输
 @note i2c_msg_ptr可以是局部变量。
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @param[in] address: i2c 传输的地址
 @param[in] cmd: i2c 命令
 @param[in] count: i2c 传输缓冲区长度 (单位：字节)
 @param[in] buffer_ptr: i2c 传输缓冲区
 @return vsf_err_t: 如果 i2c 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_i2c_master_request(vsf_i2c_t *i2c_ptr,
                                        uint16_t address,
                                        em_i2c_cmd_t cmd,
                                        uint16_t count,
                                        uint8_t *buffer_ptr);

/*============================ INCLUDES ======================================*/

#include "hal/driver/common/i2c/i2c_request.h"

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_I2C_CFG_FUNCTION_RENAME == ENABLED
#   define vsf_i2c_init(__I2C, ...)                                             \
        VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_init)             ((VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_t) *)__I2C, ##__VA_ARGS__)
#   define vsf_i2c_enable(__I2C)                                                \
        VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_enable)           ((VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_t) *)__I2C)
#   define vsf_i2c_disable(__I2C)                                               \
        VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_disable)          ((VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_t) *)__I2C)
#   define vsf_i2c_irq_enable(__I2C, ...)                                       \
        VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_irq_enable)       ((VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_t) *)__I2C, ##__VA_ARGS__)
#   define vsf_i2c_irq_disable(__I2C, ...)                                      \
        VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_irq_disable)      ((VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_t) *)__I2C, ##__VA_ARGS__)
#   define vsf_i2c_status(__I2C)                                                \
        VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_status)           ((VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_t) *)__I2C)
#   define vsf_i2c_capability(__I2C)                                            \
        VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_capability)       ((VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_t) *)__I2C)
#   define vsf_i2c_cs_active(__I2C, ...)                                        \
        VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_cs_active)        ((VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_t) *)__I2C, ##__VA_ARGS__)
#   define vsf_i2c_master_request(__I2C, ...)                                   \
        VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_master_request)   ((VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_t) *)__I2C, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__HAL_DRIVER_I2C_INTERFACE_H__*/
