#ifndef OPENSSL_MACROS_H
#define OPENSSL_MACROS_H

#ifndef OPENSSL_FILE
#   ifdef OPENSSL_NO_FILENAMES
#       define OPENSSL_FILE         ""
#       define OPENSSL_LINE         0
#   else
#       define OPENSSL_FILE         __FILE__
#       define OPENSSL_LINE         __LINE__
#   endif
#endif

#ifndef OPENSSL_FUNC
#   if defined(__STDC_VERSION__)
#       if __STDC_VERSION__ >= 199901L
#           define OPENSSL_FUNC     __func__
#       elif defined(__GNUC__) && __GNUC__ >= 2
#           define OPENSSL_FUNC     __FUNCTION__
#       endif
#   elif defined(_MSC_VER)
#       define OPENSSL_FUNC         __FUNCTION__
#   endif

#   ifndef OPENSSL_FUNC
#       define OPENSSL_FUNC         "(unknown function)"
#   endif
#endif

#endif      // OPENSSL_MACROS_H
