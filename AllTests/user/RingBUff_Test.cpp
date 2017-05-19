
#include <stdio.h>
#include <stdarg.h>
#include "CppUTest/TestHarness.h"

extern "C"
{
#include "ringbuff.h"
}

#define BUFF_SIZE 8
static uint8_t buff[BUFF_SIZE];
static RingBuff_T S_ringBuf;
static P_ringBuff Sp_ringBuf = &S_ringBuf;

TEST_GROUP(RingBuff_TEST)
{
void setup()
{
	RingBuf_create(Sp_ringBuf, buff, BUFF_SIZE);
}

void teardown()
{
	RingBuf_destory(Sp_ringBuf);
}
};

TEST(RingBuff_TEST, LedsOffAfterCreate)
{
	uint8_t src[3]={1, 2, 3};
	uint8_t dst[3];
	
	RingBuf_writeData(Sp_ringBuf, src, 3); //first write 1

	src[0] = 2;
	RingBuf_writeData(Sp_ringBuf, src, 3); //second write 2

	RingBuf_readData(Sp_ringBuf, dst, 3);  //read first 1
	LONGS_EQUAL(1, dst[0]);	

	src[0] = 3;
	RingBuf_writeData(Sp_ringBuf, src, 3); //third write 3

	RingBuf_readData(Sp_ringBuf, dst, 3);  //read second 2
	LONGS_EQUAL(2, dst[0]);

	RingBuf_readData(Sp_ringBuf, dst, 3);  //read third 3
	LONGS_EQUAL(3, dst[0]);

}
