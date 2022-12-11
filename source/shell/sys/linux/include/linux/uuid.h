#ifndef __VSF_LINUX_UUID_H__
#define __VSF_LINUX_UUID_H__

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UUID_SIZE                   16
typedef struct {
    __u8 b[UUID_SIZE];
} uuid_t;
#define UUID_INIT(a, b, c, d0, d1, d2, d3, d4, d5, d6, d7)                      \
((uuid_t){{                                                                     \
    ((a) >> 24) & 0xff, ((a) >> 16) & 0xff, ((a) >> 8) & 0xff, (a) & 0xff,      \
    ((b) >> 8) & 0xff, (b) & 0xff,                                              \
    ((c) >> 8) & 0xff, (c) & 0xff,                                              \
    (d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7)                              \
}})

#define UUID_STRING_LEN             36

typedef struct {
    __u8 b[16];
} guid_t;

#define GUID_INIT(a, b, c, d0, d1, d2, d3, d4, d5, d6, d7)                      \
((guid_t){{                                                                     \
    (a) & 0xff, ((a) >> 8) & 0xff, ((a) >> 16) & 0xff, ((a) >> 24) & 0xff,      \
    (b) & 0xff, ((b) >> 8) & 0xff,                                              \
    (c) & 0xff, ((c) >> 8) & 0xff,                                              \
    (d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7),                             \
}})

extern const guid_t guid_null;
extern const uuid_t uuid_null;

static inline bool guid_equal(const guid_t *u1, const guid_t *u2)
{
    return memcmp(u1, u2, sizeof(guid_t)) == 0;
}

static inline void guid_copy(guid_t *dst, const guid_t *src)
{
    memcpy(dst, src, sizeof(guid_t));
}

static inline void import_guid(guid_t *dst, const __u8 *src)
{
    memcpy(dst, src, sizeof(guid_t));
}

static inline void export_guid(__u8 *dst, const guid_t *src)
{
    memcpy(dst, src, sizeof(guid_t));
}

static inline bool guid_is_null(const guid_t *guid)
{
    return guid_equal(guid, &guid_null);
}

static inline bool uuid_equal(const uuid_t *u1, const uuid_t *u2)
{
    return memcmp(u1, u2, sizeof(uuid_t)) == 0;
}

static inline void uuid_copy(uuid_t *dst, const uuid_t *src)
{
    memcpy(dst, src, sizeof(uuid_t));
}

static inline void import_uuid(uuid_t *dst, const __u8 *src)
{
    memcpy(dst, src, sizeof(uuid_t));
}

static inline void export_uuid(__u8 *dst, const uuid_t *src)
{
    memcpy(dst, src, sizeof(uuid_t));
}

static inline bool uuid_is_null(const uuid_t *uuid)
{
    return uuid_equal(uuid, &uuid_null);
}

#ifdef __cplusplus
}
#endif

#endif
