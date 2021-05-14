#include "ui_ll_application.h"
#include "ui_ll_applicationLogic.h"
#include "LL_General.h"
#include "LL_Handler.h"
#include "LL_Linked_List.h"
#include "LL_Timer.h"
#include "LL_ButtonEx.h"


void ui_ll_applicationInit(void)
{
    //背景
    llBackgroundQuickCreate(ID_BACKGROUND,320,240,true,RGB888(0xf1f1f1),0);

    //文本
    llTextQuickCreate(ID_HELLO_WORLD,ID_BACKGROUND,110,50,80,30,(uint8_t *)"Hello World!!",FONT_LIB_C_SIMSUN_9,RGB888(0x000000),RGB888(0xffffff),llAlignHLeft,llAlignVTop,0,0,false,false,false);
    nTextSetEnabled(ID_HELLO_WORLD,true);

    //按键
    llButtonQuickCreate(ID_BUTTON_0,ID_BACKGROUND,110,110,80,30,(uint8_t *)"Button",FONT_LIB_C_SIMSUN_9,RGB888(0x000000),RGB888(0x55aaff),RGB888(0x55aaff),0xFFFFFFFF,0xFFFFFFFF,BUTTON_DIS_TYPE_COLOR,false);
    nButtonSetEnabled(ID_BUTTON_0,true);
    nButtonSetCheckable(ID_BUTTON_0,false);
    nButtonSetKeyValue(ID_BUTTON_0,0);

    ui_ll_applicationLogicInit();
}

void ui_ll_applicationLoop(void)
{
    ui_ll_applicationLogicLoop();
}

void ui_ll_applicationQuit(void)
{
    ui_ll_applicationLogicQuit();
}

