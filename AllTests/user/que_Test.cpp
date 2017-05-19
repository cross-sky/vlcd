
#include <stdio.h>
#include <stdarg.h>
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "queue.h"
}

TEST_GROUP(QUE_TEST)
{
void setup()
{

}

void teardown()
{

}
};

TEST(QUE_TEST, pushAndPop)
{
	uint16_t src = 16, dst=0;
	QUENode_T tnode;
	P_QUELink queLink = Queue_create();
	QUE_push(queLink, src, sizeof(uint16_t));
	QUE_popNode(queLink, &tnode);
	//memcpy(&dst, tnode.Addr_8U, tnode.length);
	LONGS_EQUAL(src, tnode.Addr_8U);
	Queue_destory(queLink);
}
