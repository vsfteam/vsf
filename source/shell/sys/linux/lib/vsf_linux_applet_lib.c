#define VSF_APPLET_VPLT_FUNC_DECORATOR(__NAME)      WEAK(__NAME)
#define __VSF_APPLET_LIB__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dlfcn.h>
#include <math.h>
#include <pthread.h>

#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>
#include <getopt.h>
#include <libgen.h>
#include <poll.h>
#include <pwd.h>
#include <semaphore.h>
#include <signal.h>
#include <syslog.h>
#include <mntent.h>
#include <grp.h>
#include <netdb.h>
#include <glob.h>
#include <errno.h>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <sys/random.h>
#include <sys/reboot.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/sendfile.h>
#include <sys/shm.h>
#include <sys/signalfd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <sys/sysmacros.h>
#include <sys/statfs.h>
#include <sys/timex.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>

#include <net/if.h>

#include <arpa/inet.h>

#include <fnmatch.h>
#include <glob.h>
#include <regex.h>

#if VSF_USE_MBEDTLS == ENABLED
#   include "component/3rd-party/mbedtls/extension/vplt/mbedtls_applet_lib.inc"
#endif
