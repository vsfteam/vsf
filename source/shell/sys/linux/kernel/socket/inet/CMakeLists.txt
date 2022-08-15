# CMakeLists head

if(VSF_HOST_SYSTEM STREQUAL win OR VSF_HOST_SYSTEM STREQUAL linux OR VSF_HOST_SYSTEM STREQUAL macos)
    target_sources(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
        vsf_linux_socket_inet_hostsock.c
    )
elseif(VSF_USE_LWIP)
    vsf_add_sources(
        vsf_linux_socket_inet_lwip.c
    )
    vsf_add_compile_definitions(
        "LWIP_SO_RCVBUF=1"
#       enable LWIP_SOCKET so that struct netconn has socket member
#       but note that DO NOT use lwip/socket.h in lwip
        "LWIP_SOCKET=1"
    )
endif()
