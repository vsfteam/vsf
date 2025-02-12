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

#include "hal/vsf_hal_cfg.h"

#if VSF_HAL_USE_ADC == ENABLED && VSF_HAL_USE_ADC128D818_ADC == ENABLED

/*\note For IPCore drivers, define __VSF_HAL_ADC128D818_ADC_CLASS_IMPLEMENT before including vsf_hal.h.
 *      For peripheral drivers, if IPCore driver is used, define __VSF_HAL_ADC128D818_ADC_CLASS_INHERIT__ before including vsf_hal.h
 */

// IPCore
#define __VSF_HAL_ADC128D818_ADC_CLASS_IMPLEMENT
// IPCore end

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

// IPCore
#define VSF_ADC_CFG_IMP_PREFIX                  vsf_adc128d818
#define VSF_ADC_CFG_IMP_UPCASE_PREFIX           VSF_ADC128D818
// IPCore end

/*============================ TYPES =========================================*/

enum {
    ADC128D818_REG_CONFIGURATION                = 0x00,
    ADC128D818_REG_INTERRUPT_STATUS             = 0x01,
    ADC128D818_REG_INTERRUPT_MASK               = 0x03,
    ADC128D818_REG_CONVERSION_RATE              = 0x07,
    ADC128D818_REG_CHANNEL_DISABLED             = 0x08,
    ADC128D818_REG_ONE_SHOT                     = 0x09,
    ADC128D818_REG_DEEP_SHUTDOWN                = 0x0A,
    ADC128D818_REG_ADVANCED_CONFIGURATION       = 0x0B,
    ADC128D818_REG_BUSY_STATUS                  = 0x0C,
    ADC128D818_REG_CHANNEL_READINGS             = 0x20,
    ADC128D818_REG_LIMIT                        = 0x2A,
    ADC128D818_REG_MANUFACTURER_ID              = 0x3E,
    ADC128D818_REG_REVISION_ID                  = 0x3F,
};

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*\note Implementation below is for hw adc only, because there is no requirements
 *          on the APIs of IPCore drivers.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

static void VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_config_channels)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr)
{
    adc_ptr->cur_reg = ADC128D818_REG_CHANNEL_DISABLED;
    adc_ptr->data_buffer[0] = ~adc_ptr->cur_channel_mask;
    vsf_i2c_master_request(adc_ptr->i2c, adc_ptr->i2c_addr,
            VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP, 2, &adc_ptr->cur_reg);
}

// HW
static void VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_i2c_irqhandler)(
    void *target_ptr, vsf_i2c_t *i2c_ptr, vsf_i2c_irq_mask_t irq_mask)
{
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr = target_ptr;

    if (irq_mask != VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE) {
        vsf_trace_error("adc128d818: i2c failure" VSF_TRACE_CFG_LINEEND);
        return;
    }

    if (adc_ptr->rx_byte_len > 0) {
        uint8_t rx_byte_len = adc_ptr->rx_byte_len;
        adc_ptr->rx_byte_len = 0;
        vsf_i2c_master_request(i2c_ptr, adc_ptr->i2c_addr,
            VSF_I2C_CMD_RESTART | VSF_I2C_CMD_READ | VSF_I2C_CMD_STOP,
            rx_byte_len, (uint8_t *)adc_ptr->data_buffer);
        return;
    }

    switch (adc_ptr->cur_reg) {
    case ADC128D818_REG_BUSY_STATUS:
        if (adc_ptr->is_inited) {
            if (adc_ptr->data & (1 << 0)) {
                goto read_status;
            }
            adc_ptr->cur_channel = 0;

        read_result:
            if (!adc_ptr->cur_channel_mask) {
                adc_ptr->cur_channel_mask = adc_ptr->configured_channel_mask;
                goto one_shot;
            } else {
                uint8_t idx = adc_ptr->channel_seq_map[adc_ptr->cur_channel++];
                adc_ptr->total_count--;
                adc_ptr->cur_channel_mask &= ~(1 << idx);
                adc_ptr->cur_reg = ADC128D818_REG_CHANNEL_READINGS + idx;
                adc_ptr->rx_byte_len = 2;
                vsf_i2c_master_request(i2c_ptr, adc_ptr->i2c_addr,
                    VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE, 1, &adc_ptr->cur_reg);
            }
        } else {
            if (adc_ptr->data & (1 << 1)) {
            read_status:
                adc_ptr->rx_byte_len = 1;
                vsf_i2c_master_request(i2c_ptr, adc_ptr->i2c_addr,
                    VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE, 1, &adc_ptr->cur_reg);
                break;
            }

            adc_ptr->cur_reg = ADC128D818_REG_ADVANCED_CONFIGURATION;
            adc_ptr->data_buffer[0] = (1 << 0) /* External Reference Enable */
                                    | (1 << 1) /* Mode1: IN0 - IN7 */;
            vsf_i2c_master_request(i2c_ptr, adc_ptr->i2c_addr,
                VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP,
                2, &adc_ptr->cur_reg);
        }
        break;
    case ADC128D818_REG_ADVANCED_CONFIGURATION: {
            uint8_t cur_channel_mask;
            vsf_protect_t orig = vsf_protect_int();
                adc_ptr->is_inited = true;
                cur_channel_mask = adc_ptr->cur_channel_mask;
            vsf_unprotect_int(orig);

            if (cur_channel_mask != 0) {
                VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_config_channels)(adc_ptr);
            }
        }
        break;
    case ADC128D818_REG_CHANNEL_DISABLED:
    one_shot:
        adc_ptr->cur_reg = ADC128D818_REG_ONE_SHOT;
        adc_ptr->data_buffer[0] = 1;
        vsf_i2c_master_request(i2c_ptr, adc_ptr->i2c_addr,
            VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE | VSF_I2C_CMD_STOP,
            2, &adc_ptr->cur_reg);
        break;
    case ADC128D818_REG_ONE_SHOT:
        adc_ptr->cur_reg = ADC128D818_REG_BUSY_STATUS;
        goto read_status;
    case ADC128D818_REG_CHANNEL_READINGS + 0:
    case ADC128D818_REG_CHANNEL_READINGS + 1:
    case ADC128D818_REG_CHANNEL_READINGS + 2:
    case ADC128D818_REG_CHANNEL_READINGS + 3:
    case ADC128D818_REG_CHANNEL_READINGS + 4:
    case ADC128D818_REG_CHANNEL_READINGS + 5:
    case ADC128D818_REG_CHANNEL_READINGS + 6:
    case ADC128D818_REG_CHANNEL_READINGS + 7:
        *adc_ptr->result_buffer++ = be16_to_cpu(adc_ptr->data) >> 4;
        if (adc_ptr->total_count > 0) {
            goto read_result;
        } else if ( (adc_ptr->irq_mask & VSF_ADC_IRQ_MASK_CPL)
                &&  (adc_ptr->isr.handler_fn != NULL)) {
            adc_ptr->isr.handler_fn(adc_ptr->isr.target_ptr, (vsf_adc_t *)adc_ptr,
                VSF_ADC_IRQ_MASK_CPL);
        }
    }
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_init)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_cfg_t *cfg_ptr
) {
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != cfg_ptr));

    vsf_i2c_t *i2c = adc_ptr->i2c;
    // configure according to cfg_ptr
    adc_ptr->cur_channel_mask = 0;
    adc_ptr->total_count = 0;
    adc_ptr->result_buffer = NULL;
    adc_ptr->isr = cfg_ptr->isr;
    adc_ptr->is_continuous_mode = (cfg_ptr->mode & VSF_ADC_SCAN_CONV_MASK) == VSF_ADC_SCAN_CONV_SEQUENCE_MODE;
    adc_ptr->is_busy = false;
    adc_ptr->is_inited = false;

    vsf_i2c_init(i2c, &(vsf_i2c_cfg_t){
        .mode           = VSF_I2C_MODE_MASTER | VSF_I2C_SPEED_FAST_MODE | VSF_I2C_ADDR_7_BITS,
        .clock_hz       = 400 * 1000,
        .isr            = {
            .handler_fn = VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_i2c_irqhandler),
            .target_ptr = adc_ptr,
            .prio       = cfg_ptr->isr.prio,
        },
    });
    vsf_i2c_irq_enable(i2c, VSF_I2C_IRQ_MASK_MASTER_TRANSFER_COMPLETE
                        |   VSF_I2C_IRQ_MASK_MASTER_ADDRESS_NACK
                        |   VSF_I2C_IRQ_MASK_MASTER_TX_NACK_DETECT);
    vsf_i2c_enable(i2c);

    adc_ptr->cur_reg = ADC128D818_REG_BUSY_STATUS;
    adc_ptr->rx_byte_len = 1;
    vsf_i2c_master_request(i2c, adc_ptr->i2c_addr,
            VSF_I2C_CMD_START | VSF_I2C_CMD_WRITE, 1, &adc_ptr->cur_reg);
    return VSF_ERR_NONE;
}

void VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_fini)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    VSF_HAL_ASSERT(adc_ptr != NULL);
}

fsm_rt_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_enable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);
    return fsm_rt_cpl;
}

fsm_rt_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_disable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);
    return fsm_rt_cpl;
}

void VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_irq_enable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);
    adc_ptr->irq_mask |= irq_mask;
}

void VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_irq_disable)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_irq_mask_t irq_mask
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);
    adc_ptr->irq_mask &= ~irq_mask;
}

vsf_adc_status_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_status)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    VSF_HAL_ASSERT(NULL != adc_ptr);
    return (vsf_adc_status_t) {
        .is_busy                = adc_ptr->is_busy,
    };
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_request_once)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_channel_cfg_t *channel_cfg_ptr,
    void *buffer_ptr
) {
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != buffer_ptr) && (NULL != channel_cfg_ptr));
    VSF_HAL_ASSERT(channel_cfg_ptr->channel < 8);
    VSF_HAL_ASSERT(!adc_ptr->is_continuous_mode);

    uint8_t cur_channel_mask = 1 << channel_cfg_ptr->channel;
    adc_ptr->result_buffer = buffer_ptr;
    adc_ptr->total_count = 1;
    vsf_protect_t orig = vsf_protect_int();
        adc_ptr->cur_channel_mask = cur_channel_mask;
        if (!adc_ptr->is_inited) {
            cur_channel_mask = 0;
        }
    vsf_unprotect_int(orig);

    if (cur_channel_mask > 0) {
        VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_config_channels)(adc_ptr);
    }
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_config)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    vsf_adc_channel_cfg_t *channel_cfgs_ptr,
    uint32_t channel_cfgs_cnt
) {
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != channel_cfgs_ptr));
    VSF_HAL_ASSERT(0 != channel_cfgs_cnt);
    VSF_HAL_ASSERT(adc_ptr->is_continuous_mode);

    uint8_t configured_channel_mask = 0;
    for (uint32_t i = 0; i < channel_cfgs_cnt; i++, channel_cfgs_ptr++) {
        VSF_HAL_ASSERT(channel_cfgs_ptr->channel < 8);
        adc_ptr->channel_seq_map[i] = channel_cfgs_ptr->channel;
        configured_channel_mask |= 1 << channel_cfgs_ptr->channel;
    }
    adc_ptr->configured_channel_mask = configured_channel_mask;
    return VSF_ERR_NONE;
}

vsf_err_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_channel_request)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr,
    void *buffer_ptr,
    uint_fast32_t count
) {
    VSF_HAL_ASSERT((NULL != adc_ptr) && (NULL != buffer_ptr) && (count > 0));
    VSF_HAL_ASSERT(adc_ptr->is_continuous_mode);

    uint8_t cur_channel_mask = adc_ptr->configured_channel_mask;
    adc_ptr->result_buffer = buffer_ptr;
    adc_ptr->total_count = count;
    vsf_protect_t orig = vsf_protect_int();
        adc_ptr->cur_channel_mask = cur_channel_mask;
        if (!adc_ptr->is_inited) {
            cur_channel_mask = 0;
        }
    vsf_unprotect_int(orig);

    if ((cur_channel_mask > 0) && (count > 0)) {
        VSF_MCONNECT(__, VSF_ADC_CFG_IMP_PREFIX, _adc_config_channels)(adc_ptr);
    }

    return VSF_ERR_NONE;
}

vsf_adc_capability_t VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_capability)(
    VSF_MCONNECT(VSF_ADC_CFG_IMP_PREFIX, _adc_t) *adc_ptr
) {
    return (vsf_adc_capability_t) {
        .irq_mask           = VSF_ADC_IRQ_MASK_CPL,
        .max_data_bits      = 12,
        .channel_count      = 8,
    };
}
// HW end

/*============================ MACROFIED FUNCTIONS ===========================*/

/*\note Instantiation below is for hw adc only.
 *      Usage of VSF_MCONNECT is not a requirement, but for convenience only,
 */

// HW
#define VSF_ADC_CFG_REIMPLEMENT_API_CAPABILITY  ENABLED
#define VSF_ADC_CFG_IMP_EXTERN_OP               ENABLED
#include "hal/driver/common/adc/adc_template.inc"
// HW end

#endif /* VSF_HAL_USE_ADC */
