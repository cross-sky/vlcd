#include "cominc.h"

static uint8_t s_sendTabC1[10]={0xfe, 0xef};
static uint8_t s_dmaSend[40];

static RTRCFlag_T S_rcFlag={
	0, 3, DONE	
};

static void setHeader(P_Uart1header header, uint16_t len)
{
	header->frame1 = 0xfe;
	header->frame2 = 0xef;
	header->len = len + 2; // add len(1) + crc(1)
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


void Uart1_send(P_QUELink queLink,uint8_t* dataddr, uint8_t dataLen)
{
	uint8_t *sendAddr;
	uint8_t crr;
	uint8_t queLen = sizeof(Uart1header_T) + dataLen + 1;
	P_Uart1header header = (P_Uart1header)s_sendTabC1;
	setHeader(header, sizeof(Uart1header_T));
	sendAddr = (s_sendTabC1 + sizeof(Uart1header_T));

	assert(dataddr);
	memcpy(sendAddr, dataddr, dataLen);
	crr = Uart_checkXrr(sendAddr - 1, dataLen + 1);
	sendAddr[dataLen] = crr;
	QUE_push(queLink, (uint8_t*)header, queLen);
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

void Uart1_Process(P_QUELink queLink,P_RTRCFlag rcFlag)
{
	static QUENode_T node;
	assert(queLink);

	//1.send done or not
	if (!sendNext(rcFlag))
	{
		//resend
		//set dma addr at S_tDmaSend1
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
	memcpy(s_dmaSend, node.Addr_8U, node.length);
	rcFlag->recFlag = UNDONE;
	rcFlag->sendCount = 0;
}

void Uart1_setRecFlag()
{
	S_rcFlag.recFlag = DONE;
}
