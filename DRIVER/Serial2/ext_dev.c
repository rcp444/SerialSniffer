#include "ext_dev.h"
#include "helpers.h"

PDEVICE_OBJECT g_extDevice = NULL;
BOOLEAN extDevAttached = FALSE;

NTSTATUS extAttachDeviceByPath(PDRIVER_OBJECT driverObject, UNICODE_STRING targetDevice)
{
	NTSTATUS status;
	status = IoCreateDevice(driverObject, sizeof(DEVICE_EXTENSION), NULL, FILE_DEVICE_KEYBOARD, 0, FALSE, &g_extDevice);

	if (!NT_SUCCESS(status))
	{
		KPrintErr("Unable to create target device object!\n");
		return status;
	}
	else
	{
		KPrintInfo("Successfully created target device.\n");
	}

	g_extDevice->Flags |= DO_BUFFERED_IO;
	g_extDevice->Flags &= ~DO_DEVICE_INITIALIZING;

	/* Zero'ing device extension */
	RtlZeroMemory(g_extDevice->DeviceExtension, sizeof(DEVICE_EXTENSION));

	/* Attaching device to drivers stack (top of the stack) */
	status = IoAttachDevice(g_extDevice, &targetDevice, &((PDEVICE_EXTENSION)g_extDevice->DeviceExtension)->LowerDevice);

	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(g_extDevice);
		return status;
	}

	extDevAttached = TRUE;
	/* Initializing remove lock*/
	IoInitializeRemoveLock(&((PDEVICE_EXTENSION)g_extDevice->DeviceExtension)->ioRemLock, 0, 0, 0);

	return STATUS_SUCCESS;

}



NTSTATUS extDetachDevice(PDRIVER_OBJECT driverObject)
{
	PDEVICE_OBJECT Device = driverObject->DeviceObject;
	if (Device != NULL && ((PDEVICE_EXTENSION)Device->DeviceExtension) != NULL && ((PDEVICE_EXTENSION)Device->DeviceExtension)->LowerDevice != NULL)
		IoDetachDevice(((PDEVICE_EXTENSION)Device->DeviceExtension)->LowerDevice);

	if (g_extDevice != NULL)
		IoDeleteDevice(g_extDevice);

	extDevAttached = FALSE;
	return STATUS_SUCCESS;
}
