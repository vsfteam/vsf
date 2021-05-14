#ifndef _UI_LL_APPLICATION_LOGIC_H_
#define _UI_LL_APPLICATION_LOGIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "LL_Background.h"
#include "LL_Button.h"
#include "LL_Image.h"
#include "LL_Text.h"
#include "LL_CheckBox.h"
#include "LL_LineEdit.h"
#include "LL_Slider.h"
#include "LL_ProgressBar.h"
#include "LL_Window.h"
#include "LL_QRCode.h"
#include "LL_Gauge.h"
#include "LL_Timer.h"
#include "LL_DateTime.h"
#include "LL_IconSlider.h"
#include "LL_User.h"

#define ID_BACKGROUND          0x400
#define ID_HELLO_WORLD          0x401
#define ID_BUTTON_0          0x402


void ui_ll_applicationLogicInit(void);
void ui_ll_applicationLogicLoop(void);
void ui_ll_applicationLogicQuit(void);
#ifdef __cplusplus
}
#endif

#endif //_UI_LL_APPLICATION_LOGIC_H_

