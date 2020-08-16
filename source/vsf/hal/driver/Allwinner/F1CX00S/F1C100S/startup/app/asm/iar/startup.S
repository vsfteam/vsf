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
 
    NAME    startup
    EXTERN  __iar_program_start
    EXTERN  _vector
    PUBLIC  _entry

    SECTION .image_head:CODE:ROOT(2)

    DATA
    // image_head_t
    // uint32_t magic;
    DCD     0x474D495F
    // uint32_t flags;
    DCD     0
    // uint32_t compressed_start;
    DCD     0
    // uint32_t compressed_size;
    DCD     0
    // uint32_t original_size;
    // TODO: fix to image size
    DCD     8 * 1024
    // uint32_t link_address;
    DCD     0x80000000

    ARM
_entry:
    // set vector to the low address
    mrc     p15, 0, r0, c1, c0, 0
    bic     r0, r0, #(1 << 13)
    mcr     p15, 0, r0, c1, c0, 0

    // copy vector to the correct address
    //adrl     r0, _vector
    ldr     r0, =_vector
    mrc     p15, 0, r2, c1, c0, 0
    ands    r2, r2, #(1 << 13)
    ldreq   r1, =0x00000000
    ldrne   r1, =0xffff0000
    ldmia   r0!, {r2-r8, r10}
    stmia   r1!, {r2-r8, r10}
    ldmia   r0!, {r2-r8, r10}
    stmia   r1!, {r2-r8, r10}

    b       __iar_program_start

    END
