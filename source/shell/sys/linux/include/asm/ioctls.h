#ifndef __VSF_LINUX_ASM_IOCTLS_H__
#define __VSF_LINUX_ASM_IOCTLS_H__

#define FIOCLEX             _IO('f', 1)
#define FIONCLEX            _IO('f', 2)
#define FIOASYNC            _IOW('f', 125, int)
#define FIONBIO             _IOW('f', 126, int)
#define FIONREAD            _IOR('f', 127, int)
#define TIOCINQ             FIONREAD
#define FIOQSIZE            _IOR('f', 128, loff_t)

#define TIOCGPTN            _IOR('T',0x30, unsigned int)
#define TIOCSPTLCK          _IOW('T',0x31, int)
#define TIOCGDEV            _IOR('T',0x32, unsigned int)
#define TIOCSIG             _IOW('T',0x36, int)

#endif      // __VSF_LINUX_ASM_IOCTLS_H__
