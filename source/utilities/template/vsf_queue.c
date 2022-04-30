/****************************************************************************
*  Copyright 2020 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
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

#define __VSF_QUEUE_CLASS_IMPLEMENT
#include "utilities/vsf_utilities_cfg.h"
#include "../compiler/compiler.h"
#include "./vsf_queue.h"

/*============================ MACROS ========================================*/
#undef vsf_this
#define vsf_this    (*this_ptr)
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void __vsf_rng_buf_init_ex( vsf_rng_buf_t* obj_ptr, 
                            uint_fast16_t buffer_item_cnt, 
                            bool is_init_as_full)
{
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);
    memset(obj_ptr, 0, sizeof(vsf_rng_buf_t));
    vsf_this.buffer_item_cnt = buffer_item_cnt;
    vsf_this.length = is_init_as_full ? buffer_item_cnt : 0;
}

int32_t __vsf_rng_buf_send_one(vsf_rng_buf_t *obj_ptr)
{
    int32_t index = -1;
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    VSF_ASSERT( NULL != obj_ptr);

    do {
        if ((vsf_this.head == vsf_this.tail) && (vsf_this.length > 0)) {
            /*! this queue is full */
            break;
        }
        index = vsf_this.tail++;
        vsf_this.length++;
        if (vsf_this.tail >= vsf_this.buffer_item_cnt) {
            vsf_this.tail = 0;
        }
    } while(0);

    return index;
}

int32_t __vsf_rng_buf_get_one(vsf_rng_buf_t* obj_ptr)
{
    int32_t index = -1;
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    VSF_ASSERT(NULL != obj_ptr);

    do {
        if ((vsf_this.head == vsf_this.tail) && (vsf_this.length == 0)) {
            /*! this queue is empty */
            break;
        }
        index = vsf_this.head++;
        vsf_this.length--;

        /* reset peek */
        vsf_this.peek = vsf_this.head;
        vsf_this.peek_cnt = 0;

        if (vsf_this.head >= vsf_this.buffer_item_cnt) {
            vsf_this.head = 0;
        }
    } while (0);

    return index;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_item_count")
uint_fast16_t __vsf_rng_buf_item_count(vsf_rng_buf_t* obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    VSF_ASSERT(NULL != obj_ptr);

    return vsf_this.length;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_send_multiple")
int32_t __vsf_rng_buf_send_multiple(vsf_rng_buf_t *obj_ptr, 
                                    uint16_t *item_cnt_ptr)
{
    
    int32_t index = -1;
    uint_fast16_t hwWritten = 0;
    uint_fast16_t hwSpaceLeft;
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);
    
    VSF_ASSERT(NULL != obj_ptr && NULL != item_cnt_ptr);

    do {
        if ((vsf_this.head == vsf_this.tail) && (vsf_this.length > 0)) {
            /*! this queue is full */
            break;
        }
        index = vsf_this.tail;

        hwSpaceLeft = vsf_this.buffer_item_cnt - vsf_this.length;
        hwWritten = vsf_this.buffer_item_cnt - vsf_this.tail;
        hwWritten = vsf_min((*item_cnt_ptr), hwWritten);
        hwWritten = vsf_min(hwWritten, hwSpaceLeft);

        *item_cnt_ptr = hwWritten;  /*!< update actual written number */
        
        vsf_this.tail += hwWritten;
        vsf_this.length += hwWritten;

        if (vsf_this.tail >= vsf_this.buffer_item_cnt) {
            vsf_this.tail = 0;
        }
    } while (0);

    return index;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_get_multiple")
int32_t __vsf_rng_buf_get_multiple( vsf_rng_buf_t* obj_ptr, 
                                    uint16_t* item_cnt_ptr)
{
    
    int32_t index = -1;
    uint_fast16_t hwRead = 0;
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);
    
    VSF_ASSERT(NULL != obj_ptr && NULL != item_cnt_ptr);

    do {
        if ((vsf_this.head == vsf_this.tail) && (vsf_this.length == 0)) {
            /*! this queue is empty */
            break;
        }
        index = vsf_this.head;

        hwRead = vsf_this.buffer_item_cnt - vsf_this.head;
        hwRead = vsf_min((*item_cnt_ptr), hwRead);
        hwRead = vsf_min(vsf_this.length, hwRead);

        *item_cnt_ptr = hwRead;     /*!< update actual written number */

        vsf_this.head += hwRead;
        vsf_this.length -= hwRead;

        /* reset peek */
        vsf_this.peek = vsf_this.head;
        vsf_this.peek_cnt = 0;

        if (vsf_this.head >= vsf_this.buffer_item_cnt) {
            vsf_this.head = 0;
        }
    } while (0);

    return index;
}


SECTION(".text.vsf.utilities.__vsf_rng_buf_peek_one")
int32_t __vsf_rng_buf_peek_one(vsf_rng_buf_t* obj_ptr)
{
    int32_t index = -1;
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    VSF_ASSERT(NULL != obj_ptr);

    do {
        if ((vsf_this.peek == vsf_this.tail) && (0 != vsf_this.peek_cnt)) {
            /*! all items have been peeked */
            break;
        }
        index = vsf_this.peek++;
        vsf_this.peek_cnt++;
        if (vsf_this.peek >= vsf_this.buffer_item_cnt) {
            vsf_this.peek = 0;
        }
    } while (0);

    return index;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_reset_peek")
void __vsf_rng_buf_reset_peek(vsf_rng_buf_t* obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);
    VSF_ASSERT(NULL != obj_ptr);

    vsf_this.peek = vsf_this.head;
    vsf_this.peek_cnt = 0;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_get_all_peeked")
void __vsf_rng_buf_get_all_peeked(vsf_rng_buf_t* obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);
    VSF_ASSERT(NULL != obj_ptr);

    vsf_this.head = vsf_this.peek;
    vsf_this.length -= vsf_this.peek_cnt;
    vsf_this.peek_cnt = 0;
}


SECTION(".text.vsf.utilities.__vsf_rng_buf_item_count_peekable")
uint_fast16_t __vsf_rng_buf_item_count_peekable(vsf_rng_buf_t* obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    VSF_ASSERT(NULL != obj_ptr);

    return vsf_this.length - vsf_this.peek_cnt;
}


SECTION(".text.vsf.utilities.__vsf_rng_buf_peek_multiple")
int32_t __vsf_rng_buf_peek_multiple(vsf_rng_buf_t* obj_ptr, 
                                    uint16_t* item_cnt_ptr)
{
    int32_t index = -1;
    uint_fast16_t hwItemLeft;
    uint_fast16_t hwPeeked  = 0;
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    VSF_ASSERT(NULL != obj_ptr && NULL != item_cnt_ptr);

    do {
        if ((vsf_this.peek == vsf_this.tail) && (vsf_this.peek_cnt != 0)) {
            /*! this queue is empty */
            break;
        }
        index = vsf_this.peek;
        hwItemLeft = vsf_this.length - vsf_this.peek_cnt;

        hwPeeked = vsf_this.buffer_item_cnt - vsf_this.peek;
        hwPeeked = vsf_min((*item_cnt_ptr), hwPeeked);
        hwPeeked = vsf_min(hwItemLeft, hwPeeked);

        *item_cnt_ptr = hwPeeked;   /*!< update actual written number */

        vsf_this.peek += hwPeeked;
        vsf_this.peek_cnt += hwPeeked;

        if (vsf_this.peek >= vsf_this.buffer_item_cnt) {
            vsf_this.peek = 0;
        }
    } while (0);

    return index;
}
