set(GCC_PREFIX      arm-none-eabi)
set(GCC_SPEC        nano)

if(VSF_APPLET)
    set(CMAKE_C_FLAGS
        "-msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative ${CMAKE_C_FLAGS}"
        CACHE INTERNAL "C compiler common flags"
    )
    set(CMAKE_CXX_FLAGS
        "-msingle-pic-base -mpic-register=r9 -mno-pic-data-is-text-relative ${CMAKE_CXX_FLAGS}"
        CACHE INTERNAL "C++ compiler common flags"
    )
    set(VSF_TARGET_DEFINITIONS
        "__PROGRAM_START=_start"
        ${VSF_TARGET_DEFINITIONS}
    )

    if(NOT APPLET_COMPILER_EMBPI AND NOT APPLET_COMPILER_GOTPI)
        set(APPLET_COMPILER_GOTPI 1)
    endif()
    if(APPLET_COMPILER_GOTPI)
        # arm-none-eabi-gcc will not pass compiler checker of cmake, so include before project
        # refer to arm-none-eabi-gcc BUG: https://answers.launchpad.net/gcc-arm-embedded/+question/675869
        # need patch after compiled
        set(CMAKE_C_FLAGS
            "-shared -nodefaultlibs -nolibc -nostdlib ${CMAKE_C_FLAGS}"
            CACHE INTERNAL "C compiler common flags"
        )
        set(CMAKE_CXX_FLAGS
            "-shared -nodefaultlibs -nolibc -nostdlib++ ${CMAKE_CXX_FLAGS}"
            CACHE INTERNAL "C++ compiler common flags"
        )
    elseif(APPLET_COMPILER_EMBPI)
        # for embedded position independency, vsf_linux_applet_lib will be included,
        #   so it's OK to use libs in compiler, because the API in these library are weak version,
        #   and will be over-written by the same strong APIs in VSF is required.
    endif()

    set(CMAKE_C_FLAGS
        "-fms-extensions -nostartfiles -e _start -fPIC -z max-page-size=4 ${CMAKE_C_FLAGS}"
        CACHE INTERNAL "C compiler common flags"
    )
    set(CMAKE_CXX_FLAGS
        "-fms-extensions -nostartfiles -e _start -fPIC -z max-page-size=4 ${CMAKE_CXX_FLAGS}"
        CACHE INTERNAL "C++ compiler common flags"
    )
endif()

include(${CMAKE_CURRENT_LIST_DIR}/gcc_common.cmake)
