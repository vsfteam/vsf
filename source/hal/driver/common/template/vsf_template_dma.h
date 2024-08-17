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

#ifndef __VSF_TEMPLATE_DMA_H__
#define __VSF_TEMPLATE_DMA_H__

/*============================ INCLUDES ======================================*/

#include "./vsf_template_hal_driver.h"
#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// multi-class support enabled by default for maximum availability.
#ifndef VSF_DMA_CFG_MULTI_CLASS
#   define VSF_DMA_CFG_MULTI_CLASS                      ENABLED
#endif

// application code can redefine it
#ifndef VSF_DMA_CFG_PREFIX
#   if VSF_DMA_CFG_MULTI_CLASS == ENABLED
#       define VSF_DMA_CFG_PREFIX                       vsf
#   elif defined(VSF_HW_DMA_COUNT) && (VSF_HW_DMA_COUNT != 0)
#       define VSF_DMA_CFG_PREFIX                       vsf_hw
#   else
#       define VSF_DMA_CFG_PREFIX                       vsf
#   endif
#endif

#ifndef VSF_DMA_CFG_FUNCTION_RENAME
#   define VSF_DMA_CFG_FUNCTION_RENAME                  ENABLED
#endif

#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE    DISABLED
#endif

#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE    DISABLED
#endif

#ifndef VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK
#   define VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK        DISABLED
#endif

#ifndef VSF_DMA_CFG_INHERT_HAL_CAPABILITY
#   define VSF_DMA_CFG_INHERT_HAL_CAPABILITY            ENABLED
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_DMA_APIS(__prefix_name) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, init,                          VSF_MCONNECT(__prefix_name, _dma_t) *dma_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                     dma, fini,                          VSF_MCONNECT(__prefix_name, _dma_t) *dma_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_dma_capability_t,     dma, capability,                    VSF_MCONNECT(__prefix_name, _dma_t) *dma_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, int8_t,                   dma, channel_request,               VSF_MCONNECT(__prefix_name, _dma_t) *dma_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, void,                     dma, channel_release,               VSF_MCONNECT(__prefix_name, _dma_t) *dma_ptr, int8_t) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_config,                VSF_MCONNECT(__prefix_name, _dma_t) *dma_ptr, int8_t channel, vsf_dma_channel_cfg_t * cfg_ptr) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_start,                 VSF_MCONNECT(__prefix_name, _dma_t) *dma_ptr, int8_t channel, uint32_t src_address, uint32_t dst_address, uint32_t count) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_err_t,                dma, channel_cancel,                VSF_MCONNECT(__prefix_name, _dma_t) *dma_ptr, int8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, uint32_t,                 dma, channel_get_transferred_count, VSF_MCONNECT(__prefix_name, _dma_t) *dma_ptr, int8_t channel) \
    __VSF_HAL_TEMPLATE_API(__prefix_name, vsf_dma_channel_status_t, dma, channel_status,                VSF_MCONNECT(__prefix_name, _dma_t) *dma_ptr, int8_t channel)

/*============================ TYPES =========================================*/

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_CHANNEL_MODE == DISABLED
typedef enum vsf_dma_channel_mode_t {
    VSF_DMA_MEMORY_TO_MEMORY        = (0x00 << 0),
    VSF_DMA_MEMORY_TO_PERIPHERAL    = (0x01 << 0),
    VSF_DMA_PERIPHERA_TO_MEMORY     = (0x02 << 0),
    VSF_DMA_PERIPHERA_TO_PERIPHERAL = (0x03 << 0),

    VSF_DMA_SRC_ADDR_INCREMENT      = (0x00 << 2),
    VSF_DMA_SRC_ADDR_DECREMENT      = (0x01 << 2),
    VSF_DMA_SRC_ADDR_NO_CHANGE      = (0x02 << 2),

    VSF_DMA_DST_ADDR_INCREMENT      = (0x00 << 4),
    VSF_DMA_DST_ADDR_DECREMENT      = (0x01 << 4),
    VSF_DMA_DST_ADDR_NO_CHANGE      = (0x02 << 4),

    VSF_DMA_SRC_WIDTH_BYTE_1        = (0x00 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_2       = (0x01 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_4       = (0x02 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_8       = (0x03 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_16      = (0x04 << 6),
    VSF_DMA_SRC_WIDTH_BYTES_32      = (0x05 << 6),

    VSF_DMA_DST_WIDTH_BYTE_1        = (0x00 << 10),
    VSF_DMA_DST_WIDTH_BYTES_2       = (0x01 << 10),
    VSF_DMA_DST_WIDTH_BYTES_4       = (0x02 << 10),
    VSF_DMA_DST_WIDTH_BYTES_8       = (0x03 << 10),
    VSF_DMA_DST_WIDTH_BYTES_16      = (0x04 << 10),
    VSF_DMA_DST_WIDTH_BYTES_32      = (0x05 << 10),

    VSF_DMA_SRC_BURST_LENGTH_1       = (0x00 << 14),
    VSF_DMA_SRC_BURST_LENGTH_2       = (0x01 << 14),
    VSF_DMA_SRC_BURST_LENGTH_4       = (0x02 << 14),
    VSF_DMA_SRC_BURST_LENGTH_8       = (0x03 << 14),
    VSF_DMA_SRC_BURST_LENGTH_16      = (0x04 << 14),
    VSF_DMA_SRC_BURST_LENGTH_32      = (0x05 << 14),
    VSF_DMA_SRC_BURST_LENGTH_64      = (0x05 << 14),
    VSF_DMA_SRC_BURST_LENGTH_128     = (0x05 << 14),

    VSF_DMA_DST_BURST_LENGTH_1       = (0x00 << 18),
    VSF_DMA_DST_BURST_LENGTH_2       = (0x01 << 18),
    VSF_DMA_DST_BURST_LENGTH_4       = (0x02 << 18),
    VSF_DMA_DST_BURST_LENGTH_8       = (0x03 << 18),
    VSF_DMA_DST_BURST_LENGTH_16      = (0x04 << 18),
    VSF_DMA_DST_BURST_LENGTH_32      = (0x05 << 18),
    VSF_DMA_DST_BURST_LENGTH_64      = (0x05 << 18),
    VSF_DMA_DST_BURST_LENGTH_128     = (0x05 << 18),

    //prio
    VSF_DMA_PRIOPIRY_LOW             = (0x00 << 22),
    VSF_DMA_PRIOPIRY_MIDIUM          = (0x01 << 22),
    VSF_DMA_PRIOPIRY_HIGH            = (0x02 << 22),
    VSF_DMA_PRIOPIRY_VERY_HIGH       = (0x03 << 22),

    // request line
} vsf_dma_channel_mode_t;
#endif

enum {
    VSF_DMA_DIRECTION_COUNT           = 4,
    VSF_DMA_DIRECTION_MASK            = VSF_DMA_MEMORY_TO_MEMORY        |
                                        VSF_DMA_MEMORY_TO_PERIPHERAL    |
                                        VSF_DMA_PERIPHERA_TO_MEMORY     |
                                        VSF_DMA_PERIPHERA_TO_PERIPHERAL,

    VSF_DMA_SRC_ADDR_COUNT            = 3,
    VSF_DMA_SRC_ADDR_MASK             = VSF_DMA_SRC_ADDR_INCREMENT |
                                        VSF_DMA_SRC_ADDR_DECREMENT |
                                        VSF_DMA_SRC_ADDR_NO_CHANGE,

    VSF_DMA_DST_ADDR_COUNT            = 3,
    VSF_DMA_DST_ADDR_MASK             = VSF_DMA_DST_ADDR_INCREMENT |
                                        VSF_DMA_DST_ADDR_DECREMENT |
                                        VSF_DMA_DST_ADDR_NO_CHANGE,

    VSF_DMA_SRC_WIDTH_COUNT           = 6,
    VSF_DMA_SRC_WIDTH_MASK            = VSF_DMA_SRC_WIDTH_BYTE_1   |
                                        VSF_DMA_SRC_WIDTH_BYTES_2  |
                                        VSF_DMA_SRC_WIDTH_BYTES_4  |
                                        VSF_DMA_SRC_WIDTH_BYTES_8  |
                                        VSF_DMA_SRC_WIDTH_BYTES_16 |
                                        VSF_DMA_SRC_WIDTH_BYTES_32,

    VSF_DMA_DST_WIDTH_COUNT           = 6,
    VSF_DMA_DST_WIDTH_MASK            = VSF_DMA_DST_WIDTH_BYTE_1   |
                                        VSF_DMA_DST_WIDTH_BYTES_2  |
                                        VSF_DMA_DST_WIDTH_BYTES_4  |
                                        VSF_DMA_DST_WIDTH_BYTES_8  |
                                        VSF_DMA_DST_WIDTH_BYTES_16 |
                                        VSF_DMA_DST_WIDTH_BYTES_32,

    VSF_DMA_SRC_BURST_COUNT           = 8,
    VSF_DMA_SRC_BURST_MASK            = VSF_DMA_SRC_BURST_LENGTH_1   |
                                        VSF_DMA_SRC_BURST_LENGTH_2   |
                                        VSF_DMA_SRC_BURST_LENGTH_4   |
                                        VSF_DMA_SRC_BURST_LENGTH_8   |
                                        VSF_DMA_SRC_BURST_LENGTH_16  |
                                        VSF_DMA_SRC_BURST_LENGTH_32  |
                                        VSF_DMA_SRC_BURST_LENGTH_64  |
                                        VSF_DMA_SRC_BURST_LENGTH_128,

    VSF_DMA_DST_BURST_COUNT           = 8,
    VSF_DMA_DST_BURST_MASK            = VSF_DMA_DST_BURST_LENGTH_1   |
                                        VSF_DMA_DST_BURST_LENGTH_2   |
                                        VSF_DMA_DST_BURST_LENGTH_4   |
                                        VSF_DMA_DST_BURST_LENGTH_8   |
                                        VSF_DMA_DST_BURST_LENGTH_16  |
                                        VSF_DMA_DST_BURST_LENGTH_32  |
                                        VSF_DMA_DST_BURST_LENGTH_64  |
                                        VSF_DMA_DST_BURST_LENGTH_128,

    VSF_DMA_PRIOPIRY_COUNT            = 4,
    VSF_DMA_PRIOPIRY_MASK             = VSF_DMA_PRIOPIRY_LOW       |
                                        VSF_DMA_PRIOPIRY_MIDIUM    |
                                        VSF_DMA_PRIOPIRY_HIGH      |
                                        VSF_DMA_PRIOPIRY_VERY_HIGH,

    VSF_DMA_MODE_MASK_COUNT           = 8,
    VSF_DMA_MODE_ALL_BITS_MASK        = VSF_DMA_DIRECTION_MASK  |
                                        VSF_DMA_SRC_ADDR_MASK   |
                                        VSF_DMA_DST_ADDR_MASK   |
                                        VSF_DMA_SRC_WIDTH_MASK  |
                                        VSF_DMA_DST_WIDTH_MASK  |
                                        VSF_DMA_SRC_BURST_MASK  |
                                        VSF_DMA_DST_BURST_MASK  |
                                        VSF_DMA_PRIOPIRY_MASK,

};

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_IRQ_MASK == DISABLED
typedef enum vsf_dma_irq_mask_t {
    VSF_DMA_IRQ_MASK_CPL        = (0x1ul << 0),
    VSF_DMA_IRQ_MASK_HALF_CPL   = (0x1ul << 1),
    VSF_DMA_IRQ_MASK_ERROR      = (0x1ul << 2),
} vsf_dma_irq_mask_t;
#endif

enum {
    VSF_DMA_IRQ_COUNT               = 3,
    VSF_DMA_IRQ_ALL_BITS_MASK       = VSF_DMA_IRQ_MASK_CPL      |
                                      VSF_DMA_IRQ_MASK_HALF_CPL |
                                      VSF_DMA_IRQ_MASK_ERROR,
};

typedef struct vsf_dma_channel_filter_t {
    uint8_t src_request;
    uint8_t dst_request;
} vsf_dma_channel_filter_t;

typedef struct vsf_dma_t vsf_dma_t;

typedef void vsf_dma_isr_handler_t(void *target_ptr, vsf_dma_t *dma_ptr, int8_t channel, vsf_dma_irq_mask_t irq_mask);

typedef struct vsf_dma_isr_t {
    vsf_dma_isr_handler_t *handler_fn;
    void                  *target_ptr;
} vsf_dma_isr_t;

//! dma configuration
typedef struct vsf_dma_channel_cfg_t {
    vsf_dma_channel_mode_t  mode;
    vsf_dma_isr_t           isr;
    //! Index of the peripheral or memory corresponding to the source address of the DMA
    uint8_t                 src_idx;
    //! Index of the peripheral or memory corresponding to the destination address of the DMA
    uint8_t                 dst_idx;
} vsf_dma_channel_cfg_t;

#if VSF_DMA_CFG_REIMPLEMENT_TYPE_STATUS == DISABLED
typedef struct vsf_dma_channel_status_t {
    union {
        inherit(vsf_peripheral_status_t)
            struct {
            uint32_t is_busy : 1;
        };
    };
} vsf_dma_channel_status_t;
#endif

typedef struct vsf_dma_capability_t {
#if VSF_DMA_CFG_INHERT_HAL_CAPABILITY == ENABLED
    inherit(vsf_peripheral_capability_t)
#endif
    vsf_dma_irq_mask_t irq_mask;

    uint32_t max_request_count;
    uint8_t channel_count;
} vsf_dma_capability_t;

typedef struct vsf_dma_op_t {
#undef  __VSF_HAL_TEMPLATE_API
#define __VSF_HAL_TEMPLATE_API VSF_HAL_TEMPLATE_API_FP

    VSF_DMA_APIS(vsf)
} vsf_dma_op_t;

#if VSF_DMA_CFG_MULTI_CLASS == ENABLED
struct vsf_dma_t  {
    const vsf_dma_op_t * op;
};
#endif

/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief initialize a dma instance.
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @return vsf_err_t: VSF_ERR_NONE if dma was initialized, or a negative error code

 @note It is not necessary to call vsf_dma_fini() to deinitialization.
       vsf_dma_init() should be called before any other DMA API except vsf_dma_capability().

 \~chinese
 @brief 初始化一个 dma 实例
 @param[in] dma_ptr: 结构体 vsf_dma_t 的指针，参考 @ref vsf_dma_t
 @return vsf_err_t: 如果 dma 初始化成功返回 VSF_ERR_NONE , 失败返回负数。

 @note 失败后不需要调用 vsf_dma_fini() 反初始化。
       vsf_dma_init() 应该在除 vsf_dma_capability() 之外的其他 DMA API 之前调用。
 */
extern vsf_err_t vsf_dma_init(vsf_dma_t *dma_ptr);

/**
 \~english
 @brief finalize a dma instance.
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @return none

 \~chinese
 @brief 终止一个 dma 实例
 @param[in] dma_ptr: 结构体 vsf_dma_t 的指针，参考 @ref vsf_dma_t
 @param[in] cfg_ptr: 结构体 vsf_dma_cfg_t 的指针，参考 @ref vsf_dma_cfg_t
 @return 无。
 */
extern void vsf_dma_fini(vsf_dma_t *dma_ptr);

/**
 \~english
 @brief get the capability of dma instance.
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @return vsf_dma_capability_t: return all capability of current dma @ref vsf_dma_capability_t

 \~chinese
 @brief 获取 dma 实例的能力
 @param[in] dma_ptr: 结构体 vsf_dma_t 的指针，参考 @ref vsf_dma_t
 @return vsf_dma_capability_t: 返回当前 dma 的所有能力 @ref vsf_dma_capability_t
 */
extern vsf_dma_capability_t vsf_dma_capability(vsf_dma_t *dma_ptr);

/**
 \~english
 @brief dma request new channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @return int8_t: positive number or 0 if the dma request was successfully, or a negative error code

 \~chinese
 @brief dma 请求一个新的通道
 @param[in] dma_ptr: 结构体 vsf_dma_t 的指针，参考 @ref vsf_dma_t
 @return int8_t: 如果 dma 请求成功返回正数或者 0 , 否则返回负数。
 */
extern int8_t vsf_dma_channel_request(vsf_dma_t *dma_ptr);

/**
 \~english
 @brief dma release channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number

 \~chinese
 @brief dma 释放一个通道
 @param[in] dma_ptr: 结构体 vsf_dma_t 的指针，参考 @ref vsf_dma_t
 @param[in] channel: 通道序号
 */
extern void vsf_dma_channel_release(vsf_dma_t *dma_ptr, int8_t channel);

/**
 \~english
 @brief dma channel configuration
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] cfg_ptr: a pointer to DMA channel configuration
 @return vsf_err_t: VSF_ERR_NONE if the dma request was successfully, or a negative error code

 \~chinese
 @brief DMA 通道配置
 @param[in] dma_ptr: 结构体 vsf_dma_t 的指针，参考 @ref vsf_dma_t
 @param[in] cfg_ptr: DMA 通道配置的指针
 @return vsf_err_t: 如果 dma 请求成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_dma_channel_config(vsf_dma_t *dma_ptr, int8_t channel, vsf_dma_channel_cfg_t *cfg_ptr);

/**
 \~english
 @brief DMA starts a channel transfer
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @param[in] src_address: source address
 @param[in] dst_address: destination address
 @param[in] count: counter of data to be transferred (in byte)
 @return vsf_err_t: VSF_ERR_NONE if the dma start request was successfully, or a negative error code

 \~chinese
 @brief dma 开始一个通道的传输
 @param[in] dma_ptr: 结构体 vsf_dma_t 的指针，参考 @ref vsf_dma_t
 @param[in] channel: 通道序号
 @param[in] src_address: 源地址
 @param[in] dst_address: 目标地址
 @param[in] count: 将要传输的数据的个数 (字节单位)
 @return vsf_err_t: 如果开始 dma 传输成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_dma_channel_start(vsf_dma_t *dma_ptr, int8_t channel, uint32_t src_address, uint32_t dst_address, uint32_t count);

/**
 \~english
 @brief DMA cancels the transmission of a specific channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @return vsf_err_t: VSF_ERR_NONE if the dma cancel request was successfully, or a negative error code

 \~chinese
 @brief dma 取消特定通道的传输
 @param[in] dma_ptr: 结构体 vsf_dma_t 的指针，参考 @ref vsf_dma_t
 @param[in] channel: 通道序号
 @return vsf_err_t: 如果 dma 传输取消成功返回 VSF_ERR_NONE , 否则返回负数。
 */
extern vsf_err_t vsf_dma_channel_cancel(vsf_dma_t *dma_ptr, int8_t channel);


/**
 \~english
 @brief DMA get counter of specific channel that have been transferred
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @return uint32_t: transferred counter
 @note: If called after VSF_DMA_IRQ_MASK_CPL interrupt trigger, it should return 0.
 @note: It is usually called after call vsf_dma_channel_cancel() to get the counter that has been transferred.

 \~chinese
 @brief dma 获取特定通道已经传输的数量
 @param[in] dma_ptr: 结构体 vsf_dma_t 的指针，参考 @ref vsf_dma_t
 @param[in] channel: 通道序号
 @return uint32_t: 已经传输的数量
 @note: 如果在VSF_DMA_IRQ_MASK_CPL中断触发之后调用，它应该返回 0.
 @note: 它应该在调用 vsf_dma_channel_cancel() 之后调用，用来获取已经传输的数量。
 */
extern uint32_t vsf_dma_channel_get_transferred_count(vsf_dma_t *dma_ptr, int8_t channel);

/**
 \~english
 @brief dma release channel
 @param[in] dma_ptr: a pointer to structure @ref vsf_dma_t
 @param[in] channel: channel number
 @return vsf_dma_channel_status_t: dma channel status

 \~chinese
 @brief dma 释放一个通道
 @param[in] dma_ptr: 结构体 vsf_dma_t 的指针，参考 @ref vsf_dma_t
 @param[in] channel: 通道序号
 @return vsf_dma_channel_status_t: dma 通道状态
 */
extern vsf_dma_channel_status_t vsf_dma_channel_status(vsf_dma_t *dma_ptr, int8_t channel);

/*============================ MACROFIED FUNCTIONS ===========================*/

#if VSF_DMA_CFG_FUNCTION_RENAME == ENABLED
#   define __vsf_dma_t                                          VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_t)
#   define vsf_dma_init(__DMA, ...)                             VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_init)                             ((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_fini(__DMA)                                  VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_fini)                             ((__vsf_dma_t *)(__DMA))
#   define vsf_dma_capability(__DMA)                            VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_capability)                       ((__vsf_dma_t *)(__DMA))
#   define vsf_dma_channel_request(__DMA, ...)                  VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_request)                  ((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_release(__DMA, ...)                  VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_release)                  ((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_config(__DMA, ...)                   VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_config)                   ((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_start(__DMA, ...)                    VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_start)                    ((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_cancel(__DMA, ...)                   VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_cancel)                   ((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_get_transferred_count(__DMA, ...)    VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_get_transferred_count)    ((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#   define vsf_dma_channel_status(__DMA, ...)                   VSF_MCONNECT(VSF_DMA_CFG_PREFIX, _dma_channel_status)                   ((__vsf_dma_t *)(__DMA), ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif  /*__VSF_TEMPLATE_DMA_H__*/
