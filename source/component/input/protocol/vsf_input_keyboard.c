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

#include "../vsf_input_cfg.h"

#if VSF_USE_INPUT == ENABLED

#include "../vsf_input.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/

const uint16_t __vsf_keyboard_keycode2scancode_0_99[100] = {
    [4]     = 0x1E,
    [5]     = 0x30,
    [6]     = 0x2E,
    [7]     = 0x20,
    [8]     = 0x12,
    [9]     = 0x21,
    [10]    = 0x22,
    [11]    = 0x23,
    [12]    = 0x17,
    [13]    = 0x24,
    [14]    = 0x25,
    [15]    = 0x26,
    [16]    = 0x32,
    [17]    = 0x31,
    [18]    = 0x18,
    [19]    = 0x19,
    [20]    = 0x10,
    [21]    = 0x13,
    [22]    = 0x1F,
    [23]    = 0x14,
    [24]    = 0x16,
    [25]    = 0x2F,
    [26]    = 0x11,
    [27]    = 0x2D,
    [28]    = 0x15,
    [29]    = 0x2C,
    [30]    = 0x02,
    [31]    = 0x03,
    [32]    = 0x04,
    [33]    = 0x05,
    [34]    = 0x06,
    [35]    = 0x07,
    [36]    = 0x08,
    [37]    = 0x09,
    [38]    = 0x0A,
    [39]    = 0x0B,
    [40]    = 0x1C,
    [41]    = 0x01,
    [42]    = 0x0E,
    [43]    = 0x0F,
    [44]    = 0x39,
    [45]    = 0x0C,
    [46]    = 0x0D,
    [47]    = 0x1A,
    [48]    = 0x1B,
    [49]    = 0x2B,
    [51]    = 0x27,
    [52]    = 0x28,
    [53]    = 0x29,
    [54]    = 0x33,
    [55]    = 0x34,
    [56]    = 0x35,
    [57]    = 0x3A,
    [58]    = 0x3B,
    [59]    = 0x3C,
    [60]    = 0x3D,
    [61]    = 0x3E,
    [62]    = 0x3F,
    [63]    = 0x40,
    [64]    = 0x41,
    [65]    = 0x42,
    [66]    = 0x43,
    [67]    = 0x44,
    [68]    = 0x57,
    [69]    = 0x58,
    [70]    = 0x54,
    [71]    = 0x46,
    [72]    = 0xE11D,
    [73]    = 0xE052,
    [74]    = 0xE047,
    [75]    = 0xE049,
    [76]    = 0xE053,
    [77]    = 0xE04F,
    [78]    = 0xE051,
    [79]    = 0xE04D,
    [80]    = 0xE04B,
    [81]    = 0xE050,
    [82]    = 0xE048,
    [83]    = 0x45,
    [84]    = 0xE035,
    [85]    = 0x37,
    [86]    = 0x4A,
    [87]    = 0x4E,
    [88]    = 0xE01C,
    [89]    = 0x4F,
    [90]    = 0x50,
    [91]    = 0x51,
    [92]    = 0x4B,
    [93]    = 0x4C,
    [94]    = 0x4D,
    [95]    = 0x47,
    [96]    = 0x48,
    [97]    = 0x49,
    [98]    = 0x52,
    [99]    = 0x53,
};

const uint16_t __vsf_keyboard_keycode2scancode_224_231[8] = {
    [0]     = 0x1D,     // left control
    [1]     = 0x2A,     // left shift
    [2]     = 0x38,     // left alt
    [3]     = 0xE05B,   // left windows
    [4]     = 0xE01D,   // right control
    [5]     = 0x36,     // right shift
    [6]     = 0xE038,   // right alt
    [7]     = 0xE05C,   // right windows
};

/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

VSF_CAL_WEAK(vsf_input_on_keyboard)
void vsf_input_on_keyboard(vk_keyboard_evt_t *keyboard_evt)
{
    vsf_input_on_evt(VSF_INPUT_TYPE_KEYBOARD, &keyboard_evt->use_as__vk_input_evt_t);
}

#endif      // VSF_USE_INPUT
