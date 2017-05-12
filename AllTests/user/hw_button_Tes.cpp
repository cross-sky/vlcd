
#include <stdio.h>
#include <stdarg.h>
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "cominc.h"
#include "spy.h"
}

TEST_GROUP(HW_BUTTON_TEST)
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

TEST(HW_BUTTON_TEST, shortKey)
{
	//0µ¼Í¨£¬ 1¶Ï¿ª
	BTN_setInput(0xffff);
	LONGS_EQUAL(0, BTN_softProcess(BTN_getInput()));
	
	BTN_setInput(~GPIO_Pin_1);
	LONGS_EQUAL(BTN_KEY_EXIT, BTN_softProcess(BTN_getInput()));
	LONGS_EQUAL(0, BTN_softProcess(BTN_getInput()));

	BTN_setInput(~GPIO_Pin_3);
	LONGS_EQUAL(BTN_KEY_DOWN, BTN_softProcess(BTN_getInput()));
}


TEST(HW_BUTTON_TEST, longKey)
{
	uint8_t i;
	BTN_setInput(~GPIO_Pin_1);
	for (i = 0; i < 23; i++)
	{
		BTN_softProcess(BTN_getInput());
	}
	LONGS_EQUAL(BTN_KEY_SHUT, BTN_getLongKey());
}

TEST(HW_BUTTON_TEST, pushPopKey)
{
	uint16_t key = BTN_KEY_EXIT;
	BTN_KeyPush(key);
	LONGS_EQUAL(key, BTN_KeyPop());
}

