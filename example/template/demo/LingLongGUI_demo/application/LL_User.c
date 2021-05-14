#include "LL_User.h"
#include "ui_ll_application.h"
#include "SimSun_9.h"

uint8_t userPageMax=PAGE_MAX;
uint8_t userFontLibTotalNum=FONT_LIB_TOTAL_NUM;

llFontLib userFontLibList[FONT_LIB_TOTAL_NUM]={
    {typeCFile,"SimSun_9",9,&SimSun_9_Lib}
};

void (*pageInitFunc[PAGE_MAX])(void);
void (*pageLoopFunc[PAGE_MAX])(void);
void (*pageQuitFunc[PAGE_MAX])(void);

void llUserInit(void)
{
    pageInitFunc[0]=ui_ll_applicationInit;

    pageLoopFunc[0]=ui_ll_applicationLoop;

    pageQuitFunc[0]=ui_ll_applicationQuit;
};

uint16_t llUserGetAngle(void)
{
    return LL_LCD_ANGLE;
}
