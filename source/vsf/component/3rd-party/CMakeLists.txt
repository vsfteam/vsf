# CMakeLists head

add_subdirectory_ifdef(CONFIG_AWTK          awtk)
add_subdirectory_ifdef(CONFIG_BTSTACK       btstack)
add_subdirectory_ifdef(CONFIG_FREETYPE      freetype)
add_subdirectory_ifdef(CONFIG_LITTLEVGL     littlevgl)
add_subdirectory_ifdef(CONFIG_LWIP          lwip)
add_subdirectory_ifdef(CONFIG_NNOM          nnom)
add_subdirectory_ifdef(CONFIG_VSFIP         vsfip)
add_subdirectory_ifdef(CONFIG_VSFVM         vsfvm)