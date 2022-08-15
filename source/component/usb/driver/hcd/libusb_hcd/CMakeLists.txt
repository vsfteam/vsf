# CMakeLists head

# libusb only available in windows/linux/macos
if(VSF_HOST_HAS_LIBUSB)
    # check libusb
    find_package(PkgConfig)
    pkg_check_modules(libusb-1.0 libusb-1.0)
    vsf_add_compile_definitions(
        VSF_LIBUSB_CFG_INCLUDE="${libusb-1.0_INCLUDE_DIRS}/libusb.h"
    )
    vsf_add_libraries(
        ${libusb-1.0_LIBRARIES}
    )

    vsf_add_sources(
        vsf_libusb_hcd.c
    )
endif()