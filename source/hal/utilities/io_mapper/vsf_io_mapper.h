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

#ifndef __VSF_IO_MAPPER_H__
#define __VSF_IO_MAPPER_H__

/*============================ INCLUDES ======================================*/

#if VSF_HW_GPIO_COUNT > 0

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

// default is 2^5 = 32-bit IO
#define VSF_HW_IO_MAPPER_PORT_BITS_LOG2         5

/*============================ MACROFIED FUNCTIONS ===========================*/

#define __VSF_IO_MAPPER_INIT(__PORT_NUM, __PORT_BITS_LOG2)                      \
            .port_num           = (__PORT_NUM),                                 \
            .port_bits_log2     = (__PORT_BITS_LOG2),                           \
            .pin_mask           = (1 << (__PORT_BITS_LOG2)) - 1,
#define VSF_IO_MAPPER_INIT(__PORT_NUM, __PORT_BITS_LOG2)                        \
            __VSF_IO_MAPPER_INIT((__PORT_NUM), (__PORT_BITS_LOG2))

#define __vsf_io_mapper_type(__name)            __name##_io_mapper_t
#define vsf_io_mapper_type(__name)              __vsf_io_mapper_type(__name)

#define __define_io_mapper(__name, __port_num)                                  \
    typedef struct vsf_io_mapper_type(__name) {                                 \
        uint8_t         port_num;       /* num of ports */                      \
        uint8_t         port_bits_log2; /* num of io bits per port in log2 */   \
        uint8_t         pin_mask;                                               \
        void *          io[(__port_num)];                                       \
    } vsf_io_mapper_type(__name);
#define define_io_mapper(__name, __port_num)    __define_io_mapper(__name, __port_num)
#define def_io_mapper(__name, __port_num)       define_io_mapper(__name, __port_num)

#define __describe_io_mapper(__name, __port_num, __port_bits_log2, ...)         \
    define_io_mapper(__name, __port_num)                                        \
    vsf_io_mapper_type(__name) __name = {                                       \
        VSF_IO_MAPPER_INIT(__port_num, __port_bits_log2)                        \
        __io = {                                                                \
            __VA_ARGS__                                                         \
        }                                                                       \
    };
#define describe_io_mapper(__name, __port_num, __port_bits_log2)                \
    __describe_io_mapper(__name, (__port_num), (__port_bits_log2))


#define vsf_io_mapper_pin(__io_mapper, __port_idx, __pin_idx)                   \
    ((__pin_idx) | ((__port_idx) << (__io_mapper)->port_bits_log2))
#define vsf_io_mapper_get_port(__io_mapper, __pin)                              \
    ((__pin) >> (__io_mapper)->port_bits_log2)
#define vsf_io_mapper_get_pin(__io_mapper, __pin)                               \
    ((__pin) & (__io_mapper)->pin_mask)

#define vsf_io_mapper_config(__io_mapper, __pin, __feature) ({                  \
        uint8_t VSF_MACRO_SAFE_NAME(port_idx) = vsf_io_mapper_get_port(__io_mapper, __pin);\
        uint8_t VSF_MACRO_SAFE_NAME(pin_idx) = vsf_io_mapper_get_pin(__io_mapper, __pin);\
        VSF_HAL_ASSERT(VSF_MACRO_SAFE_NAME(port_idx) < (__io_mapper)->port_num);\
        vsf_gpio_config_pin((__io_mapper)->io[VSF_MACRO_SAFE_NAME(port_idx)], 1 << VSF_MACRO_SAFE_NAME(pin_idx), (__feature));\
    })
#define vsf_io_mapper_set_direction(__io_mapper, __pin, __dir) ({               \
        uint8_t VSF_MACRO_SAFE_NAME(port_idx) = vsf_io_mapper_get_port(__io_mapper, __pin);\
        uint8_t VSF_MACRO_SAFE_NAME(pin_idx) = vsf_io_mapper_get_pin(__io_mapper, __pin);\
        VSF_HAL_ASSERT(VSF_MACRO_SAFE_NAME(port_idx) < (__io_mapper)->port_num);\
        vsf_gpio_set_direction((__io_mapper)->io[VSF_MACRO_SAFE_NAME(port_idx)],\
            (__dir) ? (1 << VSF_MACRO_SAFE_NAME(pin_idx)) : 0, 1 << VSF_MACRO_SAFE_NAME(pin_idx));\
    })
#define vsf_io_mapper_get_direction(__io_mapper, __pin) ({                      \
        uint8_t VSF_MACRO_SAFE_NAME(port_idx) = vsf_io_mapper_get_port(__io_mapper, __pin);\
        uint8_t VSF_MACRO_SAFE_NAME(pin_idx) = vsf_io_mapper_get_pin(__io_mapper, __pin);\
        VSF_HAL_ASSERT(VSF_MACRO_SAFE_NAME(port_idx) < (__io_mapper)->port_num);\
        vsf_gpio_get_direction((__io_mapper)->io[VSF_MACRO_SAFE_NAME(port_idx)], 1 << VSF_MACRO_SAFE_NAME(pin_idx));\
    })
#define vsf_io_mapper_switch_direction(__io_mapper, __pin) ({                   \
        uint8_t VSF_MACRO_SAFE_NAME(port_idx) = vsf_io_mapper_get_port(__io_mapper, __pin);\
        uint8_t VSF_MACRO_SAFE_NAME(pin_idx) = vsf_io_mapper_get_pin(__io_mapper, __pin);\
        VSF_HAL_ASSERT(VSF_MACRO_SAFE_NAME(port_idx) < (__io_mapper)->port_num);\
        vsf_gpio_switch_direction((__io_mapper)->io[VSF_MACRO_SAFE_NAME(port_idx)], 1 << VSF_MACRO_SAFE_NAME(pin_idx));\
    })
#define vsf_io_mapper_set_input(__io_mapper, __pin) ({                          \
        uint8_t VSF_MACRO_SAFE_NAME(port_idx) = vsf_io_mapper_get_port(__io_mapper, __pin);\
        uint8_t VSF_MACRO_SAFE_NAME(pin_idx) = vsf_io_mapper_get_pin(__io_mapper, __pin);\
        VSF_HAL_ASSERT(VSF_MACRO_SAFE_NAME(port_idx) < (__io_mapper)->port_num);\
        vsf_gpio_set_input((__io_mapper)->io[VSF_MACRO_SAFE_NAME(port_idx)], 1 << VSF_MACRO_SAFE_NAME(pin_idx));\
    })
#define vsf_io_mapper_set_output(__io_mapper, __pin) ({                         \
        uint8_t VSF_MACRO_SAFE_NAME(port_idx) = vsf_io_mapper_get_port(__io_mapper, __pin);\
        uint8_t VSF_MACRO_SAFE_NAME(pin_idx) = vsf_io_mapper_get_pin(__io_mapper, __pin);\
        VSF_HAL_ASSERT(VSF_MACRO_SAFE_NAME(port_idx) < (__io_mapper)->port_num);\
        vsf_gpio_set_output((__io_mapper)->io[VSF_MACRO_SAFE_NAME(port_idx)], 1 << VSF_MACRO_SAFE_NAME(pin_idx));\
    })
#define vsf_io_mapper_read(__io_mapper, __pin) ({                               \
        uint8_t VSF_MACRO_SAFE_NAME(port_idx) = vsf_io_mapper_get_port(__io_mapper, __pin);\
        uint8_t VSF_MACRO_SAFE_NAME(pin_idx) = vsf_io_mapper_get_pin(__io_mapper, __pin);\
        VSF_HAL_ASSERT(VSF_MACRO_SAFE_NAME(port_idx) < (__io_mapper)->port_num);\
        uint32_t pin_value = vsf_gpio_read((__io_mapper)->io[VSF_MACRO_SAFE_NAME(port_idx)]);\
        ((pin_value >> VSF_MACRO_SAFE_NAME(pin_idx)) & 1);                      \
    })
#define vsf_io_mapper_set(__io_mapper, __pin) ({                                \
        uint8_t VSF_MACRO_SAFE_NAME(port_idx) = vsf_io_mapper_get_port(__io_mapper, __pin);\
        uint8_t VSF_MACRO_SAFE_NAME(pin_idx) = vsf_io_mapper_get_pin(__io_mapper, __pin);\
        VSF_HAL_ASSERT(VSF_MACRO_SAFE_NAME(port_idx) < (__io_mapper)->port_num);\
        vsf_gpio_set((__io_mapper)->io[VSF_MACRO_SAFE_NAME(port_idx)], 1 << VSF_MACRO_SAFE_NAME(pin_idx));\
    })
#define vsf_io_mapper_clear(__io_mapper, __pin) ({                              \
        uint8_t VSF_MACRO_SAFE_NAME(port_idx) = vsf_io_mapper_get_port(__io_mapper, __pin);\
        uint8_t VSF_MACRO_SAFE_NAME(pin_idx) = vsf_io_mapper_get_pin(__io_mapper, __pin);\
        VSF_HAL_ASSERT(VSF_MACRO_SAFE_NAME(port_idx) < (__io_mapper)->port_num);\
        vsf_gpio_clear((__io_mapper)->io[VSF_MACRO_SAFE_NAME(port_idx)], 1 << VSF_MACRO_SAFE_NAME(pin_idx));\
    })
#define vsf_io_mapper_toggle(__io_mapper, __pin) ({                             \
        uint8_t VSF_MACRO_SAFE_NAME(port_idx) = vsf_io_mapper_get_port(__io_mapper, __pin);\
        uint8_t VSF_MACRO_SAFE_NAME(pin_idx) = vsf_io_mapper_get_pin(__io_mapper, __pin);\
        VSF_HAL_ASSERT(VSF_MACRO_SAFE_NAME(port_idx) < (__io_mapper)->port_num);\
        vsf_gpio_toggle((__io_mapper)->io[VSF_MACRO_SAFE_NAME(port_idx)], 1 << VSF_MACRO_SAFE_NAME(pin_idx));\
    })


#define vsf_hw_io_mapper_pin(__port_idx, __pin_idx)                             \
    ((__pin_idx) | ((__port_idx) << VSF_HW_IO_MAPPER_PORT_BITS_LOG2))

/*============================ TYPES =========================================*/

typedef struct vsf_io_mapper_t {
    uint8_t         port_num;       // num of ports
    uint8_t         port_bits_log2; // num of io bits per port in log2
    uint8_t         pin_mask;
    void           *io[0];
} vsf_io_mapper_t;

define_io_mapper(vsf_hw, VSF_HW_GPIO_COUNT)

/*============================ GLOBAL VARIABLES ==============================*/

extern const vsf_io_mapper_type(vsf_hw) vsf_hw_io_mapper;

/*============================ PROTOTYPES ====================================*/



#ifdef __cplusplus
}
#endif

#endif  // VSF_HW_GPIO_COUNT > 0
#endif  // __VSF_IO_MAPPER_H__
