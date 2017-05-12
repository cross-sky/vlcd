
#include <stdio.h>
#include <stdarg.h>
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "uart1.h"
}

P_QUELink queLink;
QUENode_T node;
static RTRCFlag_T S_rcFlag={
	0, 3, DONE	
};

TEST_GROUP(UART1_TEST)
{
void setup()
{
	queLink = Queue_create();
}

void teardown()
{
	Queue_destory(queLink);
	S_rcFlag.recFlag = DONE;
	S_rcFlag.sendCount = 0;
	S_rcFlag.sendCountMax = 3;
}
};

TEST(UART1_TEST, sendData1)
{
	uint8_t dst[10];
	Uart1S_D1T dsrc;
	dsrc.data1 = 16;
	dsrc.funcode = 0xb1;

	Uart1_send(queLink, (uint8_t*)&dsrc, sizeof(Uart1S_D1T));
	QUE_popNode(queLink, &node);
	memcpy(dst, node.Addr_8U, node.length);
	LONGS_EQUAL(dsrc.data1, dst[4]);
}

TEST(UART1_TEST, popNothing)
{
	Uart1_Process(queLink, &S_rcFlag);
	LONGS_EQUAL(DONE, S_rcFlag.recFlag);
}

TEST(UART1_TEST, recNothing)
{
	Uart1S_D1T dsrc;
	dsrc.data1 = 16;
	dsrc.funcode = 0xb1;
	Uart1_send(queLink, (uint8_t*)&dsrc, sizeof(Uart1S_D1T));

	Uart1_Process(queLink, &S_rcFlag);
	LONGS_EQUAL(UNDONE, S_rcFlag.recFlag);
	Uart1_Process(queLink, &S_rcFlag);
	Uart1_Process(queLink, &S_rcFlag);
	LONGS_EQUAL(2, S_rcFlag.sendCount);
}

