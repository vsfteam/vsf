# CMakeLists head

vsf_add_sources(
    vsf_linux_glibc.c
    vsf_linux_glibc_pthread.c
    vsf_linux_glibc_sem.c
    vsf_linux_glibc_stdio.c
    vsf_linux_glibc_stdlib.c
    vsf_linux_glibc_string.c
    vsf_linux_glibc_time.c
    vsf_linux_glibc_ctype.c
)

# add VSF_CPP to configure whether to include libc++?
if(1)
    vsf_add_sources(
        vsf_linux_glibcpp.cc
        vsf_linux_glibcpp_string.cpp
    )
endif()
