# CMakeLists head

target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    tgui_main.c
    tgui_demo/tgui_custom.c
    tgui_demo/tgui_port.c
    tgui_demo/images/demo_images.c
    tgui_demo/stopwatch/stopwatch.c
)
