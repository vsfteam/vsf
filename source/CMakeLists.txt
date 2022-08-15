# CMakeLists head

if(NOT DEFINED ${VSF_SRC_PATH})
    get_filename_component(VSF_SRC_PATH . REALPATH)
    message(STATUS "VSF_SRC_PATH: ${VSF_SRC_PATH}")
endif()

vsf_add_include_directories(
    .
)

add_subdirectory(component)
add_subdirectory(hal)
add_subdirectory(kernel)
add_subdirectory(osa_hal)
add_subdirectory(osa_service)
add_subdirectory(service)
add_subdirectory(shell)
add_subdirectory(utilities)
