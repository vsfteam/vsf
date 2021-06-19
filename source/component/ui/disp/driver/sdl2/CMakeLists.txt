# CMakeLists head

if(VSF_USE_DISP AND VSF_DISP_USE_SDL2)
    target_sources(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
        vsf_disp_sdl2.c
    )

    # check libsdl2
    find_package(SDL2 REQUIRED)
    target_include_directories(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
        ${SDL2_INCLUDE_DIRS}
    )
    target_link_libraries(${VSF_LIB_NAME} INTERFACE ${SDL2_LIBRARIES})
endif()
