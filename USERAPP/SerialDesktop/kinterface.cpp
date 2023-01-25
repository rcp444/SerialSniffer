#include "kinterface.h"

#define DEBUG

INT16 KernelInterface::IO_StartMon(std::string devPath)
{
    if (hDriver == INVALID_HANDLE_VALUE)
    {
        printf("No driver");
        return 0;
    }

    DWORD Bytes;

    std::wstring stemp = std::wstring(devPath.begin(), devPath.end());
    LPCWSTR sw = stemp.c_str();
    INT16 status;

    if (DeviceIoControl(hDriver, IO_START_MON, (LPVOID)sw, stemp.size() * sizeof(wchar_t), &status, sizeof(status), &Bytes, NULL))
    {
        return status;
    }

    return 0;
}

DWORD KernelInterface::IO_StopMon()
{
    if (hDriver == INVALID_HANDLE_VALUE)
    {
        printf("No driver");
        return 0;
    }

    DWORD Bytes;

    if (DeviceIoControl(hDriver, IO_STOP_MON, 0, 0, 0, 0, &Bytes, NULL))
    {
#ifdef DEBUG
        printf("Stopped Mon\n");
#endif // DEBUG      
    }

    return Bytes;
}

DWORD KernelInterface::IO_GetData()
{
    if (hDriver == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    DWORD Bytes;
    ioctlData_t dataPack;

    if (DeviceIoControl(hDriver, IO_GETDATA, &dataPack, sizeof(dataPack), &dataPack, sizeof(dataPack), &Bytes, NULL))
    {
        int emptyCall = wcscmp(dataPack.funcName, L"NODATA");
        if (emptyCall == 0)
        {
            return 0;
        }

        printData(dataPack);
    }

    return Bytes;
}

void printData(ioctlData_t dataPack) 
{
    printf("------------------------------------------------\n");
    printf("%ws %d\n", dataPack.funcName, dataPack.dataSize);
    if (wcscmp(dataPack.funcName, L"IRP_MJ_READ") == 0 || wcscmp(dataPack.funcName, L"IRP_MJ_WRITE") == 0)
    {
        printf("%s\n", dataPack.data);
    }
    else
    {
        if (dataPack.dataSize > 0)
            printf("L: %d\n", dataPack.dataSize);

        for (unsigned int i = 0; i < dataPack.dataSize; i++)
        {
            printf("[%02X] ", dataPack.data[i]);
        }
        if (dataPack.dataSize > 0)
            printf("\n");
    }
    printf("------------------------------------------------\n");
}