if(NOT VSF_FREETYPE_PATH)
    set(VSF_FREETYPE_PATH ${VSF_SRC_PATH}/component/3rd-party/freetype/raw)
endif()
# TODO: how to include projects based on makefile
# add_subdirectory(${VSF_LVGL_PATH} ${CMAKE_CURRENT_BINARY_DIR}/freetype)

target_include_directories(${VSF_LIB_NAME} INTERFACE
    ${VSF_SRC_PATH}/component/3rd-party/freetype/port
    ${VSF_FREETYPE_PATH}/include
)
