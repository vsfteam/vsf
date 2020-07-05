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
 
    NAME    entry
    EXTERN  __iar_program_start
    PUBLIC  _entry

save_regs       macro   
                local   b_10f, b_11f
                str     lr, [sp, #-4]
                mrs     lr, spsr
                str     lr, [sp, #-8]
                str     r1, [sp, #-12]
                str     r0, [sp, #-16]
                mov     r0, sp
                mrs     lr, cpsr
                and     lr, lr, #~(0x1f)
                orr     lr, lr, #0x13
                msr     cpsr_c, lr
                ldr     r1, [r0, #-4]
                str     r1, [sp, #-4]!
                ldr     r1, [r0, #-8]
                str     r1, [sp, #-(4 * 16)]
                ldr     r1, [r0, #-12]
                ldr     r0, [r0, #-16]
                stmdb   sp, {r0 - r14}^
                sub     sp, sp, #(4 * 16)
                ldr     r4, [sp]
                and     r0, r4, #0x1f
                cmp     r0, #0x10
                beq     b_10f
                cmp     r0, #0x13
                beq     b_11f
                b       .
b_10f:          add     r1, sp, #(4 * 17)
                str     r1, [sp, #(4 * 14)]
                str     lr, [sp, #(4 * 15)]
b_11f:          add     r1, sp, #(4 * 17)
                str     r1, [sp, #-4]!
                mov     r0, sp
                endm

restore_regs    macro
                local b_20, b_21
                mov     r12, sp
                ldr     sp, [r12], #4
                ldr     r1, [r12], #4
                msr     spsr_cxsf, r1
                and     r0, r1, #0x1f
                cmp     r0, #0x10
                beq     b_20
                cmp     r0, #0x13
                beq     b_21
                b       .
b_20:           ldr     lr, [r12, #(4 * 15)]
                ldmia   r12, {r0 - r14}^
                movs    pc, lr
b_21:           ldm     r12, {r0 - r15}^
                mov     r0, r0
                endm


    SECTION .intvec:CODE:ROOT(2)

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

    CODE32
_vector:
    // uint32_t jmp_instruction;
    b       _entry
    ldr     pc, _undefined_instruction
    ldr     pc, _software_interrupt
    ldr     pc, _prefetch_abort
    ldr     pc, _data_abort
    ldr     pc, _not_used
    ldr     pc, _irq
    ldr     pc, _fiq

    DATA32
_undefined_instruction:
    DCD     undefined_instruction
_software_interrupt:
    DCD     software_interrupt
_prefetch_abort:
    DCD     prefetch_abort
_data_abort:
    DCD     data_abort
_not_used:
    DCD     not_used
_irq:
    DCD     irq
_fiq:
    DCD     fiq

//  TODO: .align 5 for all vector entries
undefined_instruction:
    sub     lr, lr, #4
    save_regs
//    bl      UndefindInstruction_Handler
    restore_regs

software_interrupt:
    sub     lr, lr, #4
    save_regs
//    bl      SWI_Handler
    restore_regs

prefetch_abort:
    sub     lr, lr, #4
    save_regs
//    bl      PrefetchAbort_Handler
    restore_regs

data_abort:
    sub     lr, lr, #8
    save_regs
//    bl      DataAbort_Handler
    restore_regs

not_used:
    b .

irq:
    sub     lr, lr, #4
    save_regs
//    bl      IRQ_Handler
    restore_regs

fiq:
    sub     lr, lr, #4
    save_regs
//    bl      FIQ_Handler
    restore_regs

_entry:
    // TODO: setup vector, SP, etc
    b       __iar_program_start

    END
