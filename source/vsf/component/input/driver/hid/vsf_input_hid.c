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

/*============================ INCLUDES ======================================*/

#include "../../vsf_input_cfg.h"

#if VSF_USE_INPUT == ENABLED && VSF_USE_INPUT_HID == ENABLED
// TODO: use dedicated include
#include "vsf.h"

/*============================ MACROS ========================================*/

#define HID_LONG_ITEM(x)            ((x) == 0xFE)
#define HID_ITEM_SIZE(x)            ((((x) & 0x03) == 3) ? 4 : (x) & 0x03)

#define HID_ITEM_TYPE(x)            (((x) >> 2) & 0x03)
#define HID_ITEM_TYPE_MAIN          0
#define HID_ITEM_TYPE_GLOBAL        1
#define HID_ITEM_TYPE_LOCAL         2

#define HID_ITEM_TAG(x)             ((x) & 0xFC)
#define HID_ITEM_INPUT              0x80
#define HID_ITEM_OUTPUT             0x90
#define HID_ITEM_FEATURE            0xB0
#define HID_ITEM_COLLECTION         0xA0
#define HID_ITEM_END_COLLECTION     0xC0
#define HID_ITEM_USAGE_PAGE         0x04
#define HID_ITEM_LOGI_MINI          0x14
#define HID_ITEM_LOGI_MAXI          0x24
#define HID_ITEM_PHY_MINI           0x34
#define HID_ITEM_PHY_MAXI           0x44
#define HID_ITEM_UNIT_EXPT          0x54
#define HID_ITEM_UNIT               0x64
#define HID_ITEM_REPORT_SIZE        0x74
#define HID_ITEM_REPORT_ID          0x84
#define HID_ITEM_REPORT_COUNT       0x94
#define HID_ITEM_PUSH               0xA4
#define HID_ITEM_POP                0xB4
#define HID_ITEM_USAGE              0x08
#define HID_ITEM_USAGE_MIN          0x18
#define HID_ITEM_USAGE_MAX          0x28

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

struct hid_desc_t {
    int report_id;
    int collection;
    int report_size;
    int report_count;
    int usage_page;
    int usage_num;
    int logical_min;
    int logical_max;
    int physical_min;
    int physical_max;
    int usage_min;
    int usage_max;
    int generic_usage;
    int usages[16];
};
typedef struct hid_desc_t hid_desc_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#if     defined(WEAK_VSF_INPUT_ON_EVT_EXTERN)                                   \
    &&  defined(WEAK_VSF_INPUT_ON_EVT)
WEAK_VSF_INPUT_ON_EVT_EXTERN
#endif

#if     defined(WEAK_VSF_INPUT_ON_NEW_DEV_EXTERN)                               \
    &&  defined(WEAK_VSF_INPUT_ON_NEW_DEV)
WEAK_VSF_INPUT_ON_NEW_DEV_EXTERN
#endif

#if     defined(WEAK_VSF_INPUT_ON_FREE_DEV_EXTERN)                              \
    &&  defined(WEAK_VSF_INPUT_ON_FREE_DEV)
WEAK_VSF_INPUT_ON_FREE_DEV_EXTERN
#endif

/*============================ IMPLEMENTATION ================================*/

#ifndef WEAK_VSF_HID_ON_NEW_DEV
WEAK(vsf_hid_on_new_dev)
void vsf_hid_on_new_dev(vk_input_hid_t *dev)
{
#   ifndef WEAK_VSF_INPUT_ON_NEW_DEV
    vsf_input_on_new_dev(VSF_INPUT_TYPE_HID, dev);
#   else
    WEAK_VSF_INPUT_ON_NEW_DEV(VSF_INPUT_TYPE_HID, dev);
#   endif
}
#endif

#ifndef WEAK_VSF_HID_ON_FREE_DEV
WEAK(vsf_hid_on_free_dev)
void vsf_hid_on_free_dev(vk_input_hid_t *dev)
{
#   ifndef WEAK_VSF_INPUT_ON_FREE_DEV
    vsf_input_on_free_dev(VSF_INPUT_TYPE_HID, dev);
#   else
    WEAK_VSF_INPUT_ON_FREE_DEV(VSF_INPUT_TYPE_HID, dev);
#   endif
}
#endif

#ifndef WEAK_VSF_HID_ON_REPORT_INPUT
WEAK(vsf_hid_on_report_input)
void vsf_hid_on_report_input(vk_hid_event_t *hid_evt)
{
#ifndef WEAK_VSF_INPUT_ON_EVT
    vsf_input_on_evt(VSF_INPUT_TYPE_HID, &hid_evt->use_as__vk_input_evt_t);
#else
    WEAK_VSF_INPUT_ON_EVT(VSF_INPUT_TYPE_HID, &hid_evt->use_as__vk_input_evt_t);
#endif
}
#endif

static vk_hid_report_t * vk_hid_get_report(vk_input_hid_t *dev, hid_desc_t *desc, uint_fast8_t type)
{
    vk_hid_report_t *report = NULL;

    __vsf_slist_foreach_unsafe(vk_hid_report_t, report_node, &dev->report_list) {
        if ((_->type == type) && (_->id == desc->report_id)) {
            report = _;
            break;
        }
    }

    if (!report) {
        report = vsf_heap_malloc(sizeof(*report));
        if (report != NULL) {
            memset(report, 0, sizeof(*report));
            vsf_slist_add_to_head(vk_hid_report_t, report_node, &dev->report_list, report);
            report->type = type;
            report->id = (desc->report_id >= 0) ? desc->report_id : -1;
            report->generic_usage = desc->generic_usage;
        }
    }
    return report;
}

static vsf_err_t vk_hid_parse_item(vk_input_hid_t *dev,
        hid_desc_t *desc, uint_fast8_t tag, uint_fast32_t size, uint8_t *buf)
{
    vk_hid_report_t *report;
    vk_hid_usage_t *usage;
    uint_fast32_t value, ival;

    if (size == 1) {
        value = *buf;
    } else if (size == 2) {
        value = *(uint16_t *)buf;
    } else if (size == 4) {
        value = *(uint32_t *)buf;
    }

    switch (tag) {
        case HID_ITEM_INPUT:
            report = vk_hid_get_report(dev, desc, HID_ITEM_INPUT);
            if (!report) { return VSF_ERR_FAIL; }

            if ((desc->usage_min != -1) && (desc->usage_max != -1)) {
                desc->usage_num = desc->usage_max - desc->usage_min + 1;
                usage = vsf_heap_malloc(sizeof(vk_hid_usage_t));
                if (usage == NULL) { return VSF_ERR_FAIL; }

                usage->data_flag = value;
                usage->report_size = (int32_t)desc->report_size;
                usage->report_count = (int32_t)desc->report_count;

                usage->usage_page = (uint16_t)desc->usage_page;
                usage->usage_min = (uint8_t)desc->usage_min;
                usage->usage_max = (uint8_t)desc->usage_max;
                usage->bit_offset = (int32_t)report->bitlen;
                usage->bit_length = (int32_t)(desc->report_size * desc->report_count);

                usage->logical_min = desc->logical_min;
                usage->logical_max = desc->logical_max;

                vsf_slist_append(vk_hid_usage_t, usage_node, &report->usage_list, usage);

                desc->usage_min = -1;
                desc->usage_max = -1;
            } else {
                for (int i = 0; i < desc->report_count; i++) {
                    usage = vsf_heap_malloc(sizeof(vk_hid_usage_t));
                    if (usage == NULL) { return VSF_ERR_FAIL; }

                    usage->report_size = (int32_t)desc->report_size;
                    usage->report_count = 1;
                    usage->data_flag = value;

                    usage->usage_page = (uint16_t)desc->usage_page;
                    usage->usage_min = (uint8_t)desc->usages[i];
                    usage->usage_max = (uint8_t)desc->usages[i];
                    usage->bit_length = (int32_t)(desc->report_size * usage->report_count);
                    usage->bit_offset = (int32_t)(report->bitlen + i * usage->bit_length);

                    usage->logical_min = desc->logical_min;
                    usage->logical_max = desc->logical_max;

                    vsf_slist_append(vk_hid_usage_t, usage_node, &report->usage_list, usage);
                }
            }

            desc->usage_num = 0;
            report->bitlen += (desc->report_size * desc->report_count);
            break;

        case HID_ITEM_OUTPUT:
            report = vk_hid_get_report(dev, desc, HID_ITEM_OUTPUT);
            if (!report) { return VSF_ERR_FAIL; }

            if ((desc->usage_min != -1) && (desc->usage_max != -1)) {
                desc->usage_num = desc->usage_max - desc->usage_min + 1;
                usage = vsf_heap_malloc(sizeof(vk_hid_usage_t));
                if (usage == NULL) { return VSF_ERR_FAIL; }

                usage->report_size = desc->report_size;
                usage->report_count = desc->report_count;
                usage->data_flag = value;

                usage->usage_page = desc->usage_page;
                usage->usage_min = desc->usage_min;
                usage->usage_max = desc->usage_max;
                usage->bit_offset = report->bitlen;
                usage->bit_length = desc->report_size * desc->report_count;

                usage->logical_min = desc->logical_min;
                usage->logical_max = desc->logical_max;

                vsf_slist_append(vk_hid_usage_t, usage_node, &report->usage_list, usage);

                desc->usage_min = -1;
                desc->usage_max = -1;
            } else {
                for (int i = 0; i < desc->usage_num; i++) {
                    usage = vsf_heap_malloc(sizeof(vk_hid_usage_t));
                    if (usage == NULL) { return VSF_ERR_FAIL; }

                    usage->report_size = desc->report_size;
                    usage->report_count = desc->report_count;
                    usage->data_flag = value;
                    value = desc->report_size * desc->report_count / desc->usage_num;

                    usage->usage_page = desc->usage_page;
                    usage->usage_min = desc->usages[i];
                    usage->usage_max = desc->usages[i];
                    usage->bit_offset = report->bitlen + i * value;
                    usage->bit_length = value;

                    usage->logical_min = desc->logical_min;
                    usage->logical_max = desc->logical_max;

                    vsf_slist_append(vk_hid_usage_t, usage_node, &report->usage_list, usage);
                }
            }

            desc->usage_num = 0;
            report->bitlen += (desc->report_size * desc->report_count);
            break;

        case HID_ITEM_FEATURE:
            report = vk_hid_get_report(dev, desc, HID_ITEM_FEATURE);
            if (!report) { return VSF_ERR_FAIL; }
            break;

        case HID_ITEM_COLLECTION:
            desc->collection++;
            desc->usage_num = 0;
            break;

        case HID_ITEM_END_COLLECTION:
            desc->collection--;
            break;

        case HID_ITEM_USAGE_PAGE:
            desc->usage_page = value;
            break;

        case HID_ITEM_LOGI_MINI:
            if (size == 1) {
                ival = *(int8_t *)buf;
            } else if (size == 2) {
                ival = *(int16_t *)buf;
            } else if (size == 4) {
                ival = *(int32_t *)buf;
            } else {
                return VSF_ERR_NOT_SUPPORT;
            }
            desc->logical_min = ival;
            break;

        case HID_ITEM_LOGI_MAXI:
            if (size == 1) {
                ival = *(int8_t *)buf;
            } else if (size == 2) {
                ival = *(int16_t *)buf;
            } else if (size == 4) {
                ival = *(int32_t *)buf;
            } else {
                return VSF_ERR_NOT_SUPPORT;
            }
            desc->logical_max = ival;
            break;

        case HID_ITEM_PHY_MINI:
            break;

        case HID_ITEM_PHY_MAXI:
            break;

        case HID_ITEM_UNIT_EXPT:
            break;

        case HID_ITEM_UNIT:
            break;

        case HID_ITEM_REPORT_SIZE:
            desc->report_size = value;
            break;

        case HID_ITEM_REPORT_ID:
            desc->report_id = value;
            dev->report_has_id = true;
            break;

        case HID_ITEM_REPORT_COUNT:
            desc->report_count = value;
            break;

        case HID_ITEM_PUSH:
            break;

        case HID_ITEM_POP:
            break;

        case HID_ITEM_USAGE:
            if (desc->usage_num < dimof(desc->usages)) {
                desc->usages[desc->usage_num++] = value;
                if ((desc->generic_usage == 0) && (desc->collection == 0)) {
                    desc->generic_usage = value;
                }
            }
            break;

        case HID_ITEM_USAGE_MAX:
            desc->usage_max = value;
            break;

        case HID_ITEM_USAGE_MIN:
            desc->usage_min = value;
            break;
    }

    return VSF_ERR_NONE;
}

void vk_hid_new_dev(vk_input_hid_t *dev)
{
#ifndef WEAK_VSF_HID_ON_NEW_DEV
    vsf_hid_on_new_dev(dev);
#else
    WEAK_VSF_HID_ON_NEW_DEV(dev);
#endif
}

void vk_hid_free_dev(vk_input_hid_t *dev)
{
    vk_hid_report_t *report;

#ifndef WEAK_VSF_HID_ON_FREE_DEV
    vsf_hid_on_free_dev(dev);
#else
    WEAK_VSF_HID_ON_FREE_DEV(dev);
#endif

    __vsf_slist_foreach_next_unsafe(vk_hid_report_t, report_node, &dev->report_list) {
        report = _;
        __vsf_slist_foreach_next_unsafe(vk_hid_usage_t, usage_node, &report->usage_list) {
            vsf_heap_free(_);
        }

        if (_->value) {
            vsf_heap_free(_->value);
        }
        vsf_heap_free(_);
    }
}

static uint_fast32_t vk_hid_get_max_input_size(vk_input_hid_t *dev)
{
    uint_fast32_t maxsize = 0;

    __vsf_slist_foreach_unsafe(vk_hid_report_t, report_node, &dev->report_list) {
        if (_->bitlen > maxsize) {
            maxsize = _->bitlen;
        }
    }
    return (maxsize + 7) >> 3;
}

uint_fast32_t vk_hid_parse_desc(vk_input_hid_t *dev, uint8_t *desc_buf, uint_fast32_t len)
{
    hid_desc_t *desc = vsf_heap_malloc(sizeof(hid_desc_t));
    uint8_t *end = desc_buf + len;
    int item_size;
    vsf_err_t err;

    if (desc == NULL) { return 0; }
    memset(desc, 0, sizeof(*desc));
    desc->report_id = -1;
    desc->usage_min = -1;
    desc->usage_max = -1;

    memset(dev, 0, sizeof(vk_input_hid_t));
    while (desc_buf < end) {
        if (HID_LONG_ITEM(*desc_buf)) {
            item_size = *(desc_buf + 1);
        } else {
            item_size = HID_ITEM_SIZE(*desc_buf);
            err = vk_hid_parse_item(dev, desc, HID_ITEM_TAG(*desc_buf), item_size, desc_buf + 1);
            if (err) { break; }
        }
        desc_buf += (item_size + 1);
    }

    if ((desc->collection != 0) || err) {
        goto free_hid_report;
    }
    vsf_heap_free(desc);
    return vk_hid_get_max_input_size(dev);

free_hid_report:
    vsf_heap_free(desc);
    vk_hid_free_dev(dev);
    return 0;
}

void vk_hid_process_input(vk_input_hid_t *dev, uint8_t *buf, uint_fast32_t len)
{
    vk_hid_report_t *report = NULL;
    vk_hid_event_t event;
    uint_fast32_t cur_value, pre_value;
    bool reported = false;
    int_fast16_t id = dev->report_has_id ? *buf++ : -1;
    int_fast32_t i;
    uint_fast16_t usage_page, usage_id;

    __vsf_slist_foreach_unsafe(vk_hid_report_t, report_node, &dev->report_list) {
        if ((_->type == HID_ITEM_INPUT) && (_->id == id)) {
            report = _;
            break;
        }
    }
    if (!report) {
        return;
    }

    if (!report->value) {
        report->value = vsf_heap_malloc(len);
        if (!report->value) { return; }
        memset(report->value, 0, len);
    }

    event.duration = vk_input_update_timestamp(&dev->timestamp);
    event.dev = dev;
    __vsf_slist_foreach_unsafe(vk_hid_usage_t, usage_node, &report->usage_list) {
        for (i = 0; i < _->report_count; ++i) {
            /* get usage value */
            cur_value = vk_input_buf_get_value(buf, _->bit_offset + i * _->report_size, _->report_size);
            pre_value = vk_input_buf_get_value(report->value, _->bit_offset + i * _->report_size, _->report_size);

            /* compare and process */
            if (cur_value != (HID_USAGE_IS_REL(_) ? 0 : pre_value)) {
                usage_page = _->usage_page;
                usage_id = HID_USAGE_IS_VAR(_) ? (_->usage_min + i) :
                        (cur_value ? (uint16_t)cur_value : (uint16_t)pre_value);
                event.id = (uint64_t)report->generic_usage | ((uint64_t)usage_page << 16) | ((uint64_t)usage_id << 32);
                event.pre.valu32 = pre_value;
                event.cur.valu32 = cur_value;
                event.usage = _;

                reported = true;
#ifndef WEAK_VSF_HID_ON_REPORT_INPUT
                vsf_hid_on_report_input(&event);
#else
                WEAK_VSF_HID_ON_REPORT_INPUT(&event);
#endif
            }
        }
    }

    // just report input process end if changed reportted
    if (reported) {
        event.id = 0;
#ifndef WEAK_VSF_HID_ON_REPORT_INPUT
        vsf_hid_on_report_input(&event);
#else
        WEAK_VSF_HID_ON_REPORT_INPUT(&event);
#endif
    }
    memcpy(report->value, buf, len);
}

#endif      // VSF_USE_INPUT && VSF_USE_INPUT_HID
