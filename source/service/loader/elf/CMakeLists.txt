# CMakeLists head

vsf_add_sources(
    vsf_elfloader.c

    arch/vsf_elfloader_arm.c
    arch/vsf_elfloader_x64.c
)
