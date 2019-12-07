/*-----------------------------------------------------------------------------
 * Name:    LED_V2M-MPS2.c
 * Purpose: LED interface for V2M-MPS2 evaluation board
 * Rev.:    1.0.2
 * Note(s): possible defines select the used communication interface:
 *            __USE_FPGA    - use LEDs connected through FPGA
 *                          - use LEDs connected through SCC (default)
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2013 - 2017 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include "SMM_MPS2.h"                   /* Keil::Board Support:V2M-MPS2:Common */
#include "Board_LED.h"                  /* ::Board Support:LED */

#if defined __USE_FPGA
  #define LED_PORT   MPS2_FPGAIO->LED
  #define NUM_LEDS  (2)                 /* Number of available LEDs           */
#else
  #define LED_PORT   MPS2_SCC->CFG_REG1
  #define NUM_LEDS  (8)                 /* Number of available LEDs           */
#endif


/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize I/O interface for LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Initialize (void) {
#if 0
  uint32_t n;

  for (n = 0; n < NUM_LEDS; n++) {
    LED_Off (n);
  }
#endif
  LED_PORT = 0U;

  return 0;
}


/**
  \fn          int32_t LED_Uninitialize (void)
  \brief       De-initialize I/O interface for LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Uninitialize (void) {


  return 0;
}

/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on a single LED indicated by \em num
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_On (uint32_t num) {

  if (num < NUM_LEDS) {
    LED_PORT |=  (1U << num);
  }

  return 0;
}

/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off a single LED indicated by \em num
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Off (uint32_t num) {

 if (num < NUM_LEDS) {
    LED_PORT &= ~(1U << num);
  }

  return 0;
}

/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Control all LEDs with the bit vector \em val
  \param[in]   val  each bit represents the status of one LED.
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_SetOut(uint32_t val) {
#if 0
  uint32_t n;

  for (n = 0; n < NUM_LEDS; n++) {
    if (val & (1U << n)) {
      LED_On (n);
    } else {
      LED_Off(n);
    }
  }
#endif
  LED_PORT = val & (NUM_LEDS - 1);

  return 0;
}

/**
  \fn          uint32_t LED_GetCount (void)
  \brief       Get number of available LEDs on evaluation hardware
  \return      Number of available LEDs
*/
uint32_t LED_GetCount (void) {
  return NUM_LEDS;
}
