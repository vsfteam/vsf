# CMakeLists head

target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    aic8800_demo.c
)

add_subdirectory(bt_demo)
add_subdirectory(wifi_demo)
