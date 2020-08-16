/*****************************************************************************
 *   Copyright(C)2009-2019 by SimonQian                                      *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_USE_VSFIP == ENABLED

#include "kernel/vsf_kernel.h"
#include "utilities/vsf_utilities.h"

#include "../../vsfip.h"
#include "./vsfip_dnsc.h"

/*============================ MACROS ========================================*/

#ifndef VSFIP_CFG_DNSC_TIMEOUT
#define VSFIP_CFG_DNSC_TIMEOUT      1000
#endif

#define VSFIP_DNS_CLIENT_PORT       53

#define VSFIP_DNS_QTYPE_A           1           // A - IP
#define VSFIP_DNS_QTYPE_NS          2           // NS - NameServer
#define VSFIP_DNS_QTYPE_CNAME       5           // CNAME
#define VSFIP_DNS_QTYPE_SOA         6
#define VSFIP_DNS_QTYPE_WKS         11
#define VSFIP_DNS_QTYPE_PTR         12
#define VSFIP_DNS_QTYPE_HINFO       13
#define VSFIP_DNS_QTYPE_MX          15          // MX - Mail
#define VSFIP_DNS_QTYPE_AAAA        28          // IPV6

#define VSFIP_DNS_PKG_SIZE          512
#define VSFIP_DNS_AFNET             1

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

typedef struct vsfip_dns_head_t {
    uint16_t id;
    uint16_t flag;
    uint16_t ques;
    uint16_t answ;
    uint16_t auth;
    uint16_t addrrs;
} PACKED vsfip_dns_head_t;

typedef struct vsfip_dns_query_type_t {
    uint16_t type;
    uint16_t classtype;
} PACKED vsfip_dns_query_type_t;

typedef struct vsfip_dns_response_t {
    uint16_t type;
    uint16_t classtype;
    uint32_t ttl;
    uint16_t len;
} PACKED vsfip_dns_response_t;

typedef struct vsfip_dnsc_local_t {
    uint16_t id;
    vsfip_ipaddr_t server[2];
} vsfip_dnsc_local_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static NO_INIT vsfip_dnsc_local_t __vsfip_dnsc;

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static uint16_t __vsfip_dns_parse_name(uint8_t *orgin, uint_fast16_t size)
{
    uint_fast8_t n;
    uint8_t *query = orgin;

    do {
        n = *query++;

        /** @see RFC 1035 - 4.1.4. Message compression */
        if ((n & 0xc0) == 0xc0) {
            /* Compressed name */
            break;
        } else {
            /* Not compressed name */
            while (n > 0) {
                ++query;
                --n;
            }
        }
    } while (*query != 0);

    return query + 1 - orgin;
}

static vsfip_netbuf_t * __vsfip_dns_build_query(char *domain, uint_fast16_t id)
{
    vsfip_netbuf_t *buf;
    vsfip_dns_head_t *head;
    vsfip_dns_query_type_t *end;
    uint8_t *name;
    uint_fast16_t count;

    buf = VSFIP_UDPBUF_GET(VSFIP_DNS_PKG_SIZE);
    if (NULL == buf) {
        return NULL;
    }

    // fill header
    head = (vsfip_dns_head_t *)buf->app.buffer;
    head->id = id;
    head->flag = cpu_to_be16(0x0100);
    head->ques = cpu_to_be16(1);
    head->answ = cpu_to_be16(0);
    head->auth = cpu_to_be16(0);
    head->addrrs = cpu_to_be16(0);
    name = (uint8_t *)head + sizeof(vsfip_dns_head_t);
    // fill name array
    // empty next size
    count = 0;
    name++;

    while ((*domain != '\0') && (*domain != '/')) {
        if (*domain == '.') {
            // fill the size
            *(name - count - 1) = count;
            // empty next size
            name++;
            domain++;
            count = 0;
        } else {
            *name = *domain;
            name++;
            domain++;
            count++;
        }
    }

    *name = 0;
    // fill last size
    // fill the size
    *(name - count - 1) = count;
    name++;
    // finish
    end = (vsfip_dns_query_type_t *)name;
    end->type = cpu_to_be16(VSFIP_DNS_QTYPE_A);
    end->classtype = cpu_to_be16(VSFIP_DNS_AFNET);
    buf->app.size = name - (uint8_t *)head + sizeof(vsfip_dns_query_type_t);
    return buf;
}

static vsf_err_t __vsfip_dns_decode_ans(uint8_t *ans , uint_fast16_t size, uint_fast16_t id, vsfip_ipaddr_t *domain_ip)
{
    vsfip_dns_head_t *head = (vsfip_dns_head_t *)ans;
    vsfip_dns_response_t *atype;
    uint_fast16_t qc, ac, i;
    uint8_t *pdat;
    uint_fast16_t dsize;

    if ((size < sizeof(vsfip_dns_head_t)) || (head->id != id)) {
        return VSF_ERR_FAIL;
    }

    qc = be16_to_cpu(head->ques);
    ac = be16_to_cpu(head->answ);
    pdat = (uint8_t *)ans + sizeof(vsfip_dns_head_t);
    dsize = size - sizeof(vsfip_dns_head_t);

    for (i = 0; i < qc; i++) {
        uint_fast16_t nsize = __vsfip_dns_parse_name(pdat, dsize);
        if (dsize < (nsize + sizeof(vsfip_dns_query_type_t))) {
            return VSF_ERR_FAIL;
        }

        // skip name and type class
        pdat += nsize + sizeof(vsfip_dns_query_type_t);
        dsize -= nsize + sizeof(vsfip_dns_query_type_t);
    }

    for (i = 0 ; i < ac ; i++) {
        uint_fast16_t nsize = __vsfip_dns_parse_name(pdat, dsize);

        pdat += nsize;
        dsize -= nsize;
        if (dsize < sizeof(vsfip_dns_response_t)) {
            return VSF_ERR_FAIL;
        }

        atype = (vsfip_dns_response_t *)pdat;
        if (    (atype->classtype != cpu_to_be16(VSFIP_DNS_AFNET))
            ||  (atype->type != cpu_to_be16(VSFIP_DNS_QTYPE_A))) {
            uint_fast16_t nsize = sizeof(vsfip_dns_response_t) + cpu_to_be16(atype->len);

            if (dsize < nsize) {
                return VSF_ERR_FAIL;
            }

            pdat += nsize;
            dsize -= nsize;
            continue;
        }

        // no need ttl
        // get ip size
        domain_ip->size = be16_to_cpu(atype->len);
        if (dsize < (sizeof(vsfip_dns_response_t) + domain_ip->size)) {
            return VSF_ERR_FAIL;
        }

        pdat += sizeof(vsfip_dns_response_t);

        // copy addr
        if (domain_ip->size == 4) {
            domain_ip->addr32 = *(uint32_t *)pdat;
            return VSF_ERR_NONE;
        }
    }

    return VSF_ERR_FAIL;
}

vsf_err_t vsfip_dnsc_set_server(uint_fast8_t dns_idx, vsfip_ipaddr_t *dns_server)
{
    if (dns_idx < dimof(__vsfip_dnsc.server)) {
        __vsfip_dnsc.server[dns_idx] = *dns_server;
    } else {
        return VSF_ERR_NOT_AVAILABLE;
    }

    return VSF_ERR_NONE;
}

vsf_err_t vsfip_dnsc_init(void)
{
    memset(&__vsfip_dnsc, 0, sizeof(__vsfip_dnsc));
    return VSF_ERR_NONE;
}


__vsf_component_peda_private_entry(__vsfip_gethostbyname,
    char * domain;
    vsfip_ipaddr_t *domain_ip;
    // private
    vsfip_socket_t *so;
    vsfip_netbuf_t *inbuf;
    vsfip_netbuf_t *outbuf;
    vsfip_sock_addr_t sock_addr;
    uint16_t id;
    uint8_t i;
) {
    vsf_err_t err;

    vsf_peda_begin();

    switch (evt) {
    case VSF_EVT_INIT:
        vsf_local.outbuf = NULL;
        vsf_local.so = vsfip_socket(AF_INET, IPPROTO_UDP);
        if (vsf_local.so == NULL) {
            err = VSF_ERR_NOT_AVAILABLE;

        close:
            if (vsf_local.so != NULL) {
                vsfip_close(vsf_local.so);
            }
            if (vsf_local.outbuf != NULL) {
                vsfip_netbuf_deref(vsf_local.outbuf);
            }
            vsf_eda_return(err);
            break;
        }

        err = vsfip_listen(vsf_local.so, 0);
        if (err < 0) { goto close; }

        __vsfip_dnsc.id++;
        vsf_local.id = cpu_to_be16(__vsfip_dnsc.id);
        vsf_local.outbuf = __vsfip_dns_build_query(vsf_local.domain, vsf_local.id);
        vsf_local.so->rx_timeout_ms = VSFIP_CFG_DNSC_TIMEOUT + VSFIP_ARP_MAXDELAY;
        vsf_local.i = 0;

    try_next_server:
        if (    (vsf_local.i >= dimof(__vsfip_dnsc.server))
            ||  !__vsfip_dnsc.server[vsf_local.i].size) {
            err = VSF_ERR_FAIL;
            goto close;
        }

        vsf_local.sock_addr.port = VSFIP_DNS_CLIENT_PORT;
        vsf_local.sock_addr.addr = __vsfip_dnsc.server[vsf_local.i];

        vsfip_udp_send(vsf_local.so, &vsf_local.sock_addr, vsf_local.outbuf);
        vsf_local.outbuf = NULL;

        vsfip_udp_recv(vsf_local.so, &vsf_local.sock_addr, &vsf_local.inbuf);
        break;
    case VSF_EVT_RETURN:
        vsf_local.i++;

        err = vsf_eda_get_return_value();
        if (err < 0) { goto try_next_server; }

        err = __vsfip_dns_decode_ans(vsf_local.inbuf->app.buffer,
                        vsf_local.inbuf->app.size, vsf_local.id, vsf_local.domain_ip);
        vsfip_netbuf_deref(vsf_local.inbuf);
        goto close;
    }

    vsf_peda_end();
}

vsf_err_t vsfip_gethostbyname(char *domain, vsfip_ipaddr_t *domain_ip)
{
    vsf_err_t err;

    __vsf_component_call_peda(__vsfip_gethostbyname, err, NULL,
        .domain     = domain,
        .domain_ip  = domain_ip,
    );
    return err;
}

#endif        // VSF_USE_TCPIP
