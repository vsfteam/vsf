#ifndef __VSF_LINUX_FD_H__
#define __VSF_LINUX_FD_H__

#include "shell/sys/linux/vsf_linux_cfg.h"

#if VSF_LINUX_CFG_RELATIVE_PATH == ENABLED
#   include "../sys/ioctl.h"
#else
#   include <sys/ioctl.h>
#endif

struct floppy_struct {
    unsigned int            size;
    unsigned int            sect;
    unsigned int            head;
    unsigned int            track;
    unsigned int            stretch;
#define FD_STRETCH          1
#define FD_SWAPSIDES        2
#define FD_ZEROBASED        4
#define FD_SECTBASEMASK     0x3FC
#define FD_MKSECTBASE(s)    (((s) ^ 1) << 2)
#define FD_SECTBASE(floppy) ((((floppy)->stretch & FD_SECTBASEMASK) >> 2) ^ 1)

    unsigned char           gap;

    unsigned char           rate;
#define FD_2M 0x4
#define FD_SIZECODEMASK 0x38
#define FD_SIZECODE(floppy) (((((floppy)->rate&FD_SIZECODEMASK)>> 3)+ 2) %8)
#define FD_SECTSIZE(floppy) ( (floppy)->rate & FD_2M ? \
                 512 : 128 << FD_SIZECODE(floppy) )
#define FD_PERP 0x40

    unsigned char           spec1;
    unsigned char           fmt_gap;
    const char *            name;
};

#define FDGETPRM            _IOR(2, 0x04, struct floppy_struct)

#endif
