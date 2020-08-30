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
 
    NAME    spl_entry
    EXTERN  spl_c_entry
    PUBLIC  _entry

    SECTION .image_entry:CODE:ROOT(2)
    CODE32

    // boot header

    // uint32_t instruction;
    B       _entry

    DATA
    // magic[8];
    DCB     'e', 'G', 'O', 'N', '.', 'B', 'T', '0'

    // uint32_t checksum;       // in section checksum
    

    SECTION .image_length:CODE:ROOT(2)
    DATA
    // uint32_t length;
    DCD     8 * 1024
    // uint8_t spl_signature[4];
    DCB     'S', 'P', 'L', 2
    // uint32_t fel_script_address, fel_uenv_length, dt_name_offset, reserved, boot_media;
    DCD     0, 0, 0, 0, 0
    // uint32_t string_pool[13];
    DCD     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

    CODE32
_entry
    // Enter svc mode and mask fiq&irq
    mrs     r0, cpsr
    bic     r0, r0, #0x1f
    orr     r0, r0, #0xd3
    msr     cpsr_cxsf, r0

    bl      spl_c_entry
    bx      lr

    END
