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

#ifndef __VSF_TEMPLATE_I2S_H__
#define __VSF_TEMPLATE_I2S_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

/**
 * \~english
 * @brief Enable multi-class support by default for maximum availability..
 * \~chinese
 * @brief 默认启用多类支持，以获得最大可用性。
 */
#ifndef VSF_I2S_CFG_MULTI_CLASS
#   define VSF_I2S_CFG_MULTI_CLASS                      ENABLED
#endif

/**
 * \~english
 * @brief Define I2S hardware mask if count is defined.
 * \~chinese
 * @brief 如果定义了I2S硬件数量，则定义对应的掩码。
 */
#if defined(VSF_HW_I2S_COUNT) && !defined(VSF_HW_I2S_MASK)
#   define VSF_HW_I2S_MASK                              VSF_HAL_COUNT_TO_MASK(VSF_HW_I2S_COUNT)
#endif

/**
 * \~english
 * @brief Define I2S hardware count if mask is defined.
 * \~chinese
 * @brief 如果定义了I2S硬件掩码，则定义对应的数量。
 */
#if defined(VSF_HW_I2S_MASK) && !defined(VSF_HW_I2S_COUNT)
#   define VSF_HW_I2S_COUNT                             VSF_HAL_MASK_TO_COUNT(VSF_HW_I2S_MASK)
#endif

/**
 * \~english
 * @brief We can redefine macro VSF_I2S_CFG_PREFIX to specify a prefix
 * to call a specific driver directly in the application code.
 * \~chinese
 * @brief 可重新定义宏 VSF_I2S_CFG_PREFIX，以在应用代码中直接调用特定驱动。
 */
#ifndef VSF_I2S_CFG_PREFIX
#   if VSF_I2S_CFG_MULTI_CLASS == ENABLED
#       define VSF_I2S_CFG_PREFIX                   vsf
#   elif defined(VSF_HW_I2S_COUNT) && (VSF_HW_I2S_COUNT != 0)
#       define VSF_I2S_CFG_PREFIX                   vsf_hw
#   else
#       define VSF_I2S_CFG_PREFIX                   vsf
#   endif
#endif


/**
 * \~english
 * @brief Disable VSF_I2S_CFG_FUNCTION_RENAME to use the original function names
 * (e.g., vsf_i2s_init()).
 * \~chinese
 * @brief 禁用 VSF_I2S_CFG_FUNCTION_RENAME 以使用原始函数名(例如 vsf_i2s_init())。
 */
#ifndef VSF_I2S_CFG_FUNCTION_RENAME
#   define VSF_I2S_CFG_FUNCTION_RENAME              ENABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_I2S_CFG_REIMPLEMENT_MODE to redefine enum @ref vsf_i2s_mode_t.
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_I2S_CFG_REIMPLEMENT_MODE 来重新定义枚举
 * @ref vsf_i2s_mode_t。
 */
#ifndef VSF_I2S_CFG_REIMPLEMENT_TYPE_MODE
#   define VSF_I2S_CFG_REIMPLEMENT_TYPE_MODE        DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_I2S_CFG_REIMPLEMENT_IRQ_MASK to redefine enum @ref vsf_i2s_irq_mask_t.
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_I2S_CFG_REIMPLEMENT_IRQ_MASK 来重新定义枚举
 * @ref vsf_i2s_irq_mask_t。
 */
#ifndef VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK    DISABLED
#endif

/**
 * \~english
 * @brief In specific hardware driver, we can enable macro
 * VSF_I2S_CFG_REIMPLEMENT_STATUS to redefine struct @ref vsf_i2s_status_t.
 * \~chinese
 * @brief 在具体硬件驱动中，我们可以启用宏 VSF_I2S_CFG_REIMPLEMENT_STATUS 来重新定义结构体
 * @ref vsf_i2s_status_t。
 */
#ifndef VSF_I2S_CFG_REIMPLEMENT_TYPE_STATUS
#   define VSF_I2S_CFG_REIMPLEMENT_TYPE_STATUS      DISABLED
#endif



/**
 * \~english @brief In specific hardware driver, we can enable macro
 * VSF_I2S_CFG_REIMPLEMENT_TYPE_CFG to redefine struct @ref vsf_i2s_cfg_t. For
 * compatibility, members should not be deleted when redefining.
 * \~chinese @brief 在具体的硬件驱动中，可以启用宏
 * VSF_I2S_CFG_REIMPLEMENT_TYPE_CFG 来重新定义结构体 @ref
 * vsf_i2s_cfg_t。为了保证兼容性，重新定义时不应删除成员。
 */
#if VSF_I2S_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
#    define VSF_I2S_CFG_REIMPLEMENT_TYPE_CFG DISABLED
#endif

/**
 * \~english @brief In specific hardware driver, we can enable macro
 * VSF_I2S_CFG_REIMPLEMENT_TYPE_CAPABILITY to redefine struct @ref
 * vsf_i2s_capability_t. For compatibility, members should not be deleted when
 * struct @ref vsf_i2s_capability_t redefining.
 * \~chinese @brief 在具体的硬件驱动中，可以启用宏
 * VSF_I2S_CFG_REIMPLEMENT_TYPE_CAPABILITY 来重新定义结构体 @ref
 * vsf_i2s_capability_t。为了保证兼容性，重新定义时不应删除成员。
 */
#if VSF_I2S_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
#    define VSF_I2S_CFG_REIMPLEMENT_TYPE_CAPABILITY DISABLED
#endif

/**
 * \~english @brief In specific hardware driver, we can enable macro
 * VSF_I2S_CFG_INHERIT_HAL_CAPABILITY to inherit structure @ref
 * vsf_peripheral_capability_t.
 * \~chinse @brief 在具体的硬件驱动中，可以启用宏
 * VSF_I2S_CFG_INHERIT_HAL_CAPABILITY 来继承结构体 @ref
 * vsf_peripheral_capability_t。
 */
#ifndef VSF_I2S_CFG_INHERIT_HAL_CAPABILITY
#   define VSF_I2S_CFG_INHERIT_HAL_CAPABILITY       ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

/**
 * \~english
 * @brief I2S API template, used to generate I2S type, specific prefix function declarations, etc.
 * @param[in] __prefix_name The prefix used for generating I2S functions.
 * \~chinese
 * @brief I2S API 模板，用于生成 I2S 类型、特定前缀的函数声明等。
 * @param[in] __prefix_name 用于生成 I2S 函数的前缀。
 */
#define VSF_I2S_APIS(__prefix_name)                                                                                                                         \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            i2s, init,          VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)   \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            i2s, tx_init,       VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)   \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 i2s, tx_fini,       VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            i2s, tx_start,      VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            i2s, rx_init,       VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr, vsf_i2s_cfg_t *cfg_ptr)   \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                 i2s, rx_fini,       VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,            i2s, rx_start,      VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             i2s, enable,        VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, fsm_rt_t,             i2s, disable,       VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_i2s_status_t,     i2s, status,        VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)                           \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_i2s_capability_t, i2s, capability,    VSF_MCONNECT(__prefix_name, _i2s_t) *i2s_ptr)

/*============================ TYPES =========================================*/

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_MODE == DISABLED
typedef enum vsf_i2s_mode_t {
    VSF_I2S_MODE_MASTER             = (0x01ul << 0),     // select master mode
    VSF_I2S_MODE_SLAVE              = (0x00ul << 0),     // select slave mode

    VSF_I2S_DATA_BITLEN_16          = (0x01ul << 1),
    VSF_I2S_DATA_BITLEN_24          = (0x02ul << 1),
    VSF_I2S_DATA_BITLEN_32          = (0x03ul << 1),

    VSF_I2S_FRAME_BITLEN_16         = (0x01ul << 3),
    VSF_I2S_FRAME_BITLEN_24         = (0x02ul << 3),
    VSF_I2S_FRAME_BITLEN_32         = (0x03ul << 3),

    VSF_I2S_STANDARD_PHILIPS        = (0x01ul << 5),
    VSF_I2S_STANDARD_MSB            = (0x02ul << 5),
    VSF_I2S_STANDARD_LSB            = (0x03ul << 5),

    VSF_I2S_LRCK_POL                = (0x01ul << 7),    // normally used to switch LR channel
    VSF_I2S_BCK_POL                 = (0x01ul << 8),    // bck level while idle
    VSF_I2S_MCLK_OUTPUT             = (0x01ul << 9),
} vsf_i2s_mode_t;
#endif

enum {
    VSF_I2S_MODE_COUNT              = 0,
    VSF_I2S_MODE_MASK               = VSF_I2S_MODE_MASTER   |
                                      VSF_I2S_MODE_SLAVE,

    VSF_I2S_DATA_BITLEN_COUNT       = 0,
    VSF_I2S_DATA_BITLEN_MASK        = VSF_I2S_DATA_BITLEN_16 |
                                      VSF_I2S_DATA_BITLEN_24 |
                                      VSF_I2S_DATA_BITLEN_32,

    VSF_I2S_FRAME_BITLEN_COUNT      = 0,
    VSF_I2S_FRAME_BITLEN_MASK       = VSF_I2S_FRAME_BITLEN_16 |
                                      VSF_I2S_FRAME_BITLEN_24 |
                                      VSF_I2S_FRAME_BITLEN_32,

    VSF_I2S_STANDARD_COUNT          = 0,
    VSF_I2S_STANDARD_MASK           = VSF_I2S_STANDARD_PHILIPS  |
                                      VSF_I2S_STANDARD_MSB      |
                                      VSF_I2S_STANDARD_LSB,

    VSF_I2S_MODE_ALL_BITS_MASK      = VSF_I2S_MODE_MASK         |
                                      VSF_I2S_DATA_BITLEN_MASK  |
                                      VSF_I2S_FRAME_BITLEN_MASK |
                                      VSF_I2S_STANDARD_MASK     |
                                      VSF_I2S_LRCK_POL          |
                                      VSF_I2S_BCK_POL           |
                                      VSF_I2S_MCLK_OUTPUT,
};

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_i2s_irq_mask_t {
    VSF_I2S_IRQ_MASK_TX_TGL_BUFFER  = (0x1ul <<  0),
    VSF_I2S_IRQ_MASK_RX_TGL_BUFFER  = (0x1ul <<  1),
} vsf_i2s_irq_mask_t;
#endif

enum {
    VSF_I2S_IRQ_ALL_BITS_MASK       = VSF_I2S_IRQ_MASK_TX_TGL_BUFFER |
                                      VSF_I2S_IRQ_MASK_RX_TGL_BUFFER,
};

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_i2s_status_t {
    union {
        inherit(vsf_peripheral_status_t)
        uint32_t value;
    };
} vsf_i2s_status_t;
#endif

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_CAPABILITY == DISABLED
typedef struct vsf_i2s_capability_t {
#if VSF_I2S_CFG_INHERIT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    struct {
        // sample rate converter
        bool is_src_supported;
        // double buffer
        bool is_dbuffer_supported;
    } i2s_capability;
} vsf_i2s_capability_t;
#endif

#if VSF_I2S_CFG_REIMPLEMENT_TYPE_CFG == DISABLED
typedef struct vsf_i2s_t vsf_i2s_t;

/**
 \~english
 @brief i2s interrupt callback function prototype.
 @note It is called after interrupt occurs.

 @param target_ptr pointer of user.
 @param i2s_ptr pointer of i2s instance.
 @param irq_mask one or more value of enum vsf_i2s_irq_mask_t
 @return None.

 \~chinese
 @brief i2s 中断回调函数原型
 @note 这个回调函数在中断发生之后被调用

 @param target_ptr 用户指针
 @param i2s_ptr i2s 实例的指针
 @param irq_mask 一个或者多个枚举 vsf_i2s_irq_mask_t 的值的按位或
 @return 无。

 \~
 \code {.c}
    static void __user_i2s_irqhandler(void *target_ptr, vsf_i2s_t *i2s_ptr, enum irq_mask)
    {
        if (irq_mask & I2S_IRQ_MASK_TX_TGL_BUFFER) {
            // do something
        }

        if (irq_mask & I2S_IRQ_MASK_RX_TGL_BUFFER) {
            // do something
        }
    }
 \endcode
 */
typedef void vsf_i2s_isr_handler_t(void *target_ptr,
                                   vsf_i2s_t *i2s_ptr,
                                   vsf_i2s_irq_mask_t irq_mask);

/**
 \~english
 @brief i2s interrupt configuration

 \~chinese
 @brief i2s 中断配置
 */
typedef struct vsf_i2s_isr_t {
    vsf_i2s_isr_handler_t *handler_fn;      //!< \~english TODO
                                            //!< \~chinese 中断回调函数
    void                  *target_ptr;      //!< \~english pointer of user target
                                            //!< \~chinese 用户传入的指针
    vsf_arch_prio_t        prio;            //!< \~english interrupt priority
                                            //!< \~chinesh 中断优先级
} vsf_i2s_isr_t;

/**
 \~english
 @brief i2s configuration

 \~chinese
 @brief i2s 配置
 */
typedef struct vsf_i2s_cfg_t {
    vsf_i2s_mode_t mode;                    //!< \~english i2s mode \ref vsf_i2s_mode_t
                                            //!< \~chinese i2s 模式 \ref vsf_i2s_mode_t
    uint32_t data_sample_rate;              //!< \~english i2s sample rate of audio data (in Hz), valid only if is_src_supported capability is enabled, else ignored
                                            //!< \~chinese i2s 音频数据的时钟频率 (单位：Hz)，支持 is_src_supported 特性的时候可用，否则忽略
    uint32_t hw_sample_rate;                //!< \~english i2s sample rate of hw audio data to codec (in Hz)
                                            //!< \~chinese i2s codec 的硬件音频数据时钟频率 (单位：Hz)
    uint8_t *buffer;                        //!< \~english i2s channel buffer, valid if is_dbuffer_supported capability is enabled
                                            //!< \~chinese i2s 通道缓冲，支持 is_dbuffer_supported 特性的时候可用，否则忽略
    uint16_t buffer_size;                   //!< \~english i2s channel buffer byte size, valid if is_dbuffer_supported capability is enabled
                                            //!< \~chinese i2s 通道缓冲字节大小，支持 is_dbuffer_supported 特性的时候可用，否则忽略
    uint8_t channel_num;                    //!< \~english i2s channel number
                                            //!< \~chinese i2s 通道数
    vsf_i2s_isr_t isr;                      //!< \~english i2s interrupt
                                            //!< \~chinese i2s 中断
} vsf_i2s_cfg_t;
#endif

typedef struct vsf_i2s_op_t {
/// @cond
#undef __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP
/// @endcond

    VSF_I2S_APIS(vsf)
} vsf_i2s_op_t;

#if VSF_I2S_CFG_MULTI_CLASS == ENABLED
struct vsf_i2s_t  {
    const vsf_i2s_op_t * op;
};
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

/**
 * \~english
 * @brief Initialize an I2S instance
 * @param[in] i2s_ptr: pointer to structure @ref vsf_i2s_t
 * @param[in] cfg_ptr: pointer to configuration structure @ref vsf_i2s_cfg_t
 * @return vsf_err_t: VSF_ERR_NONE if initialization successful, otherwise return error code
 *
 *  @note It is not necessary to call vsf_i2s_fini() to deinitialization.
 *     vsf_i2s_init() should be called before any other I2S API except vsf_i2s_capability().
 * \~chinese
 * @brief 初始化一个 I2S 实例
 * @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 * @param[in] cfg_ptr: 指向配置结构体 @ref vsf_i2s_cfg_t 的指针
 * @return vsf_err_t: 如果初始化成功返回 VSF_ERR_NONE，否则返回错误码
 *
 * @note 失败后不需要调用 vsf_i2s_fini() 反初始化。
 *       vsf_i2s_init() 应该在除 vsf_i2s_capability() 之外的其他 I2S API 之前调用。
 */
extern vsf_err_t vsf_i2s_init(vsf_i2s_t *i2s_ptr, vsf_i2s_cfg_t *i2s_cfg);

/**
 \~english
 @brief Initialize i2s tx channel.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_i2s_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if i2s initialized successfully, or a negative error code

 \~chinese
 @brief 初始化 i2s 发送通道
 @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 @param[in] cfg_ptr: 指向结构体 @ref vsf_i2s_cfg_t 的指针
 @return vsf_err_t: 如果 i2s 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_i2s_tx_init(vsf_i2s_t *i2s_ptr, vsf_i2s_cfg_t *i2s_cfg);

/**
 \~english
 @brief Finalize i2s tx channel.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return void

 \~chinese
 @brief 终止化 i2s 发送通道
 @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 @return 无
 */
extern void vsf_i2s_tx_fini(vsf_i2s_t *i2s_ptr);

/**
 \~english
 @brief Start i2s tx channel.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return vsf_err_t: VSF_ERR_NONE if i2s started successfully, or a negative error code

 \~chinese
 @brief 启动 i2s 发送通道
 @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 @return vsf_err_t: 如果 i2s 启动完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_i2s_tx_start(vsf_i2s_t *i2s_ptr);

/**
 \~english
 @brief Initialize i2s rx channel.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @param[in] cfg_ptr: a pointer to structure @ref vsf_i2s_cfg_t
 @return vsf_err_t: VSF_ERR_NONE if i2s initialized successfully, or a negative error code

 \~chinese
 @brief 初始化 i2s 接收通道
 @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 @param[in] cfg_ptr: 指向结构体 @ref vsf_i2s_cfg_t 的指针
 @return vsf_err_t: 如果 i2s 初始化完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_i2s_rx_init(vsf_i2s_t *i2s_ptr, vsf_i2s_cfg_t *i2s_cfg);

/**
 \~english
 @brief Finalize i2s rx channel.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return void

 \~chinese
 @brief 终止化 i2s 接收通道
 @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 @return 无
 */
extern void vsf_i2s_rx_fini(vsf_i2s_t *i2s_ptr);

/**
 \~english
 @brief Start i2s rx channel.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return vsf_err_t: VSF_ERR_NONE if i2s started successfully, or a negative error code

 \~chinese
 @brief 启动 i2s 接收通道
 @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 @return vsf_err_t: 如果 i2s 启动完成返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_i2s_rx_start(vsf_i2s_t *i2s_ptr);

/**
 \~english
 @brief Finalize a i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return none

 \~chinese
 @brief 结束一个 i2s 实例
 @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 @return 无
 */
extern void vsf_i2s_fini(vsf_i2s_t *i2s_ptr);

/**
 * \~english
 * @brief Enable I2S instance
 * @param[in] i2s_ptr: pointer to structure @ref vsf_i2s_t
 * @return fsm_rt_t: return fsm_rt_cpl if enabled, fsm_rt_onging if waiting to be enabled
 *
 * \~chinese
 * @brief 启用 I2S 实例
 * @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 * @return fsm_rt_t: 如果已经启用返回 fsm_rt_cpl，如果等待启用返回 fsm_rt_onging
 */
extern fsm_rt_t vsf_i2s_enable(vsf_i2s_t *i2s_ptr);

/**
 \~english
 @brief Disable i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return fsm_rt_t: fsm_rt_cpl if disable complete, else return fsm_rt_onging

 \~chinese
 @brief 禁用 i2s 实例
 @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 @return fsm_rt_t: 如果禁用成功，返回 fsm_rt_cpl, 未完成初始化返回 fsm_rt_onging
 */
extern fsm_rt_t vsf_i2s_disable(vsf_i2s_t *i2s_ptr);

/**
 \~english
 @brief Get the status of i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return vsf_i2s_status_t: return all status of current i2s

 \~chinese
 @brief 获取 i2s 实例的状态
 @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 @return vsf_i2s_status_t: 返回当前i2s的所有状态
 */
extern vsf_i2s_status_t vsf_i2s_status(vsf_i2s_t *i2s_ptr);

/**
 \~english
 @brief Get the capability of i2s instance.
 @param[in] i2s_ptr: a pointer to structure @ref vsf_i2s_t
 @return vsf_i2s_capability_t: return all capability of current i2s @ref vsf_i2s_capability_t

 \~chinese
 @brief 获取 i2s 实例的能力
 @param[in] i2s_ptr: 指向结构体 @ref vsf_i2s_t 的指针
 @return vsf_i2s_capability_t: 返回当前 i2s 的所有能力 @ref vsf_i2s_capability_t
 */
extern vsf_i2s_capability_t vsf_i2s_capability(vsf_i2s_t *i2s_ptr);

/*============================ MACROFIED FUNCTIONS ===========================*/

/// @cond
#if VSF_I2S_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_i2s_t                      VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_t)
#   define vsf_i2s_init(__i2s, ...)         VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_init)         ((__vsf_i2s_t *)(__i2s), ##__VA_ARGS__)
#   define vsf_i2s_enable(__i2s)            VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_enable)       ((__vsf_i2s_t *)(__i2s))
#   define vsf_i2s_disable(__i2s)           VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_disable)      ((__vsf_i2s_t *)(__i2s))
#   define vsf_i2s_status(__i2s)            VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_status)       ((__vsf_i2s_t *)(__i2s))
#   define vsf_i2s_capability(__i2s)        VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_capability)   ((__vsf_i2s_t *)(__i2s))

#   define vsf_i2s_tx_init(__i2s, ...)      VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_tx_init)      ((__vsf_i2s_t *)(__i2s), ##__VA_ARGS__)
#   define vsf_i2s_tx_fini(__i2s, ...)      VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_tx_fini)      ((__vsf_i2s_t *)(__i2s), ##__VA_ARGS__)
#   define vsf_i2s_tx_start(__i2s)          VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_tx_start)     ((__vsf_i2s_t *)(__i2s))

#   define vsf_i2s_rx_init(__i2s, ...)      VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_rx_init)      ((__vsf_i2s_t *)(__i2s), ##__VA_ARGS__)
#   define vsf_i2s_rx_fini(__i2s, ...)      VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_rx_fini)      ((__vsf_i2s_t *)(__i2s), ##__VA_ARGS__)
#   define vsf_i2s_rx_start(__i2s)          VSF_MCONNECT(VSF_I2S_CFG_PREFIX, _i2s_rx_start)     ((__vsf_i2s_t *)(__i2s))
#endif
/// @endcond

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_I2S_H__*/
