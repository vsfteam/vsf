#include <unistd.h>
#include <thread>

#ifdef __WIN__
#   include <fstream>
#endif

#ifndef __OOC_CPP__
#   error Please define __OOC_CPP__ to support cpp
#endif

namespace std {
    extern "C" void * __thread_routine(void *p) {
        thread::__impl_base * t = static_cast<thread::__impl_base *>(p);
        thread::__shared_base_type local;
        local.swap(t->__this_ptr);
        try {
            t->__run();
        } catch (...) {
            std::terminate();
        }
        return (void *)0;
    }

    void thread::__start_thread(__shared_base_type s) {
        s->__this_ptr = s;
        if (pthread_create(&__id.__thread_id, NULL, __thread_routine, s.get()) != 0) {
            s->__this_ptr.reset();
            std::terminate();
        }
    }

#ifdef __WIN__
    FILE * _Fiopen(const char *filename, ios_base::openmode mode, int prot) {
        return fopen(filename, "");
    }
#endif
}

void *operator new(size_t size)
{
    return malloc(size);
}
void *operator new[](size_t size)
{
    return malloc(size);
}
void operator delete(void *ptr)
{
    free(ptr);
}
void operator delete[](void *ptr)
{
    free(ptr);
}