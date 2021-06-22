# CMakeLists head

if(VSF_USE_SDL2)
    target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
        include
        include/SDL2
    )
    target_sources(${VSF_LIB_NAME} INTERFACE
        vsf_sdl2.c
        vsf_sdl2_event.c
        vsf_sdl2_pixelformat.c
    )

    add_subdirectory(3rd-party)
endif()
