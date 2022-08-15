# CMakeLists head

vsf_add_sources(
    vsf_tgui.c
    vsf_tgui_common.c
    vsf_tgui_input.c
)

add_subdirectory(controls)
add_subdirectory(utilities)
add_subdirectory(view)
