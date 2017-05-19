#ifndef __UART1_H
#define __UART1_H

#include "commdata.h"
#include "queue.h"

#define USART1_Rx				USART1
#define USART1_Rx_GPIO			GPIOA
#define USART1_Rx_CLK			RCC_APB2Periph_USART1
#define USART1_Rx_GPIO_CLK		RCC_APB2Periph_GPIOA
#define USART1_RxPin			GPIO_Pin_10

#define USART1_Tx				USART1
#define USART1_Tx_GPIO			GPIOA
#define USART1_Tx_CLK			RCC_APB2Periph_USART1
#define USART1_Tx_GPIO_CLK		RCC_APB2Periph_GPIOA
#define USART1_TxPin			GPIO_Pin_9
//这是一个双向寄存器，包含了TDR和RDR，对它读操作，读取的是RDR寄存器的值，对它的写操作，实际上是写到TDR寄存器的
#define USART1_Tx_DR_Base		0x40013804
#define DMA1_Ch_Usart1_Tx		DMA1_Channel4
#define DMA1_Ch_Usart1_Tx_IRQn	DMA1_Channel4_IRQn

#define USART1_Rx_DR_Base		0x40013804
#define DMA1_Ch_Usart1_Rx		DMA1_Channel5
#define DMA1_Ch_Usart1_Rx_IRQn	DMA1_Channel5_IRQn

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

void Uart1_send(P_QUELink queLink, P_ringBuff ringBuf,uint8_t* dataddr, uint8_t dataLen);
uint8_t Uart_checkXrr(void* addr, uint8_t len);
void Uart1_Process(P_QUELink queLink, P_ringBuff ringBuf, P_RTRCFlag rcFlag);

void Uart1_DmaTxDataEnable(uint16_t len, uint8_t *address);
void Uart1_taskProces(void);
void Uart1_init(void);

void Uart1_sendCodeA(void);

void Uart1_DmaRxHandle_ISR(void);
void Uart1_DmaTxHandler_ISR(void);
void Usart1_IdlHandle_ISR(void);

#endif
