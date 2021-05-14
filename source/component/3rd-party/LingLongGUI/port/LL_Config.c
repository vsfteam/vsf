/*
 * Copyright 2021 Ou Jianbo 59935554@qq.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * additional license
 * If you use this software to write secondary development type products,
 * must be released under GPL compatible free software license or commercial license.
*/

#include "vsf.h"
#include "LL_Config.h"

uint8_t cfgButtonTextLengthMax = LL_BUTTON_TEXT_LENGTH_MAX;
uint8_t cfgLineEditTextLengthMax = LL_LINEEDIT_TEXT_LENGTH_MAX;
uint8_t cfgCheckBoxTextLengthMax = LL_CHECKBOX_TEXT_LENGTH_MAX;

uint8_t cfgColorDepth = CONFIG_COLOR_DEPTH;
uint16_t cfgMonitorWidth = 0;
uint16_t cfgMonitorHeight = 0;

void *llMalloc(uint32_t size)
{
    return vsf_heap_malloc(size);
}

void llFree(void *p)
{
    vsf_heap_free(p);
}

void *llRealloc(void *ptr,uint32_t newSize)
{
    if (NULL == ptr) {
        return vsf_heap_malloc(newSize);
    }
    if (0 == newSize) {
        if (ptr != NULL) {
            vsf_heap_free(ptr);
        }
        return NULL;
    }
    return vsf_heap_realloc(ptr, newSize);
}
