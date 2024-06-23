#ifndef __VSF_LINUX_XATTR_H__
#define __VSF_LINUX_XATTR_H__

#ifdef __cplusplus
extern "C" {
#endif

#define XATTR_CREATE                0x1
#define XATTR_REPLACE               0x2

#define XATTR_OS2_PREFIX            "os2."
#define XATTR_OS2_PREFIX_LEN        (sizeof(XATTR_OS2_PREFIX) - 1)

#define XATTR_MAC_OSX_PREFIX        "osx."
#define XATTR_MAC_OSX_PREFIX_LEN    (sizeof(XATTR_MAC_OSX_PREFIX) - 1)

#define XATTR_BTRFS_PREFIX          "btrfs."
#define XATTR_BTRFS_PREFIX_LEN      (sizeof(XATTR_BTRFS_PREFIX) - 1)

#define XATTR_HURD_PREFIX           "gnu."
#define XATTR_HURD_PREFIX_LEN       (sizeof(XATTR_HURD_PREFIX) - 1)

#define XATTR_SECURITY_PREFIX       "security."
#define XATTR_SECURITY_PREFIX_LEN   (sizeof(XATTR_SECURITY_PREFIX) - 1)

#define XATTR_SYSTEM_PREFIX         "system."
#define XATTR_SYSTEM_PREFIX_LEN     (sizeof(XATTR_SYSTEM_PREFIX) - 1)

#define XATTR_TRUSTED_PREFIX        "trusted."
#define XATTR_TRUSTED_PREFIX_LEN    (sizeof(XATTR_TRUSTED_PREFIX) - 1)

#define XATTR_USER_PREFIX           "user."
#define XATTR_USER_PREFIX_LEN       (sizeof(XATTR_USER_PREFIX) - 1)

#define XATTR_EVM_SUFFIX            "evm"
#define XATTR_NAME_EVM              XATTR_SECURITY_PREFIX XATTR_EVM_SUFFIX

#define XATTR_IMA_SUFFIX            "ima"
#define XATTR_NAME_IMA              XATTR_SECURITY_PREFIX XATTR_IMA_SUFFIX

#define XATTR_SELINUX_SUFFIX        "selinux"
#define XATTR_NAME_SELINUX          XATTR_SECURITY_PREFIX XATTR_SELINUX_SUFFIX

#define XATTR_APPARMOR_SUFFIX       "apparmor"
#define XATTR_NAME_APPARMOR         XATTR_SECURITY_PREFIX XATTR_APPARMOR_SUFFIX

#define XATTR_CAPS_SUFFIX           "capability"
#define XATTR_NAME_CAPS             XATTR_SECURITY_PREFIX XATTR_CAPS_SUFFIX

#ifdef __cplusplus
}
#endif

#endif
