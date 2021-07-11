# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    vsf_input_gamepad.c
    vsf_input_keyboard.c
    vsf_input_mouse.c
    vsf_input_sensor.c
    vsf_input_touchscreen.c
)
