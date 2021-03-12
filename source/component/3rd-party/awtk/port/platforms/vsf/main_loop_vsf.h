/**
 * File:   main_loop_vsf.h
 * Author: AWTK Develop Team
 * Brief:  a main loop for vsf
 *
 * Copyright (c) 2018 - 2019  Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * this program is distributed in the hope that it will be useful,
 * but without any warranty; without even the implied warranty of
 * merchantability or fitness for a particular purpose.  see the
 * license file for more details.
 *
 */

/**
 * history:
 * ================================================================
 * 2020-01-07 simonqian <simonqian@vsfos.com> created
 *
 */

#ifndef TK_MAIN_LOOP_VSF_H
#define TK_MAIN_LOOP_VSF_H

#include "component/ui/vsf_ui_cfg.h"

#if VSF_USE_UI == ENABLED && VSF_USE_AWTK == ENABLED

#include "base/idle.h"
#include "base/timer.h"
#include "base/main_loop.h"
#include "base/event_queue.h"
#include "base/font_manager.h"
#include "base/window_manager.h"

BEGIN_C_DECLS

struct _main_loop_vsf_t;
typedef struct _main_loop_vsf_t main_loop_vsf_t;

typedef ret_t (*main_loop_dispatch_input_t)(main_loop_vsf_t* loop);

struct _main_loop_vsf_t {
  main_loop_t base;
  event_queue_t* queue;

  wh_t w;
  wh_t h;
  bool_t pressed;
  bool_t key_pressed;
  xy_t last_x;
  xy_t last_y;
  uint8_t last_key;
  void* user1;
  void* user2;
  void* user3;
  void* user4;
  main_loop_dispatch_input_t dispatch_input;
};

main_loop_vsf_t* main_loop_vsf_init(int w, int h);

ret_t main_loop_vsf_reset(main_loop_vsf_t* loop);
ret_t main_loop_post_key_event(main_loop_t* l, bool_t pressed, uint8_t key);
ret_t main_loop_post_pointer_event(main_loop_t* l, bool_t pressed, xy_t x, xy_t y);

END_C_DECLS

#endif
#endif /*TK_MAIN_LOOP_VSF_H*/
