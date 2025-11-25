#ifndef __VSF_LINUX_SCATTERLIST_H__
#define __VSF_LINUX_SCATTERLIST_H__

struct scatterlist {
    void *buf;
    unsigned int buflen;
};

static inline void sg_set_buf(struct scatterlist *sg, const void *buf, unsigned int buflen)
{
    sg->buf = (void *)buf;
    sg->buflen = buflen;
}

static inline void sg_init_one(struct scatterlist *sg, const void *buf, unsigned int buflen)
{
    sg_set_buf(sg, buf, buflen);
}

#endif
