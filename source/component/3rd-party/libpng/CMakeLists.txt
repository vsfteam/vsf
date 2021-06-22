if(NOT VSF_LIBPNG_PATH)
    set(VSF_LIBPNG_PATH ${VSF_SRC_PATH}/component/3rd-party/libpng/raw)
endif()

target_include_directories(${VSF_LIB_NAME} INTERFACE
    port
    ${VSF_LIBPNG_PATH}
)
#target_compile_definitions(${VSF_LIB_NAME} INTERFACE
#)

target_sources(${VSF_LIB_NAME} INTERFACE
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
