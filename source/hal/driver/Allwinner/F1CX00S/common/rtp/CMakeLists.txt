# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    rtp.c

    mean.c
    median.c
    tsfilter.c
)
