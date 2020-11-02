# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
  vsf_arch_abstraction.c
)

add_subdirectory_existed(${VSF_ARCH_SERIES})
