#pragma once
#include <ntddk.h>

#define FUNC_NAME_MAX 100
#define DATA_SIZE_MAX 512

typedef struct ioctlData
{
	WCHAR funcName[FUNC_NAME_MAX];
	UINT32 dataSize;
	UCHAR data[DATA_SIZE_MAX];

}ioctlData_t;