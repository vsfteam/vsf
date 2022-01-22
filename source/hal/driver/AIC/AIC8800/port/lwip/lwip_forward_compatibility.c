// remove this file after vendor supports lwip 2.1.2

#include "vsf_cfg.h"

#if VSF_USE_LWIP == ENABLED

#include "lwip/arch.h"
#include "lwip/pbuf.h"

// libwifi depends on errno, but errno in vsf.linux is a MACRO
int errno;

// API copied from LWIP 2.1.2
// remove this until vendor updates lwip to 2.1.2
u8_t
pbuf_remove_header(struct pbuf *p, size_t header_size_decrement)
{
  void *payload;
  u16_t increment_magnitude;

  LWIP_ASSERT("p != NULL", p != NULL);
  if ((p == NULL) || (header_size_decrement > 0xFFFF)) {
    return 1;
  }
  if (header_size_decrement == 0) {
    return 0;
  }

  increment_magnitude = (u16_t)header_size_decrement;
  /* Check that we aren't going to move off the end of the pbuf */
  LWIP_ERROR("increment_magnitude <= p->len", (increment_magnitude <= p->len), return 1;);

  /* remember current payload pointer */
  payload = p->payload;
  LWIP_UNUSED_ARG(payload); /* only used in LWIP_DEBUGF below */

  /* increase payload pointer (guarded by length check above) */
  p->payload = (u8_t *)p->payload + header_size_decrement;
  /* modify pbuf length fields */
  p->len = (u16_t)(p->len - increment_magnitude);
  p->tot_len = (u16_t)(p->tot_len - increment_magnitude);

  LWIP_DEBUGF(PBUF_DEBUG | LWIP_DBG_TRACE, ("pbuf_remove_header: old %p new %p (%"U16_F")\n",
              (void *)payload, (void *)p->payload, increment_magnitude));

  return 0;
}

struct pbuf *
pbuf_free_header(struct pbuf *q, u16_t size)
{
  struct pbuf *p = q;
  u16_t free_left = size;
  while (free_left && p) {
    if (free_left >= p->len) {
      struct pbuf *f = p;
      free_left = (u16_t)(free_left - p->len);
      p = p->next;
      f->next = 0;
      pbuf_free(f);
    } else {
      pbuf_remove_header(p, free_left);
      free_left = 0;
    }
  }
  return p;
}

#endif      // VSF_USE_LWIP
