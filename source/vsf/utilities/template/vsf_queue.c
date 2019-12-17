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
#include "vsf_cfg.h"
#include "./vsf_queue.h"

/*============================ MACROS ========================================*/
#undef this
#define this    (*ptThis)
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

void __vsf_rng_buf_init(  vsf_rng_buf_t* ptObj, 
                        uint_fast16_t hwBufferItemCount, 
                        bool bInitAsFull)
{
    class_internal(ptObj, ptThis, vsf_rng_buf_t);
    memset(ptObj, 0, sizeof(vsf_rng_buf_t));
    this.hwBufferItemCount = hwBufferItemCount;
    this.hwLength = bInitAsFull ? hwBufferItemCount : 0;
}

int32_t __vsf_rng_buf_send_one(vsf_rng_buf_t *ptObj)
{
    int32_t nIndex = -1;
    class_internal(ptObj, ptThis, vsf_rng_buf_t);

    ASSERT( NULL != ptObj);

    do {
        if ((this.hwHead == this.hwTail) && (this.hwLength > 0)) {
            /*! this queue is full */
            break;
        }
        nIndex = this.hwTail++;
        this.hwLength++;
        if (this.hwTail >= this.hwBufferItemCount) {
            this.hwTail = 0;
        }
    } while(0);

    return nIndex;
}

int32_t __vsf_rng_buf_get_one(vsf_rng_buf_t* ptObj)
{
    int32_t nIndex = -1;
    class_internal(ptObj, ptThis, vsf_rng_buf_t);

    ASSERT(NULL != ptObj);

    do {
        if ((this.hwHead == this.hwTail) && (this.hwLength == 0)) {
            /*! this queue is empty */
            break;
        }
        nIndex = this.hwHead++;
        this.hwLength--;

        /* reset peek */
        this.hwPeek = this.hwHead;
        this.hwPeekCount = 0;

        if (this.hwHead >= this.hwBufferItemCount) {
            this.hwHead = 0;
        }
    } while (0);

    return nIndex;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_item_count")
uint_fast16_t __vsf_rng_buf_item_count(vsf_rng_buf_t* ptObj)
{
    class_internal(ptObj, ptThis, vsf_rng_buf_t);

    ASSERT(NULL != ptObj);

    return this.hwLength;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_send_multiple")
int32_t __vsf_rng_buf_send_multiple(vsf_rng_buf_t *ptObj, uint16_t *phwItemCount)
{
    
    int32_t nIndex = -1;
    uint_fast16_t hwWritten = 0;
    uint_fast16_t hwSpaceLeft;
    class_internal(ptObj, ptThis, vsf_rng_buf_t);
    
    ASSERT(NULL != ptObj && NULL != phwItemCount);

    do {
        if ((this.hwHead == this.hwTail) && (this.hwLength > 0)) {
            /*! this queue is full */
            break;
        }
        nIndex = this.hwTail;

        hwSpaceLeft = this.hwBufferItemCount - this.hwLength;
        hwWritten = this.hwBufferItemCount - this.hwTail;
        hwWritten = min((*phwItemCount), hwWritten);
        hwWritten = min(hwWritten, hwSpaceLeft);

        *phwItemCount = hwWritten;          /*!< update actual written number */
        
        this.hwTail += hwWritten;
        this.hwLength += hwWritten;

        if (this.hwTail >= this.hwBufferItemCount) {
            this.hwTail = 0;
        }
    } while (0);

    return nIndex;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_get_multiple")
int32_t __vsf_rng_buf_get_multiple(vsf_rng_buf_t* ptObj, uint16_t* phwItemCount)
{
    
    int32_t nIndex = -1;
    uint_fast16_t hwRead = 0;
    class_internal(ptObj, ptThis, vsf_rng_buf_t);
    
    ASSERT(NULL != ptObj && NULL != phwItemCount);

    do {
        if ((this.hwHead == this.hwTail) && (this.hwLength == 0)) {
            /*! this queue is empty */
            break;
        }
        nIndex = this.hwHead;

        hwRead = this.hwBufferItemCount - this.hwHead;
        hwRead = min((*phwItemCount), hwRead);
        hwRead = min(this.hwLength, hwRead);

        *phwItemCount = hwRead;          /*!< update actual written number */

        this.hwHead += hwRead;
        this.hwLength -= hwRead;

        /* reset peek */
        this.hwPeek = this.hwHead;
        this.hwPeekCount = 0;

        if (this.hwHead >= this.hwBufferItemCount) {
            this.hwHead = 0;
        }
    } while (0);

    return nIndex;
}


SECTION(".text.vsf.utilities.__vsf_rng_buf_peek_one")
int32_t __vsf_rng_buf_peek_one(vsf_rng_buf_t* ptObj)
{
    int32_t nIndex = -1;
    class_internal(ptObj, ptThis, vsf_rng_buf_t);

    ASSERT(NULL != ptObj);

    do {
        if ((this.hwPeek == this.hwTail) && (0 != this.hwPeekCount)) {
            /*! all items have been peeked */
            break;
        }
        nIndex = this.hwPeek++;
        this.hwPeekCount++;
        if (this.hwPeek >= this.hwBufferItemCount) {
            this.hwPeek = 0;
        }
    } while (0);

    return nIndex;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_reset_peek")
void __vsf_rng_buf_reset_peek(vsf_rng_buf_t* ptObj)
{
    class_internal(ptObj, ptThis, vsf_rng_buf_t);
    ASSERT(NULL != ptObj);

    this.hwPeek = this.hwHead;
    this.hwPeekCount = 0;
}

SECTION(".text.vsf.utilities.__vsf_rng_buf_get_all_peeked")
void __vsf_rng_buf_get_all_peeked(vsf_rng_buf_t* ptObj)
{
    class_internal(ptObj, ptThis, vsf_rng_buf_t);
    ASSERT(NULL != ptObj);

    this.hwHead = this.hwPeek;
    this.hwLength -= this.hwPeekCount;
    this.hwPeekCount = 0;
}


SECTION(".text.vsf.utilities.__vsf_rng_buf_item_count_peekable")
uint_fast16_t __vsf_rng_buf_item_count_peekable(vsf_rng_buf_t* ptObj)
{
    class_internal(ptObj, ptThis, vsf_rng_buf_t);

    ASSERT(NULL != ptObj);

    return this.hwLength - this.hwPeekCount;
}


SECTION(".text.vsf.utilities.__vsf_rng_buf_peek_multiple")
int32_t __vsf_rng_buf_peek_multiple(vsf_rng_buf_t* ptObj, uint16_t* phwItemCount)
{
    int32_t nIndex = -1;
    uint_fast16_t hwItemLeft;
    uint_fast16_t hwPeeked  = 0;
    class_internal(ptObj, ptThis, vsf_rng_buf_t);

    ASSERT(NULL != ptObj && NULL != phwItemCount);

    do {
        if ((this.hwPeek == this.hwTail) && (this.hwPeekCount != 0)) {
            /*! this queue is empty */
            break;
        }
        nIndex = this.hwPeek;
        hwItemLeft = this.hwLength - this.hwPeekCount;

        hwPeeked = this.hwBufferItemCount - this.hwPeek;
        hwPeeked = min((*phwItemCount), hwPeeked);
        hwPeeked = min(hwItemLeft, hwPeeked);

        *phwItemCount = hwPeeked;          /*!< update actual written number */

        this.hwPeek += hwPeeked;
        this.hwPeekCount += hwPeeked;

        if (this.hwPeek >= this.hwBufferItemCount) {
            this.hwPeek = 0;
        }
    } while (0);

    return nIndex;
}
