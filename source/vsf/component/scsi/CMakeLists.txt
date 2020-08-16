# CMakeLists head

target_sources(vsf INTERFACE
    vsf_scsi.c
)

add_subdirectory(driver)
