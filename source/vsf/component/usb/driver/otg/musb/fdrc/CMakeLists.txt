# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    __vsf_musb_fdrc_common.c
    vsf_musb_fdrc_dcd.c
    vsf_musb_fdrc_hcd.c
)
