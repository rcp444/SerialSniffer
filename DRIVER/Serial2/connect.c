#include "connect.h"
#include "helpers.h"
#include "ext_dev.h"
#include "data.h"
#include "data_queue.h"

NTSTATUS connDispatchPass(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	UNREFERENCED_PARAMETER(deviceObject);
	return STATUS_SUCCESS;
}

NTSTATUS connCloseCall(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	KPrintInfoN("Connection with user application terminated!");
	
	if (extDevAttached == TRUE)
		extDetachDevice(deviceObject->DriverObject);

	queueClean();

	UNREFERENCED_PARAMETER(deviceObject);
	return STATUS_SUCCESS;
}

NTSTATUS connCreateCall(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	KPrintInfoN("Connection established with user app!");
	queueInitialize();

	UNREFERENCED_PARAMETER(deviceObject);
	return STATUS_SUCCESS;
}


NTSTATUS connCreateAttach(PDRIVER_OBJECT driverObject)
{
	NTSTATUS status = STATUS_SUCCESS;
	RtlInitUnicodeString(&dev, L"\\Device\\serialmondev");
	RtlInitUnicodeString(&dos, L"\\DosDevices\\serialmondev");

	status = IoCreateDevice(driverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &g_connDevice);
	if (!NT_SUCCESS(status))
	{
		KPrintErrN("Cannot create conn device!");
		return status;
	}

	status = IoCreateSymbolicLink(&dos, &dev); 
	if (!NT_SUCCESS(status))
	{
		KPrintErrN("Cannot create symbolic link for conn device!");
		return status;
	}

	return status;
}

NTSTATUS connDeviceControl(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(deviceObject);

	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG ByteIo = 0;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

	if (ControlCode == IO_START_MON)
	{
		UNICODE_STRING devicePath;
		RtlInitUnicodeString(&devicePath, (PCWSTR)Irp->AssociatedIrp.SystemBuffer);
		
		status = STATUS_SUCCESS;
		KPrintInfoN("Start mon from user application!");
		KPrintInfoN("Target: %wZ", &devicePath);
		ByteIo = 0;
		status = extAttachDeviceByPath(deviceObject->DriverObject, devicePath);


		/*Output buffer that wil be passed to application */
		PINT16 statusOutput = (PINT16)Irp->AssociatedIrp.SystemBuffer;
		*statusOutput = 0;

		if (!NT_SUCCESS(status))
		{
			*statusOutput = 2;
			KPrintErr("Unable to attach device to stack!!! QUITING!\n");
		}
		else
		{
			*statusOutput = 1;
			KPrintInfo("Successfully created and attached device.\n");
		}
		ByteIo = sizeof(*statusOutput);
	}
	else if (ControlCode == IO_STOP_MON)
	{
		status = STATUS_SUCCESS;
		KPrintInfoN("Stop mon from user application!");
		ByteIo = 0;
		extDetachDevice(deviceObject->DriverObject);
	}
	else if (ControlCode == IO_GETDATA)
	{
		ioctlData_t* Output = (ioctlData_t*)Irp->AssociatedIrp.SystemBuffer;
		queueNode_t* newData = queueGet();
		
		if (newData != NULL)
		{
			*Output = newData->data;
		}
		else
		{
			ioctlData_t mockup;
			WCHAR funcName[FUNC_NAME_MAX] = L"NODATA";
			RtlZeroMemory(&mockup, sizeof(mockup));
			RtlCopyMemory(mockup.funcName, funcName, FUNC_NAME_MAX);
			*Output = mockup;
		}
		
		status = STATUS_SUCCESS;
		ByteIo = sizeof(*Output);
	}
	else
	{
		ByteIo = 0;
	}


	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = ByteIo;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return status;
}