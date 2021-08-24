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

#ifndef __HAL_DRIVER_I2C_INTERFACE_H__
#define __HAL_DRIVER_I2C_INTERFACE_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

/**
 \~english
 @brief i2c feature configuration
 @note em_i2c_feature_t is implemented by specific driver.
       Usually it uses the value of the register.

 This is an example for em_i2c_feature_t:

 \~chinese
 @brief i2c 特性配置
 @note em_i2c_feature_t 由具体驱动实现。通常是使用寄存器的值。

 这是一个 em_i2c_feature_t 的例子:

 \~
 \code {.c}
    enum em_i2c_feature_t {
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
    };
 \endcode
 */
typedef enum em_i2c_feature_t em_i2c_feature_t;

/**
 \~english
 @brief i2c status
 @note i2c_status_t is implemented by specific driver.

 This is an example for i2c_status_t:

 \~chinese
 @brief i2c 状态
 @note i2c_status_t 由具体驱动实现。

 这是一个 i2c_status_t 的例子:

 \~
 \code {.c}
    struct i2c_status_t {
        union {
            inherit(peripheral_status_t)
            uint32_t value;
        };
    };
 \endcode
 */
typedef struct i2c_status_t i2c_status_t;

/**
 \~english
 @brief i2c capability
 @note i2c_capability_t is implemented by specific driver.

 This is an example for i2c_capability_t:

 \~chinese
 @brief i2c 能力
 @note i2c_capability_t 由具体驱动实现。

 这是一个 i2c_capability_t 的例子:

 \~
 \code {.c}
    enum i2c_capability_t {
        // TODO
    };
 \endcode
 */
typedef struct i2c_capability_t i2c_capability_t;

/**
 \~english
 @brief i2c interrupt masks
 @note em_i2c_irq_mask_t is implemented by specific driver.

 em_i2c_irq_mask_t should provide irq masks.
 This is an example for em_i2c_irq_mask_t:

 \~chinese
 @brief i2c 中断掩码
 @note em_i2c_irq_mask_t 由具体驱动实现。

 em_i2c_irq_mask_t 将提供中断状态控制信息。
 这是一个 em_i2c_irq_mask_t 的例子:

 \~
 \code {.c}
    // use enumeration implementation
    enum em_i2c_irq_mask_t {
        // i2c hardware interrupt status, usually used by i2c template
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
        I2C_IRQ_MASK_MASTER_ERROR                   = (0x1ul << 10),

        // TODO: add slave interrupt
    };
 \endcode
 */
typedef enum em_i2c_irq_mask_t em_i2c_irq_mask_t;

/**
 \~english
 @brief i2c instance
 @note vsf_i2c_t is implemented by specific driver.

 vsf_i2c_t contains the presistence information of the instance.
 This is an example for vsf_i2c_t:

 \~chinese
 @brief i2c 实例
 @note vsf_i2c_t 由具体驱动实现。

 vsf_i2c_t 里包括了实例的持久化信息。
 这是一个 vsf_i2c_t 的例子:

 \~
 \code {.c}
    struct vsf_i2c_t {
        reg_t *reg;

        // other members
    };
 \endcode
 */
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
    void __user_i2c_irchandler(void *target_ptr,
                               vsf_i2c_t *i2c_ptr,
                               uint32_t irq_mask)
    {
        if (irq_mask & I2C_IRQ_MASK_MASTER_START) {
            // do something
        }

        if (irq_mask & I2C_IRQ_MASK_MASTER_STOP) {
            // do something
        }
    }
 \endcode
 */
typedef void vsf_i2c_isrhandler_t(void *target_ptr,
                                  vsf_i2c_t *i2c_ptr,
                                  uint32_t irq_mask);

/**
 \~english
 @brief i2c configuration

 \~chinese
 @brief i2c 中断配置
 */
typedef struct vsf_i2c_isr_t {
    vsf_i2c_isrhandler_t *handler_fn;   //!< \~english TODO
                                        //!< \~chinese 中断回调函数

    void                 *target_ptr;   //!< \~english pointer of user target
                                        //!< \~chinese 用户传入的指针

    vsf_arch_prio_t      prio;          //!< \~english interrupt priority
                                        //!< \~chinesh 中断优先级
} vsf_i2c_isr_t;

/**
 \~english
 @brief i2c configuration

 \~chinese
 @brief i2c 配置
 */
typedef struct i2c_cfg_t {
    uint32_t      mode;         //!< \~english i2c mode \ref em_i2c_feature_t
                                //!< \~chinese i2c 模式 \ref em_i2c_feature_t

    uint32_t      clock_hz;     //!< \~english i2c clock (in Hz)
                                //!< \~chinese i2c 时钟频率 (单位：Hz)

    vsf_i2c_isr_t isr;          //!< \~english i2c interrupt
                                //!< \~chinese i2c 中断
} i2c_cfg_t;

/**
 \~english
 @brief flags of i2c tranfer messages
 @note em_i2c_cmd_t is implemented by specific driver.

 \~chinese
 @brief i2c 传输的标志
 @note em_i2c_cmd_t 由具体驱动实现。

 \~
 \code {.c}
    typedef enum em_i2c_cmd_t {
        I2C_CMD_WRITE      = (0x00ul << 0),
        I2C_CMD_READ       = (0x01ul << 0),
        I2C_CMD_RW_MASK    = (0x01ul << 1),

        I2C_CMD_START      = (0x01ul << 1),
        I2C_CMD_STOP       = (0x01ul << 1),
        I2C_CMD_RESTAR     = (0x01ul << 1),

        I2C_CMD_7_BITS     = (0x00ul << 2),
        I2C_CMD_10_BITS    = (0x01ul << 2),
        I2C_CMD_BITS_MASK  = (0x01ul << 2),
    } em_i2c_cmd_t;
 \endcode
 */
typedef enum em_i2c_cmd_t em_i2c_cmd_t;

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
 @brief禁能 i2c 实例的中断
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
 @brief禁能 i2c 实例
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @return i2c_status_t: 返回当前i2c的所有状态
 */
extern i2c_status_t vsf_i2c_status(vsf_i2c_t *i2c_ptr);

/**
 \~english
 @brief i2c instance as master mode request a transfer
 @note i2c_msg_ptr can be a local variable.
 @param[in] i2c_ptr: a pointer to structure @ref vsf_i2c_t
 @param[in] address: address of i2c transfer
 @param[in] cmd: i2c cmd
 @param[in] count: i2c transfer buffer count (in byte)
 @param[in] buffer_ptr: i2c transfer buffer
 @return i2c_status_t: return all status of current i2c

 \~chinese
 @brief i2c主机请求一次传输
 @note i2c_msg_ptr可以是局部变量。
 @param[in] i2c_ptr: 结构体 vsf_i2c_t 的指针，参考 @ref vsf_i2c_t
 @param[in] address: i2c 传输的地址
 @param[in] cmd: i2c 命令
 @param[in] count: i2c 传输缓冲区长度 (单位：字节)
 @param[in] buffer_ptr: i2c 传输缓冲区
 @return vsf_err_t: 返回当前i2c的所有状态
 */
extern vsf_err_t vsf_i2c_master_request(vsf_i2c_t *i2c_ptr,
                                        uint16_t   address,
                                        uint8_t    cmd,
                                        uint16_t   count,
                                        uint8_t*   buffer_ptr);

#endif

