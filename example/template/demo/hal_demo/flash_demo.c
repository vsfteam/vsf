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

#include "vsf.h"

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_FLASH_DEMO == ENABLED && VSF_HAL_USE_FLASH == ENABLED

#if VSF_USE_LINUX == ENABLED
#   include <getopt.h>
#endif

#include "hal_demo.h"

/*============================ MACROS ========================================*/

#ifdef APP_FLASH_DEMO_CFG_DEFAULT_INSTANCE_PREFIX
#   undef VSF_FLASH_CFG_PREFIX
#   define VSF_FLASH_CFG_PREFIX                         APP_FLASH_DEMO_CFG_DEFAULT_INSTANCE_PREFIX
#endif

#ifndef APP_FLASH_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_FLASH_DEMO_CFG_DEFAULT_INSTANCE          vsf_hw_flash0
#endif

#ifndef APP_FLASH_DEMO_CFG_DEVICES_COUNT
#   define APP_FLASH_DEMO_CFG_DEVICES_COUNT             1
#endif

#ifndef APP_FLASH_DEMO_CFG_DEVICES_ARRAY_INIT
#   define APP_FLASH_DEMO_CFG_DEVICES_ARRAY_INIT        \
        { .cnt = dimof(vsf_hw_flash_devices), .devices = vsf_hw_flash_devices},
#endif

#ifndef APP_FLASH_DEMO_CFG_OFFSET
#   define APP_FLASH_DEMO_CFG_OFFSET                    0x001FB000
#endif

#ifndef APP_FLASH_DEMO_CFG_SIZE
#   define APP_FLASH_DEMO_CFG_SIZE                      4096
#endif

/*============================ IMPLEMENTATION ================================*/
/*============================ TYPES =========================================*/

typedef enum METHOD_t {
    METHOD_NO_ERASE         = 0x00 << 0,
    METHOD_ERASE_ONE        = 0x01 << 0,
    METHOD_ERASE_MULTI      = 0x02 << 0,
    METHOD_ERASE_ALL        = 0x03 << 0,
    METHOD_ERASE_MASK       = 0x03 << 0,

    METHOD_NO_WRITE         = 0x00 << 2,
    METHOD_WRITE_ONE        = 0x01 << 2,
    METHOD_WRITE_MULTI      = 0x02 << 2,
    METHOD_WRITE_ALL        = 0x03 << 2,
    METHOD_WRITE_MASK       = 0x03 << 2,

    METHOD_NO_READ          = 0x00 << 4,
    METHOD_READ_ONE         = 0x01 << 4,
    METHOD_READ_MULTI       = 0x02 << 4,
    METHOD_READ_ALL         = 0x03 << 4,
    METHOD_READ_MASK        = 0x03 << 4,

    METHOD_ERASE_WAIT_POLL  = 0x00 << 5,
    METHOD_ERASE_WAIT_ISR   = 0x01 << 5,
    METHOD_ERASE_WAIT_MASK  = 0x01 << 5,

    METHOD_WRITE_WAIT_POLL  = 0x00 << 6,
    METHOD_WRITE_WAIT_ISR   = 0x01 << 6,
    METHOD_WRITE_WAIT_MASK  = 0x01 << 6,

    METHOD_READ_WAIT_POLL   = 0x00 << 7,
    METHOD_READ_WAIT_ISR    = 0x01 << 7,
    METHOD_READ_WAIT_MASK   = 0x01 << 7,
} METHOD_t;

typedef enum flash_demo_evt_t {
    VSF_EVT_FLASH_ERASE = __VSF_EVT_HAL_LAST,
    VSF_EVT_FLASH_ERASE_WAIT_POLL,
    VSF_EVT_FLASH_ERASE_WAIT_ISR,

    VSF_EVT_FLASH_WRITE,
    VSF_EVT_FLASH_WRITE_WAIT_POLL,
    VSF_EVT_FLASH_WRITE_WAIT_ISR,

    VSF_EVT_FLASH_READ,
    VSF_EVT_FLASH_READ_WAIT_POLL,
    VSF_EVT_FLASH_READ_WAIT_ISR,
} flash_demo_evt_t;

typedef struct flash_test_t  {
    implement(hal_test_t)

    vsf_flash_cfg_t cfg;
    uint32_t size;
    uint32_t offset;

    struct {
        uint32_t erase_sector_size;
        uint32_t sector_size;

        uint32_t erase_offset;
        uint32_t write_offset;
        uint32_t read_offset;
    };
} flash_test_t;

typedef struct flash_demo_const_t {
    implement(hal_demo_const_t)
    flash_test_t test;
} flash_demo_const_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/

static const hal_option_t __irq_options[] = {
    HAL_DEMO_OPTION(VSF_FLASH_IRQ_ERASE_MASK, VSF_FLASH_IRQ_ERASE_MASK),
    HAL_DEMO_OPTION(VSF_FLASH_IRQ_WRITE_MASK, VSF_FLASH_IRQ_WRITE_MASK),
    HAL_DEMO_OPTION(VSF_FLASH_IRQ_READ_MASK, VSF_FLASH_IRQ_READ_MASK),
    HAL_DEMO_OPTION(VSF_FLASH_IRQ_ERASE_ERROR_MASK, VSF_FLASH_IRQ_ERASE_ERROR_MASK),
    HAL_DEMO_OPTION(VSF_FLASH_IRQ_WRITE_ERROR_MASK, VSF_FLASH_IRQ_WRITE_ERROR_MASK),
    HAL_DEMO_OPTION(VSF_FLASH_IRQ_READ_ERROR_MASK, VSF_FLASH_IRQ_READ_ERROR_MASK),
};

static const hal_option_t __method_options[] = {
    HAL_DEMO_OPTION_EX(METHOD_ERASE_MASK, METHOD_NO_ERASE,    "no-erase"),
    HAL_DEMO_OPTION_EX(METHOD_ERASE_MASK, METHOD_ERASE_ONE,   "erase-one"),
    HAL_DEMO_OPTION_EX(METHOD_ERASE_MASK, METHOD_ERASE_MULTI, "erase-multi"),
    HAL_DEMO_OPTION_EX(METHOD_ERASE_MASK, METHOD_ERASE_ALL,   "erase-all"),
    HAL_DEMO_OPTION_EX(METHOD_ERASE_WAIT_MASK, METHOD_ERASE_WAIT_POLL, "erase-poll"),
    HAL_DEMO_OPTION_EX(METHOD_ERASE_WAIT_MASK, METHOD_ERASE_WAIT_ISR, "erase-isr"),

    HAL_DEMO_OPTION_EX(METHOD_WRITE_MASK, METHOD_NO_WRITE,    "no-write"),
    HAL_DEMO_OPTION_EX(METHOD_WRITE_MASK, METHOD_WRITE_ONE,   "write-one"),
    HAL_DEMO_OPTION_EX(METHOD_WRITE_MASK, METHOD_WRITE_MULTI, "write-multi"),
    HAL_DEMO_OPTION_EX(METHOD_WRITE_MASK, METHOD_WRITE_ALL,   "write-all"),
    HAL_DEMO_OPTION_EX(METHOD_WRITE_WAIT_MASK, METHOD_WRITE_WAIT_POLL, "write-poll"),
    HAL_DEMO_OPTION_EX(METHOD_WRITE_WAIT_MASK, METHOD_WRITE_WAIT_ISR, "write-isr"),

    HAL_DEMO_OPTION_EX(METHOD_READ_MASK, METHOD_NO_READ,      "no-read"),
    HAL_DEMO_OPTION_EX(METHOD_READ_MASK, METHOD_READ_ONE,     "read-one"),
    HAL_DEMO_OPTION_EX(METHOD_READ_MASK, METHOD_READ_MULTI,   "read-multi"),
    HAL_DEMO_OPTION_EX(METHOD_READ_MASK, METHOD_READ_ALL,     "read-all"),
    HAL_DEMO_OPTION_EX(METHOD_READ_WAIT_MASK, METHOD_READ_WAIT_POLL, "read-poll"),
    HAL_DEMO_OPTION_EX(METHOD_READ_WAIT_MASK, METHOD_READ_WAIT_ISR, "read-isr"),
};

HAL_DEMO_INIT(flash, APP_FLASH,
    "flash-test" VSF_TRACE_CFG_LINEEND
    "  -h, --help                     show this screen and exit" VSF_TRACE_CFG_LINEEND
    "  -v, --verbose [level]          verbose show parameters" VSF_TRACE_CFG_LINEEND
    "  -l, --list-device              list device" VSF_TRACE_CFG_LINEEND
    "  -m, --method [no-erase|erase-one|erase-multi|erase-all] [erase-poll|erase-isr]" VSF_TRACE_CFG_LINEEND
    "               [no-write|write-one|write-multil] [write-poll|write-isr]" VSF_TRACE_CFG_LINEEND
    "               [no-read|read-one|read-multil] [read-poll|read-isr]" VSF_TRACE_CFG_LINEEND
    "  -d, --device DEVICE            vsf_hw_flash0" VSF_TRACE_CFG_LINEEND
    "  -r, --repeat REPEAT            repeat count" VSF_TRACE_CFG_LINEEND
    "  -p, --prio vsf_prio_X          test task priority, @ref \"vsf_prio_t\"" VSF_TRACE_CFG_LINEEND
    "  -i, --isr_prio vsf_arch_prio_X interrupt priority, @ref \"vsf_arch_prio_0\"" VSF_TRACE_CFG_LINEEND
    "  -t, --timeout TIMEOUT          timeout of flash interrupt" VSF_TRACE_CFG_LINEEND
    "  -o, --offset OFFSET            @ref offset_of_bytes in vsf_flash_[erase|write|read].*" VSF_TRACE_CFG_LINEEND
    "  -s, --size  SIZE               @ref size_of_bytes in vsf_flash_[erase|write|read].*y" VSF_TRACE_CFG_LINEEND,

    .test.method = METHOD_ERASE_ONE | METHOD_ERASE_WAIT_POLL |
                   METHOD_WRITE_ONE | METHOD_WRITE_WAIT_POLL |
                   METHOD_READ_ONE  | METHOD_READ_WAIT_POLL,

    .init_has_cfg       = true,
    .device_init        = (hal_init_fn_t       )vsf_flash_init,
    .device_fini        = (hal_fini_fn_t       )vsf_flash_fini,
    .device_enable      = (hal_enable_fn_t     )vsf_flash_enable,
    .device_disable     = (hal_disable_fn_t    )vsf_flash_disable,
    .device_irq_enable  = (hal_irq_enable_fn_t )vsf_flash_irq_enable,
    .device_irq_disable = (hal_irq_disable_fn_t)vsf_flash_irq_disable,

    .mode.options       = NULL,
    .mode.cnt           = 0,

    .irq.options        = __irq_options,
    .irq.cnt            = dimof(__irq_options),

    .method.options     = __method_options,
    .method.cnt         = dimof(__method_options),
);

/*============================ IMPLEMENTATION ================================*/

static void __flash_isr_handler(void *target_ptr, vsf_flash_t *flash_ptr, vsf_flash_irq_mask_t irq_mask)
{
    flash_test_t *test = (flash_test_t *)target_ptr;
    VSF_ASSERT(test != NULL);
    VSF_ASSERT(flash_ptr != NULL);
    vsf_eda_t *eda = &test->teda.use_as__vsf_eda_t;

    if (!hal_test_irq_check(&test->use_as__hal_test_t, irq_mask)) {
        return;
    }

    if (irq_mask & VSF_FLASH_IRQ_ERASE_MASK) {
        test->expected_irq_mask &= ~VSF_FLASH_IRQ_ERASE_MASK;
        vsf_eda_post_evt(eda, VSF_EVT_FLASH_ERASE_WAIT_ISR);
    }

    if (irq_mask & VSF_FLASH_IRQ_WRITE_MASK) {
        test->expected_irq_mask &= ~VSF_FLASH_IRQ_WRITE_MASK;
        vsf_eda_post_evt(eda, VSF_EVT_FLASH_WRITE_WAIT_ISR);
    }

    if (irq_mask & VSF_FLASH_IRQ_READ_MASK) {
        test->expected_irq_mask &= ~VSF_FLASH_IRQ_READ_MASK;
        vsf_eda_post_evt(eda, VSF_EVT_FLASH_READ_WAIT_ISR);
    }
}

static bool __flash_demo_check(hal_test_t *hal_test)
{
    VSF_ASSERT(hal_test != NULL);
    flash_test_t *test = vsf_container_of(hal_test, flash_test_t, use_as__hal_test_t);

    VSF_ASSERT(test->device != NULL);

    vsf_flash_capability_t cap = vsf_flash_capability(test->device);
    if (test->size > cap.max_size) {
        vsf_trace_error("programming size(%u) too big, max size: %d!" VSF_TRACE_CFG_LINEEND,
                        test->size, cap.max_size);
        return false;
    }
    if (test->size < cap.erase_sector_size) {
        vsf_trace_warning("test warning: programming size(%u) is smaller than the minimum erase size(%u)." VSF_TRACE_CFG_LINEEND,
                          test->size, cap.erase_sector_size);
    }

    if (!cap.can_write_any_address) {
        if (test->offset % cap.write_sector_size) {
            vsf_trace_error("programming address(%u) must be aligned  to the sector size(%u)" VSF_TRACE_CFG_LINEEND,
                            test->size, cap.write_sector_size);
            return false;
        }
        if (test->size % cap.write_sector_size) {
            vsf_trace_error("programming size(%u) must be an integer multiple of the sector size(%u)" VSF_TRACE_CFG_LINEEND,
                            test->size, cap.write_sector_size);
            return false;
        }
    } else {
        if (test->size < cap.write_sector_size) {
            vsf_trace_warning("test warning: programming size(%u) is smaller than the minimum write size(%u)." VSF_TRACE_CFG_LINEEND,
                              test->size, cap.erase_sector_size);
        }
    }

    test->erase_sector_size = cap.erase_sector_size;
    test->sector_size = cap.write_sector_size;

    if ((test->method & METHOD_ERASE_WAIT_MASK) == METHOD_ERASE_WAIT_ISR) {
        test->irq_mask = VSF_FLASH_IRQ_ERASE_MASK | VSF_FLASH_IRQ_ERASE_ERROR_MASK;
        test->expected_irq_mask = VSF_FLASH_IRQ_ERASE_MASK;
    }
    if ((test->method & METHOD_WRITE_WAIT_MASK) == METHOD_WRITE_WAIT_ISR) {
        test->irq_mask = VSF_FLASH_IRQ_WRITE_MASK | VSF_FLASH_IRQ_WRITE_ERROR_MASK;
        test->expected_irq_mask = VSF_FLASH_IRQ_WRITE_MASK;
    }
    if ((test->method & METHOD_READ_WAIT_MASK) == METHOD_READ_WAIT_ISR) {
        test->irq_mask = VSF_FLASH_IRQ_READ_MASK | VSF_FLASH_IRQ_READ_ERROR_MASK;
        test->expected_irq_mask = VSF_FLASH_IRQ_READ_MASK;
    }
    if (test->irq_mask != 0) {
        test->cfg.isr.handler_fn = __flash_isr_handler;
        test->cfg.isr.target_ptr = test;
    }

    test->hal_cfg = (void *)&test->cfg;

    return true;
}

static vsf_err_t __flash_erase_one_sector(flash_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_flash_t *flash_ptr = test->device;
    VSF_ASSERT(flash_ptr != NULL);

    vsf_err_t err = vsf_flash_erase_one_sector(flash_ptr, test->erase_offset);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_flash_erase_one_sector(&%s, 0x%08x/*offset*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->erase_offset, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("flash erase one sector faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __flash_erase_multi_sector(flash_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_flash_t *flash_ptr = test->device;
    VSF_ASSERT(flash_ptr != NULL);

    vsf_err_t err = vsf_flash_erase_multi_sector(flash_ptr, test->offset, test->size);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_flash_erase_multi_sector(&%s, 0x%08x/*offset*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->offset, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("flash erase multi sector faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __flash_erase_all(flash_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_flash_t *flash_ptr = test->device;
    VSF_ASSERT(flash_ptr != NULL);

    vsf_err_t err = vsf_flash_erase_all(flash_ptr);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_flash_erase_all(&%s) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("flash erase all faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __flash_write_one_sector(flash_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_flash_t *flash_ptr = test->device;
    VSF_ASSERT(flash_ptr != NULL);

    vsf_err_t err = vsf_flash_write_one_sector(flash_ptr, test->write_offset, test->send.buffer, test->sector_size);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_flash_write_one_sector(&%s, 0x%08x/*offset*/, %p/*buffer*/, 0x%08x/*size*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->write_offset,
                        test->send.buffer, test->sector_size, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("flash write one sector faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __flash_write_multi_sector(flash_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_flash_t *flash_ptr = test->device;
    VSF_ASSERT(flash_ptr != NULL);

    vsf_err_t err = vsf_flash_write_multi_sector(flash_ptr, test->offset, test->send.buffer, test->size);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_flash_write_multi_sector(&%s, 0x%08x/*offset*/, %p/*buffer*/, 0x%08x/*size*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->offset,
                        test->send.buffer, test->size, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("flash write multi sector faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __flash_read_one_sector(flash_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_flash_t *flash_ptr = test->device;
    VSF_ASSERT(flash_ptr != NULL);

    vsf_err_t err = vsf_flash_read_one_sector(flash_ptr, test->read_offset, test->recv.buffer, test->sector_size);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_flash_read_one_sector(&%s, 0x%08x/*offset*/, %p/*buffer*/, 0x%08x/*size*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->read_offset,
                        test->recv.buffer, test->sector_size, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("flash read one sector faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static vsf_err_t __flash_read_multi_sector(flash_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_flash_t *flash_ptr = test->device;
    VSF_ASSERT(flash_ptr != NULL);

    vsf_err_t err = vsf_flash_read_multi_sector(flash_ptr, test->offset, test->recv.buffer, test->size);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_flash_read_multi_sector(&%s, 0x%08x/*offset*/, %p/*buffer*/, 0x%08x/*size*/) = %d" VSF_TRACE_CFG_LINEEND,
                        test->device_name, test->offset,
                        test->recv.buffer, test->size, err);
    }
    if (VSF_ERR_NONE != err) {
        vsf_trace_error("flash read multi sector faild: %d" VSF_TRACE_CFG_LINEEND, err);
    }
    return err;
}

static bool __flash_wait_idle(flash_test_t *test)
{
    VSF_ASSERT(test != NULL);
    vsf_flash_t *flash_ptr = test->device;
    VSF_ASSERT(flash_ptr != NULL);

    vsf_flash_status_t status = vsf_flash_status(flash_ptr);
    if (test->verbose >= 2) {
        vsf_trace_debug("vsf_flash_status(&%s)" VSF_TRACE_CFG_LINEEND, test->device_name);
    }
    return !status.is_busy;
}

static void __flash_demo_evthandler(vsf_eda_t *eda, vsf_evt_t evt)
{
    VSF_ASSERT(NULL != eda);
    flash_test_t *test = vsf_container_of(eda, flash_test_t, teda);
    vsf_flash_t * flash_ptr = test->device;
    VSF_ASSERT(flash_ptr != NULL);

    switch (evt) {
    case VSF_EVT_HAL_TEST_RUN:
        test->erase_offset = test->offset;
        test->write_offset = test->offset;
        test->read_offset  = test->offset;

    case VSF_EVT_FLASH_ERASE:
        if ((test->method & METHOD_ERASE_MASK) == METHOD_ERASE_ONE) {
            if (VSF_ERR_NONE != __flash_erase_one_sector(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else if ((test->method & METHOD_ERASE_MASK) == METHOD_ERASE_MULTI) {
            if (VSF_ERR_NONE != __flash_erase_multi_sector(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else if ((test->method & METHOD_ERASE_MASK) == METHOD_ERASE_ALL) {
            if (VSF_ERR_NONE != __flash_erase_all(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_WRITE);
        }
        if ((test->method & METHOD_ERASE_WAIT_MASK) == METHOD_ERASE_WAIT_POLL) {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_ERASE_WAIT_POLL);
        } else {
            vsf_teda_set_timer_ms(test->timeout_ms.test);
        }
        break;

    case VSF_EVT_FLASH_ERASE_WAIT_POLL:
        if (!__flash_wait_idle(test)) {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_ERASE_WAIT_POLL);
            break;
        }
        // fall through
    case VSF_EVT_FLASH_ERASE_WAIT_ISR:
        if ((test->method & METHOD_ERASE_MASK) == METHOD_ERASE_ONE) {
            test->erase_offset += test->erase_sector_size;
            if (test->erase_offset >= (test->offset + test->size)) {
                vsf_eda_post_evt(eda, VSF_EVT_FLASH_WRITE);
            } else {
                vsf_eda_post_evt(eda, VSF_EVT_FLASH_ERASE);
            }
        } else if ((test->method & METHOD_ERASE_MASK) == METHOD_ERASE_MULTI) {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_WRITE);
        } else if ((test->method & METHOD_ERASE_MASK) == METHOD_ERASE_ALL) {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_WRITE);
        } else {
            VSF_ASSERT(0);
        }
        break;

    case VSF_EVT_FLASH_WRITE:
        if ((test->method & METHOD_WRITE_MASK) == METHOD_WRITE_ONE) {
            if (VSF_ERR_NONE != __flash_write_one_sector(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else if ((test->method & METHOD_WRITE_MASK) == METHOD_WRITE_MULTI) {
            if (VSF_ERR_NONE != __flash_write_multi_sector(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_WRITE);
        }
        if ((test->method & METHOD_WRITE_WAIT_MASK) == METHOD_WRITE_WAIT_POLL) {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_WRITE_WAIT_POLL);
        } else {
            vsf_teda_set_timer_ms(test->timeout_ms.test);
        }
        break;

    case VSF_EVT_FLASH_WRITE_WAIT_POLL:
        if (!__flash_wait_idle(test)) {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_WRITE_WAIT_POLL);
            break;
        }
        // fall through
    case VSF_EVT_FLASH_WRITE_WAIT_ISR:
        if ((test->method & METHOD_WRITE_MASK) == METHOD_WRITE_ONE) {
            test->write_offset += test->sector_size;
            if (test->write_offset >= (test->offset + test->size)) {
                vsf_eda_post_evt(eda, VSF_EVT_FLASH_READ);
            } else {
                vsf_eda_post_evt(eda, VSF_EVT_FLASH_WRITE);
            }
        } else if ((test->method & METHOD_WRITE_MASK) == METHOD_WRITE_MULTI) {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_READ);
        } else if ((test->method & METHOD_WRITE_MASK) == METHOD_WRITE_ALL) {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_READ);
        } else {
            VSF_ASSERT(0);
        }
        break;

    case VSF_EVT_FLASH_READ:
        if ((test->method & METHOD_READ_MASK) == METHOD_READ_ONE) {
            if (VSF_ERR_NONE != __flash_read_one_sector(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else if ((test->method & METHOD_READ_MASK) == METHOD_READ_MULTI) {
            if (VSF_ERR_NONE != __flash_read_multi_sector(test)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
                break;
            }
        } else {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_READ);
        }
        if ((test->method & METHOD_READ_WAIT_MASK) == METHOD_READ_WAIT_POLL) {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_READ_WAIT_POLL);
        } else {
            vsf_teda_set_timer_ms(test->timeout_ms.test);
        }
        break;

    case VSF_EVT_FLASH_READ_WAIT_POLL:
        if (!__flash_wait_idle(test)) {
            vsf_eda_post_evt(eda, VSF_EVT_FLASH_READ_WAIT_POLL);
            break;
        }
        // fall through
    case VSF_EVT_FLASH_READ_WAIT_ISR:
        if ((test->method & METHOD_READ_MASK) == METHOD_READ_ONE) {
            test->read_offset += test->sector_size;
            if (test->read_offset >= (test->offset + test->size)) {
                vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
            } else {
                vsf_eda_post_evt(eda, VSF_EVT_FLASH_READ);
            }
        } else if ((test->method & METHOD_READ_MASK) == METHOD_READ_MULTI) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        } else if ((test->method & METHOD_READ_MASK) == METHOD_READ_ALL) {
            vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_NEXT);
        } else {
            VSF_ASSERT(0);
        }
        break;

    case VSF_EVT_TIMER:
        vsf_trace_error("flash timeout, not received interrupt" VSF_TRACE_CFG_LINEEND);
        vsf_eda_post_evt(eda, VSF_EVT_HAL_TEST_FAILED);
        break;

    default:
        hal_evthandler(eda, evt);
        break;
    }
}

#if APP_USE_LINUX_DEMO == ENABLED
static vsf_err_t __flash_parser_args(hal_test_t *hal_test, int argc, char *argv[])
{
    flash_test_t *test = (flash_test_t *)hal_test;
    VSF_ASSERT(test != NULL);
    hal_demo_t *demo = test->demo;
    VSF_ASSERT(demo != NULL);

    vsf_err_t err = VSF_ERR_NONE;

    int c = 0;
    int option_index = 0;
    static const char *__short_options = "hlv::m:r:d:p:i:t:o:s:";
    static const struct option __long_options[] = {
        { "help",       no_argument,       NULL, 'h'  },
        { "list-device",no_argument,       NULL, 'l'  },
        { "verbose",    optional_argument, NULL, 'v'  },
        { "method",     required_argument, NULL, 'm'  },
        { "repeat",     required_argument, NULL, 'r'  },
        { "device",     required_argument, NULL, 'd'  },
        { "prio",       required_argument, NULL, 'p'  },
        { "isr_prio",   required_argument, NULL, 'i'  },
        { "timeout",    required_argument, NULL, 't'  },
        { "offset",     required_argument, NULL, 'o'  },
        { "size",       required_argument, NULL, 's'  },
        { NULL,         0                , NULL, '\0' },
    };

    optind = 1;
    while(EOF != (c = getopt_long(argc, argv, __short_options, __long_options, &option_index))) {
        switch(c) {
        case 'o':
            test->offset = strtol(optarg, NULL, 0);
            break;
        case 's':
            test->size = strtol(optarg, NULL, 0);
            if (test->size == 0) {
                vsf_trace_error("flash program size cannot be 0" VSF_TRACE_CFG_LINEEND);
            }
            break;
        default:
            err = hal_test_evthandler(hal_test, argc, argv, c);
            if (err != VSF_ERR_NONE) {
                return err;
            }
            break;
        }
    }

    if ((test->method & METHOD_WRITE_MASK) != METHOD_NO_WRITE) {
        test->send.size = test->size;
    }

    if ((test->method & METHOD_READ_MASK) != METHOD_NO_READ) {
        test->recv.size = test->size;
    }

    return err;
}

int flash_main(int argc, char *argv[])
{
    return hal_main(&__flash_demo.use_as__hal_demo_t, argc, argv);
}
#else
int VSF_USER_ENTRY(void)
{
    return hal_main(&__flash_demo.use_as__hal_demo_t);
}
#endif

#endif
