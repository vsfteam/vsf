# CMakeLists head

if(VSF_USE_LWIP)
    target_sources(${VSF_LIB_NAME} INTERFACE
        vsf_linux_socket_inet_lwip.c
    )
    target_compile_definitions(${VSF_LIB_NAME} INTERFACE
        "LWIP_SO_RCVBUF=1"
#       enable LWIP_SOCKET so that struct netconn has socket member
#       but note that DO NOT use lwip/socket.h in lwip
        "LWIP_SOCKET=1"
    )
elseif(VSF_HOST_SYSTEM STREQUAL win OR VSF_HOST_SYSTEM STREQUAL linux OR VSF_HOST_SYSTEM STREQUAL macos)
    target_sources(${VSF_HOST_SYSTEM_LIB_NAME} PUBLIC
        vsf_linux_socket_inet_hostsock.c
    )
endif()
