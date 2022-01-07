if(NOT VSF_LVGL_PATH)
    set(VSF_LVGL_PATH ${VSF_SRC_PATH}/component/3rd-party/lvgl/raw/lvgl)
endif()

target_sources(${VSF_LIB_NAME} INTERFACE
    ${VSF_SRC_PATH}/component/3rd-party/lvgl/port/vsf_lvgl_port.c
)
target_include_directories(${VSF_LIB_NAME} INTERFACE
    # some app will include lvgl/lvgl.h
    ${VSF_SRC_PATH}/component/3rd-party/lvgl/raw
    # some app will include lvgl.h
    ${VSF_SRC_PATH}/component/3rd-party/lvgl/raw/lvgl
)
if (VSF_LVGL_USE_FREETYPE)
    target_sources(${VSF_LIB_NAME} INTERFACE
        ${VSF_SRC_PATH}/component/3rd-party/lvgl/extension/lv_lib_freetype/raw/lv_freetype.c
    )
endif()

add_subdirectory(${VSF_LVGL_PATH} ${CMAKE_CURRENT_BINARY_DIR}/lvgl_bin)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/lvgl_bin)
target_link_libraries(${VSF_LIB_NAME} INTERFACE
    lvgl
)

# add vsf configuration for lvgl
target_compile_definitions(lvgl PRIVATE
    ${VSF_TARGET_DEFINITIONS}
)
target_include_directories(lvgl PRIVATE
    ${VSF_COMMON_INCLUDE_DIRECTORIES}
)
if(VSF_LVGL_CONFIG_PATH)
    message(STATUS "VSF_LVGL_CONFIG_PATH: ${VSF_LVGL_CONFIG_PATH}")
    target_include_directories(lvgl PUBLIC
        ${VSF_LVGL_CONFIG_PATH}
    )
endif()
