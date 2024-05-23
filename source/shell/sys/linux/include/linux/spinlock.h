#ifndef __VSF_LINUX_SPINLOCK_H__
#define __VSF_LINUX_SPINLOCK_H__

#include "hal/arch/vsf_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef vsf_spinlock_t

typedef vsf_spinlock_t                      spinlock_t;

#define SPIN_LOCK_INIT()                    (0)
#define spin_lock_init(lock)                vsf_spinlock_init(lock)
#define spin_lock(lock)                     vsf_spin_lock(lock)
#define spin_unlock(lock)                   vsf_spin_unlock(lock)
#define spin_trylock(lock)                  vsf_spin_trylock(lock)
#define spin_lock_irq(lock)                 do { vsf_disable_interrupt(); spin_lock(lock); } while(0)
#define spin_unlock_irq(lock)               do { spin_unlock(lock); vsf_enable_interrupt(); } while(0)
#define spin_lock_irqsave(lock, flags)      do { flags = vsf_protect_int(); spin_lock(lock); } while(0)
#define spin_unlock_irqrestore(lock, flags) do { spin_unlock(lock); vsf_unprotect_int(flags); } while(0)

#else

typedef int                                 spinlock_t;

#define SPIN_LOCK_INIT()                    (0)
#define spin_lock_init(lock)                do { *lock = 0; } while(0)
#define __spin_lock(lock)                   do { *lock = 1; } while (0)
#define __spin_unlock(lock)                 do { *lock = 0; } while (0)
#define spin_lock(lock)                     do { vsf_protect_t orig = vsf_protect_int(); __spin_lock(lock); vsf_unprotect_int(orig); } while (0)
#define spin_unlock(lock)                   do { vsf_protect_t orig = vsf_protect_int(); __spin_unlock(lock); vsf_unprotect_int(orig); } while (0)
#define spin_trylock(lock)                  (spin_lock(lock), 1)
#define spin_lock_irq(lock)                 do { vsf_disable_interrupt(); __spin_lock(lock); } while(0)
#define spin_unlock_irq(lock)               do { __spin_unlock(lock); vsf_enable_interrupt(); } while(0)
#define spin_lock_irqsave(lock, flags)      do { flags = vsf_protect_int(); __spin_lock(lock); } while(0)
#define spin_unlock_irqrestore(lock, flags) do { __spin_unlock(lock); vsf_unprotect_int(flags); } while(0)

#endif

#ifdef __cplusplus
}
#endif

#endif
