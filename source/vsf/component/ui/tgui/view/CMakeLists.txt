# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_tgui_v.c
)

add_subdirectory(simple_view)
add_subdirectory(template)
