#ifndef _LL_USER_H_
#define _LL_USER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "LL_General.h"

#define FONT_LIB_TOTAL_NUM        1
#define PAGE_MAX                 1
#define LL_LCD_ANGLE         0


#define FONT_LIB_C_SIMSUN_9         &userFontLibList[0]

extern uint8_t userPageMax;
extern uint8_t userFontLibTotalNum;

extern llFontLib userFontLibList[FONT_LIB_TOTAL_NUM];
extern void (*pageInitFunc[PAGE_MAX])(void);
extern void (*pageLoopFunc[PAGE_MAX])(void);
extern void (*pageQuitFunc[PAGE_MAX])(void);

void llUserInit(void);

uint16_t llUserGetAngle(void);

#ifdef __cplusplus
}
#endif

#endif //_LL_USER_H_
