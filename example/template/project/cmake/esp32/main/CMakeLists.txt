set(vsf_template_path "../../../../")
set(vsf_template_srcs
    "${vsf_template_path}demo/common/usrapp_common.c"
    "${vsf_template_path}demo/linux_demo/linux_demo.c"
    "${vsf_template_path}demo/fakefat32.c"
)
set(vsf_template_include_dirs
    "${vsf_template_path}config"
)

idf_component_register(
    SRCS "usr_dbg_stream.c" "app_main.c" ${vsf_template_srcs}
    INCLUDE_DIRS ${vsf_template_include_dirs})

idf_component_set_property(main ORIG_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
