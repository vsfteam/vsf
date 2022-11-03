#include <err.h>

#define MAX_ERRNO               4095

#define IS_ERR_VALUE(__X)       ((unsigned long)(__X) >= (unsigned long)-MAX_ERRNO)

static inline void * ERR_PTR(long error)
{
    return (void *)error;
}

static inline long PTR_ERR(const void *ptr)
{
    return (long) ptr;
}

static inline bool IS_ERR(const void *ptr)
{
    return IS_ERR_VALUE(ptr);
}

static inline bool IS_ERR_OR_NULL(const void *ptr)
{
	return !ptr || IS_ERR_VALUE(ptr);
}

static inline void * ERR_CAST(const void *ptr)
{
    return (void *)ptr;
}

static inline int PTR_ERR_OR_ZERO(const void *ptr)
{
    if (IS_ERR(ptr)) {
        return PTR_ERR(ptr);
    } else {
        return 0;
    }
}
