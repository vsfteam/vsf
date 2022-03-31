if(NOT VSF_MBEDTLS_PATH)
    set(VSF_MBEDTLS_PATH ${VSF_SRC_PATH}/component/3rd-party/mbedtls/raw)
endif()

target_sources(${VSF_LIB_NAME} INTERFACE
    port/platform_vsf.c
)
target_compile_definitions(${VSF_LIB_NAME} INTERFACE
    MBEDTLS_USER_CONFIG_FILE="component/3rd-party/mbedtls/port/platform_vsf.h"
)

if(VSF_MBEDTLS_CONFIG_PATH)
    message(STATUS "VSF_MBEDTLS_CONFIG_PATH: ${VSF_MBEDTLS_CONFIG_PATH}")
    target_include_directories(${VSF_LIB_NAME} INTERFACE
        ${VSF_MBEDTLS_CONFIG_PATH}
    )
endif()
target_include_directories(${VSF_LIB_NAME} INTERFACE
    ${VSF_MBEDTLS_PATH}/include
)

file(GLOB VSF_MBEDTLS_LIBRARY_SRCS ${VSF_MBEDTLS_PATH}/library/*.c)
target_sources(${VSF_LIB_NAME} INTERFACE
    ${VSF_MBEDTLS_LIBRARY_SRCS}
)