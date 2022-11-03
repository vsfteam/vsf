#ifndef __VSF_LINUX_IDR_H__
#define __VSF_LINUX_IDR_H__

#ifdef __cplusplus
extern "C" {
#endif

struct ida {
    int dummy;
};
#define IDA_INIT(__NAME)                        { 0 }
#define DEFINE_IDA(__NAME)                      struct ida __NAME = IDA_INIT(__NAME)

#ifdef __cplusplus
}
#endif

#endif
