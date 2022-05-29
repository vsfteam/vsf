# CMakeLists head

target_sources(${VSF_LIB_NAME} INTERFACE
    driver.c
    interrupt_swi.c

    startup/app/startup_F1C100S.c
)
