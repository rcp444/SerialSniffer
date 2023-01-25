#pragma once

#include "connect.h"
#include <stdio.h>
#include <string>

#define FUNC_NAME_MAX 100
#define DATA_SIZE_MAX 512

typedef struct ioctlData
{
    WCHAR funcName[FUNC_NAME_MAX];
    UINT32 dataSize;
    UCHAR data[DATA_SIZE_MAX];

}ioctlData_t;

void printData(ioctlData_t);

class KernelInterface
{
public:
    HANDLE hDriver;

    KernelInterface(LPCSTR RegistryPath)
    {
        hDriver = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    }

    INT16 IO_StartMon(std::string devPath);

    DWORD IO_StopMon();

    DWORD IO_GetData();
};