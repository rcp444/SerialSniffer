#pragma once
#include <ntddk.h>
#include "data.h"
#include "helpers.h"

#define QUEUE_SIZE 100
#define QUEUE_TAG	(ULONG)'smon'

typedef struct queueNode_t queueNode_t;

struct queueNode_t
{
	ioctlData_t data;
	queueNode_t* next;
};

queueNode_t* pHead;
queueNode_t* pTail;
KSPIN_LOCK kSLock;
LONG queueSize;
extern BOOLEAN isQueueInitialized;

VOID queueInitialize();

BOOLEAN queueAdd(ioctlData_t inData);

queueNode_t* queueGet();

VOID queueDelete(queueNode_t* pItem);

VOID queueClean();