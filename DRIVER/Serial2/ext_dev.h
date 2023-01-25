#pragma once
#include <ntddk.h>

extern PDEVICE_OBJECT g_extDevice;
extern BOOLEAN extDevAttached;

typedef struct
{
	PDEVICE_OBJECT LowerDevice;
	/* This is used to know when we can remove device */
	IO_REMOVE_LOCK	ioRemLock;
}DEVICE_EXTENSION, * PDEVICE_EXTENSION;

NTSTATUS extAttachDevice(PDRIVER_OBJECT driverObject);

NTSTATUS extAttachDeviceByPath(PDRIVER_OBJECT driverObject, UNICODE_STRING targetDevice);

NTSTATUS extDetachDevice(PDRIVER_OBJECT driverObject);

