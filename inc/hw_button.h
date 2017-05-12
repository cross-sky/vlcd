#ifndef __HW_BUTTON_H
#define __HW_BUTTON_H

#include "stm32f10x.h"
#include "commdata.h"

typedef enum{
	BTN1_SHUT=0,	//k1
	BTN2_DOWN,		//k2
	BTN3_UP,		//k3
	BTN4,			//k4
	BTN5,			//k5
	BTN_MAX
}BTNSF_ENUM;

typedef enum
{
	BTN_KEY_NULL = 0,	
	BTN_KEY_EXIT =	0x0001,	//	
	BTN_KEY_DOWN =	0x0002,	//	
	BTN_KEY_UP =	0x0004,	//
	BTN_KEY_SHUT =	0x1001,	// long key
}BUTTON_STATUS_t;

typedef struct{
	BTNSF_ENUM sfPin;
//	GPIO_TypeDef* port;
	uint16_t hwPin;
//	uint16_t sfPin;
}BTNHW_T;

#define KeyBuffLen 8

typedef struct 
{
	uint16_t buff[KeyBuffLen];
	uint8_t in;
	uint8_t out;
}BTNKEY_t;

uint16_t BTN_getInput(void);
uint16_t BTN_softProcess(uint16_t hwValue);
uint16_t BTN_getLongKey(void);
bool BTN_KeyPush(uint16_t keyValue);
uint16_t BTN_KeyPop(void);

void BTN_destory(void);
void BTN_create(void);

void BTN_taskCheckKey(void);
void BTN_init(void);
#endif

