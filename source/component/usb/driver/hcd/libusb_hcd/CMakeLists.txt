# CMakeLists head

# check libusb
find_package(PkgConfig)
pkg_check_modules(libusb-1.0 REQUIRED libusb-1.0)
target_compile_definitions(${VSF_LIB_NAME} INTERFACE
    VSF_LIBUSB_CFG_INCLUDE="${libusb-1.0_INCLUDE_DIRS}/libusb.h"
)
target_link_libraries(${VSF_LIB_NAME} INTERFACE
    ${libusb-1.0_LIBRARIES}
)

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_libusb_hcd.c
)
