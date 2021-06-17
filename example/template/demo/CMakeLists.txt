# CMakeLists head

cmake_policy(SET CMP0076 NEW)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    .
)

target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    fakefat32.c
)

add_subdirectory(common)
add_subdirectory(linux_demo)
add_subdirectory(lvgl_demo)
add_subdirectory(freetype_demo)
#add_subdirectory(evm_demo)
