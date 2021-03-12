// include configuration only before include any other headers
// so maybe compilation will be OK even without __OOC_CPP__
#include "vsf_cfg.h"

#if APP_USE_CPP_DEMO == ENABLED

#ifndef __OOC_CPP__
#   error __OOC_CPP__ MUST be defined, so that LPOOC is compatible with c++
#endif

#include <unistd.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

#ifdef __WIN__
#   define STREAM_FILE          "./winfs_root/readme.txt"
#else
#   define STREAM_FILE          "/memfs/readme.txt"
#endif

extern "C" {
    int cpp_main(int argc, char *argv[]);
}

int cpp_main(int argc, char *argv[])
{
    std::string str1 = "1234";
    string *str2 = new string("5678");

    std::cout << str1 << str2 << std::endl;
    delete str2;

    ifstream fin(STREAM_FILE);
    if (!fin.is_open()) {
        std::cout << "Fail to open " << STREAM_FILE << "\r\n";
        return -1;
    }

    char *buffer = new char[256];
        fin.getline(buffer, 256);
        cout << buffer << endl;
    delete [] buffer;
    return 0;
}

#endif
