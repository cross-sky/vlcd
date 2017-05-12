#include "cominc.h"
#define SCREEN_BUFF (SCREEN_WIDTH * SCREEN_HEIGHT / 8)  // /8

uint8_t glcd_buffer[SCREEN_BUFF];	

void glcd_initPins(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB,PE端口时钟
	//lcd rst scl sd cs --- pa
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(LCD_GPIOA, &GPIO_InitStructure);

	//init lcd_a0 -- pb0
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(LCD_GPIOB, &GPIO_InitStructure);
}

//row 1-8, column 1-128
void glcd_pixel(uint8_t x, uint8_t y, uint8_t colour) 
{
	uint16_t array_pos;
	if (x > SCREEN_WIDTH || y > SCREEN_HEIGHT) return;

	// Real screen coordinates are 0-63, not 1-64.
	x -= 1;
	y -= 1;

	array_pos = x + ((y >> 3) << 7); //x + ((y / 8) * 128);

	if (colour) {
		glcd_buffer[array_pos] |= 1 << (y % 8);
	} else {
		glcd_buffer[array_pos] &= 0xFF ^ 1 << (y % 8);
	}
}

//x 0-15, y 0-7
void glcd_char(uint8_t x, uint8_t y, uint8_t c) 
{
	uint16_t array_pos;
	if (x > SCREEN_WIDTH || y > SCREEN_HEIGHT) return;

	// Real screen coordinates are 0-63, not 1-64.
	//x -= 1;
	//y -= 1;

	array_pos = x + ((y >> 3) << 7); //x + ((y / 8) * 128);
	glcd_buffer[array_pos] = c;
}

void glcd_command(uint8_t command) {
	uint8_t n;
	// A0 is low for command data.
	LCD_A0_L;

	// Select the chip
	LCD_CS_L;

	for (n = 0; n < 8; n++) {
		LCD_SCLK_L;
		if (command & 0x80) {
			LCD_SDA_H;
		} else {
			LCD_SDA_L;
		}

		// Pulse SCL	
		LCD_SCLK_H;
		__nop();
		command <<= 1;
	}

	// Unselect the chip
	LCD_CS_H;
}

void glcd_data(unsigned char data) {
	uint8_t n;
	// A0 is high for display data
	LCD_A0_H;

	// Select the chip
	LCD_CS_L;

	for (n = 0; n < 8; n++) {
		LCD_SCLK_L;
		//delay_us(1);

		if (data & 0x80) {
			LCD_SDA_H;
		} else {
			LCD_SDA_L;
		}

		// Pulse SCL
		
		LCD_SCLK_H;
		__nop();
		data <<= 1;
		
	}
	// Unselect the chip
	LCD_CS_H;
}

void glcd_blank(void) {
	uint16_t y, x;
	int16_t n;
	// Reset the internal buffer
	for (n = 0; n < (SCREEN_BUFF); n++) {
		glcd_buffer[n] = 0;
	}

	// Clear the actual screen
	for (y = 0; y < 8; y++) {
		glcd_command(GLCD_CMD_SET_PAGE | y);

		// Reset column to 0 (the left side)
		glcd_command(GLCD_CMD_COLUMN_LOWER);
		glcd_command(GLCD_CMD_COLUMN_UPPER);

		// We iterate to 132 as the internal buffer is 65*132, not
		// 64*124.
		for (x = 0; x < 132; x++) {
			glcd_data(0x00);
		}
	}
}

void GLCD_refresh(void) {
	uint16_t x, y;
	for (y = 0; y < 8; y++) {
		glcd_command(GLCD_CMD_SET_PAGE | y);

		glcd_command(GLCD_CMD_COLUMN_LOWER);
		glcd_command(GLCD_CMD_COLUMN_UPPER);

		for (x = 0; x < 128; x++) {
			glcd_data(glcd_buffer[(y << 7) + x]); //y * 128
		}
	}
}

void GLCD_init(void) {
	glcd_initPins();
	LCD_RESET_L;
	delay_ms(20);
	LCD_RESET_H;
	delay_ms(20);

	////-----------------------------
 	glcd_command(0xae);// disp off
 	delay_us(20);
 	glcd_command(0xa2);// 1 / 9 bias
 	delay_us(20);
 	glcd_command(0xa0);// adc s0- s128
//	glcd_command(0xc0);
 	delay_us(20);
 	glcd_command(0xc0);// com1- com64
 	glcd_command(0x25);// ---
 	glcd_command(0x81);// sets v0
 	glcd_command(0x05);  // 
 	glcd_command(0x2f);// voltage follower ON regulator ON booster ON
 	glcd_command(0xa6);// Normal Display (not reverse dispplay)
 	glcd_command(0xa4);// Entire Display Disable
 	glcd_command(0xa3);	   //起到了关键性作用
 	glcd_command(0x40);// Set Display Start Line = com0
//  	glcd_command(0xb0);// Set Page Address = 0
//  	glcd_command(0x10);// Set Column Address 4 higher bits = 0
//  	glcd_command(0x01);// Set Column Address 4 lower bits = 1 , from IC SEG1 -> SEG128
 	glcd_command(0xaf);
	glcd_blank();
	GRA_drawStr(LcdOffStr, 0, 0);

//---------------------
	//glcd_command(0xe2);	   // LCD软件复位 

	//glcd_command(0xa0);  //  左右 A0：反向 A1：正向
	//glcd_command(0xc0);	 //  上下 C0 正向，C8反向

	//glcd_command(0x2f); //开关内部电源 后面的三位为1全开`2C 2E 2F
	//glcd_command(0x25); // 对比度20-27

	//glcd_command(0xaf);	 // 开显示 AE关

	//glcd_command(0x40);	//显示RAM的行地址  0x40为第0行
	//glcd_command(0xc8);	//  dizhi

	//glcd_command(0xa6); 
	//glcd_command(0xa4);
	//glcd_command(0xa3);	   // 偏压 A2 A3

	//glcd_command(0x81);	// 进入细调命令
	//glcd_command(0x05);	// 细调数据 00-3f
	////glcd_command(0xf8);	//	
	////glcd_command(0x3);		
	//glcd_blank();

}

void glcd_test_card(void) {
	int16_t n;
	uint8_t p = 0xFF;

	for (n = 0; n < SCREEN_BUFF; n++) {
		glcd_buffer[n] = 0;

		if (n % 4 == 0) {
			glcd_buffer[n] = p;
		}
	}
	GLCD_refresh();
}


void GLCD_test(void)
{
	static uint8_t i = 0;
	if ((i++) & 0x01)
	{
		GRA_drawStr("0abcdefgh", 0, 0);
		GRA_drawStr("1ijklmnop", 0, 1);
		GRA_drawStr("2qrstuvwx", 0, 2);
		GRA_drawStr("3yz", 0, 3);
		GRA_drawStr("4abcd", 0, 4);
		GRA_drawStr("5abcd", 0, 5);
		GRA_drawStr("6abcd", 0, 6);
		GRA_drawStr("7H0123", 0, 7);
		GLCD_refresh();
	}else
	{
		glcd_blank();
	}
}

