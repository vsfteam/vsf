#ifndef __VSF_LINUX_VT_H__
#define __VSF_LINUX_VT_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VT_OPENQRY              (('V' << 8) | 0)
#define VT_GETMODE              (('V' << 8) | 1)
#define VT_SETMODE              (('V' << 8) | 2)
#define VT_GETSTATE             (('V' << 8) | 3)
#define VT_SENDSIG              (('V' << 8) | 4)
#define VT_RELDISP              (('V' << 8) | 5)
#define VT_ACTIVATE             (('V' << 8) | 6)
#define VT_WAITACTIVE           (('V' << 8) | 7)
#define VT_DISALLOCATE          (('V' << 8) | 8)
#define VT_RESIZE               (('V' << 8) | 9)
#define VT_RESIZEX              (('V' << 8) | 10)
#define VT_LOCKSWITCH           (('V' << 8) | 11)
#define VT_UNLOCKSWITCH         (('V' << 8) | 12)
#define VT_GETHIFONTMASK        (('V' << 8) | 13)
#define VT_WAITEVENT            (('V' << 8) | 14)
#define VT_SETACTIVATE          (('V' << 8) | 15)

struct vt_mode {
    char mode;
    char waitv;
    short relsig;
    short acqsig;
    short frsig;
};

struct vt_stat {
    unsigned short v_active;
    unsigned short v_signal;
    unsigned short v_state;
};

struct vt_sizes {
    unsigned short v_rows;
    unsigned short v_cols;
    unsigned short v_scrollsize;
};

#ifdef __cplusplus
}
#endif

#endif
