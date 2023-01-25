#include "dist_ioctl.h"
#include "ext_dev.h"
#include "ext_ioctl.h"
#include "connect.h"

NTSTATUS distDeviceControl(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	if (deviceObject == g_extDevice)
	{
		/* External device */
		return extDeviceControl(deviceObject, Irp);
	}
	else
	{
		/* This device*/
		return connDeviceControl(deviceObject, Irp);
	}

}

NTSTATUS distDispatchPass(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	if (deviceObject == g_extDevice)
	{
		/* External device */
		return extDispatchPass(deviceObject, Irp);
	}
	else
	{
		/* This device*/
		return connDispatchPass(deviceObject, Irp);
	}
}

NTSTATUS distCreateCloseCall(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	PIO_STACK_LOCATION pStack;

	pStack = IoGetCurrentIrpStackLocation(Irp);

	if (deviceObject == g_extDevice)
	{
		/* External device */
		return extDispatchPass(deviceObject, Irp);
	}
	else
	{
		/* This device*/
		switch (pStack->MajorFunction)
		{
			case IRP_MJ_CREATE:
				return connCreateCall(deviceObject, Irp);
				break;

			case IRP_MJ_CLOSE:
				return connCloseCall(deviceObject, Irp);
				break;
			default:
				return connDispatchPass(deviceObject, Irp);
				break;
		}
	}
}