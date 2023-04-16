#ifndef __VSF_LINUX_LOOP_H__
#define __VSF_LINUX_LOOP_H__

#include <linux/types.h>

#define LO_NAME_SIZE        64
#define LO_KEY_SIZE         32

#define LOOP_SET_FD         0x4C00
#define LOOP_CLR_FD         0x4C01
#define LOOP_SET_STATUS     0x4C02
#define LOOP_GET_STATUS     0x4C03
#define LOOP_SET_STATUS64   0x4C04
#define LOOP_GET_STATUS64   0x4C05
#define LOOP_CHANGE_FD      0x4C06
#define LOOP_SET_CAPACITY   0x4C07
#define LOOP_SET_DIRECT_IO  0x4C08
#define LOOP_SET_BLOCK_SIZE 0x4C09
#define LOOP_CONFIGURE      0x4C0A

struct loop_info64 {
    __u64                   lo_device;
    __u64                   lo_inode;
    __u64                   lo_rdevice;
    __u64                   lo_offset;
    __u64                   lo_sizelimit;
    __u64                   lo_number;
    __u64                   lo_flags;
    __u8                    lo_file_name[LO_NAME_SIZE];
	__u8                    lo_crypt_name[LO_NAME_SIZE];
    __u64                   lo_init[2];
};

#endif      // __VSF_LINUX_LOOP_H__
