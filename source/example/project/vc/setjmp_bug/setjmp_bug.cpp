#include <stdint.h>
#include <setjmp.h>
#include <stdio.h>

struct usrapp_t {
    jmp_buf ret __attribute__((aligned(16)));
    jmp_buf pos __attribute__((aligned(16)));
    uint8_t stack[8192] __attribute__((aligned(16)));
};
typedef struct usrapp_t usrapp_t;

usrapp_t usrapp;

static inline __attribute__((always_inline)) void vsf_arch_set_stack(uintptr_t stack)
{
#if     defined(__CPU_X86__)
    __asm__("movl %0, %%esp" : : "r"(stack));
#elif   defined(__CPU_X64__)
    __asm__("movq %0, %%rsp" : : "r"(stack));
#endif
}

static void __vsf_thread_entry(void)
{
    printf("longjmp\r\n");
    longjmp(usrapp.ret, 0);
}

int main(void)
{
    printf("enter\r\n");
    if (!setjmp(usrapp.ret)) {
        printf("setjmp called\r\n");
        /*  If stack is switched, longjmp will fail.
            According to longjmp assembly code, it seems to access the variables on current stack.
        */
        vsf_arch_set_stack((uintptr_t)&usrapp.stack[sizeof(usrapp.stack)]);
        __vsf_thread_entry();
    }
    printf("leave\r\n");
    return 0;
}
