if(NOT VSF_MBEDTLS_PATH)
    set(VSF_MBEDTLS_PATH ${VSF_SRC_PATH}/component/3rd-party/mbedtls/raw)
endif()

vsf_add_sources(
    port/platform_vsf.c
    extension/vplt/mbedtls_vplt.c
    extension/tls_session/mbedtls_tls_session.c
)
vsf_add_compile_definitions(
    MBEDTLS_USER_CONFIG_FILE="component/3rd-party/mbedtls/port/platform_vsf.h"
)

if(VSF_MBEDTLS_CONFIG_PATH)
    message(STATUS "VSF_MBEDTLS_CONFIG_PATH: ${VSF_MBEDTLS_CONFIG_PATH}")
    vsf_add_include_directories(
        ${VSF_MBEDTLS_CONFIG_PATH}
    )
endif()
vsf_add_include_directories(
    ${VSF_MBEDTLS_PATH}/include
)

file(GLOB VSF_MBEDTLS_LIBRARY_SRCS ${VSF_MBEDTLS_PATH}/library/*.c)
vsf_add_sources(
    ${VSF_MBEDTLS_LIBRARY_SRCS}
)