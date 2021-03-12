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

    SECTION FIQ_STACK:DATA:NOROOT(3)
    SECTION IRQ_STACK:DATA:NOROOT(3)
    SECTION SVC_STACK:DATA:NOROOT(3)
    SECTION ABT_STACK:DATA:NOROOT(3)
    SECTION UND_STACK:DATA:NOROOT(3)

    EXTERN  SWI_Handler
    EXTERN  IRQ_Handler
    EXTERN  FIQ_Handler
    EXTERN  _entry
    PUBLIC  _vector

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
    CODE32
_vector:
    // uint32_t jmp_instruction;
    ldr     pc, _reset_handler
    ldr     pc, _undefined_instruction
    ldr     pc, _software_interrupt
    ldr     pc, _prefetch_abort
    ldr     pc, _data_abort
    ldr     pc, _not_used
    ldr     pc, _irq
    ldr     pc, _fiq

    DATA32
_reset_handler
    DCD     reset_handler
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

    ARM

reset_handler:
; --------------------
; Mode, correspords to bits 0-5 in CPSR

#define MODE_MSK 0x1F            ; Bit mask for mode bits in CPSR

#define USR_MODE 0x10            ; User mode
#define FIQ_MODE 0x11            ; Fast Interrupt Request mode
#define IRQ_MODE 0x12            ; Interrupt Request mode
#define SVC_MODE 0x13            ; Supervisor mode
#define ABT_MODE 0x17            ; Abort mode
#define UND_MODE 0x1B            ; Undefined Instruction mode
#define SYS_MODE 0x1F            ; System mode


    MRS     r0, cpsr                ; Original PSR value

    ;; Set up the svc stack pointer.
    BIC     r0, r0, #MODE_MSK       ; Clear the mode bits
    ORR     r0, r0, #SVC_MODE       ; Set IRQ mode bits
    MSR     cpsr_c, r0              ; Change the mode
    ldr     r1, =sfe(SVC_STACK)     ; End of SVC_STACK
    BIC     sp,r1,#0x7              ; Make sure SP is 8 aligned

    ;; Set up the abort stack pointer.
    BIC     r0, r0, #MODE_MSK       ; Clear the mode bits
    ORR     r0, r0, #ABT_MODE       ; Set IRQ mode bits
    MSR     cpsr_c, r0              ; Change the mode
    ldr     r1, =sfe(ABT_STACK)     ; End of ABT_STACK
    BIC     sp,r1,#0x7              ; Make sure SP is 8 aligned

    ;; Set up the undefined instruction stack pointer.
    BIC     r0, r0, #MODE_MSK       ; Clear the mode bits
    ORR     r0, r0, #UND_MODE       ; Set IRQ mode bits
    MSR     cpsr_c, r0              ; Change the mode
    ldr     r1, =sfe(UND_STACK)     ; End of UND_STACK
    BIC     sp,r1,#0x7              ; Make sure SP is 8 aligned

    b       _entry

//  TODO: .align 5 for all vector entries
undefined_instruction:
prefetch_abort:
data_abort:
not_used:
    b .

software_interrupt:
    sub     lr, lr, #4
    save_regs
    bl      SWI_Handler
    restore_regs

irq:
    ; IAR provide __irq __nested __arm to handle those
    ;sub     lr, lr, #4
    ;save_regs
    b      IRQ_Handler
    ;restore_regs

fiq:
    ; IAR provide __fiq __nested __arm to handle those
    ;sub     lr, lr, #4
    ;save_regs
    bl      FIQ_Handler
    ;restore_regs

    END
