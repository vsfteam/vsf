# CMakeLists head

cmake_policy(SET CMP0076 NEW)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    config
)

add_subdirectory(demo)
