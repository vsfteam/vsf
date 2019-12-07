// See LICENSE file for licence details

#ifndef N200_TIMER_H
#define N200_TIMER_H

#define TIMER_MSIP 0xFFC
#define TIMER_MSIP_size   0x4
#define TIMER_MTIMECMP 0x8
#define TIMER_MTIMECMP_size 0x8
#define TIMER_MTIME 0x0
#define TIMER_MTIME_size 0x8

#define TIMER_CTRL_ADDR           0xd1000000
#define TIMER_REG(offset)         _REG32(TIMER_CTRL_ADDR, offset)
//#define TIMER_FREQ	            ((uint32_t)SystemCoreClock/4)  //units HZ

#define __RESERVED_4B	uint32_t    : 32;
#define __RESERVED_8B	uint64_t    : 64;

#define __RESERVED_16B		__RESERVED_4B	\
							__RESERVED_4B	\
							__RESERVED_4B	\
							__RESERVED_4B

#define __RESERVED_32B		__RESERVED_16B	\
							__RESERVED_16B

#define __RESERVED_64B		__RESERVED_16B	\
							__RESERVED_16B	\
							__RESERVED_16B	\
							__RESERVED_16B

#define __RESERVED_128B		__RESERVED_64B	\
							__RESERVED_64B

#define __RESERVED_256B 	__RESERVED_64B	\
							__RESERVED_64B	\
							__RESERVED_64B	\
							__RESERVED_64B

#define __RESERVED_512B		__RESERVED_256B	\
							__RESERVED_256B

#define __RESERVED_1K		__RESERVED_256B	\
							__RESERVED_256B	\
							__RESERVED_256B \
							__RESERVED_256B

#define __RESERVED_2K		__RESERVED_1K	\
							__RESERVED_1K

typedef volatile struct {
    uint64_t COUNTER;
    uint64_t COMPARE;

    __RESERVED_2K
    __RESERVED_1K

    __RESERVED_512B
    __RESERVED_256B
    __RESERVED_128B
    __RESERVED_64B
    __RESERVED_32B

    __RESERVED_8B
    uint32_t CTRL;
    uint32_t PENDSV;
}io_systick_reg_t;

#define SYSTICK_CTRL_DISABLE_POS    0
#define SYSTICK_CTRL_DISABLE_MSK    (1<<SYSTICK_CTRL_DISABLE_POS)

#define SYSTICK_PENDSV_PENDING_POS  0
#define SYSTICK_PENDSV_PENDING_MSK  (1<<SYSTICK_PENDSV_PENDING_POS)

#define SYSTICK_BASE_ADDR           0xD1000000ul
#define SYSTICK                     (*((io_systick_reg_t *)SYSTICK_BASE_ADDR))

#endif

