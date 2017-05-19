#include "cominc.h"

static uint8_t s_queDbuf[10]={0xfe, 0xef};

#define TXBUF_SIZE 40
static uint8_t s_dmaTxBuf[TXBUF_SIZE];

#define RXBUF_SIZE 40
static uint8_t s_dmaRxBuf[RXBUF_SIZE];

static RTRCFlag_T S_rcFlag={
	0, 3, DONE	
};

#define UART1_FRAME1 0xfe
#define UART1_FRAME2 0xef

#define Bounds	9600

//que and ringbuf
P_QUELink queLink;
QUENode_T node;
#define RING_BUFF_SIZE 50
static uint8_t Stab_ringbuff[RING_BUFF_SIZE];
static RingBuff_T S_ring;
static P_ringBuff Sp_ringBuf = &S_ring;

static void setHeader(P_Uart1header header, uint16_t len)
{
	header->frame1 = 0xfe;
	header->frame2 = 0xef;
	header->len = len + 1; // add len(1) + crc(1)
}

uint8_t Uart_checkXrr(void* addr, uint8_t len)
{
	uint8_t result = 0;
	uint8_t* src = (uint8_t*)addr;
	while(len > 0)
	{
		result ^= *src++;
		len--;
	}
	return result;
}

void Uart1_send(P_QUELink queLink, P_ringBuff ringBuf, uint8_t* dataddr, uint8_t dataLen)
{
	uint8_t *sendAddr;
	uint8_t crr;
	uint8_t queLen = sizeof(Uart1header_T) + dataLen + 1;
	uint16_t position;
	//create data 
	P_Uart1header header = (P_Uart1header)s_queDbuf;
	setHeader(header, sizeof(Uart1header_T));
	sendAddr = (s_queDbuf + sizeof(Uart1header_T));
	assert(dataddr);
	memcpy(sendAddr, dataddr, dataLen);
	crr = Uart_checkXrr(sendAddr - 1, dataLen + 1);
	sendAddr[dataLen] = crr;

	//push data to ringbuff
	position = RingBuf_writePosition(ringBuf, s_queDbuf, queLen);
	QUE_push(queLink, position, queLen);
}

static bool sendNext(P_RTRCFlag rcFlag)
{
	if (rcFlag->sendCount >= rcFlag->sendCountMax)
	{
		//recv err
		//reset rcvflag done
		rcFlag->recFlag = DONE;
		return true;
	}
	if (rcFlag->recFlag == DONE)
	{
		return true;
	}else
	{
		//not receive
		rcFlag->sendCount++;
		return false;
	}
}

void Uart1_DmaTxDataEnable(uint16_t len, uint8_t *address)
{
	DMA1_Ch_Usart1_Tx->CNDTR =len;	//
	DMA1_Ch_Usart1_Tx->CMAR =(uint32_t)&s_dmaTxBuf;
	DMA_Cmd(DMA1_Ch_Usart1_Tx,ENABLE);
}

void Uart1_Process(P_QUELink queLink, P_ringBuff ringBuf, P_RTRCFlag rcFlag)
{
	static QUENode_T node;
	assert(queLink);

	//1.send done or not
	if (!sendNext(rcFlag))
	{
		//resend
		//set dma addr at S_tDmaSend1
		Uart1_DmaTxDataEnable(node.length, s_dmaTxBuf);
		return;
	}

	//len == 0 return
	if (!QUE_popNode(queLink, &node))
	{
		return;
	}

	//2.set dma addr, len
	//3.set rcFlag->recFlag = UNDONE;
	//reset sendCount
	//memcpy(s_dmaSend, node.Addr_8U, node.length);
	//RingBuf_readData(Sp_ringBuf, s_dmaSend, node.length);
	RingBuf_readPosition(ringBuf, node.Addr_8U, s_dmaTxBuf, node.length);
	Uart1_DmaTxDataEnable(node.length, s_dmaTxBuf);
	rcFlag->recFlag = UNDONE;
	rcFlag->sendCount = 0;
}

void Uart1_setRecFlag()
{
	S_rcFlag.recFlag = DONE;
}

static uint8_t* puartGetRTxAddress(void)
{
	return s_dmaRxBuf;
}

static void hwUart1Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;
	DMA_InitTypeDef		DMA_InitStructure;
	NVIC_InitTypeDef	NVIC_InitStructure;
	USART_InitTypeDef	USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|USART1_Tx_GPIO_CLK,ENABLE);
	RCC_APB2PeriphClockCmd(USART1_Tx_CLK,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);

	// Configure USART_Rx as input floating 
	GPIO_InitStructure.GPIO_Pin = USART1_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART1_Rx_GPIO,&GPIO_InitStructure);

	// Configure USART_Tx as alternate function push-pull 
	GPIO_InitStructure.GPIO_Pin = USART1_TxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(USART1_Tx_GPIO,&GPIO_InitStructure);

	DMA_DeInit(DMA1_Ch_Usart1_Tx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_Tx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&s_dmaTxBuf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = TXBUF_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Ch_Usart1_Tx,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Ch_Usart1_Tx,DMA_IT_TC,ENABLE);
	DMA_ITConfig(DMA1_Ch_Usart1_Tx,DMA_IT_TE,ENABLE);
	USART_DMACmd(USART1_Tx, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Ch_Usart1_Tx,DISABLE);

	DMA_DeInit(DMA1_Ch_Usart1_Rx);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_Rx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)puartGetRTxAddress();
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = RXBUF_SIZE;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

	DMA_Init(DMA1_Ch_Usart1_Rx,&DMA_InitStructure);
	DMA_ITConfig(DMA1_Ch_Usart1_Rx,DMA_IT_TC,ENABLE);//transmit complete
	DMA_ITConfig(DMA1_Ch_Usart1_Rx,DMA_IT_TE,ENABLE);//transmit enable
	USART_DMACmd(USART1_Rx, USART_DMAReq_Rx, ENABLE);
	DMA_Cmd(DMA1_Ch_Usart1_Rx,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//Enable DMA Channel4 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Ch_Usart1_Tx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//Enable DMA Channel5 Interrupt 
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Ch_Usart1_Rx_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_InitStructure.USART_BaudRate = Bounds;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1_Tx, &USART_InitStructure);

	//空闲中断
	USART_ITConfig(USART1_Tx, USART_IT_IDLE , ENABLE);//
	USART_Cmd(USART1_Tx, ENABLE);
	USART_ClearFlag(USART1_Tx, USART_FLAG_TC);     //* 清发送外城标志，

}

void Uart1_init(void)
{
	queLink = Queue_create();
	RingBuf_create(Sp_ringBuf, Stab_ringbuff, RING_BUFF_SIZE);
	hwUart1Init();
}



void Usart1_IdlHandle_ISR(void)
{
	uint16_t DataLen;
	uint8_t* buff = puartGetRTxAddress();
	uint8_t i = 0;
	if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
	{
		DMA_Cmd(DMA1_Ch_Usart1_Rx, DISABLE);
		DataLen = RXBUF_SIZE - DMA_GetCurrDataCounter(DMA1_Ch_Usart1_Rx);
		//
		if (DataLen >0)
		{
			//RT_uartRxPush((uint8_t)DataLen, &uartRxProcess);
			while(DataLen>0)
			{
				if (buff[i] == UART1_FRAME1 && buff[i+1] == UART1_FRAME2)
				{
					break;
				}
				i++;
				DataLen--;
			}

			if (DataLen>0)
			{
				if (buff[i + 3] == 0xbf)
				{
					Uart1_setRecFlag();
				}
			}
		}
		DMA_ClearFlag(DMA1_FLAG_GL5|DMA1_FLAG_TC5|DMA1_FLAG_TE5|DMA1_FLAG_HT5);//清标志
		DMA1_Ch_Usart1_Rx->CNDTR =RXBUF_SIZE;	//
		DMA1_Ch_Usart1_Rx->CMAR =(uint32_t)puartGetRTxAddress();
		DMA_Cmd(DMA1_Ch_Usart1_Rx, ENABLE);//处理完,重开DMA
		//读SR后读DR清除Idle
		DataLen = USART1->SR;
		DataLen = USART1->DR;
	}
	if (USART_GetITStatus(USART1, USART_IT_PE | USART_IT_FE | USART_IT_NE) != RESET)
	{
		USART_ClearITPendingBit(USART1, USART_IT_PE | USART_IT_FE | USART_IT_NE);
	}
	USART_ClearITPendingBit(USART1, USART_IT_TC);
	USART_ClearITPendingBit(USART1, USART_IT_IDLE);
}

void Uart1_DmaTxHandler_ISR(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC4|DMA1_IT_TE4);
	DMA_Cmd(DMA1_Ch_Usart1_Tx, DISABLE);
	//vUart_setTxStateOn();
}

void Uart1_DmaRxHandle_ISR(void)
{
	DMA_ClearITPendingBit(DMA1_IT_TC5|DMA1_IT_TE5);
	DMA_Cmd(DMA1_Ch_Usart1_Rx, DISABLE);//关闭DMA,防止处理其间有数据, rx
	DMA1_Ch_Usart1_Rx->CNDTR =RXBUF_SIZE;	//
	DMA1_Ch_Usart1_Rx->CMAR =(uint32_t)puartGetRTxAddress();
	DMA_Cmd(DMA1_Ch_Usart1_Rx, ENABLE);
}

void Uart1_taskProces(void)
{
	Uart1_Process(queLink,Sp_ringBuf, &S_rcFlag);
}

void Uart1_sendCodeA(void)
{
	Uart1S_D1T dsrc;
	dsrc.data1 = Menu_getOnFlag();
	dsrc.funcode = 0xb1;
	Uart1_send(queLink, Sp_ringBuf, (uint8_t*)&dsrc, sizeof(Uart1S_D1T));
}
