# CMakeLists head

cmake_policy(SET CMP0076 NEW)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    .
)

add_subdirectory(linux_demo)
add_subdirectory(lvgl_demo)
#add_subdirectory(evm_demo)
