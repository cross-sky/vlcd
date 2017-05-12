#include "cominc.h"
static uint8_t menuOnFlag=1;

const char* LcdOnStr = "Lcd on";
const char* LcdOffStr = "Lcd off";

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
		}
		else
		{
			GRA_drawStr(LcdOffStr, 0, 0);
		}
		
	}
}

