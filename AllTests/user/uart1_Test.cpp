
#include <stdio.h>
#include <stdarg.h>
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "cominc.h"
}

P_QUELink queLink;
QUENode_T node;
static RTRCFlag_T S_rcFlag={
	0, 3, DONE	
};

#define BUFF_SIZE 40
static uint8_t buff[BUFF_SIZE];
static RingBuff_T S_ringBuf;
static P_ringBuff Sp_ringBuf = &S_ringBuf;

TEST_GROUP(UART1_TEST)
{
void setup()
{
	queLink = Queue_create();
	RingBuf_create(Sp_ringBuf, buff, BUFF_SIZE);
}

void teardown()
{
	Queue_destory(queLink);
	S_rcFlag.recFlag = DONE;
	S_rcFlag.sendCount = 0;
	S_rcFlag.sendCountMax = 3;
	RingBuf_destory(Sp_ringBuf);
}
};

TEST(UART1_TEST, sendData1)
{
	uint8_t dst[10];
	Uart1S_D1T dsrc[2];
	dsrc[0].data1 = 16;
	dsrc[0].funcode = 0xb1;
	Uart1_send(queLink, Sp_ringBuf, (uint8_t*)&dsrc[0], sizeof(Uart1S_D1T));
	dsrc[1].data1 = 17;
	dsrc[1].funcode = 0xb1;
	Uart1_send(queLink, Sp_ringBuf, (uint8_t*)&dsrc[1], sizeof(Uart1S_D1T));

	QUE_popNode(queLink, &node);
	RingBuf_readPosition(Sp_ringBuf, node.Addr_8U, dst, node.length);
	LONGS_EQUAL(16, dst[4]);
	QUE_popNode(queLink, &node);
	RingBuf_readPosition(Sp_ringBuf, node.Addr_8U, dst, node.length);
	LONGS_EQUAL(17, dst[4]);
}

TEST(UART1_TEST, popNothing)
{
	Uart1_Process(queLink, Sp_ringBuf, &S_rcFlag);
	LONGS_EQUAL(DONE, S_rcFlag.recFlag);
}

TEST(UART1_TEST, recNothing)
{
	Uart1S_D1T dsrc;
	dsrc.data1 = 16;
	dsrc.funcode = 0xb1;
	Uart1_send(queLink, Sp_ringBuf, (uint8_t*)&dsrc, sizeof(Uart1S_D1T));

	Uart1_Process(queLink,Sp_ringBuf,  &S_rcFlag);
	LONGS_EQUAL(UNDONE, S_rcFlag.recFlag);
	Uart1_Process(queLink,Sp_ringBuf, &S_rcFlag);
	Uart1_Process(queLink,Sp_ringBuf, &S_rcFlag);
	LONGS_EQUAL(2, S_rcFlag.sendCount);
}

