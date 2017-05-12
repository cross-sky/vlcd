#ifndef __ST7565_H_
#define __ST7565_H_

#include <stdint.h>

/** Screen width in pixels (tested with 128) */
#define SCREEN_WIDTH 128
/** Screen height in pixels (tested with 64) */
#define SCREEN_HEIGHT 64

#define LCD_GPIOA				GPIOA
#define LCD_GPIOB				GPIOB

#define LCD_SDA_H				LCD_GPIOA->BSRR = GPIO_Pin_7
#define LCD_SDA_L				LCD_GPIOA->BRR = GPIO_Pin_7

#define LCD_SCLK_H				LCD_GPIOA->BSRR = GPIO_Pin_5
#define LCD_SCLK_L				LCD_GPIOA->BRR = GPIO_Pin_5

//复位输入端，低有效
#define LCD_RESET_H				LCD_GPIOA->BSRR = GPIO_Pin_4
#define LCD_RESET_L				LCD_GPIOA->BRR = GPIO_Pin_4

//芯片选通端，低有效
#define LCD_CS_H				LCD_GPIOA->BSRR = GPIO_Pin_6
#define LCD_CS_L				LCD_GPIOA->BRR = GPIO_Pin_6

//命令数据选择端，高电平：数据，低电平：命令.
#define LCD_A0_H				LCD_GPIOB->BSRR = GPIO_Pin_0
#define LCD_A0_L				LCD_GPIOB->BRR = GPIO_Pin_0

/** Command: set the least significant 4 bits of the column address. */
#define GLCD_CMD_COLUMN_LOWER		0x10//0b00000000
/** Command: set the most significant 4 bits of the column address. */
#define GLCD_CMD_COLUMN_UPPER		0x01//0b00010000
/** Command: Set the current page (0..7). */
#define GLCD_CMD_SET_PAGE			0xb0//0b10110000

void GLCD_init(void);
void GLCD_test(void);

void glcd_char(uint8_t x, uint8_t y, uint8_t c);
void GLCD_refresh(void);


#endif

