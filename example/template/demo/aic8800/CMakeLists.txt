# CMakeLists head

target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    aic8800_demo.c
    aic8800_driver_init.c
)

add_subdirectory(bt_demo)
add_subdirectory(wifi_demo)
