#ifndef __VSF_LINUX_SOCKIOS_H__
#define __VSF_LINUX_SOCKIOS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SIOCGIFNAME     0x8910      /* get iface name           */
#define SIOCGIFCONF     0x8912      /* get iface list           */
#define SIOCGIFFLAGS    0x8913      /* get flags                */
#define SIOCSIFFLAGS    0x8914      /* set flags                */
#define SIOCGIFADDR     0x8915      /* get PA address           */
#define SIOCSIFADDR     0x8916      /* set PA address           */
#define SIOCGIFDSTADDR	0x8917      /* get remote PA address    */
#define SIOCSIFDSTADDR	0x8918      /* set remote PA address    */
#define SIOCGIFBRDADDR	0x8919      /* get broadcast PA address */
#define SIOCSIFBRDADDR	0x891a      /* set broadcast PA address */
#define SIOCGIFNETMASK  0x891b      /* get network PA mask      */
#define SIOCSIFNETMASK  0x891c      /* set network PA mask      */
#define SIOCGIFMTU      0x8921      /* get MTU size             */
#define SIOCSIFMTU      0x8922      /* set MTU size             */
#define SIOCSIFNAME     0x8923      /* set interface name       */
#define SIOCSIFHWADDR   0x8924      /* set hardware address     */
#define SIOCGIFHWADDR   0x8927      /* Get hardware address	    */

#ifdef __cplusplus
}
#endif

#endif
