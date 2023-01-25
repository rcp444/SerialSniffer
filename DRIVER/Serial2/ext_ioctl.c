#include "ext_ioctl.h"
#include "ext_dev.h"
#include "data_queue.h"
#include "helpers.h"

NTSTATUS extDispatchPass(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	IoCopyCurrentIrpStackLocationToNext(Irp);
	return IoCallDriver(((PDEVICE_EXTENSION)deviceObject->DeviceExtension)->LowerDevice, Irp);
}

NTSTATUS extSkipPass(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	IoSkipCurrentIrpStackLocation(Irp);
	return IoCallDriver(((PDEVICE_EXTENSION)deviceObject->DeviceExtension)->LowerDevice, Irp);
}

NTSTATUS extDeviceControl(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	PDEVICE_EXTENSION pDevExt = NULL;
	NTSTATUS status = STATUS_SUCCESS;

	pDevExt = (PDEVICE_EXTENSION)deviceObject->DeviceExtension;


	status = IoAcquireRemoveLock(&pDevExt->ioRemLock, (PVOID)Irp);
	
	if (!NT_SUCCESS(status)) {
		KPrintErr("Error acquire remove lock! %d", status);
		extSkipPass(deviceObject, Irp);
		return status;
	}

	PIO_STACK_LOCATION	pStack = NULL;
	pStack = IoGetCurrentIrpStackLocation(Irp);

	switch (pStack->Parameters.DeviceIoControl.IoControlCode) {
	case IOCTL_SERIAL_GET_BAUD_RATE:
		return extHandleGetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_GET_HANDFLOW:
		return extHandleGetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_GET_LINE_CONTROL:
		return extHandleGetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_GET_TIMEOUTS:
		return extHandleGetIoctl(deviceObject, Irp);
		break;

	case IOCTL_SERIAL_SET_BAUD_RATE:
		return extHandleSetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_SET_HANDFLOW:
		return extHandleSetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_SET_LINE_CONTROL:
		return extHandleSetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_SET_TIMEOUTS:
		return extHandleSetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_SET_QUEUE_SIZE:
		return extHandleSetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_CONFIG_SIZE:
		KPrintN("IOCTL_SERIAL_SERIAL_CONFIG_SIZE");
		//RtlInitUnicodeString(&usStrDat, L"IOCTL_SERIAL_CONFIG_SIZE");
		//DkTgtCompletePendedIrp(usStrDat.Buffer, usStrDat.Length, NULL, 0);
		break;


	case IOCTL_SERIAL_GET_CHARS:
		return extHandleGetIoctl(deviceObject, Irp);
		break;

	case IOCTL_SERIAL_SET_CHARS:
		return extHandleSetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_SET_DTR:
		return extHandleSetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_SET_RTS:
		return extHandleSetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_CLR_DTR:
		KPrintN("IOCTL_SERIAL_CLR_DTR");
		//RtlInitUnicodeString(&usStrDat, L"IOCTL_SERIAL_CLR_DTR");
		//DkTgtCompletePendedIrp(usStrDat.Buffer, usStrDat.Length, NULL, 0);
		break;


	case IOCTL_SERIAL_CLR_RTS:
		KPrintN("IOCTL_SERIAL_CLR_RTS");
		//RtlInitUnicodeString(&usStrDat, L"IOCTL_SERIAL_CLR_RTS");
		//DkTgtCompletePendedIrp(usStrDat.Buffer, usStrDat.Length, NULL, 0);
		break;


	case IOCTL_SERIAL_SET_WAIT_MASK:
		return extHandleSetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_GET_WAIT_MASK:
		return extHandleGetIoctl(deviceObject, Irp);
		break;


	case IOCTL_SERIAL_WAIT_ON_MASK:
		KPrintN("IOCTL_SERIAL_WAIT_ON_MASK");
		//RtlInitUnicodeString(&usStrDat, L"IOCTL_SERIAL_WAIT_ON_MASK");
		//DkTgtCompletePendedIrp(usStrDat.Buffer, usStrDat.Length, NULL, 0);
		break;

	case IOCTL_SERIAL_PURGE:
		KPrintN("IOCTL_SERIAL_PURGE");
		//RtlInitUnicodeString(&usStrDat, L"IOCTL_SERIAL_PURGE");
		//DkTgtCompletePendedIrp(usStrDat.Buffer, usStrDat.Length, NULL, 0);
		break;

	case IOCTL_SERIAL_GET_COMMSTATUS:
		KPrintN("IOCTL_SERIAL_GET_COMMSTATUS");
		//return DkTgtHandleIoCtlGetReq(pDevExt, pIrp);
		break;

	default:
		KPrintInfo("Unknown Ctl code! %x\n", pStack->Parameters.DeviceIoControl.IoControlCode);
		break;
	}

	IoReleaseRemoveLock(&pDevExt->ioRemLock, (PIRP)Irp);
	IoSkipCurrentIrpStackLocation(Irp);
	return IoCallDriver(((PDEVICE_EXTENSION)deviceObject->DeviceExtension)->LowerDevice, Irp);
}

NTSTATUS extDeviceRead(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	PDEVICE_EXTENSION pDevExt = NULL;
	NTSTATUS status = STATUS_SUCCESS;

	pDevExt = (PDEVICE_EXTENSION)deviceObject->DeviceExtension;
	status = IoAcquireRemoveLock(&pDevExt->ioRemLock, (PVOID)Irp);

	KPrintN("IRP_MJ_READ");

	IoCopyCurrentIrpStackLocationToNext(Irp);
	IoSetCompletionRoutine(Irp, (PIO_COMPLETION_ROUTINE) extDeviceReadCompletion, NULL, TRUE, TRUE, TRUE);
	return IoCallDriver(((PDEVICE_EXTENSION)deviceObject->DeviceExtension)->LowerDevice, Irp);

}

NTSTATUS extDeviceWrite(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	PDEVICE_EXTENSION pDevExt = NULL;
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION pStack;

	pDevExt = (PDEVICE_EXTENSION)deviceObject->DeviceExtension;
	status = IoAcquireRemoveLock(&pDevExt->ioRemLock, (PVOID)Irp);

	KPrintN("IRP_MJ_WRITE");

	pStack = IoGetCurrentIrpStackLocation(Irp);

	collectData(L"IRP_MJ_WRITE", pStack->Parameters.Write.Length, Irp->AssociatedIrp.SystemBuffer);

	IoReleaseRemoveLock(&pDevExt->ioRemLock, (PVOID)Irp);

	IoSkipCurrentIrpStackLocation(Irp);
	return IoCallDriver(((PDEVICE_EXTENSION)deviceObject->DeviceExtension)->LowerDevice, Irp);
}

NTSTATUS extDeviceReadCompletion(PDEVICE_OBJECT deviceObject, PIRP Irp, PVOID Context)
{
	PDEVICE_EXTENSION		pDevExt = NULL;
	PIO_STACK_LOCATION		pStack;

	if (Irp->PendingReturned)
		IoMarkIrpPending(Irp);

	pStack = IoGetCurrentIrpStackLocation(Irp);

	//KPrint("L: %d %s\r\n", pStack->Parameters.Read.Length,(char*)Irp->AssociatedIrp.SystemBuffer);	
	//KPrintN("END IRP_MJ_READ");
	collectData(L"IRP_MJ_READ", pStack->Parameters.Read.Length, Irp->AssociatedIrp.SystemBuffer);

	pDevExt = (PDEVICE_EXTENSION)g_extDevice->DeviceExtension;
	IoReleaseRemoveLock(&pDevExt->ioRemLock, (PVOID)Irp);

	return STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(deviceObject);
	UNREFERENCED_PARAMETER(Context);
}

NTSTATUS completePendedIrp(PDEVICE_OBJECT deviceObject, PIRP Irp, PVOID Context)
{
	PDEVICE_EXTENSION		pDevExt = NULL;
	PIO_STACK_LOCATION		pStack = NULL;
	
	// Does device exist?
	if (deviceObject == NULL)
		return STATUS_UNSUCCESSFUL;

	if (Irp->PendingReturned)
		IoMarkIrpPending(Irp);

	pStack = IoGetCurrentIrpStackLocation(Irp);
	/* Here we can acquire both function name and data from the irp call */
	ULONG ctrCode = pStack->Parameters.DeviceIoControl.IoControlCode;
	WCHAR* funcName = evaluteSerialFuncName(ctrCode);
	
	collectData(funcName, pStack->Parameters.Write.Length, Irp->AssociatedIrp.SystemBuffer);
	
	pDevExt = (PDEVICE_EXTENSION)g_extDevice->DeviceExtension;
	IoReleaseRemoveLock(&pDevExt->ioRemLock, (PVOID)Irp);
	
	return STATUS_SUCCESS;
	UNREFERENCED_PARAMETER(Context);
}

NTSTATUS extHandleGetIoctl(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	IoCopyCurrentIrpStackLocationToNext(Irp);
	IoSetCompletionRoutine(Irp, (PIO_COMPLETION_ROUTINE)completePendedIrp, NULL, TRUE, TRUE, TRUE);

	return IoCallDriver(((PDEVICE_EXTENSION)deviceObject->DeviceExtension)->LowerDevice, Irp);
}

NTSTATUS extHandleSetIoctl(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;
	PIO_STACK_LOCATION pStack = IoGetCurrentIrpStackLocation(Irp);
	
	PDEVICE_EXTENSION pDevExt = NULL;

	pDevExt = (PDEVICE_EXTENSION)deviceObject->DeviceExtension;
	status = IoAcquireRemoveLock(&pDevExt->ioRemLock, (PVOID)Irp);

	ULONG ctrCode = pStack->Parameters.DeviceIoControl.IoControlCode;
	WCHAR* funcName = evaluteSerialFuncName(ctrCode);

	collectData(funcName, pStack->Parameters.Write.Length, Irp->AssociatedIrp.SystemBuffer);

	IoReleaseRemoveLock(&pDevExt->ioRemLock, (PVOID)Irp);
	IoSkipCurrentIrpStackLocation(Irp);
	return IoCallDriver(((PDEVICE_EXTENSION)deviceObject->DeviceExtension)->LowerDevice, Irp);
}

VOID collectData(WCHAR funcName[FUNC_NAME_MAX], UINT32 dataSize, UCHAR data[DATA_SIZE_MAX])
{
	/* Creating new data pack that will be stored in queue*/
	ioctlData_t dataPack;
	RtlFillMemory(&dataPack, sizeof(ioctlData_t), '\0');
	
	RtlCopyMemory(dataPack.funcName, funcName, FUNC_NAME_MAX);
	dataPack.dataSize = dataSize;
	
	if (dataSize > 0 && dataSize <= DATA_SIZE_MAX)
	{
		RtlCopyMemory(dataPack.data, data, dataSize);
	}
	
	KPrintN("%ws", funcName);
	if (dataPack.dataSize > 0)
		KPrintN("L: %d", dataPack.dataSize);
	
	for (unsigned int i = 0; i < dataSize; i++)
	{
		KPrint("[%02X] ", dataPack.data[i]);
	}
	if (dataPack.dataSize > 0)
		KPrint("\n");
	
	queueAdd(dataPack);
}

WCHAR* evaluteSerialFuncName(ULONG IoControlCode)
{
	switch (IoControlCode) {
	case IOCTL_SERIAL_GET_BAUD_RATE:
		return L"IOCTL_SERIAL_GET_BAUD_RATE";
		break;


	case IOCTL_SERIAL_GET_HANDFLOW:
		return L"IOCTL_SERIAL_GET_HANDFLOW";
		break;


	case IOCTL_SERIAL_GET_LINE_CONTROL:
		return L"IOCTL_SERIAL_GET_LINE_CONTROL";
		break;


	case IOCTL_SERIAL_GET_TIMEOUTS:
		return L"IOCTL_SERIAL_GET_TIMEOUTS";
		break;

	case IOCTL_SERIAL_SET_BAUD_RATE:
		return L"IOCTL_SERIAL_SET_BAUD_RATE";
		break;


	case IOCTL_SERIAL_SET_HANDFLOW:
		return L"IOCTL_SERIAL_SET_HANDFLOW";
		break;

	case IOCTL_SERIAL_SET_LINE_CONTROL:
		return L"IOCTL_SERIAL_SET_LINE_CONTROL";
		break;


	case IOCTL_SERIAL_SET_TIMEOUTS:
		return L"IOCTL_SERIAL_SET_TIMEOUTS";
		break;


	case IOCTL_SERIAL_SET_QUEUE_SIZE:
		return L"IOCTL_SERIAL_SET_QUEUE_SIZE";
		break;


	case IOCTL_SERIAL_CONFIG_SIZE:
		return L"IOCTL_SERIAL_SERIAL_CONFIG_SIZE";
		break;


	case IOCTL_SERIAL_GET_CHARS:
		return L"IOCTL_SERIAL_GET_CHARS";
		break;

	case IOCTL_SERIAL_SET_CHARS:
		return L"IOCTL_SERIAL_SET_CHARS";
		break;


	case IOCTL_SERIAL_SET_DTR:
		return L"IOCTL_SERIAL_SET_DTR";
		break;


	case IOCTL_SERIAL_SET_RTS:
		return L"IOCTL_SERIAL_SET_RTS";
		break;


	case IOCTL_SERIAL_CLR_DTR:
		return L"IOCTL_SERIAL_CLR_DTR";
		break;


	case IOCTL_SERIAL_CLR_RTS:
		return L"IOCTL_SERIAL_CLR_RTS";
		break;


	case IOCTL_SERIAL_SET_WAIT_MASK:
		return L"IOCTL_SERIAL_SET_WAIT_MASK";
		break;


	case IOCTL_SERIAL_GET_WAIT_MASK:
		return L"IOCTL_SERIAL_GET_WAIT_MASK";
		break;


	case IOCTL_SERIAL_WAIT_ON_MASK:
		return L"IOCTL_SERIAL_WAIT_ON_MASK";
		break;

	case IOCTL_SERIAL_PURGE:
		return L"IOCTL_SERIAL_PURGE";
		break;

	case IOCTL_SERIAL_GET_COMMSTATUS:
		return L"IOCTL_SERIAL_GET_COMMSTATUS";
		break;

	default:
		return L"UKNOWN_CTL";
		break;
	}
}
