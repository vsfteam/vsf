#include <cstdio>
#include <ios>

#ifdef __WIN__
namespace std {
    // just for compile test, not functioning
    FILE * _Fiopen(char const *filename, ios_base::openmode mode, int prot)
    {
        return fopen(filename, 0);
    }
};
#endif
