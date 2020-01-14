/**
 * File:   main_loop_vsf.c
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

#include "vsf.h"

#if VSF_USE_UI == ENABLED && VSF_USE_UI_AWTK == ENABLED

#include "tkc/time_now.h"
#include "native_window/native_window_raw.h"

#include "./main_loop_vsf.h"

#ifndef VSF_AWTK_CFG_PROTECT_LEVEL
/*! \note   By default, the event queue operation will be interrupt-safe,
 *!
 *!         in the case when you want to disable it,
 *!         please use following macro:
 *!         #define VSF_AWTK_CFG_PROTECT_LEVEL  none
 *!         
 *!         in the case when you want to use scheduler-safe,
 *!         please use following macro:
 *!         #define VSF_AWTK_CFG_PROTECT_LEVEL  scheduler
 *!         
 *!         NOTE: This macro should be defined in vsf_usr_cfg.h
 */
#   define VSF_AWTK_CFG_PROTECT_LEVEL       interrupt
#endif

#define vsf_awtk_protect                    vsf_protect(VSF_AWTK_CFG_PROTECT_LEVEL)
#define vsf_awtk_unprotect                  vsf_unprotect(VSF_AWTK_CFG_PROTECT_LEVEL)

static ret_t main_loop_vsf_queue_event(main_loop_t* l, const event_queue_req_t* r) {
  ret_t ret = RET_FAIL;
  main_loop_vsf_t* loop = (main_loop_vsf_t*)l;

  vsf_protect_t orig = vsf_awtk_protect();
    ret = event_queue_send(loop->queue, r);
  vsf_awtk_unprotect(orig);

  return ret;
}

static ret_t main_loop_vsf_recv_event(main_loop_vsf_t* loop, event_queue_req_t* r) {
  ret_t ret = RET_FAIL;

  vsf_protect_t orig = vsf_awtk_protect();
    ret = event_queue_recv(loop->queue, r);
  vsf_awtk_unprotect(orig);

  return ret;
}

ret_t main_loop_post_pointer_event(main_loop_t* l, bool_t pressed, xy_t x, xy_t y) {
  event_queue_req_t r;
  pointer_event_t event;
  main_loop_vsf_t* loop = (main_loop_vsf_t*)l;

  return_value_if_fail(loop != NULL, RET_BAD_PARAMS);

  event.x = x;
  event.y = y;
  event.button = 0;
  event.e.time = time_now_ms();

  if (pressed) {
    loop->last_x = x;
    loop->last_y = y;

    if (loop->pressed) {
      event.e.type = EVT_POINTER_MOVE;
    } else {
      event.e.type = EVT_POINTER_DOWN;
    }
    loop->pressed = TRUE;
    event.pressed = loop->pressed;

    r.pointer_event = event;

    return main_loop_queue_event(l, &r);
  } else {
    if (loop->pressed) {
      event.e.type = EVT_POINTER_UP;
      event.pressed = loop->pressed;
      event.x = loop->last_x;
      event.y = loop->last_y;
      r.pointer_event = event;

      loop->pressed = FALSE;

      return main_loop_queue_event(l, &r);
    }
  }

  return RET_OK;
}

ret_t main_loop_post_key_event(main_loop_t* l, bool_t pressed, uint8_t key) {
  event_queue_req_t r;
  key_event_t event;
  main_loop_vsf_t* loop = (main_loop_vsf_t*)l;
  return_value_if_fail(loop != NULL, RET_BAD_PARAMS);

  loop->last_key = key;
  event.key = key;

  if (pressed) {
    loop->key_pressed = TRUE;
    event.e.type = EVT_KEY_DOWN;

    r.key_event = event;
    return main_loop_queue_event(l, &r);
  } else {
    if (loop->key_pressed) {
      loop->key_pressed = FALSE;
      event.e.type = EVT_KEY_UP;

      r.key_event = event;
      return main_loop_queue_event(l, &r);
    }
  }

  return RET_OK;
}

static ret_t main_loop_dispatch_events(main_loop_vsf_t* loop) {
  event_queue_req_t r;
  widget_t* widget = loop->base.wm;
  int time_in = time_now_ms();
  int time_out = time_in;

  while (main_loop_vsf_recv_event(loop, &r) == RET_OK && time_out - time_in < 20) {
    switch (r.event.type) {
      case EVT_POINTER_DOWN:
      case EVT_POINTER_MOVE:
      case EVT_POINTER_UP:
        window_manager_dispatch_input_event(widget, (event_t*)&(r.pointer_event));
        break;
      case EVT_KEY_DOWN:
      case EVT_KEY_UP:
        window_manager_dispatch_input_event(widget, (event_t*)&(r.key_event));
        break;
      case REQ_ADD_IDLE:
        idle_add(r.add_idle.func, r.add_idle.e.target);
        break;
      case REQ_ADD_TIMER:
        timer_add(r.add_timer.func, r.add_timer.e.target, r.add_timer.duration);
        break;
      default: {
        if (r.event.target != NULL) {
          widget = WIDGET(r.event.target);
        }
        widget_dispatch(widget, &(r.event));
        break;
      }
    }
    time_out = time_now_ms();
    /*HANDLE OTHER EVENT*/
  }

  return RET_OK;
}

static ret_t main_loop_dispatch_input(main_loop_vsf_t* loop) {
  if (loop->dispatch_input) {
    loop->dispatch_input(loop);
  }

  return RET_OK;
}

static ret_t main_loop_vsf_step(main_loop_t* l) {
  main_loop_vsf_t* loop = (main_loop_vsf_t*)l;

  timer_dispatch();
  main_loop_dispatch_input(loop);
  main_loop_dispatch_events(loop);
  idle_dispatch();

  window_manager_paint(loop->base.wm);

  return RET_OK;
}

static ret_t main_loop_vsf_run(main_loop_t* l) {
  main_loop_vsf_t* loop = (main_loop_vsf_t*)l;

  loop->pressed = FALSE;
  while (l->running) {
    main_loop_step(l);
    main_loop_sleep(l);
  }

  return RET_OK;
}

main_loop_vsf_t* main_loop_vsf_init(int w, int h) {
  static main_loop_vsf_t s_main_loop_vsf;
  main_loop_vsf_t* loop = &s_main_loop_vsf;

  memset(loop, 0x00, sizeof(main_loop_vsf_t));

  loop->w = w;
  loop->h = h;
  loop->base.wm = window_manager();
  return_value_if_fail(loop->base.wm != NULL, NULL);

  loop->queue = event_queue_create(20);
  return_value_if_fail(loop->queue != NULL, NULL);

  loop->base.run = main_loop_vsf_run;
  loop->base.step = main_loop_vsf_step;
  loop->base.queue_event = main_loop_vsf_queue_event;

  window_manager_post_init(loop->base.wm, w, h);
  main_loop_set((main_loop_t*)loop);

  return loop;
}

ret_t main_loop_vsf_reset(main_loop_vsf_t* loop) {
  return_value_if_fail(loop != NULL, RET_BAD_PARAMS);
  event_queue_destroy(loop->queue);

  memset(loop, 0x00, sizeof(main_loop_vsf_t));

  return RET_OK;
}

static ret_t main_loop_raw_dispatch(main_loop_vsf_t* loop) {
  platform_disaptch_input((main_loop_t*)loop);

  return RET_OK;
}

static ret_t main_loop_raw_destroy(main_loop_t* l) {
  main_loop_vsf_t* loop = (main_loop_vsf_t*)l;
  main_loop_vsf_reset(loop);
  native_window_raw_deinit();

  return RET_OK;
}

static ret_t main_loop_raw_init_canvas(uint32_t w, uint32_t h) {
  extern lcd_t *platform_create_lcd(wh_t w, wh_t h);
  lcd_t* lcd = platform_create_lcd(w, h);

  return_value_if_fail(lcd != NULL, RET_OOM);
  native_window_raw_init(lcd);

  return RET_OK;
}

main_loop_t* main_loop_init(int w, int h) {
  main_loop_vsf_t* loop = NULL;
  return_value_if_fail(main_loop_raw_init_canvas(w, h) == RET_OK, NULL);

  loop = main_loop_vsf_init(w, h);
  return_value_if_fail(loop != NULL, NULL);

  loop->base.destroy = main_loop_raw_destroy;
  loop->dispatch_input = main_loop_raw_dispatch;

  return (main_loop_t*)loop;
}

#endif
