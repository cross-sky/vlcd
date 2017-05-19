#ifndef __RINGBUFF_H
#define __RINGBUFF_H

#include "commdata.h"

typedef struct _ringBuff{
	uint16_t in;
	uint16_t out;
	uint16_t len;
	uint16_t maxSize;
	uint8_t* addr;
}RingBuff_T, *P_ringBuff;

bool RingBuf_create(P_ringBuff ringBuf, uint8_t* addr, uint16_t maxSize);
bool RingBuf_destory(P_ringBuff ringBuf);

int16_t RingBuf_writeData(P_ringBuff ringBuf, uint8_t* src, uint16_t len);
bool RingBuf_readData(P_ringBuff ringBuf, uint8_t* dst, uint16_t len);
uint16_t RingBuf_capacity(P_ringBuff ringBuf);

int16_t RingBuf_writePosition(P_ringBuff ringBuf, uint8_t* src, uint16_t len);
bool RingBuf_readPosition(P_ringBuff ringBuf, uint16_t position, uint8_t* dst, uint16_t len);

#endif

