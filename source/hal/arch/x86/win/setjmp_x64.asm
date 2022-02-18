;------------------------------------------------------------------------------
;
; Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
; This program and the accompanying materials
; are licensed and made available under the terms and conditions of the BSD License
; which accompanies this distribution.  The full text of the license may be found at
; http://opensource.org/licenses/bsd-license.php.
;
; THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
; WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
;
; Module Name:
;
;   SetJump.Asm
;
; Abstract:
;
;   Implementation of SetJump() on x64.
;
;------------------------------------------------------------------------------

    .code

EXTERNDEF   InternalAssertJumpBuffer:PROC

;------------------------------------------------------------------------------
; UINTN
; EFIAPI
; SetJump (
;   OUT     BASE_LIBRARY_JUMP_BUFFER  *JumpBuffer
;   );
;------------------------------------------------------------------------------
SetJumpUser     PROC
    push    rcx
    ;add     rsp, -20h
    ;call    InternalAssertJumpBuffer
    ;add     rsp, 20h
    pop     rcx
    pop     rdx
    mov     [rcx], rbx
    mov     [rcx + 8], rsp
    mov     [rcx + 10h], rbp
    mov     [rcx + 18h], rdi
    mov     [rcx + 20h], rsi
    mov     [rcx + 28h], r12
    mov     [rcx + 30h], r13
    mov     [rcx + 38h], r14
    mov     [rcx + 40h], r15
    mov     [rcx + 48h], rdx
    ; save non-volatile fp registers
    stmxcsr [rcx + 50h]
    movdqu  [rcx + 58h], xmm6
    movdqu  [rcx + 68h], xmm7
    movdqu  [rcx + 78h], xmm8
    movdqu  [rcx + 88h], xmm9
    movdqu  [rcx + 98h], xmm10
    movdqu  [rcx + 0A8h], xmm11
    movdqu  [rcx + 0B8h], xmm12
    movdqu  [rcx + 0C8h], xmm13
    movdqu  [rcx + 0D8h], xmm14
    movdqu  [rcx + 0E8h], xmm15
    xor     rax, rax
    jmp     rdx
SetJumpUser     ENDP

;------------------------------------------------------------------------------
; VOID
; EFIAPI
; InternalLongJump (
;   IN      BASE_LIBRARY_JUMP_BUFFER  *JumpBuffer,
;   IN      UINTN                     Value
;   );
;------------------------------------------------------------------------------
LongJumpUser    PROC
    mov     rbx, [rcx]
    mov     rsp, [rcx + 8]
    mov     rbp, [rcx + 10h]
    mov     rdi, [rcx + 18h]
    mov     rsi, [rcx + 20h]
    mov     r12, [rcx + 28h]
    mov     r13, [rcx + 30h]
    mov     r14, [rcx + 38h]
    mov     r15, [rcx + 40h]
    ; load non-volatile fp registers
    ldmxcsr [rcx + 50h]
    movdqu  xmm6,  [rcx + 58h]
    movdqu  xmm7,  [rcx + 68h]
    movdqu  xmm8,  [rcx + 78h]
    movdqu  xmm9,  [rcx + 88h]
    movdqu  xmm10, [rcx + 98h]
    movdqu  xmm11, [rcx + 0A8h]
    movdqu  xmm12, [rcx + 0B8h]
    movdqu  xmm13, [rcx + 0C8h]
    movdqu  xmm14, [rcx + 0D8h]
    movdqu  xmm15, [rcx + 0E8h]
    mov     rax, rdx               ; set return value
    jmp     qword ptr [rcx + 48h]
LongJumpUser    ENDP

    END