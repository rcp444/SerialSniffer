#include "data_queue.h"

BOOLEAN isQueueInitialized = FALSE;

VOID queueInitialize()
{
	pHead = pTail = NULL;
	queueSize = 0;
	KeInitializeSpinLock(&kSLock);
	isQueueInitialized = TRUE;
}


BOOLEAN queueAdd(ioctlData_t inData)
{
	if (isQueueInitialized == FALSE)
	{
		return FALSE;
	}

	queueNode_t* pNew = NULL;
	KIRQL kIrql, curIrql;

	curIrql = KeGetCurrentIrql();
	if (curIrql > DISPATCH_LEVEL) {
		KPrintErr("Current IRQL != spinlock IRQL");
		return FALSE;
	}

	KeAcquireSpinLock(&kSLock, &kIrql);

	if (queueSize > QUEUE_SIZE) {
		KeReleaseSpinLock(&kSLock, kIrql);
		KPrintErrN("Queue too long!");
		return FALSE;
	}

	pNew = (queueNode_t*)ExAllocatePool2( POOL_FLAG_NON_PAGED, sizeof(queueNode_t), QUEUE_TAG);
	if (pNew == NULL) {
		KeReleaseSpinLock(&kSLock, kIrql);
		KPrintErrN("Cannot allocate new queue node!");
		return FALSE;
	}

	RtlFillMemory(pNew, sizeof(queueNode_t), '\0');
	RtlCopyMemory(&pNew->data, &inData, sizeof(queueNode_t));

	pNew->next = NULL;

	if (pHead == NULL) {
		pHead = pNew;
		pTail = pNew;
	}
	else {
		pTail->next = pNew;
		pTail = pNew;
	}
	queueSize++;

	KeReleaseSpinLock(&kSLock, kIrql);
	return TRUE;
}

queueNode_t* queueGet()
{
	queueNode_t* pRet = NULL;
	KIRQL kIrql;

	KeAcquireSpinLock(&kSLock, &kIrql);
	pRet = pHead;
	if (pRet != NULL) {
		pHead = pRet->next;
		queueSize--;
	}
	else {
		pTail = pHead;
	}
	KeReleaseSpinLock(&kSLock, kIrql);

	return pRet;
}

VOID queueDelete(queueNode_t* node)
{
	if (node == NULL)
		return;

	ExFreePoolWithTag((PVOID)node, QUEUE_TAG);
}

VOID queueClean()
{
	queueNode_t* node = queueGet();
	isQueueInitialized = FALSE;

	while (node != NULL)
	{
		queueDelete(node);
		node = queueGet();
	}
}
