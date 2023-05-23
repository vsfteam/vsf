#ifndef __VSF_LINUX_HDREG_H__
#define __VSF_LINUX_HDREG_H__

struct hd_geometry {
    unsigned char heads;
    unsigned char sectors;
    unsigned short cylinders;
    unsigned long start;
};

#define HDIO_GETGEO         0x0301

#endif
