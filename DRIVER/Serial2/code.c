#include "helpers.h"
#include "ext_dev.h"
#include "dist_ioctl.h"
#include "ext_ioctl.h"
#include "data_queue.h"
#include "connect.h"


VOID UnloadDriver(PDRIVER_OBJECT driverObject)
{
	KPrintInfo("=== Unloading driver! ===\n");

	/* Unlink and delete conn device */
	IoDeleteSymbolicLink(&dos);
	
	if(g_connDevice != NULL)
		IoDeleteDevice(g_connDevice);

	/* Detach and delete external device */
	if (extDevAttached == TRUE)
	{
		extDetachDevice(driverObject);
	}
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT driverObject, IN PUNICODE_STRING regPath)
{
	NTSTATUS status;
	driverObject->DriverUnload = UnloadDriver;

	KPrintInfo("=== Loading driver ===\n");

	for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		driverObject->MajorFunction[i] = distDispatchPass;
	}

	/* Creating connection device for user application */
	status = connCreateAttach(driverObject);
	if (!NT_SUCCESS(status))
	{
		KPrintErrN("Problem with conn device creation!");
	}
	else
	{
		KPrintInfoN("Successfully created conn device.");
	}

	/* User application connection open and close */
	driverObject->MajorFunction[IRP_MJ_CREATE] = distCreateCloseCall;
	driverObject->MajorFunction[IRP_MJ_CLOSE] = distCreateCloseCall;
	driverObject->MajorFunction[IRP_MJ_CLEANUP] = distDispatchPass;

	/* External device controls filter */
	driverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = distDeviceControl;
	driverObject->MajorFunction[IRP_MJ_READ] = extDeviceRead;
	driverObject->MajorFunction[IRP_MJ_WRITE] = extDeviceWrite;
		
	UNREFERENCED_PARAMETER(regPath);

	return status;
}