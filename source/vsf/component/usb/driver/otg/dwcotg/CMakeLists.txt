# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_dwcotg_common.c
    vsf_dwcotg_dcd.c
    vsf_dwcotg_hcd.c
)
