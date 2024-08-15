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

#ifndef __HAL_DRIVER_SDIO_PROBE_H__
#define __HAL_DRIVER_SDIO_PROBE_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_SDIO == ENABLED

#undef public_const
#if     defined(__VSF_SDIO_PROBE_CLASS_IMPLEMENT)
#   undef __VSF_SDIO_PROBE_CLASS_IMPLEMENT
#   define __VSF_CLASS_IMPLEMENT__
#   define public_const
#else
#   define public_const const
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

vsf_class(vsf_sdio_probe_t) {
    public_member(
        uint32_t                    voltage;
        uint32_t                    working_clock_hz;
        uint8_t                     bus_width;
        bool                        uhs_en;

        public_const bool           high_capacity;
        public_const uint8_t        delay_ms;
        public_const uint32_t       version;
        public_const uint32_t       ocr;
        public_const uint64_t       capacity;
        public_const vsf_sdio_csd_t csd;
        public_const vsf_sdio_cid_t cid;
    )
    private_member(
        uint8_t                     state;
        uint8_t                     is_app_cmd : 1;
        uint8_t                     is_resp_r1 : 1;
        uint8_t                     is_to_retry : 1;
        uint8_t                     is_to_ignore_fail : 1;
        uint32_t                    rca;
        uint32_t                    r1_expected_card_status_mask;
        uint32_t                    r1_expected_card_status;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief start sdio probe, SDIO MUST be initialized by calling vsf_sdio_init first.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] vsf_sdio_probe_t: a pointer to struct @ref vsf_sdio_probe_t
 @return vsf_err_t: on success, returns VSF_ERR_NONE(0); on error, returns err code(< 0)

 \~chinese
 @brief 启动 sdio 外设检测, SDIO 必须已经经过 vsf_sdio_init 完成初始化。
 @param[in] sdio_ptr: 结构体 vsf_sdio_t 的指针, 参考 @ref vsf_sdio_t
 @param[in] vsf_sdio_probe_t sdio 实例的指针
 @return vsf_err_t: 操作完成返回 VSF_ERR_NONE(0); 出错返回错误码 (< 0)。
 */
extern vsf_err_t vsf_sdio_probe_start(vsf_sdio_t *sdio, vsf_sdio_probe_t *probe);

/**
 \~english
 @brief called in sdio irqhandler while probing sdio.
 @note if probe->delay_ms is none zero after returned with VSF_ERR_NOT_READY,
        vsf_sdio_probe_irqhandler(irq_mask: 0, status: 0) should be called again after delay_ms delayed.
 @param[in] sdio_ptr: a pointer to structure @ref vsf_sdio_t
 @param[in] vsf_sdio_probe_t: a pointer to struct @ref vsf_sdio_probe_t
 @param[in] irq_mask: one or more value of enum vsf_sdio_irq_mask_t
 @param[in] status: transact status.
 @param[in] resp: response.
 @return vsf_err_t: on success, returns VSF_ERR_NONE(0); on going, returns VSF_ERR_NOT_READY(> 0); on error, returns err code(< 0)

 \~chinese
 @brief 检测 sdio 外设的时候，在 sdio 传输完成中断中调用的中断处理函数
 @note 如果调用返回 VSF_ERR_NOT_READY 后， probe->delay_ms 非零,
        需要在 delay_ms 等待完成之后，再次调用 vsf_sdio_probe_irqhandler(irq_mask: 0, status: 0).
 @param[in] sdio_ptr: 结构体 vsf_sdio_t 的指针, 参考 @ref vsf_sdio_t
 @param[in] vsf_sdio_probe_t sdio_probe 实例的指针
 @param[in] irq_mask: 一个或者多个枚举 vsf_sdio_irq_mask_t 的值的按位或
 @param[in] status: 传输状态
 @param[in] resp: 应答
 @return vsf_err_t: 操作完成返回 VSF_ERR_NONE(0); 未完成返回 VSF_ERR_NOT_READY(> 0); 出错返回错误码 (< 0)。
 */
extern vsf_err_t vsf_sdio_probe_irqhandler(vsf_sdio_t *sdio, vsf_sdio_probe_t *probe,
        vsf_sdio_irq_mask_t irq_mask, vsf_sdio_transact_status_t status,
        uint32_t resp[4]);

#ifdef __cplusplus
}
#endif

#endif /* VSF_HAL_USE_SDIO */
#endif /* __HAL_DRIVER_SDIO_PROBE_H__ */
