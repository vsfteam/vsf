# CMakeLists head

target_sources(vsf 
    INTERFACE driver.c
    INTERFACE startup_AT32F435.c
    INTERFACE interrupt_swi.c
)
