if(NOT VSF_ZLIB_PATH)
    set(VSF_ZLIB_PATH ${VSF_SRC_PATH}/component/3rd-party/zlib/raw)
endif()

target_include_directories(${VSF_LIB_NAME} INTERFACE
    ${VSF_ZLIB_PATH}
)
#target_compile_definitions(${VSF_LIB_NAME} INTERFACE
#)

target_sources(${VSF_LIB_NAME} INTERFACE
    ${VSF_ZLIB_PATH}/adler32.c
    ${VSF_ZLIB_PATH}/compress.c
    ${VSF_ZLIB_PATH}/crc32.c
    ${VSF_ZLIB_PATH}/deflate.c
    ${VSF_ZLIB_PATH}/gzclose.c
    ${VSF_ZLIB_PATH}/gzlib.c
    ${VSF_ZLIB_PATH}/gzread.c
    ${VSF_ZLIB_PATH}/gzwrite.c
    ${VSF_ZLIB_PATH}/infback.c
    ${VSF_ZLIB_PATH}/inffast.c
    ${VSF_ZLIB_PATH}/inflate.c
    ${VSF_ZLIB_PATH}/inftrees.c
    ${VSF_ZLIB_PATH}/trees.c
    ${VSF_ZLIB_PATH}/uncompr.c
    ${VSF_ZLIB_PATH}/zutil.c
)
