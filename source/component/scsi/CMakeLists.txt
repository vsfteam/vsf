# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_scsi.c
)

add_subdirectory(driver)
