#ifndef __QUEUE_H
#define __QUEUE_H

#include "commdata.h"
#include "ringbuff.h"

typedef struct QUENode_T{
	uint16_t		Addr_8U;
	uint16_t		length;
	struct	QUENode_T *Next;
}QUENode_T, *P_QUENode;

//typedef struct{
//	P_QUENode front, rear;
//}QUELink_T, *P_QUELink;
typedef struct{
	P_QUENode front, rear;
	//P_ringBuff Sp_ringBuf;
}QUELink_T, *P_QUELink;


bool QUE_push(QUELink_T *q, uint16_t addres_8u, uint16_t length);
bool QUE_getNode(QUELink_T *q, P_QUENode e);
bool QUE_popNode(QUELink_T *q, P_QUENode e);
QUELink_T* Queue_create(void);
void Queue_destory(QUELink_T *link);


#endif
