/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_DISP_MIPI_LCD_DISPLAY_COMMAND_SET_H__
#define __VSF_DISP_MIPI_LCD_DISPLAY_COMMAND_SET_H__

/*============================ INCLUDES ======================================*/

#if VSF_USE_UI == ENABLED && VSF_DISP_USE_MIPI_LCD == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROFIED FUNCTIONS ===========================*/

#define VSF_DISP_MIPI_LCD_WRITE(__CMD, __PARAM_LEN, ...)                        \
            (__CMD), (__PARAM_LEN),  ##__VA_ARGS__

#define VSF_DISP_MIPI_LCD_DELAY_MS(__MS)                                        \
            (0), (__MS)

/*============================ MACROS ========================================*/

// MIPI Display Command Set
// Current Specification Version is MIPI DCS v1.5 (March 2021)
// The main reference for this code here is v1.2: mipi-DCS-specification-v1.2a.pdf

// No Operation
#define MIPI_DCS_CMD_HEX_CODE_NOP                       0x00
#define MIPI_DCS_CMD_NOP  \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_NOP, 0)

// Software Reset
#define MIPI_DCS_CMD_HEX_CODE_SOFT_RESET                0x01
#define MIPI_DCS_CMD_SOFT_RESET  \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SOFT_RESET, 0)

// TODO: support read
//#define MIPI_DCS_CMD_HEX_CODE_GET_COMPRESSION_MODE    0x03
//#define MIPI_DCS_CMD_HEX_CODE_GET_DISPLAY_ID          0x04
//#define MIPI_DCS_CMD_HEX_CODE_GET_ERROR_COUNT_ON_DSI  0x05
//#define MIPI_DCS_CMD_HEX_CODE_GET_RED_CHANNEL         0x06
//#define MIPI_DCS_CMD_HEX_CODE_GET_GREEN_CHANNEL       0x07
//#define MIPI_DCS_CMD_HEX_CODE_GET_BLUE_CHANNEL        0x08
//#define MIPI_DCS_CMD_HEX_CODE_GET_DISPLAY_STATUS      0x09
//#define MIPI_DCS_CMD_HEX_CODE_GET_POWER_MODE          0x0A
//#define MIPI_DCS_CMD_HEX_CODE_GET_ADDRESS_MODE        0x0B
//#define MIPI_DCS_CMD_HEX_CODE_GET_PIXEL_FORMAT        0x0C
//#define MIPI_DCS_CMD_HEX_CODE_GET_DISPLAY_MODE        0x0D
//#define MIPI_DCS_CMD_HEX_CODE_GET_SIGNAL_MODE         0x0E
//#define MIPI_DCS_CMD_HEX_CODE_GET_DIAGNOSTIC_RESULT   0x0F

// Power for the display panel is off
#define MIPI_DCS_CMD_HEX_CODE_ENTER_SLEEP_MODE          0x10
#define MIPI_DCS_CMD_ENTER_SLEEP_MODE \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_ENTER_SLEEP_MODE, 0)

// Power for the display panel is on
#define MIPI_DCS_CMD_HEX_CODE_EXIT_SLEEP_MODE           0x11
#define MIPI_DCS_CMD_EXIT_SLEEP_MODE \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_EXIT_SLEEP_MODE, 0)

// Part of the display area is used for image display
#define MIPI_DCS_CMD_HEX_CODE_ENTER_PARTIAL_MODE        0x12
#define MIPI_DCS_CMD_ENTER_PARTIAL_MODE \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_ENTER_PARTIAL_MODE, 0)

// The whole display area is used for image display
#define MIPI_DCS_CMD_HEX_CODE_ENTER_NORMAL_MODE         0x13
#define MIPI_DCS_CMD_ENTER_NORMAL_MODE \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_ENTER_NORMAL_MODE, 0)

// TODO: support read
//#define MIPI_DCS_CMD_HEX_CODE_GET_IMAGE_CHECKSUM_RGB  0x14
//#define MIPI_DCS_CMD_HEX_CODE_GET_IMAGE_CHECKSUM_CT   0x15

// Displayed image colors are not inverted
#define MIPI_DCS_CMD_HEX_CODE_EXIT_INVERT_MODE          0x20
#define MIPI_DCS_CMD_EXIT_INVERT_MODE \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_EXIT_INVERT_MODE, 0)

// Displayed image colors are inverted
#define MIPI_DCS_CMD_HEX_CODE_ENTER_INVERT_MODE         0x21
#define MIPI_DCS_CMD_ENTER_INVERT_MODE \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_ENTER_INVERT_MODE, 0)

// Selects the gamma curve used by the display device.
#define MIPI_DCS_CMD_HEX_CODE_SET_GAMMA_CURVE           0x26
#define MIPI_DCS_CMD_GAMMA_CURVE_GC0                    0x01
#define MIPI_DCS_CMD_GAMMA_CURVE_GC1                    0x02
#define MIPI_DCS_CMD_GAMMA_CURVE_GC2                    0x04
#define MIPI_DCS_CMD_GAMMA_CURVE_GC3                    0x08
#define MIPI_DCS_CMD_SET_GAMMA_CURVE(__GC_MASK) \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_GAMMA_CURVE, 0, __GC_MASK)

// Blanks the display device
#define MIPI_DCS_CMD_HEX_CODE_SET_DISPLAY_OFF           0x28
#define MIPI_DCS_CMD_SET_DISPLAY_OFF \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_DISPLAY_OFF, 0)

// Show the image on the display device
#define MIPI_DCS_CMD_HEX_CODE_SET_DISPLAY_ON            0x29
#define MIPI_DCS_CMD_SET_DISPLAY_ON \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_DISPLAY_ON, 0)

// Set the column extent
#define MIPI_DCS_CMD_HEX_CODE_SET_COLUMN_ADDRESS        0x2A
// SC: Start Column, EC: End Column, Address: closed interval [SC, EC]
#define MIPI_DCS_CMD_SET_COLUMN_ADDRESS(__SC, __EC)   \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_COLUMN_ADDRESS, 4,\
                                    ((uint16_t)__SC >> 8), (__SC & 0xFF),       \
                                    ((uint16_t)__EC >> 8), (__EC & 0xFF))

// Set the page extent
#define MIPI_DCS_CMD_HEX_CODE_SET_PAGE_ADDRESS          0x2B
// SP: Start Page, EP: End Page, Address: closed interval [SP, EP]
#define MIPI_DCS_CMD_SET_PAGE_ADDRESS(__SP, __EP)     \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_PAGE_ADDRESS, 4,  \
                                    ((uint16_t)__SP >> 8), (__SP & 0xFF),       \
                                    ((uint16_t)__EP >> 8), (__EP & 0xFF))

// Transfer image data from the Host Processor to the peripheral starting at
// the location provided by set_column_address and set_page_address
#define MIPI_DCS_CMD_HEX_CODE_WRITE_MEMORY_START        0x2C
#define MIPI_DCS_CMD_WRITE_MEMORY_START(__NUM, ...) \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_WRITE_MEMORY_START, __NUM, ##__VA_ARGS__)

// Fills the peripheral look-up table with the provided data
#define MIPI_DCS_CMD_HEX_CODE_WRITE_LUT                 0x2D
#define MIPI_DCS_CMD_WRITE_LUT(__NUM, ...) \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_WRITE_LUT, __NUM, ##__VA_ARGS__)

// TODO: support read
//#define MIPI_DCS_CMD_HEX_CODE_READ_MEMORY_START       0x2E

// Defines the number of rows in the partial display area on the display device.
#define MIPI_DCS_CMD_HEX_CODE_SET_PARTIAL_ROWS          0x30
// SR: Start Row, ER: End Row, Address: closed interval [SR, ER]
#define MIPI_DCS_CMD_SET_PARTIAL_ROWS(__SR, __ER)                                   \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_PARTIAL_ROWS,  4,     \
                                            ((uint16_t)__SR >> 8), (__SR & 0xFF),   \
                                            ((uint16_t)__ER >> 8), (__ER & 0xFF))

#define MIPI_DCS_CMD_HEX_CODE_SET_PARTIAL_COLUMNS       0x31
// SC: Start Column, EC: End Column, Address: closed interval [SC, EC]
#define MIPI_DCS_CMD_SET_PARTIAL_COLUMNS(__SC, __EC)                                \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_PARTIAL_COLUMNS, 4,   \
                                            ((uint16_t)__SC >> 8), (__SC & 0xFF),   \
                                            ((uint16_t)__EC >> 8), (__EC & 0xFF))

// Defines the vertical scrolling and fixed area on display device
#define MIPI_DCS_CMD_HEX_CODE_SET_SCROLL_AREA           0x33
// __TFA: TOP_FIXED_AREA, __VSA: VERTICAL_SCROLLING_AREA, __BFA: BOTTOM_FIXED_AREA
#define MIPI_DCS_CMD_SET_SCROLL_AREA(__TFA, __VERTICAL_SCROLLING_AREA, __BFA)       \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_SCROLL_AREA, 6,       \
                                    ((uint16_t)__TFA >> 8), (__TFA & 0xFF),         \
                                    ((uint16_t)__VSA >> 8), (__VSA & 0xFF),         \
                                    ((uint16_t)__BFA >> 8), (__BFA & 0xFF))

// Synchronization information is not sent from the display module to the host processor
#define MIPI_DCS_CMD_HEX_CODE_SET_TEAR_OFF              0x34
#define MIPI_DCS_CMD_SET_TEAR_OFF \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_TEAR_OFF, 0)

// Synchronization information is sent from the display module to the host processor at the start of VFP
#define MIPI_DCS_CMD_HEX_CODE_SET_TEAR_ON               0x35
#define MIPI_DCS_CMD_SET_TEAR_ON \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_TEAR_ON, 0)

#define MIPI_DCS_CMD_HEX_CODE_SET_ADDRESS_MODE          0x36
// bits[7:5]: host processor to display module＊s frame memory
// Page Address Order, 0 = Top to Bottom, 1 = Bottom to Top
#define MIPI_DCS_PAGE_ADDRESS_TOP_TO_BOTTOM             (0 << 7)
#define MIPI_DCS_PAGE_ADDRESS_BOTTOM_TO_TOP             (1 << 7)
// Column Address Order, 0 = Left to Right, 1 = Right to Left
#define MIPI_DCS_COLUME_ADDRESS_LEFT_TO_RIGHT           (0 << 6)
#define MIPI_DCS_COLUME_ADDRESS_RIGHT_TO_LEFT           (1 << 6)
// Page/Column Addressing Order,
// 0 = Normal Mode, column register is first increment
// 1 = Reverse Mode, page register is first increment
#define MIPI_DCS_PAGE_COLUMN_NORMAL_ORDER               (0 << 5)
#define MIPI_DCS_PAGE_COLUMN_REVERSE_ORDER              (1 << 5)
// bits[4:0]: display module＊s frame memory to the display device
// Display Device Line Refresh Order, 0 = Top line to Bottom line, 1 = Bottom line to Top line
#define MIPI_DCS_DEVICE_REFRESH_TOP_TO_BOTTOM           (0 << 4)
#define MIPI_DCS_DEVICE_REFRESH_BOTTOM_TO_TOP           (1 << 4)
// RGB/BGR Order, 0 = RGB, 1 = BGR
#define MIPI_DCS_DEVICE_REFRESH_RGB                     (0 << 3)
#define MIPI_DCS_DEVICE_REFRESH_BGR                     (1 << 3)
// Display Data Latch Data Order, 0 = left side to the right side, 1 = right side to the left side
#define MIPI_DCS_LCD_REFRESH_LEFT_TO_RIGHT              (0 << 2)
#define MIPI_DCS_LCD_REFRESH_RIGHT_TO_LEFT              (1 << 2)
// Flip Horizontal, 0 = Normal, 1 = Flipped
#define MIPI_DCS_FLIP_HORIZONTAL_NORMAL                 (0 << 1)
#define MIPI_DCS_FLIP_HORIZONTAL_FLIPPED                (1 << 1)
// Flip Vertical, 0 = Normal, 1 = Flipped
#define MIPI_DCS_FLIP_VERTICAL_NORMAL                   (0 << 0)
#define MIPI_DCS_FLIP_VERTICAL_FLIPPED                  (1 << 0)
// TODO: define some mode
#define MIPI_DCS_CMD_SET_ADDRESS_MODE(__MODE_MASK) \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_ADDRESS_MODE, 1, __MODE_MASK)

// Defines the vertical scrolling starting point
#define MIPI_DCS_CMD_HEX_CODE_SET_SCROLL_START          0x37
#define MIPI_DCS_CMD_SET_SCROLL_START(__VSP) \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_SCROLL_START, 2, \
                                    ((uint16_t)__VSP >> 8), (__VSP & 0xFF))

// Idle Mode Off
#define MIPI_DCS_CMD_HEX_CODE_EXIT_IDLE_MODE            0x38
#define MIPI_DCS_CMD_EXIT_IDLE_MODE \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_EXIT_IDLE_MODE, 0)

// Idle Mode On
#define MIPI_DCS_CMD_HEX_CODE_ENTER_IDLE_MODE           0x39
#define MIPI_DCS_CMD_ENTER_IDLE_MODE \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_ENTER_IDLE_MODE, 0)

// TODO
// Defines how many bits per pixel are used in the interface.
#define MIPI_DCS_CMD_HEX_CODE_SET_PIXEL_FORMAT          0x3A
#define MIPI_DCS_PIXEL_FORMAT_BITS_3                    0x01
#define MIPI_DCS_PIXEL_FORMAT_BITS_8                    0x02
#define MIPI_DCS_PIXEL_FORMAT_BITS_12                   0x03
#define MIPI_DCS_PIXEL_FORMAT_BITS_16                   0x05
#define MIPI_DCS_PIXEL_FORMAT_BITS_18                   0x06
#define MIPI_DCS_PIXEL_FORMAT_BITS_24                   0x07
// __BITS IN [3, 8, 12, 16, 18, 24]
#define MIPI_DCS_PIXEL_FORMAT_BITS(__BITS)              MIPI_DCS_PIXEL_FORMAT_BITS_ ## __BITS
// DPI: Display Pixel Interface, or MCU Interface
// __BITS IN [3, 8, 12, 16, 18, 24]
#define MIPI_DCS_PIXEL_FORMAT_DBI_BITS(__BITS)          MIPI_DCS_PIXEL_FORMAT_BITS(__BITS)
// DBI: Display Bus Interface, or System-80 interface
// __BITS IN [3, 8, 12, 16, 18, 24]
#define MIPI_DCS_PIXEL_FORMAT_DPI_BITS(__BITS)          (MIPI_DCS_PIXEL_FORMAT_BITS(__BITS) << 4)
#define MIPI_DCS_CMD_SET_PIXEL_FORMAT(__PF) \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_PIXEL_FORMAT, 1, __PF)

// Transfer image information from the Host Processor interface to the peripheral from the last written location.
#define MIPI_DCS_CMD_HEX_CODE_WRITE_MEMORY_CONTINUE     0x3C
#define MIPI_DCS_CMD_WRITE_MEMORY_CONTINUE(__LEN, ...) \
    VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_WRITE_MEMORY_CONTINUE, __LEN, ##__VA_ARGS__)

// 3D is used on the display panel
#define MIPI_DCS_CMD_HEX_CODE_SET_3D_CONTROL            0x3D
// 3DL/R 每 Left / Right Order, 0 = Data sent left eye first, 1 = Data sent right eye first.
#define MIPI_DCS_3D_CONTROL_LEFT_EYE_FIRST              (0 << 5)
#define MIPI_DCS_3D_CONTROL_RIGHT_EYE_FIRST             (1 << 5)
// 3DVSYNC 每 Second VSYNC Enabled between Left and Right images
// 0 = No sync pulses between left and right data.
// 1 = Sync pulse (HSYNC, VSYNC, blanking) between left and right data.
#define MIPI_DCS_3D_CONTROL_3DVSYNC_NO                  (0 << 4)
#define MIPI_DCS_3D_CONTROL_3DVSYNC_SYNC                (1 << 4)
// 3DFMT[1:0] 每 Stereoscopic Image Format
// 00 = Line (alternating lines of left and right data)
// 01 = Frame (alternating frames of left and right data)
// 10 = Pixel (alternating pixels of left and right data)
#define MIPI_DCS_3D_CONTROL_3DFMT_LINE                  (0 << 2)
#define MIPI_DCS_3D_CONTROL_3DFMT_FRAME                 (1 << 2)
#define MIPI_DCS_3D_CONTROL_3DFMT_PIXEL                 (2 << 2)
// 3DMODE[1:0] 每 3D Mode On / Off, Display Orientation
// 00 = 3D Mode Off (2D Mode On).
// 01 = 3D Mode On, Portrait Orientation
// 10 = 3D Mode On, Landscape Orientation
#define MIPI_DCS_3D_CONTROL_3DMODE_OFF                  (0 << 0)
#define MIPI_DCS_3D_CONTROL_3DMODE_ON_PO                (1 << 0)
#define MIPI_DCS_3D_CONTROL_3DMODE_ON_LO                (2 << 0)
#define MIPI_DCS_CMD_SET_3D_CONTROL(__V)                                        \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_3D_CONTROL, 2, __V, 0)

// TODO: support read
//#define MIPI_DCS_CMD_HEX_CODE_READ_MEMORY_CONTINUE    0x3E
//#define MIPI_DCS_CMD_HEX_CODE_GET_3D_CONTROL          0x3F

// Set VSYNC timing
#define MIPI_DCS_CMD_HEX_CODE_SET_VSYNC_TIMING          0x40
// RESET 每 Restart display update, 0 = No operation, 1 = Restart display update
#define MIPI_DCS_VSYNC_TIMING_NO_RESET                  (0 << 7)
#define MIPI_DCS_VSYNC_TIMING_RESET                     (1 << 7)
// DIR 每 Line Direction, 0 = Later (Down), 1 = Earlier (Up)
#define MIPI_DCS_VSYNC_TIMING_LD_LATER                  (0 << 7)
#define MIPI_DCS_VSYNC_TIMING_LD_EARLIER                (1 << 7)
// LINES[4:0] 每 Number of Lines in Adjustment
#define MIPI_DCS_VSYNC_TIMING_LD_LINES(__L)             (__L << 1)
// FRAME 每 Adjustment Frame, 0 = Next Frame, 1 = Frame After Next Frame
#define MIPI_DCS_VSYNC_TIMING_NEXT_FRAME                (0 << 0)
#define MIPI_DCS_VSYNC_TIMING_FRAME_AFTER_NEXT_FRAME    (1 << 0)
#define MIPI_DCS_CMD_SET_VSYNC_TIMING(__V) \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_VSYNC_TIMING, 1)

// TODO
// Synchronization information is sent from the display module to
// the host processor when the display device refresh reaches the provided scanline.
#define MIPI_DCS_CMD_HEX_CODE_SET_TEAR_SCANLINE         0x44
#define MIPI_DCS_CMD_SET_TEAR_SCANLINE(__N) \
            VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_TEAR_SCANLINE, 2, \
                                    ((uint16_t)__N >> 8), (__N & 0xFF)

// TODO: support read
//#define MIPI_DCS_CMD_HEX_CODE_GET_SCANLINE            0x45

// TODO: need more doc
#define MIPI_DCS_CMD_HEX_CODE_SET_DISPLAY_BRIGHTNESS    0x51
#define MIPI_DCS_CMD_SET_DISPLAY_BRIGHTNESS(__B) \
    VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_DISPLAY_BRIGHTNESS, 1, __B)

// TODO: support read
//#define MIPI_DCS_CMD_HEX_CODE_GET_DISPLAY_BRIGHTNESS  0x52

// TODO: need more doc
#define MIPI_DCS_CMD_HEX_CODE_WRITE_CONTROL_DISPLAY     0x53
#define MIPI_DCS_CMD_WRITE_CONTROL_DISPLAY(__DISP) \
    VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_WRITE_CONTROL_DISPLAY, 1, __DISP)

// TODO: support read
//#define MIPI_DCS_CMD_HEX_CODE_GET_CONTROL_DISPLAY     0x54

// TODO: need more doc
#define MIPI_DCS_CMD_HEX_CODE_WRITE_POWER_SAVE          0x55
#define MIPI_DCS_CMD_WRITE_POWER_SAVE(__V) \
    VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_WRITE_POWER_SAVE, 1, __V)

// TODO: support read
//#define MIPI_DCS_CMD_HEX_CODE_GET_POWER_SAVE          0x56

// TODO: need more doc
#define MIPI_DCS_CMD_HEX_CODE_SET_CABC_MIN_BRIGHTNESS   0x5E
#define MIPI_DCS_CMD_SET_CABC_MIN_BRIGHTNESS(__B) \
    VSF_DISP_MIPI_LCD_WRITE(MIPI_DCS_CMD_HEX_CODE_SET_CABC_MIN_BRIGHTNESS, 1, __B)

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#ifdef __cplusplus
}
#endif

#endif  // VSF_USE_UI
#endif  // __VSF_DISP_MIPI_SPI_LCD_H__
