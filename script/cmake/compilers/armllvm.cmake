# define LLVM_TOOLCHAIN_PATH and LLVM_TOOLCHAIN_EXE_SUFIX and include this file

if(VSF_APPLET)
    set(VSF_TARGET_DEFINITIONS
        "__PROGRAM_START=_start"
        ${VSF_TARGET_DEFINITIONS}
    )

    if(NOT APPLET_COMPILER_EMBPI AND NOT APPLET_COMPILER_GOTPI)
        set(APPLET_COMPILER_EMBPI 1)
    endif()
    if(APPLET_COMPILER_EMBPI)
        # embedded position independency
        set(CMAKE_C_FLAGS
            "-mthumb -fno-builtin-printf -fno-builtin-fprintf -fropi -frwpi"
            CACHE INTERNAL "C compiler common flags"
        )
        set(CMAKE_CXX_FLAGS
            "-mthumb -fno-builtin-printf -fno-builtin-fprintf -fropi -frwpi"
            CACHE INTERNAL "C++ compiler common flags"
        )
    elseif(APPLET_COMPILER_GOTPI)
        # GOT-base position independency
        set(CMAKE_C_FLAGS
            "-mthumb -fno-builtin-printf -fno-builtin-fprintf -fPIC -mno-pic-data-is-text-relative"
            CACHE INTERNAL "C compiler common flags"
        )
        set(CMAKE_CXX_FLAGS
            "-mthumb -fno-builtin-printf -fno-builtin-fprintf -fPIC -mno-pic-data-is-text-relative"
            CACHE INTERNAL "C++ compiler common flags"
        )
    else()
        message(FATAL_ERROR "Either APPLET_COMPILER_EMBPI or APPLET_COMPILER_GOTPI should be set to 1")
    endif()
endif()

set(CMAKE_SYSTEM_NAME           Generic)
set(CMAKE_SYSTEM_PROCESSOR      ${VSF_HAL_ARCH_SERIES})

set(CMAKE_C_COMPILER ${LLVM_TOOLCHAIN_PATH}/bin/clang${LLVM_TOOLCHAIN_EXE_SUFIX})
set(CMAKE_CXX_COMPILER ${LLVM_TOOLCHAIN_PATH}/bin/clang++${LLVM_TOOLCHAIN_EXE_SUFIX})

set(CMAKE_OBJCOPY ${LLVM_TOOLCHAIN_PATH}/bin/llvm-objcopy${LLVM_TOOLCHAIN_EXE_SUFIX})
set(CMAKE_OBJDUMP ${LLVM_TOOLCHAIN_PATH}/bin/llvm-objdump${LLVM_TOOLCHAIN_EXE_SUFIX})
set(CMAKE_SIZE ${LLVM_TOOLCHAIN_PATH}/bin/llvm-size${LLVM_TOOLCHAIN_EXE_SUFIX})

set(CMAKE_C_FLAGS
    "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -fms-extensions --target=armv7m-none-eabi"
    CACHE INTERNAL "C compiler common flags"
)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -fdata-sections -ffunction-sections -fms-extensions --target=armv7m-none-eabi"
    CACHE INTERNAL "C++ compiler common flags"
)

# Optimization Level
#   -O0       None
#   -Os       Optimize for size
#   -O1       Optimize
#   -O2       Optimize more
#   -O3       Optimize most
#   -Og       Optimize for debug
#   -Ofast    Optimize for speed
set(CMAKE_C_FLAGS_DEBUG "-Og -g3" CACHE STRING "c debug compiler flags")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -g3" CACHE STRING "cxx debug compiler flags")
set(CMAKE_ASM_FLAGS_DEBUG "-g" CACHE INTERNAL "asm debug compiler flags")

#set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections --specs=nosys.specs"
#  CACHE INTERNAL "exe link flags")
if(DEFINED LLVM_TOOLCHAIN_SYSROOT)
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} --sysroot ${LLVM_TOOLCHAIN_SYSROOT}"
      CACHE STRING "exe link flags")
endif()
set(CMAKE_EXE_LINKER_FLAGS
  "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -flto"
  CACHE STRING "exe link flags")

set(CMAKE_C_COMPILER_WORKS ON)
set(CMAKE_CXX_COMPILER_WORKS ON)