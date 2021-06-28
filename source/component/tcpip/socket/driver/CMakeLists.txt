# CMakeLists head

add_subdirectory_ifdef(VSF_USE_LWIP             lwip)
add_subdirectory_ifdef(VSF_TCPIP_USE_WINSOCK    win)
