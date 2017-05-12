#ifndef __UART1_H
#define __UART1_H

#include "commdata.h"
#include "queue.h"

typedef enum{
	BIT_FRAME1 = 0,
	BIT_FRAME2,
	BIT_LEN,
}BIT_ENUM;

typedef struct _RTSendFlagStruct{
	uint8_t sendCount;
	uint8_t sendCountMax;
	uint8_t recFlag;
}RTRCFlag_T, *P_RTRCFlag;

typedef struct{
	uint8_t frame1;
	uint8_t frame2;
	uint8_t len;
}Uart1header_T, *P_Uart1header;

typedef struct{
	uint8_t funcode;
	uint8_t data1;
}Uart1S_D1T, *P_Uart1S_D1;

void Uart1_send(P_QUELink queLink,uint8_t* dataddr, uint8_t dataLen);
uint8_t Uart_checkXrr(void* addr, uint8_t len);
void Uart1_Process(P_QUELink queLink,P_RTRCFlag rcFlag);



#endif
