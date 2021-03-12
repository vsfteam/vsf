# CMakeLists head

target_sources(vsf 
    INTERFACE entry.S
    INTERFACE handlers.c
    INTERFACE init.c
    INTERFACE start.S
    # INTERFACE your_printf.c
)