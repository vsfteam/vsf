# CMakeLists head

add_subdirectory_ifdef(VSF_USE_LVGL             lvgl)
add_subdirectory_ifdef(VSF_USE_FREETYPE         freetype)
add_subdirectory_ifdef(VSF_USE_LWIP             lwip)
add_subdirectory_ifdef(VSF_USE_BTSTACK          btstack)
add_subdirectory_ifdef(VSF_USE_COREMARK         coremark)
add_subdirectory_ifdef(VSF_USE_LUA              lua)
add_subdirectory_ifdef(VSF_USE_LIBJPEG_TURBO    libjpeg-turbo)
add_subdirectory_ifdef(VSF_USE_LIBPNG           libpng)
add_subdirectory_ifdef(VSF_USE_ZLIB             zlib)
add_subdirectory_ifdef(VSF_USE_EVM              evm)
add_subdirectory_ifdef(VSF_USE_LITTLEFS         littlefs)
add_subdirectory_ifdef(VSF_USE_MBEDTLS          mbedtls)

# protected by configurations
add_subdirectory(nuconsole)
add_subdirectory(segger)
