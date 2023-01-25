#pragma once
#include <ntddk.h>
#include <ntddser.h>
#include "data.h"

typedef enum{
	IOCTL_GET,
	IOCTL_SET
} EIOCTL_TYPE;

NTSTATUS extDispatchPass(PDEVICE_OBJECT deviceObject, PIRP Irp);
NTSTATUS extDeviceControl(PDEVICE_OBJECT deviceObject, PIRP Irp);

NTSTATUS extDeviceRead(PDEVICE_OBJECT deviceObject, PIRP Irp);
NTSTATUS extDeviceReadCompletion(PDEVICE_OBJECT deviceObject, PIRP Irp, PVOID Context);

NTSTATUS extDeviceWrite(PDEVICE_OBJECT deviceObject, PIRP Irp);

NTSTATUS extHandleGetIoctl(PDEVICE_OBJECT deviceObject, PIRP Irp);
NTSTATUS extHandleSetIoctl(PDEVICE_OBJECT deviceObject, PIRP Irp);

VOID collectData(
	WCHAR funcName[FUNC_NAME_MAX],
	UINT32 dataSize,
	UCHAR data[DATA_SIZE_MAX]);

WCHAR* evaluteSerialFuncName(ULONG IoControlCode);