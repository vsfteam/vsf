#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_AWTK == ENABLED

#include "tkc/fs.h"

fs_t* os_fs(void) {
  return NULL;
}

#endif
