# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    retarget_io.c
)

target_include_directories(${VSF_LIB_NAME} INTERFACE
  CMSIS/Core/Include
  CMSIS/DSP/Include
  CMSIS/NN/Include
)
