#ifndef __VSF_LINUX_NET_IF_H__
#define __VSF_LINUX_NET_IF_H__

unsigned int if_nametoindex(const char *ifname);
char *if_indextoname(unsigned int ifindex, char *ifname);

#endif      // __VSF_LINUX_NET_IF_H__
