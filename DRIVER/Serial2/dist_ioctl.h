#pragma once
#include <ntddk.h>

/* Distribiution between devices */
NTSTATUS distDispatchPass(PDEVICE_OBJECT deviceObject, PIRP Irp);
NTSTATUS distDeviceControl(PDEVICE_OBJECT deviceObject, PIRP Irp);
NTSTATUS distCreateCloseCall(PDEVICE_OBJECT deviceObject, PIRP Irp);