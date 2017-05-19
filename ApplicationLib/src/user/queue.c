#include "queue.h"
#include <stdlib.h>

bool QUE_push(QUELink_T *q, uint16_t addres_8u, uint16_t length)
{
	P_QUENode s = (P_QUENode)malloc(sizeof(QUENode_T));
	if (!s)
		return false;
	s->Addr_8U = addres_8u;
	s->length = length;
	s->Next = NULL;
	q->rear->Next = s;
	q->rear = s;
	return true;
}

bool QUE_getNode(QUELink_T *q, P_QUENode e)
{
	P_QUENode p;
	if(q->front == q->rear)
		return false;
	p = q->front->Next;
	e->Addr_8U = p->Addr_8U;
	e->length = p->length;

	return true;
}

bool QUE_popNode(QUELink_T *q, P_QUENode e)
{
	P_QUENode p;
	if(q->front == q->rear)
		return false;
	p = q->front->Next;
	e->Addr_8U = p->Addr_8U;
	e->length = p->length;

	q->front->Next = p->Next;
	if(q->rear == p)
		q->rear = q->front;
	free(p);
	return true;
}

QUELink_T* Queue_create(void)
{
	QUELink_T *q;
	P_QUENode p;

	q = (QUELink_T *)malloc(sizeof(QUELink_T));
	p = (P_QUENode)malloc(sizeof(QUENode_T));
	p->Next = NULL;
	q->front = q->rear = p;
	return q;
}

void Queue_destory(QUELink_T *link)
{
	//应该逐个释放里面的元素，@@@@@@
	if (link != NULL)
	{
		free(link->front);
		free(link);
	}
}

bool QUE_popData(QUELink_T *q, uint8_t *addr)
{
	return true;
}

