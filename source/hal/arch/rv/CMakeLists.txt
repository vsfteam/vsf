# CMakeLists head

vsf_add_sources(
    rv_generic.c
    # gp-aware setjmp/longjmp(each function in its own section, gc-sections
    #  drops them when unused): standard RISC-V setjmp does not save gp(x3),
    #  which is required when gp serves as the thread register. the setjmp
    #  injection(VSF_ARCH_SETJMP/LONGJMP) is done in rv_generic.h at C
    #  level, where VSF_ARCH_USE_THREAD_REG is authoritatively visible
    rv_setjmp.S
)

if(CONFIG_64BIT)
    set_property(GLOBAL PROPERTY PROPERTY_OUTPUT_FORMAT elf64-littleriscv)
else()
    set_property(GLOBAL PROPERTY PROPERTY_OUTPUT_FORMAT elf32-littleriscv)
endif()
