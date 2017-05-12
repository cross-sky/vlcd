#include "graphics.h"
#include "font3.h"

void GRA_drawChar(char c, uint8_t col, uint8_t page)
{
	uint8_t i;
	uint8_t t;
	t = (c - 0x20); 
	for (i = 0; i < 8; i++)
	{
		glcd_char(col + i, page, Font8x8[t][i]);
	}
}

void GRA_drawStr(const char *str, uint8_t col, uint8_t page)
{
	uint8_t x=0, y;
	y = page << 3;
	while(*str != '\0')
	{
		x = (col++) << 3;
		GRA_drawChar(*str++, x, y);
	}
}
