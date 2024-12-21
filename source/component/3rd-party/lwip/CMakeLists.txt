# if lwip from vendor, do not use liwp in 3rd-party
if(NOT VSF_LWIP_VENDOR)
    if(NOT VSF_LWIP_PATH)
        set(VSF_LWIP_PATH ${VSF_SRC_PATH}/component/3rd-party/lwip/raw)
    endif()

    vsf_add_include_directories(
        ${VSF_SRC_PATH}/component/3rd-party/lwip/port/vsf
        ${VSF_LWIP_PATH}/src/include
    )
    if(VSF_LWIP_CONFIG_PATH)
        vsf_add_include_directories(
            ${VSF_LWIP_CONFIG_PATH}
        )
    endif()

    vsf_add_sources(
        ./port/vsf/arch/sys_arch.c

        ${VSF_LWIP_PATH}/src/apps/mdns/mdns.c
        ${VSF_LWIP_PATH}/src/api/api_lib.c
        ${VSF_LWIP_PATH}/src/api/api_msg.c
        ${VSF_LWIP_PATH}/src/api/err.c
        ${VSF_LWIP_PATH}/src/api/netbuf.c
        ${VSF_LWIP_PATH}/src/api/netdb.c
        ${VSF_LWIP_PATH}/src/api/netifapi.c
#        ${VSF_LWIP_PATH}/src/api/sockets.c
        ${VSF_LWIP_PATH}/src/api/tcpip.c
        ${VSF_LWIP_PATH}/src/core/ipv4/autoip.c
        ${VSF_LWIP_PATH}/src/core/ipv4/dhcp.c
        ${VSF_LWIP_PATH}/src/core/ipv4/etharp.c
        ${VSF_LWIP_PATH}/src/core/ipv4/icmp.c
        ${VSF_LWIP_PATH}/src/core/ipv4/igmp.c
        ${VSF_LWIP_PATH}/src/core/ipv4/ip4.c
        ${VSF_LWIP_PATH}/src/core/ipv4/ip4_addr.c
        ${VSF_LWIP_PATH}/src/core/ipv4/ip4_frag.c
        ${VSF_LWIP_PATH}/src/core/ipv6/dhcp6.c
        ${VSF_LWIP_PATH}/src/core/ipv6/ethip6.c
        ${VSF_LWIP_PATH}/src/core/ipv6/icmp6.c
        ${VSF_LWIP_PATH}/src/core/ipv6/inet6.c
        ${VSF_LWIP_PATH}/src/core/ipv6/ip6.c
        ${VSF_LWIP_PATH}/src/core/ipv6/ip6_addr.c
        ${VSF_LWIP_PATH}/src/core/ipv6/ip6_frag.c
        ${VSF_LWIP_PATH}/src/core/ipv6/mld6.c
        ${VSF_LWIP_PATH}/src/core/ipv6/nd6.c
        ${VSF_LWIP_PATH}/src/core/def.c
        ${VSF_LWIP_PATH}/src/core/dns.c
        ${VSF_LWIP_PATH}/src/core/inet_chksum.c
        ${VSF_LWIP_PATH}/src/core/init.c
        ${VSF_LWIP_PATH}/src/core/ip.c
        ${VSF_LWIP_PATH}/src/core/mem.c
        ${VSF_LWIP_PATH}/src/core/memp.c
        ${VSF_LWIP_PATH}/src/core/netif.c
        ${VSF_LWIP_PATH}/src/core/pbuf.c
        ${VSF_LWIP_PATH}/src/core/raw.c
        ${VSF_LWIP_PATH}/src/core/stats.c
        ${VSF_LWIP_PATH}/src/core/sys.c
        ${VSF_LWIP_PATH}/src/core/tcp.c
        ${VSF_LWIP_PATH}/src/core/tcp_in.c
        ${VSF_LWIP_PATH}/src/core/tcp_out.c
        ${VSF_LWIP_PATH}/src/core/timeouts.c
        ${VSF_LWIP_PATH}/src/core/udp.c
        ${VSF_LWIP_PATH}/src/netif/ethernet.c
    )
endif()

if(VSF_USE_LINUX)
    vsf_add_compile_definitions(
        # enable LWIP_NETIF_EXT_STATUS_CALLBACK for netlink support in linux
        LWIP_NETIF_EXT_STATUS_CALLBACK=1
    )
endif()
vsf_add_sources(
    ./port/lwip_netdrv_adapter.c
)
