#include "ringbuff.h"

bool RingBuf_create(P_ringBuff ringBuf, uint8_t* addr, uint16_t maxSize)
{
	ringBuf->in = 0;
	ringBuf->out = 0;
	ringBuf->len = 0;
	ringBuf->maxSize = maxSize;
	ringBuf->addr = addr;
	return true;
}
bool RingBuf_destory(P_ringBuff ringBuf)
{
	ringBuf->in = 0;
	ringBuf->out = 0;
	ringBuf->len = 0;
	ringBuf->maxSize = 0;
	ringBuf->addr = NULL;
	return true;
}

static uint16_t nextDataHandle(P_ringBuff ringBuf, uint16_t in)
{
	return (in + 1) >= ringBuf->maxSize ? 0 : in + 1;
}

int16_t RingBuf_writeData(P_ringBuff ringBuf, uint8_t* src, uint16_t len)
{
	
	uint16_t i;
	uint16_t position = ringBuf->in;
	if (len > RingBuf_capacity(ringBuf))
	{
		return -1;
	}
	for (i = 0; i < len; i++)
	{
		ringBuf->addr[ringBuf->in] = src[i];
		ringBuf->in = nextDataHandle(ringBuf, ringBuf->in);
		ringBuf->len++;
	}
	return position;
}
bool RingBuf_readData(P_ringBuff ringBuf, uint8_t* dst, uint16_t len)
{
	uint16_t i;
	if (len > ringBuf->len)
	{
		return false;
	}
	for (i = 0; i < len; i++)
	{
		dst[i]= ringBuf->addr[ringBuf->out];
		ringBuf->out = nextDataHandle(ringBuf, ringBuf->out);
		ringBuf->len--;
	}
	return true;
}
uint16_t RingBuf_capacity(P_ringBuff ringBuf)
{
	return ringBuf->maxSize - ringBuf->len;
}

int16_t RingBuf_writePosition(P_ringBuff ringBuf, uint8_t* src, uint16_t len)
{
	uint16_t i;
	uint16_t position = ringBuf->in;
	if (len > RingBuf_capacity(ringBuf))
	{
		return -1;
	}
	for (i = 0; i < len; i++)
	{
		ringBuf->addr[ringBuf->in] = src[i];
		ringBuf->in = nextDataHandle(ringBuf, ringBuf->in);
		ringBuf->len++;
	}
	return position;
}
bool RingBuf_readPosition(P_ringBuff ringBuf, uint16_t position, uint8_t* dst, uint16_t len)
{
	uint16_t i;
	uint16_t t = position;
	if (len > ringBuf->len)
	{
		return false;
	}
	for (i = 0; i < len; i++)
	{
		dst[i]= ringBuf->addr[t];
		t = nextDataHandle(ringBuf, t);
		ringBuf->len--;
	}
	return true;
}
