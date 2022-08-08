# CMakeLists head

if("${VSF_HOST_SYSTEM}" STREQUAL "macos")
    add_subdirectory_existed(linux)
else()
    add_subdirectory_existed(${VSF_HOST_SYSTEM})
endif()
