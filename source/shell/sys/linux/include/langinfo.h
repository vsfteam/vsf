#ifndef __SIMPLE_LIBC_LANGINFO_H__
#define __SIMPLE_LIBC_LANGINFO_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CODESET = 0,
} nl_item;

char * nl_langinfo(nl_item item);
//char * nl_langinfo_l(nl_item item, locale_t locale);

#ifdef __cplusplus
}
#endif

#endif