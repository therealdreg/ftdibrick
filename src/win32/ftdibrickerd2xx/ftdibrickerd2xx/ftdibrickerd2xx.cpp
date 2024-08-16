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

#include <windows.h>

#include <stdio.h>
#include <tchar.h>

#include "ftd2xx.h"

int intmain(int argc, char* argv[])
{
    FT_HANDLE fthandle = 0;
    FT_STATUS status = 0;
    BOOLEAN Dev_Found = FALSE;
    FT_PROGRAM_DATA ftData = { 0 };

    WORD VendorIdBuf = 0x0403;
    WORD ProductIdBuf = 0x6001;
    char ManufacturerBuf[32];
    char ManufacturerIdBuf[16];
    char DescriptionBuf[64];
    char SerialNumberBuf[16];

    ftData.Signature1 = 0x00000000;		// Always 0x00000000
    ftData.Signature2 = 0xffffffff;		// Always 0xFFFFFFFF
    ftData.Version = 2;	// Header - FT_PROGRAM_DATA version 0 = original (FT232B), 1 = FT2232 extensions, 2 = FT232R extensions, 3 = FT2232H extensions, 4 = FT4232H extensions, 5 = FT232H extensions

    ftData.VendorId = VendorIdBuf;
    ftData.ProductId = ProductIdBuf;
    ftData.Manufacturer = ManufacturerBuf;
    ftData.ManufacturerId = ManufacturerIdBuf;
    ftData.Description = DescriptionBuf;
    ftData.SerialNumber = SerialNumberBuf;

    ftData.MaxPower;
    ftData.PnP;
    ftData.SelfPowered;
    ftData.RemoteWakeup;
    //'R features require section below
    ftData.UseExtOsc; // Use External Oscillator
    ftData.HighDriveIOs; // High Drive I/Os
    ftData.EndpointSize; // Endpoint size
    ftData.PullDownEnableR; // non-zero if pull down enabled
    ftData.SerNumEnableR; // non-zero if serial number to be used
    ftData.InvertTXD; // non-zero if invert TXD
    ftData.InvertRXD; // non-zero if invert RXD
    ftData.InvertRTS; // non-zero if invert RTS
    ftData.InvertCTS; // non-zero if invert CTS
    ftData.InvertDTR; // non-zero if invert DTR
    ftData.InvertDSR; // non-zero if invert DSR
    ftData.InvertDCD; // non-zero if invert DCD
    ftData.InvertRI; // non-zero if invert RI
    ftData.Cbus0; // Cbus Mux control
    ftData.Cbus1; // Cbus Mux control
    ftData.Cbus2; // Cbus Mux control
    ftData.Cbus3; // Cbus Mux control
    ftData.Cbus4; // Cbus Mux control
    ftData.RIsD2XX; // non-zero if using D2XX drivers

    FT_DEVICE_LIST_INFO_NODE *devInfo = NULL;
    DWORD numDevs = 0;
    DWORD i = 0;

    status = FT_CreateDeviceInfoList(&numDevs);

    if (status != FT_OK)
    {
        printf("FT_CreateDeviceInfoList status not ok %d\n", status);
        return 0;
    }
    else
    {
        printf("Number of devices is %d\n", numDevs);
        if (numDevs > 0)
        {
            devInfo =
                (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*numDevs);
            status = FT_GetDeviceInfoList(devInfo, &numDevs);
            if (status == FT_OK)
            {
                for (i = 0; i < numDevs; i++)
                {
                    printf("Dev %d:\n", i);
                    printf("Flags=0x%x\n", devInfo[i].Flags);
                    printf("Type=0x%x\n", devInfo[i].Type);
                    printf("ID=0x%x\n", devInfo[i].ID);
                    printf("LocId=0x%x\n", devInfo[i].LocId);
                    printf("SerialNumber=%s\n", devInfo[i].SerialNumber);
                    printf("Description=%s\n", devInfo[i].Description);
                    printf("\n");
                }
            }
        }
    }

    for (i = 0; i < numDevs; i++)
    {
        if ((Dev_Found == FALSE) && (devInfo[i].Type == FT_DEVICE_232R))
        {
            Dev_Found = TRUE;

            status = FT_OpenEx("USB <-> Serial Converter", FT_OPEN_BY_DESCRIPTION, &fthandle);

            if (status != FT_OK)
            {
                printf("Open status not ok %d\n", status);
                printf("Trying to open default EEPROM device...\n");
                status = FT_OpenEx("FT232R USB UART", FT_OPEN_BY_DESCRIPTION, &fthandle);
                if (status != FT_OK)
                {
                    printf("Open status not ok %d\n", status);
                    printf("\n");
                    return 0;
                }
                else
                {
                    printf("Open status OK %d\n", status);
                    printf("\n");
                }

                printf("\n");
            }
            else
            {
                printf("Open status OK %d\n", status);
                printf("\n");
            }

            status = FT_EE_Read(fthandle, &ftData);

            if (status != FT_OK)
            {
                printf("EE_Read status not ok %d\n", status);
                return 0;
            }
            else
            {
                printf("Signature1 =  0x%04x\n", ftData.Signature1);
                printf("Signature2 =  0x%04x\n", ftData.Signature2);
                printf("Version =  0x%04x\n", ftData.Version);


                printf("VendorID =  0x%04x\n", ftData.VendorId);
                printf("ProductID =  0x%04x\n", ftData.ProductId);
                printf("Manufacturer =  %s\n", ftData.Manufacturer);
                printf("ManufacturerID =  %s\n", ftData.ManufacturerId);
                printf("Description =  %s\n", ftData.Description);
                printf("SerialNumber =  %s\n", ftData.SerialNumber);
                printf("MaxPower =  %d\n", ftData.MaxPower);
                printf("PnP =  %x\n", ftData.PnP);
                printf("SelfPowered =  %x\n", ftData.SelfPowered);
                printf("RemoteWakeup =  %x\n", ftData.RemoteWakeup);

                printf("UseExtOsc =  %x\n", ftData.UseExtOsc);
                printf("HighDriveIOs =  %x\n", ftData.HighDriveIOs);
                printf("EndpointSize =  %x\n", ftData.EndpointSize);
                printf("PullDownEnableR =  %x\n", ftData.PullDownEnableR);
                printf("SerNumEnableR =  %x\n", ftData.SerNumEnableR);
                printf("InvertTXD =  %x\n", ftData.InvertTXD);
                printf("InvertRXD =  %x\n", ftData.InvertRXD);
                printf("InvertRTS =  %x\n", ftData.InvertRTS);
                printf("InvertCTS =  %x\n", ftData.InvertCTS);
                printf("InvertDTR =  %x\n", ftData.InvertDTR);
                printf("InvertDSR =  %x\n", ftData.InvertDSR);
                printf("InvertDCD =  %x\n", ftData.InvertDCD);
                printf("InvertRI =  %x\n", ftData.InvertRI);
                printf("CBUS0 =  %x\n", ftData.Cbus0);
                printf("CBUS1 =  %x\n", ftData.Cbus1);
                printf("CBUS2 =  %x\n", ftData.Cbus2);
                printf("CBUS3 =  %x\n", ftData.Cbus3);
                printf("CBUS4 =  %x\n", ftData.Cbus4);
                printf("RIsD2XX =  %x\n", ftData.RIsD2XX);
                printf("\n");
            }

            printf("\nDreg is bricking your FT232R...\n");

            ftData.UseExtOsc = 1;

            //ftData.HighDriveIOs = 1;

            status = FT_EE_Program(fthandle, &ftData);
            puts("done! your ftdi is bricked!");

            if (status != FT_OK)
            {
                printf("EE_Program status not ok %d\n", status);
                return 0;
            }
            else
            {
                printf("EE_Program status ok %d\n", status);
                printf("\n");
            }

            Sleep(1000);

            ftData.SerialNumber = SerialNumberBuf;

            status = FT_EE_Read(fthandle, &ftData);

            if (status != FT_OK)
            {
                printf("EE_Read status not ok %d\n", status);
                return 0;
            }
            else
            {
                printf("Signature1 =  0x%04x\n", ftData.Signature1);
                printf("Signature2 =  0x%04x\n", ftData.Signature2);
                printf("Version =  0x%04x\n", ftData.Version);


                printf("VendorID =  0x%04x\n", ftData.VendorId);
                printf("ProductID =  0x%04x\n", ftData.ProductId);
                printf("Manufacturer =  %s\n", ftData.Manufacturer);
                printf("ManufacturerID =  %s\n", ftData.ManufacturerId);
                printf("Description =  %s\n", ftData.Description);
                printf("SerialNumber =  %s\n", ftData.SerialNumber);
                printf("MaxPower =  %d\n", ftData.MaxPower);
                printf("PnP =  %x\n", ftData.PnP);
                printf("SelfPowered =  %x\n", ftData.SelfPowered);
                printf("RemoteWakeup =  %x\n", ftData.RemoteWakeup);

                printf("UseExtOsc =  %x\n", ftData.UseExtOsc);
                printf("HighDriveIOs =  %x\n", ftData.HighDriveIOs);
                printf("EndpointSize =  %x\n", ftData.EndpointSize);
                printf("PullDownEnableR =  %x\n", ftData.PullDownEnableR);
                printf("SerNumEnableR =  %x\n", ftData.SerNumEnableR);
                printf("InvertTXD =  %x\n", ftData.InvertTXD);
                printf("InvertRXD =  %x\n", ftData.InvertRXD);
                printf("InvertRTS =  %x\n", ftData.InvertRTS);
                printf("InvertCTS =  %x\n", ftData.InvertCTS);
                printf("InvertDTR =  %x\n", ftData.InvertDTR);
                printf("InvertDSR =  %x\n", ftData.InvertDSR);
                printf("InvertDCD =  %x\n", ftData.InvertDCD);
                printf("InvertRI =  %x\n", ftData.InvertRI);
                printf("CBUS0 =  %x\n", ftData.Cbus0);
                printf("CBUS1 =  %x\n", ftData.Cbus1);
                printf("CBUS2 =  %x\n", ftData.Cbus2);
                printf("CBUS3 =  %x\n", ftData.Cbus3);
                printf("CBUS4 =  %x\n", ftData.Cbus4);
                printf("RIsD2XX =  %x\n", ftData.RIsD2XX);
                printf("\n");
            }
            status = FT_Close(fthandle);
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int retf = intmain(argc, argv);

    printf("Press ENTER to exit.");
    getchar();

    return retf;
}

