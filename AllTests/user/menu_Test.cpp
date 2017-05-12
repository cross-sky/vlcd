
#include <stdio.h>
#include <stdarg.h>
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "cominc.h"
#include "spy.h"
}

TEST_GROUP(MENU_TEST)
{
void setup()
{
	BTN_create();
}

void teardown()
{
	BTN_destory();
}
};

TEST(MENU_TEST, MenuOn)
{
	
	BTN_setInput(~GPIO_Pin_1);
	BTN_taskCheckKey();
	Menu_process();
	STRCMP_EQUAL(LcdOnStr, Menu_getStr());
	BTN_setInput(0xffff);
	BTN_taskCheckKey();
	BTN_setInput(~GPIO_Pin_1);
	BTN_taskCheckKey();
	Menu_process();
	STRCMP_EQUAL(LcdOffStr, Menu_getStr());
}
