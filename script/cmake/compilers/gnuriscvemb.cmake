if(NOT DEFINED GCC_PREFIX)
    set(GCC_PREFIX      riscv64-none-elf)
endif()

if(VSF_APPLET)
    set(CMAKE_C_FLAGS
        "-shared -nodefaultlibs -nolibc -nostdlib ${CMAKE_C_FLAGS}"
        CACHE INTERNAL "C compiler common flags"
    )
    set(CMAKE_CXX_FLAGS
        "-shared -nodefaultlibs -nolibc -nostdlib++ ${CMAKE_CXX_FLAGS}"
        CACHE INTERNAL "C++ compiler common flags"
    )
    set(CMAKE_C_FLAGS
        "-fms-extensions -nostartfiles -e _start -fPIC -mcmodel=medany -mno-relax -msmall-data-limit=0 -z max-page-size=4 ${CMAKE_C_FLAGS}"
        CACHE INTERNAL "C compiler common flags"
    )
    set(CMAKE_CXX_FLAGS
        "-fms-extensions -nostartfiles -e _start -fPIC -mcmodel=medany -mno-relax -msmall-data-limit=0 -z max-page-size=4 ${CMAKE_CXX_FLAGS}"
        CACHE INTERNAL "C++ compiler common flags"
    )
endif()

include(${CMAKE_CURRENT_LIST_DIR}/gcc_common.cmake)
