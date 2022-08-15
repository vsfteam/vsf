# CMakeLists head

vsf_add_sources(
    rv_generic.c
)

if(CONFIG_64BIT)
    set_property(GLOBAL PROPERTY PROPERTY_OUTPUT_FORMAT elf64-littleriscv)
else()
    set_property(GLOBAL PROPERTY PROPERTY_OUTPUT_FORMAT elf32-littleriscv)
endif()
