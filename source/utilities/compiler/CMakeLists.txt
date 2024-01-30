# CMakeLists head

add_subdirectory(__common)
add_subdirectory_existed(${VSF_ARCH_SERIES})

vsf_add_sources(
    vsf_compiler.c
)
