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

#ifndef __HAL_DRIVER_MMC_PROBE_H__
#define __HAL_DRIVER_MMC_PROBE_H__

/*============================ INCLUDES ======================================*/

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_MMC == ENABLED

#if     defined(__VSF_MMC_PROBE_CLASS_IMPLEMENT)
#   define __VSF_CLASS_IMPLEMENT__
#endif

#include "utilities/ooc_class.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef enum vsf_mmc_probe_state_t {
    VSF_MMC_PROBE_STATE_GO_IDLE,
    VSF_MMC_PROBE_STATE_SEND_IF_COND,
} vsf_mmc_probe_state_t;

vsf_class(vsf_mmc_probe_t) {
    public_member(
        uint32_t                voltage;
    )
    private_member(
        vsf_mmc_probe_state_t   state;
    )
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

/**
 \~english
 @brief start mmc probe, mmc MUST be initialized by calling vsf_mmc_init first.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] vsf_mmc_probe_t: a pointer to struct @ref vsf_mmc_probe_t
 @return vsf_err_t: on success，returns VSF_ERR_NONE(0); on error, returns err code(< 0)

 \~chinese
 @brief 启动 mmc 外设检测, mmu 必须是已经通过 vsf_mmc_init 初始化的。
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] vsf_mmc_probe_t: 结构体 vsf_mmc_probe_t 的指针，参考 @ref vsf_mmc_probe_t
 @return vsf_err_t: 如果检测成功，返回 VSF_ERR_NONE(零); 如果出错, 返回负值错误码
 */
extern vsf_err_t vsf_mmc_probe_start(vsf_mmc_t *mmc, vsf_mmc_probe_t *probe);

/**
 \~english
 @brief called in mmc irqhandler while probing mmc.
 @param[in] mmc_ptr: a pointer to structure @ref vsf_mmc_t
 @param[in] vsf_mmc_probe_t: a pointer to struct @ref vsf_mmc_probe_t
 @param[in] irq_mask one or more value of enum vsf_mmc_irq_mask_t
 @param[in] status transact status.
 @param[in] resp response.
 @return vsf_err_t: on success，returns VSF_ERR_NONE(0); on going, returns VSF_ERR_NOT_READY(> 0); on error, returns err code(< 0)

 \~chinese
 @brief 检测 mmc 外设的时候，在 mmc 中断中调用的中断处理原函数
 @param[in] mmc_ptr: 结构体 vsf_mmc_t 的指针，参考 @ref vsf_mmc_t
 @param[in] vsf_mmc_probe_t: 结构体 vsf_mmc_probe_t 的指针，参考 @ref vsf_mmc_probe_t
 @param[in] irq_mask 一个或者多个枚举 vsf_mmc_irq_mask_t 的值的按位或
 @param[in] status 传输状态
 @param[in] resp 应答
 @return vsf_err_t: 如果检测成功，返回 VSF_ERR_NONE(零); 如果未完成, 返回 VSF_ERR_NOT_READY(正值); 如果出错, 返回负值错误码
 */
extern vsf_err_t vsf_mmc_probe_irqhandler(vsf_mmc_t *mmc, vsf_mmc_probe_t *probe,
        vsf_mmc_irq_mask_t irq_mask, vsf_mmc_transact_status_t status,
        uint32_t resp[4]);

#ifdef __cplusplus
}
#endif

#endif /* VSF_HAL_USE_MMC */
#endif /* __HAL_DRIVER_MMC_PROBE_H__ */
