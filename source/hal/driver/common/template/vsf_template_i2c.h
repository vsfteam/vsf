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

#ifndef __VSF_TEMPLATE_I2C_H__
#define __VSF_TEMPLATE_I2C_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_I2C_CFG_MULTI_CLASS
#   define VSF_I2C_CFG_MULTI_CLASS                  ENABLED
#endif

#if defined(VSF_HW_I2C_COUNT) && !defined(VSF_HW_I2C_MASK)
#   define VSF_HW_I2C_MASK                          VSF_HAL_COUNT_TO_MASK(VSF_HW_I2C_COUNT)
#endif

#if defined(VSF_HW_I2C_MASK) && !defined(VSF_HW_I2C_COUNT)
#   define VSF_HW_I2C_COUNT                         VSF_HAL_MASK_TO_COUNT(VSF_HW_I2C_MASK)
#endif

// application code can redefine it
#ifndef VSF_I2C_CFG_PREFIX
#   if VSF_I2C_CFG_MULTI_CLASS == ENABLED
#       define VSF_I2C_CFG_PREFIX                   vsf
#   elif defined(VSF_HW_I2C_COUNT) && (VSF_HW_I2C_COUNT != 0)
#       define VSF_I2C_CFG_PREFIX                   vsf_hw
#   else
#       define VSF_I2C_CFG_PREFIX                   vsf
#   endif
#endif

#ifndef VSF_I2C_CFG_FUNCTION_RENAME
#   define VSF_I2C_CFG_FUNCTION_RENAME              ENABLED
#endif

#ifndef VSF_I2C_CFG_REQUEST_TEMPLATE
#   define VSF_I2C_CFG_REQUEST_TEMPLATE             DISABLED
#endif

#ifndef VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE        DISABLED
#endif

#ifndef VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD
#   define VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD         DISABLED
#endif

#ifndef VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK    DISABLED
#endif

#ifndef VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS      DISABLED
#endif

#ifndef VSF_I2C_CFG_INHERT_HAL_CAPABILITY
#   define VSF_I2C_CFG_INHERT_HAL_CAPABILITY       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_I2C_APIS(__PREFIX_NAME)                                                                                                                                    \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            i2c, init,                  VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, vsf_i2c_cfg_t *cfg_ptr)      \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 i2c, fini,                  VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, fsm_rt_t,             i2c, enable,                VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, fsm_rt_t,             i2c, disable,               VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 i2c, irq_enable,            VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, vsf_i2c_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, void,                 i2c, irq_disable,           VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, vsf_i2c_irq_mask_t irq_mask) \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_i2c_status_t,     i2c, status,                VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_i2c_capability_t, i2c, capability,            VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, uint_fast32_t,        i2c, get_transferred_count, VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr)                              \
    __VSF_HAL_TEMPLATE_API(__PREFIX_NAME, vsf_err_t,            i2c, master_request,        VSF_MCONNECT(__PREFIX_NAME, _i2c_t) *i2c_ptr, uint16_t address, vsf_i2c_cmd_t cmd, uint16_t count, uint8_t* buffer_ptr)

/*============================ TYPES =========================================*/

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
typedef enum vsf_i2c_mode_t {
    VSF_I2C_MODE_MASTER           = (0x1ul << 28),  // select master mode
    VSF_I2C_MODE_SLAVE            = (0x0ul << 28),  // select slave mode

    // TODO: Ultra Fast-mode I2C-bus protocol
    VSF_I2C_SPEED_STANDARD_MODE   = (0x0ul << 29),  // up to 100 kbit/s
    VSF_I2C_SPEED_FAST_MODE       = (0x1ul << 29),  // up to 400 kbit/s
    VSF_I2C_SPEED_FAST_MODE_PLUS  = (0x2ul << 29),  // up to 1 Mbit/s
    VSF_I2C_SPEED_HIGH_SPEED_MODE = (0x3ul << 29),  // up to 3.4 Mbit/s

    VSF_I2C_ADDR_7_BITS           = (0x0ul << 31),
    VSF_I2C_ADDR_10_BITS          = (0x1ul << 31),
} vsf_i2c_mode_t;
#endif

enum {
    VSF_I2C_MODE_COUNT            = 2,
    VSF_I2C_MODE_MASK             = VSF_I2C_MODE_MASTER |
                                    VSF_I2C_MODE_SLAVE,

    VSF_I2C_SPEED_COUNT           = 4,
    VSF_I2C_SPEED_MASK            = VSF_I2C_SPEED_STANDARD_MODE |
                                    VSF_I2C_SPEED_FAST_MODE |
                                    VSF_I2C_SPEED_FAST_MODE_PLUS |
                                    VSF_I2C_SPEED_HIGH_SPEED_MODE,

    VSF_I2C_ADDR_COUNT            = 3,
    VSF_I2C_ADDR_MASK             = VSF_I2C_ADDR_7_BITS |
                                    VSF_I2C_ADDR_10_BITS,

    VSF_I2C_MODE_MASK_COUNT       = 3,
    VSF_I2C_MODE_ALL_BITS_MASK    = VSF_I2C_MODE_MASK |
                                    VSF_I2C_SPEED_MASK |
                                    VSF_I2C_ADDR_MASK,
};

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_CMD == DISABLED
typedef enum vsf_i2c_cmd_t {
    VSF_I2C_CMD_WRITE           = (0x00ul << 0),
    VSF_I2C_CMD_READ            = (0x01ul << 0),

    VSF_I2C_CMD_START           = (0x00ul << 28),
    VSF_I2C_CMD_NO_START        = (0x01ul << 28),

    VSF_I2C_CMD_STOP            = (0x00ul << 29),
    VSF_I2C_CMD_RESTART         = (0x01ul << 30),
    VSF_I2C_CMD_NO_STOP_RESTART = (0x01ul << 30),

    VSF_I2C_CMD_7_BITS          = (0x00ul << 31),
    VSF_I2C_CMD_10_BITS         = (0x01ul << 31),
} vsf_i2c_cmd_t;
#endif

enum {
    VSF_I2C_CMD_RW_COUNT            = 2,
    VSF_I2C_CMD_RW_MASK             = VSF_I2C_CMD_WRITE | VSF_I2C_CMD_READ,

    VSF_I2C_CMD_BITS_COUNT          = 2,
    VSF_I2C_CMD_BITS_MASK           = VSF_I2C_CMD_7_BITS |
                                      VSF_I2C_CMD_10_BITS,

    VSF_I2C_CMD_START_COUNT         = 2,
    VSF_I2C_CMD_START_MASK          = VSF_I2C_CMD_START | VSF_I2C_CMD_NO_START,

    VSF_I2C_CMD_STOP_RESTART_COUNT  = 3,
    VSF_I2C_CMD_STOP_RESTART_MASK   = VSF_I2C_CMD_STOP
                                    | VSF_I2C_CMD_RESTART
                                    | VSF_I2C_CMD_NO_STOP_RESTART,

    VSF_I2C_CMD_MASK_COUNT          = 5,
    VSF_I2C_CMD_ALL_BITS_MASK       = VSF_I2C_CMD_RW_MASK
                                    | VSF_I2C_CMD_START
                                    | VSF_I2C_CMD_STOP
                                    | VSF_I2C_CMD_RESTART
                                    | VSF_I2C_CMD_BITS_MASK,
};

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_i2c_irq_mask_t {
    VSF_I2C_IRQ_MASK_MASTER_STARTED                 = (0x1ul <<  0),
    VSF_I2C_IRQ_MASK_MASTER_STOPPED                 = (0x1ul <<  1),
    VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT             = (0x1ul <<  2),    // for multi master
    VSF_I2C_IRQ_MASK_MASTER_NACK_DETECT             = (0x1ul <<  4),
    VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST        = (0x1ul <<  5),
    VSF_I2C_IRQ_MASK_MASTER_TX_EMPTY                = (0x1ul <<  6),
    VSF_I2C_IRQ_MASK_MASTER_ERROR                   = (0x1ul <<  7),

    VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE       = (0x1ul <<  8),
    VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK            = (0x1ul <<  9),
} vsf_i2c_irq_mask_t;
#endif

enum {
    VSF_I2C_IRQ_COUNT                               = 8,
    VSF_I2C_IRQ_ALL_BITS_MASK                       = VSF_I2C_IRQ_MASK_MASTER_STARTED
                                                    | VSF_I2C_IRQ_MASK_MASTER_STOPPED
                                                    | VSF_I2C_IRQ_MASK_MASTER_STOP_DETECT
                                                    | VSF_I2C_IRQ_MASK_MASTER_NACK_DETECT
                                                    | VSF_I2C_IRQ_MASK_MASTER_ARBITRATION_LOST
                                                    | VSF_I2C_IRQ_MASK_MASTER_TX_EMPTY
                                                    | VSF_I2C_IRQ_MASK_MASTER_ERROR
                                                    | VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                                                    | VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK,
};

#if VSF_I2C_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_i2c_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        uint32_t value;
    };
} vsf_i2c_status_t;
#endif

typedef struct vsf_i2c_capability_t {
#if VSF_I2C_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_i2c_irq_mask_t irq_mask;

    // If manual control of the start/stop signal is not supported,
    // then every transmission will generate a start/stop
    uint8_t support_no_start        : 1;
    uint8_t support_no_stop_restart : 1;
    uint8_t support_restart         : 1;

    // maximum transfer size
    // specifically, 0 is not a legal value
    uint16_t max_transfer_size;
    // 0, there is no data, just send start condition
    uint16_t min_transfer_size;
} vsf_i2c_capability_t;

typedef struct vsf_i2c_t vsf_i2c_t;

/**
 \~english
 @brief i2c interrupt callback function prototype.
 @note It is called after interrupt occurs.

 @param target_ptr pointer of user.
 @param i2c_ptr pointer of i2c instance.
 @param irq_mask one or more value of enum vsf_i2c_irq_mask_t
 @return None.

 \~chinese
 @brief i2c 中断回调函数原型
 @note 这个回调函数在中断发生之后被调用

 @param target_ptr 用户指针
 @param i2c_ptr i2c 实例的指针
 @param irq_mask 一个或者多个枚举 vsf_i2c_irq_mask_t 的值的按位或
 @return 无。

 \~
 \code {.c}
    static void __user_i2c_irqhandler(void *target_ptr, vsf_i2c_t *i2c_ptr, enum irq_mask)
    {
        if (irq_mask & VSF_I2C_IRQ_MASK_MASTER_STARTED) {
            // do something
        }

        if (irq_mask & VSF_I2C_IRQ_MASK_MASTER_STOPPED) {
            // do something
        }
    }
 \endcode
 */
typedef void vsf_i2c_isr_handler_t(void *target_ptr,
                                   vsf_i2c_t *i2c_ptr,
                                   vsf_i2c_irq_mask_t irq_mask);

/**
 \~english
 @brief i2c interrupt configuration

 \~chinese
 @brief i2c 中断配置
 */
typedef struct vsf_i2c_isr_t {
    vsf_i2c_isr_handler_t *handler_fn;      //!< \~english TODO
                                            //!< \~chinese 中断回调函数
    void                  *target_ptr;      //!< \~english pointer of user target
                                            //!< \~chinese 用户传入的指针
    vsf_arch_prio_t        prio;            //!< \~english interrupt priority
                                            //!< \~chinesh 中断优先级
} vsf_i2c_isr_t;

/**
 \~english
 @brief i2c configuration

 \~chinese
 @brief i2c 配置
 */
typedef struct vsf_i2c_cfg_t {
    vsf_i2c_mode_t mode;                    //!< \~english i2c mode \ref vsf_i2c_mode_t
                                            //!< \~chinese i2c 模式 \ref vsf_i2c_mode_t
    uint32_t clock_hz;                      //!< \~english i2c clock (in Hz)
                                            //!< \~chinese i2c 时钟频率 (单位：Hz)
    vsf_i2c_isr_t isr;                      //!< \~english i2c interrupt
                                            //!< \~chinese i2c 中断
    uint16_t slave_addr;                    //!< \~english i2c slave address, only valid in slave mode
                                            //!< \~chinese i2c 从机地址，仅在从机模式下有效
} vsf_i2c_cfg_t;

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
 @param[in] cfg_ptr: a pointer to structure @ref vsf_i2c_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if i2c was initialized, or a negative error code

 @note It is not necessary to call vsf_i2c_fini() to deinitialization.
       vsf_i2c_init() should be called before any other i2c API except vsf_i2c_capability().

 \~chinese
 @brief 初始化一个 i2c 实例
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @param[in] cfg_ptr: 结构体 vsf_i2c_cfg_t 的指针，参考 @ref vsf_i2c_cfg_t
 @return vsf_err_t: 如果 i2c 初始化成功返回 VSF_ERR_NONE , 失败返回负数。

 @note 失败后不需要调用 vsf_i2c_fini() 反初始化。
       vsf_i2c_init() 应该在除 vsf_i2c_capability() 之外的其他 i2c API 之前调用。
 */
extern vsf_err_t vsf_i2c_init(vsf_i2c_t *i2c_ptr, vsf_i2c_cfg_t *cfg_ptr);

/**
 \~english
 @brief termination a i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @note Termination an i2c instance should include turning off the corresponding
       power gating.
 \~chinese
 @brief 终止一个 i2c 实例
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @note 终止一个i2c实例应该包括关闭对应的电源门控
 */
extern void vsf_i2c_fini(vsf_i2c_t *i2c_ptr);

/**
 \~english
 @brief enable i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @return fsm_rt_t: fsm_rt_cpl if enable complete, else return fsm_rt_onging
 @note vsf_i2c_enable() should be called after vsf_i2c_init().
       The clock gating bit only needs to be ensured to be turned on after
       vsf_i2c_enable().

 \~chinese
 @brief 使能 i2c 实例
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return fsm_rt_t: 如果使能成功，返回 fsm_rt_cpl, 未完成初始化返回 fsm_rt_onging
 @note vsf_i2c_enable() 应该在 vsf_i2c_init() 之后调用。时钟门控位只需要确保在
       vsf_i2c_enable() 之后开启。
 */
extern fsm_rt_t vsf_i2c_enable(vsf_i2c_t *i2c_ptr);

/**
 \~english
 @brief disable i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @return fsm_rt_t: fsm_rt_cpl if disable complete, else return fsm_rt_onging
 @note Need to make sure that calling vsf_i2c_enable() after vsf_i2c_disable()
       is working without calling vsf_i2c_init()

 \~chinese
 @brief禁能 i2c 实例
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return fsm_rt_t: 如果禁能成功，返回 fsm_rt_cpl, 未完成初始化返回 fsm_rt_onging
 @note 需要确保在 vsf_i2c_disable() 之后调用 vsf_i2c_enable() 是可以正常工作的，
       而不需要调用 vsf_i2c_init()
 */
extern fsm_rt_t vsf_i2c_disable(vsf_i2c_t *i2c_ptr);

/**
 \~english
 @brief enable interrupt masks of i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @param[in] irq_mask: one or more value of enum @ref vsf_i2c_irq_mask_t
 @return none.
 @note All pending interrupts should be cleared before interrupts are enabled.

 \~chinese
 @brief 使能 i2c 实例的中断
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_i2c_irq_mask_t 的值的按位或，@ref vsf_i2c_irq_mask_t
 @return 无。
 @note 在中断使能之前，应该清除所有悬挂的中断。
 */
extern void vsf_i2c_irq_enable(vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask);

/**
 \~english
 @brief disable interrupt masks of i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @param[in] irq_mask: one or more value of enum vsf_i2c_irq_mask_t, @ref vsf_i2c_irq_mask_t
 @return none.

 \~chinese
 @brief 禁能 i2c 实例的中断
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @param[in] irq_mask: 一个或者多个枚举 vsf_i2c_irq_mask_t 的值的按位或，@ref vsf_i2c_irq_mask_t
 @return 无。
 */
extern void vsf_i2c_irq_disable(vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask);

/**
 \~english
 @brief get the status of i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @return vsf_i2c_status_t: return all status of current i2c

 \~chinese
 @brief 获取 i2c 实例的状态
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return vsf_i2c_status_t: 返回当前 i2c 的所有状态
 */
extern vsf_i2c_status_t vsf_i2c_status(vsf_i2c_t *i2c_ptr);

/**
 \~english
 @brief get the capability of i2c instance.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @return vsf_i2c_capability_t: return all capability of current i2c @ref vsf_i2c_capability_t

 \~chinese
 @brief 获取 i2c 实例的能力
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return vsf_i2c_capability_t: 返回当前 i2c 的所有能力 @ref vsf_i2c_capability_t
 */
extern vsf_i2c_capability_t vsf_i2c_capability(vsf_i2c_t *i2c_ptr);

/**
 \~english
 @brief i2c instance as master mode request a transfer
 @note i2c_msg_ptr can be a local variable.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @param[in] address: address of i2c transfer
 @param[in] cmd: i2c cmd
 @param[in] count: i2c transfer buffer count (in byte)
 @param[in] buffer_ptr: i2c transfer buffer
 @return vsf_err_t: VSF_ERR_NONE if i2c was successfully, or a negative error code

 \~chinese
 @brief i2c主机请求一次传输
 @note i2c_msg_ptr可以是局部变量。
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @param[in] address: i2c 传输的地址
 @param[in] cmd: i2c 命令
 @param[in] count: i2c 传输缓冲区长度 (单位：字节)
 @param[in] buffer_ptr: i2c 传输缓冲区
 @return vsf_err_t: 如果 i2c 主机请求成功完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_i2c_master_request(vsf_i2c_t *i2c_ptr,
                                        uint16_t address,
                                        vsf_i2c_cmd_t cmd,
                                        uint16_t count,
                                        uint8_t *buffer_ptr);

/**
 \~english
 @brief get the counter of transfers for current request  by the i2c master
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @return uint_fast32_t: counter of transferred
 @note This API can be used after the slave NAK and until the next transmission

 \~chinese
 @brief 获取 i2c 主机当前请求已经传输的数量
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return uint_fast32_t: 已传输的数量
 @note 在从机 NAK 的后到下一次传输之前都可以使用这个API
 */
extern uint_fast32_t vsf_i2c_get_transferred_count(vsf_i2c_t *i2c_ptr);

/*============================ INCLUDES ======================================*/

#include "hal/driver/common/i2c/i2c_request.h"

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_I2C_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_i2c_t                              VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_t)
#   define vsf_i2c_init(__I2C, ...)                 VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_init)                 ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#   define vsf_i2c_fini(__I2C)                      VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_fini)                 ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_enable(__I2C)                    VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_enable)               ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_disable(__I2C)                   VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_disable)              ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_irq_enable(__I2C, ...)           VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_irq_enable)           ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#   define vsf_i2c_irq_disable(__I2C, ...)          VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_irq_disable)          ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#   define vsf_i2c_status(__I2C)                    VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_status)               ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_capability(__I2C)                VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_capability)           ((__vsf_i2c_t *)(__I2C))
#   define vsf_i2c_master_request(__I2C, ...)       VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_master_request)       ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#   define vsf_i2c_get_transferred_count(__I2C, ...) VSF_MCONNECT(VSF_I2C_CFG_PREFIX, _i2c_get_transferred_count) ((__vsf_i2c_t *)(__I2C), ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_I2C_H__*/
