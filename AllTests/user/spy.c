#include "spy.h"

static uint16_t mock_input;
static char lcd_str[20];

char* Menu_getStr(void)
{
	return lcd_str;
}

uint16_t BTN_getInput(void)
{
	return mock_input;
}

void BTN_setInput(uint16_t btn)
{
	mock_input = btn;
}

void GRA_drawStr(const char *str, uint8_t col, uint8_t page)
{
	char *ptr = lcd_str;
	while(*str != '\0')
	{
		*ptr++ = *str++;
	}
	*ptr = '\0';

}