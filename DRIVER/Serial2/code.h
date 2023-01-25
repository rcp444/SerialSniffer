#pragma once
#include <ntddk.h>
#include <ntddkbd.h>
#include <ntddser.h>

VOID UnloadDriver(PDRIVER_OBJECT driverObject);
NTSTATUS DriverEntry(IN PDRIVER_OBJECT driverObject, IN PUNICODE_STRING regPath);
