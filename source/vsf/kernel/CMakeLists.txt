# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_eda.c
    vsf_eda_slist_queue.c
    vsf_evtq_array.c
    vsf_evtq_list.c
    vsf_kernel_bsp.c
    vsf_os.c
)

add_subdirectory(__eda)
add_subdirectory(shell)
add_subdirectory(task)
