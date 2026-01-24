#include "../atomic"

// Interlocked intrinsic mapping for _nf/_acq/_rel
#if defined(_M_CEE_PURE) || (defined(_M_IX86) && !defined(_M_HYBRID_X86_ARM64)) \
    || (defined(_M_X64) && !defined(_M_ARM64EC))
#define _INTRIN_RELAXED(x) x
#define _INTRIN_ACQUIRE(x) x
#define _INTRIN_RELEASE(x) x
#define _INTRIN_ACQ_REL(x) x
#ifdef _M_CEE_PURE
//#define _YIELD_PROCESSOR()
#else // ^^^ defined(_M_CEE_PURE) / !defined(_M_CEE_PURE) vvv
//#define _YIELD_PROCESSOR() _mm_pause()
#endif // ^^^ !defined(_M_CEE_PURE) ^^^

#elif defined(_M_ARM64) || defined(_M_ARM64EC) || defined(_M_HYBRID_X86_ARM64)
#define _INTRIN_RELAXED(x) VSF_MCONNECT(x, _nf)
#define _INTRIN_ACQUIRE(x) VSF_MCONNECT(x, _acq)
#define _INTRIN_RELEASE(x) VSF_MCONNECT(x, _rel)
// We don't have interlocked intrinsics for acquire-release ordering, even on
// ARM64, so fall back to sequentially consistent.
#define _INTRIN_ACQ_REL(x) x
//#define _YIELD_PROCESSOR() __yield()

#else // ^^^ ARM64/ARM64EC/HYBRID_X86_ARM64 / unsupported hardware vvv
#error Unsupported hardware
#endif // hardware

#define _MT_INCR(x) _INTRIN_RELAXED(_InterlockedIncrement)(reinterpret_cast<volatile long*>(&x))
#define _MT_DECR(x) _INTRIN_ACQ_REL(_InterlockedDecrement)(reinterpret_cast<volatile long*>(&x))

namespace std {
    using _Atomic_counter_t = unsigned long;
}
