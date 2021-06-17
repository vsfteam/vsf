if(NOT VSF_LVGL_PATH)
    set(VSF_LVGL_PATH ${VSF_SRC_PATH}/component/3rd-party/lvgl/raw/lvgl)
endif()
add_subdirectory(${VSF_LVGL_PATH} ${CMAKE_CURRENT_BINARY_DIR}/lvgl)

target_sources(${VSF_LIB_NAME} INTERFACE
    ${VSF_SRC_PATH}/component/3rd-party/lvgl/port/vsf_lvgl_port.c
)
target_include_directories(${VSF_LIB_NAME} INTERFACE
    ${VSF_SRC_PATH}/component/3rd-party/lvgl/raw
)
if (VSF_LVGL_USE_FREETYPE)
    target_sources(${VSF_LIB_NAME} INTERFACE
        ${VSF_SRC_PATH}/component/3rd-party/lvgl/extension/lv_lib_freetype/raw/lv_freetype.c
    )
endif()
