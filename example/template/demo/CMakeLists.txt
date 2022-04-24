# CMakeLists head

cmake_policy(SET CMP0076 NEW)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    .
)

target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    fakefat32.c
)

add_subdirectory(btstack_demo)
add_subdirectory(common)
add_subdirectory(disp_demo)
add_subdirectory(freetype_demo)
add_subdirectory(hal_demo)
add_subdirectory(kernel_test)
add_subdirectory(linux_demo)
add_subdirectory(lvgl_demo)
add_subdirectory(lua_demo)
add_subdirectory(lwip_demo)
add_subdirectory(sdl2_demo)
add_subdirectory(tgui_demo)
add_subdirectory(usbd_demo)
add_subdirectory(usbh_demo)
add_subdirectory(cpu_usage_demo)
add_subdirectory(scsi_demo)

if(VSF_HAL_CHIP_NAME STREQUAL "AIC8800M")
    add_subdirectory(aic8800)
endif()
