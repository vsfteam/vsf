#include <unistd.h>
#include <thread>
#include <cmath>

#ifndef __VSF_CPP__
#   error Please define __VSF_CPP__ to support cpp
#endif

namespace std {
    extern "C" void * __thread_routine(void *p) {
        thread::__impl_base * t = static_cast<thread::__impl_base *>(p);
        thread::__shared_base_type local;
        local.swap(t->__this_ptr);
        // TODO: how to detect if exception is enabled?
//        try {
            t->__run();
//        } catch (...) {
            std::terminate();
//        }
        return (void *)0;
    }

    void thread::__start_thread(__shared_base_type s) {
        s->__this_ptr = s;
        if (pthread_create(&__id.__thread_id, NULL, __thread_routine, s.get()) != 0) {
            s->__this_ptr.reset();
            std::terminate();
        }
    }
}

#ifdef __WIN__
#   include <fstream>

#   if defined(_DEBUG)
#       warning ********windows: In debug mode, some cpp class will overwrite new/delete,\
        which may cause problems if new allocate memory from windows heap and delete call free. ********
#   endif

    // copy from https://github.com/microsoft/STL/blob/main/stl/src/fiopen.cpp
    // Copyright (c) Microsoft Corporation.
    // SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
namespace std {
    FILE* _Xfsopen(_In_z_ const char *filename, _In_ int mode, _In_ int prot) {
        static const char* const mods[] = {// fopen mode strings corresponding to valid[i]
            "r", "w", "w", "a", "rb", "wb", "wb", "ab", "r+", "w+", "a+", "r+b", "w+b", "a+b", nullptr};

        return fopen(filename, mods[mode]);
    }
    FILE* _Xfsopen(_In_z_ const wchar_t *filename, _In_ int mode, _In_ int prot) {
        size_t len = wcstombs(nullptr, filename, 0);
        if (len == (size_t)-1) {
            return NULL;
        }
        char mbfilename[len + 1];
        wcstombs(mbfilename, filename, len + 1);
        mbfilename[len] = '\0';
        return _Xfsopen(mbfilename, mode, prot);
    }

    template <class CharT>
    FILE* _Xfiopen(const CharT* filename, ios_base::openmode mode, int prot) {
        static const ios_base::openmode valid[] = {
            // valid combinations of open flags
            ios_base::in,
            ios_base::out,
            ios_base::out | ios_base::trunc,
            ios_base::out | ios_base::app,
            ios_base::in | ios_base::binary,
            ios_base::out | ios_base::binary,
            ios_base::out | ios_base::trunc | ios_base::binary,
            ios_base::out | ios_base::app | ios_base::binary,
            ios_base::in | ios_base::out,
            ios_base::in | ios_base::out | ios_base::trunc,
            ios_base::in | ios_base::out | ios_base::app,
            ios_base::in | ios_base::out | ios_base::binary,
            ios_base::in | ios_base::out | ios_base::trunc | ios_base::binary,
            ios_base::in | ios_base::out | ios_base::app | ios_base::binary,
        };

        FILE* fp                     = nullptr;
        ios_base::openmode atendflag = mode & ios_base::ate;
        ios_base::openmode norepflag = mode & ios_base::_Noreplace;

        if (mode & ios_base::_Nocreate) {
            mode |= ios_base::in; // file must exist
        }

        if (mode & ios_base::app) {
            mode |= ios_base::out; // extension -- app implies out
        }

        mode &= ~(ios_base::ate | ios_base::_Nocreate | ios_base::_Noreplace);

        // look for a valid mode
        int n = 0;
        while (valid[n] != mode) {
            if (++n == static_cast<int>(_STD size(valid))) {
                return nullptr; // no valid mode
            }
        }

        if (norepflag && (mode & (ios_base::out | ios_base::app))
            && (fp = _Xfsopen(filename, 0, prot)) != nullptr) { // file must not exist, close and fail
            fclose(fp);
            return nullptr;
        }

        if (fp != nullptr && fclose(fp) != 0) {
            return nullptr; // can't close after test open
        }

        if ((fp = _Xfsopen(filename, n, prot)) == nullptr) {
            return nullptr; // open failed
        }

        if (atendflag && fseek(fp, 0, SEEK_END) != 0) {
            fclose(fp); // can't position at end
            return nullptr;
        }

        return fp; // no need to seek to end, or seek succeeded
    }

    _CRTIMP2_PURE FILE* __CLRCALL_PURE_OR_CDECL _Fiopen(
        const char *filename, ios_base::openmode mode, int prot) { // open wide-named file with byte name
        return _Xfiopen(filename, mode, prot);
    }
    _CRTIMP2_PURE FILE* __CLRCALL_PURE_OR_CDECL _Fiopen(
        const wchar_t *filename, ios_base::openmode mode, int prot) {
        return _Xfiopen(filename, mode, prot);
    }
}
#endif

// math
namespace std {
    float hypot(float x, float y, float z)
    {
        return sqrtf(x * x + y * y + z * z);
    }
    double hypot(double x, double y, double z)
    {
        return sqrt(x * x + y * y + z * z);
    }
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