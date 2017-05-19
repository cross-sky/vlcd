#include "cominc.h"
static uint8_t menuOnFlag=1;

const char* LcdOnStr = "Lcd on";
const char* LcdOffStr = "Lcd off";

typedef struct{
	uint8_t onflag;
}MenuData_T;

static MenuData_T S_menuData;

void Menu_initPara(void)
{
	S_menuData.onflag = 0;
}

static void setOnflag(uint8_t onOrOff)
{
	S_menuData.onflag = onOrOff & 0x01;
}

uint8_t Menu_getOnFlag(void)
{
	return S_menuData.onflag;
}

void Menu_process(void)
{
	uint16_t key;
	key = BTN_KeyPop();
	if (!key)
	{
		return;
	}
	if (key == BTN_KEY_EXIT)
	{
		
		if ((menuOnFlag++)&0x01)
		{
			GRA_drawStr(LcdOnStr, 0, 0);
			setOnflag(1);
		}
		else
		{
			GRA_drawStr(LcdOffStr, 0, 0);
			setOnflag(0);
		}
		Uart1_sendCodeA();
	}
}




