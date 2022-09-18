﻿#include <thread>
#include <iostream>
#include "DCAM560/Vzense_api_560.h"


#define SNlength 17
using namespace std;

int main() {
	cout << "---DeviceInfoGet---\n";

	uint32_t deviceCount;
	PsDeviceInfo* pDeviceListInfo = NULL;
	PsDeviceHandle deviceHandle = 0;
	PsReturnStatus status = PsRetOthers;
	uint32_t sessionIndex = 0;

	//SDK Initialize
	status = Ps2_Initialize();
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "PsInitialize failed status:" <<status << endl;
		system("pause");
		return -1;
	}

	//1.Search and notice the count of devices.
	//2.get infomation of the devices. 
	//3.open devices accroding to the info.
GET:
	status = Ps2_GetDeviceCount(&deviceCount);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "PsGetDeviceCount failed! make sure the DCAM is connected" << endl;
		this_thread::sleep_for(chrono::seconds(1));
		goto GET;
	}
	cout << "Get device count: " << deviceCount << endl;
	if (0 == deviceCount)
	{
		this_thread::sleep_for(chrono::seconds(1));
		goto GET;
	}

	pDeviceListInfo = new PsDeviceInfo[deviceCount];
	status = Ps2_GetDeviceListInfo(pDeviceListInfo, deviceCount);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "GetDeviceListInfo failed status:" <<status<< endl;
		return -1;
	}
	else
	{
		if (Connected != pDeviceListInfo[0].status)
		{
			cout << "connect statu" <<pDeviceListInfo[0].status << endl;
			cout << "Call Ps2_OpenDevice with connect status :" << Connected << endl;
			return -1;
		}
	}

	cout << "uri:" << pDeviceListInfo[0].uri << endl
		<< "alias:" << pDeviceListInfo[0].alias << endl
		<< "connectStatus:" << pDeviceListInfo[0].status << endl;
	
	status = Ps2_OpenDevice(pDeviceListInfo[0].uri, &deviceHandle);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "OpenDevice failed status:" <<status << endl;
		return -1;
	}

	//get SN 
	//get FW
	char sn[63] = { 0 };
	status = Ps2_GetSerialNumber(deviceHandle,sessionIndex, sn,SNlength);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_GetSerialNumber failed status:" <<status << endl;
		return -1;
	}

	char fw[63] = { 0 };
	Ps2_GetFirmwareVersionNumber(deviceHandle, sessionIndex, fw, 63);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_GetFirmwareVersionNumber failed status:" <<status << endl;
		return -1;
	}

	cout
		<< "sn:" << sn << endl
		<< "FirmwareVersionNum:" << fw << endl;

	//1.close device
	//2.SDK shutdown
	status = Ps2_CloseDevice(&deviceHandle);
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_CloseDevice failed status:" <<status<< endl;
		return -1;
	}
	status = Ps2_Shutdown();
	if (status != PsReturnStatus::PsRetOK)
	{
		cout << "Ps2_Shutdown failed status:" <<status<< endl;
		return -1;
	}
	cout << "--end--";
	
	return 0;
}
