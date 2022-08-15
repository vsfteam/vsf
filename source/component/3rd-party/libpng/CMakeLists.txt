if(NOT VSF_LIBPNG_PATH)
    set(VSF_LIBPNG_PATH ${VSF_SRC_PATH}/component/3rd-party/libpng/raw)
endif()

vsf_add_include_directories(
    port
    ${VSF_LIBPNG_PATH}
)
#vsf_add_compile_definitions(
#)

vsf_add_sources(
    ${VSF_LIBPNG_PATH}/png.c
    ${VSF_LIBPNG_PATH}/pngerror.c
    ${VSF_LIBPNG_PATH}/pngget.c
    ${VSF_LIBPNG_PATH}/pngmem.c
    ${VSF_LIBPNG_PATH}/pngpread.c
    ${VSF_LIBPNG_PATH}/pngread.c
    ${VSF_LIBPNG_PATH}/pngrio.c
    ${VSF_LIBPNG_PATH}/pngrtran.c
    ${VSF_LIBPNG_PATH}/pngrutil.c
    ${VSF_LIBPNG_PATH}/pngset.c
    ${VSF_LIBPNG_PATH}/pngtrans.c
    ${VSF_LIBPNG_PATH}/pngwio.c
    ${VSF_LIBPNG_PATH}/pngwrite.c
    ${VSF_LIBPNG_PATH}/pngwtran.c
    ${VSF_LIBPNG_PATH}/pngwutil.c
)
