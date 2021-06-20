# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    retarget_io.c
)

# TODO: check for Core_Legacy
target_include_directories(${VSF_LIB_NAME} INTERFACE
    3rd-party/CMSIS/CMSIS/Core/Include
    3rd-party/CMSIS/CMSIS/DSP/Include
    3rd-party/CMSIS/CMSIS/NN/Include
)
