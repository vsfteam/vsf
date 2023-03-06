if(NOT VSF_LIBTUV_PATH)
    set(VSF_LIBTUV_PATH ${VSF_SRC_PATH}/shell/sys/linux/lib/3rd-party/libtuv/raw)
endif()

if(NOT VSF_USE_LINUX)
    message(FATAL_ERROR "libtuv depends on linux, please enable VSF_USE_LINUX")
endif()

vsf_add_include_directories(
    port
    ${VSF_LIBTUV_PATH}/include
    ${VSF_LIBTUV_PATH}/src
)
vsf_add_compile_definitions(
    TUV_FEATURE_PIPE
)

vsf_add_sources(
    ${VSF_LIBTUV_PATH}/src/fs-poll.c
    ${VSF_LIBTUV_PATH}/src/inet.c
    ${VSF_LIBTUV_PATH}/src/threadpool.c
    ${VSF_LIBTUV_PATH}/src/tuv_debuglog.c
    ${VSF_LIBTUV_PATH}/src/uv-common.c
    ${VSF_LIBTUV_PATH}/src/version.c

    ${VSF_LIBTUV_PATH}/src/unix/async.c
    ${VSF_LIBTUV_PATH}/src/unix/core.c
    ${VSF_LIBTUV_PATH}/src/unix/fs.c
    ${VSF_LIBTUV_PATH}/src/unix/getaddrinfo.c
    ${VSF_LIBTUV_PATH}/src/unix/getnameinfo.c
    ${VSF_LIBTUV_PATH}/src/unix/linux-core.c
    ${VSF_LIBTUV_PATH}/src/unix/linux-syscalls.c
    ${VSF_LIBTUV_PATH}/src/unix/loop.c
    ${VSF_LIBTUV_PATH}/src/unix/loop-watcher.c
    ${VSF_LIBTUV_PATH}/src/unix/pipe.c
    ${VSF_LIBTUV_PATH}/src/unix/poll.c
    ${VSF_LIBTUV_PATH}/src/unix/process.c
    ${VSF_LIBTUV_PATH}/src/unix/pthread-fixes.c
    ${VSF_LIBTUV_PATH}/src/unix/signal.c
    ${VSF_LIBTUV_PATH}/src/unix/stream.c
    ${VSF_LIBTUV_PATH}/src/unix/tcp.c
    ${VSF_LIBTUV_PATH}/src/unix/thread.c
    ${VSF_LIBTUV_PATH}/src/unix/timer.c
    ${VSF_LIBTUV_PATH}/src/unix/udp.c
)
