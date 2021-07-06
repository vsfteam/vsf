#include "vsf.h"

#if VSF_USE_EVM == ENABLED && VSF_EVM_USE_LUA == ENABLED

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

int luat_socket_tsend(const char* hostname, int port, void* buff, int len)
{
    int ret, i;
    struct hostent *host;
    int sock = -1, bytes_received;
    struct sockaddr_in server_addr;

    host = gethostbyname(hostname);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket error\n");
        goto __exit;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0) {
        printf("Connect fail!\n");
        goto __exit;
    }

    ret = send(sock, buff, len, 0);
    if (ret <= 0) {
        printf("send error,close the socket.\n");
        goto __exit;
    }

__exit:
    if (sock >= 0) {
        close(sock);
    }
    return 0;
}

int luat_socket_is_ready(void) {
    return 1;
}

uint32_t luat_socket_selfip(void) {
    // TODO: get local ip
    return 0;
}

#endif      // VSF_USE_EVM && VSF_EVM_USE_LUA
