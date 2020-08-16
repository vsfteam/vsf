/****************************************************************************
*  Copyright 2018 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
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
#include "../compiler/compiler.h"
#include "./vsf_queue.h"

/*============================ MACROS ========================================*/
#undef this
#define this    (*this_ptr)
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
    this.buffer_item_cnt = buffer_item_cnt;
    this.length = is_init_as_full ? buffer_item_cnt : 0;
}

int32_t __vsf_rng_buf_send_one(vsf_rng_buf_t *obj_ptr)
{
    int32_t index = -1;
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    ASSERT( NULL != obj_ptr);

    do {
        if ((this.head == this.tail) && (this.length > 0)) {
            /*! this queue is full */
            break;
        }
        index = this.tail++;
        this.length++;
        if (this.tail >= this.buffer_item_cnt) {
            this.tail = 0;
        }
    } while(0);

    return index;
}

int32_t __vsf_rng_buf_get_one(vsf_rng_buf_t* obj_ptr)
{
    int32_t index = -1;
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    ASSERT(NULL != obj_ptr);

    do {
        if ((this.head == this.tail) && (this.length == 0)) {
            /*! this queue is empty */
            break;
        }
        index = this.head++;
        this.length--;

        /* reset peek */
        this.peek = this.head;
        this.peek_cnt = 0;

        if (this.head >= this.buffer_item_cnt) {
            this.head = 0;
        }
    } while (0);

    return index;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_item_count")
uint_fast16_t __vsf_rng_buf_item_count(vsf_rng_buf_t* obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    ASSERT(NULL != obj_ptr);

    return this.length;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_send_multiple")
int32_t __vsf_rng_buf_send_multiple(vsf_rng_buf_t *obj_ptr, 
                                    uint16_t *item_cnt_ptr)
{
    
    int32_t index = -1;
    uint_fast16_t hwWritten = 0;
    uint_fast16_t hwSpaceLeft;
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);
    
    ASSERT(NULL != obj_ptr && NULL != item_cnt_ptr);

    do {
        if ((this.head == this.tail) && (this.length > 0)) {
            /*! this queue is full */
            break;
        }
        index = this.tail;

        hwSpaceLeft = this.buffer_item_cnt - this.length;
        hwWritten = this.buffer_item_cnt - this.tail;
        hwWritten = min((*item_cnt_ptr), hwWritten);
        hwWritten = min(hwWritten, hwSpaceLeft);

        *item_cnt_ptr = hwWritten;  /*!< update actual written number */
        
        this.tail += hwWritten;
        this.length += hwWritten;

        if (this.tail >= this.buffer_item_cnt) {
            this.tail = 0;
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
    
    ASSERT(NULL != obj_ptr && NULL != item_cnt_ptr);

    do {
        if ((this.head == this.tail) && (this.length == 0)) {
            /*! this queue is empty */
            break;
        }
        index = this.head;

        hwRead = this.buffer_item_cnt - this.head;
        hwRead = min((*item_cnt_ptr), hwRead);
        hwRead = min(this.length, hwRead);

        *item_cnt_ptr = hwRead;     /*!< update actual written number */

        this.head += hwRead;
        this.length -= hwRead;

        /* reset peek */
        this.peek = this.head;
        this.peek_cnt = 0;

        if (this.head >= this.buffer_item_cnt) {
            this.head = 0;
        }
    } while (0);

    return index;
}


SECTION(".text.vsf.utilities.__vsf_rng_buf_peek_one")
int32_t __vsf_rng_buf_peek_one(vsf_rng_buf_t* obj_ptr)
{
    int32_t index = -1;
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    ASSERT(NULL != obj_ptr);

    do {
        if ((this.peek == this.tail) && (0 != this.peek_cnt)) {
            /*! all items have been peeked */
            break;
        }
        index = this.peek++;
        this.peek_cnt++;
        if (this.peek >= this.buffer_item_cnt) {
            this.peek = 0;
        }
    } while (0);

    return index;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_reset_peek")
void __vsf_rng_buf_reset_peek(vsf_rng_buf_t* obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);
    ASSERT(NULL != obj_ptr);

    this.peek = this.head;
    this.peek_cnt = 0;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_get_all_peeked")
void __vsf_rng_buf_get_all_peeked(vsf_rng_buf_t* obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);
    ASSERT(NULL != obj_ptr);

    this.head = this.peek;
    this.length -= this.peek_cnt;
    this.peek_cnt = 0;
}


SECTION(".text.vsf.utilities.__vsf_rng_buf_item_count_peekable")
uint_fast16_t __vsf_rng_buf_item_count_peekable(vsf_rng_buf_t* obj_ptr)
{
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    ASSERT(NULL != obj_ptr);

    return this.length - this.peek_cnt;
}


SECTION(".text.vsf.utilities.__vsf_rng_buf_peek_multiple")
int32_t __vsf_rng_buf_peek_multiple(vsf_rng_buf_t* obj_ptr, 
                                    uint16_t* item_cnt_ptr)
{
    int32_t index = -1;
    uint_fast16_t hwItemLeft;
    uint_fast16_t hwPeeked  = 0;
    class_internal(obj_ptr, this_ptr, vsf_rng_buf_t);

    ASSERT(NULL != obj_ptr && NULL != item_cnt_ptr);

    do {
        if ((this.peek == this.tail) && (this.peek_cnt != 0)) {
            /*! this queue is empty */
            break;
        }
        index = this.peek;
        hwItemLeft = this.length - this.peek_cnt;

        hwPeeked = this.buffer_item_cnt - this.peek;
        hwPeeked = min((*item_cnt_ptr), hwPeeked);
        hwPeeked = min(hwItemLeft, hwPeeked);

        *item_cnt_ptr = hwPeeked;   /*!< update actual written number */

        this.peek += hwPeeked;
        this.peek_cnt += hwPeeked;

        if (this.peek >= this.buffer_item_cnt) {
            this.peek = 0;
        }
    } while (0);

    return index;
}
