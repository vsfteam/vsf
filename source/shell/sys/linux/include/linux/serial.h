#ifndef __VSF_LINUX_SERIAL_H__
#define __VSF_LINUX_SERIAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ASYNC_SPD_CUST              (1U << 0)
#define ASYNC_SPD_MASK              (ASYNC_SPD_CUST)

struct serial_struct {
    int flags;
    int custom_divisor;
    int baud_base;
};

#ifdef __cplusplus
}
#endif

#endif
