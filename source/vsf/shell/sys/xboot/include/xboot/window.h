#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "list.h"
#include <graphic/surface.h>

// xboot/event.h
enum {
    MOUSE_BUTTON_LEFT                    = (0x1 << 0),
    MOUSE_BUTTON_RIGHT                    = (0x1 << 1),
    MOUSE_BUTTON_MIDDLE                    = (0x1 << 2),
};

enum event_type_t {
    EVENT_TYPE_KEY_DOWN,
    EVENT_TYPE_KEY_UP,

    EVENT_TYPE_MOUSE_DOWN,
    EVENT_TYPE_MOUSE_MOVE,
    EVENT_TYPE_MOUSE_UP,
    EVENT_TYPE_MOUSE_WHEEL,

    EVENT_TYPE_TOUCH_BEGIN,
    EVENT_TYPE_TOUCH_MOVE,
    EVENT_TYPE_TOUCH_END,
};
struct event_t {
    void * device;
    enum event_type_t type;
    union {
        /* Key */
        struct {
            uint32_t key;
        } key_down;
        struct {
            uint32_t key;
        } key_up;

        /* Mouse */
        struct {
            int32_t x, y;
            uint32_t button;
        } mouse_down;
        struct {
            int32_t x, y;
        } mouse_move;
        struct {
            int32_t x, y;
            uint32_t button;
        } mouse_up;
        struct {
            int32_t dx, dy;
        } mouse_wheel;

        /* Touch */
        struct {
            int32_t x, y;
            uint32_t id;
        } touch;
        struct {
            int32_t x, y;
            uint32_t id;
        } touch_begin;
        struct {
            int32_t x, y;
            uint32_t id;
        } touch_move;
        struct {
            int32_t x, y;
            uint32_t id;
        } touch_end;
    } e;
};

// xboot/window.h
struct window_manager_t {
    spinlock_t lock;
    vsf_dlist_t window_list;
    int refresh;
    struct {
        int dirty;
        int show;
    } cursor;

    vk_disp_t *disp;
};

struct window_t {
    vsf_dlist_node_t window_node;
    struct surface_t * s;
    struct window_manager_t * wm;
    void * priv;
};

struct window_t * window_alloc(const char * fb, const char * input, void * data);
void window_free(struct window_t * w);
int window_is_active(struct window_t * w);
int window_get_width(struct window_t * w);
int window_get_height(struct window_t * w);
void window_present(struct window_t * w, struct color_t * c, void * o, void (*draw)(struct window_t *, void *));
int window_pump_event(struct window_t * w, struct event_t * e);

#endif      // __WINDOW_H__
