#include <iostream>
#include <string>
#include <Windows.h>
#include <vector>
#include "kinterface.h"
#include <thread>

using namespace std;

typedef struct SerialEntry{
    int comNumber = -1;
    string path;
}SerialEntry_t;

vector<SerialEntry_t> getAvailablePorts()
{
    wchar_t lpTargetPath[5000];
    vector<SerialEntry_t> ports;

    for (int i = 0; i < 255; i++) // checking ports from COM0 to COM255
    {
        wstring str = L"COM" + to_wstring(i); 
        DWORD res = QueryDosDevice(str.c_str(), lpTargetPath, 5000);

        if (res != 0)
        {
            SerialEntry_t sr;
            wstring ws(lpTargetPath);
            sr.path = string(ws.begin(), ws.end());
            sr.comNumber = i;

            ports.push_back(sr);
        }
        if (::GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
        }
    }
    return ports;
}

void shortcutListener(bool* status)
{
    while (true)
    {
        if (GetAsyncKeyState(VK_F8) != 0)
        {
            system("cls");
            printf("==== Detached from target device =====\n");
            *status = true;
            break;
        }
    }
}

void listen(KernelInterface driver, bool* intStatus)
{
    while (*intStatus == false)
    {
        driver.IO_GetData();
        Sleep(10);
    }
}

int main()
{
    KernelInterface driver = KernelInterface("\\\\.\\serialmondev");   
    
    while (true)
    {
        printf("Select device: \n");

        int i = 0;
        vector<SerialEntry_t> ports = getAvailablePorts();
        for (SerialEntry_t l : ports)
        {
            printf("%d: COM%d %s\n", i, l.comNumber, l.path.c_str());
            i++;
        }

        int selectedEntry = -1;
        printf("Device index: ");
        int sstatus = scanf_s("%d", &selectedEntry);

        if (sstatus && selectedEntry >= 0 && selectedEntry < i)
        {
            printf("Selected port: COM%d\n", ports[selectedEntry].comNumber);
            /* Path to driver */
            INT16 status = driver.IO_StartMon(ports[selectedEntry].path);
            if (status == 1)
            {
                system("cls");
                printf("==== Attached to target device %s =====\n", ports[selectedEntry].path.c_str());
                printf("Press F8 to stop monitor\n");
                /* Thread listining for interupt shortcut currently F8*/
                bool intStatus = false;
                thread threadObj(shortcutListener, &intStatus);

                listen(driver, &intStatus);
                driver.IO_StopMon();
                threadObj.join();
            }
            else
            {
                system("cls");
                printf("\n ==== Cannot attach target device ===== \n");
            }
          
        }

    }
    
}