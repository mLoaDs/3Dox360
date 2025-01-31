#ifndef _CONSOLE_INPUT_H_
#define _CONSOLE_INPUT_H_

#include "stdafx.h"

class ConsoleInput
{
public:
	ConsoleInput();
	~ConsoleInput();

	void GetPbusData(unsigned char* data);

private:
	int CheckDownButton(int deviceNumber,int key);
	unsigned char CalculateDeviceLowByte(int deviceNumber, int deviceCount);
	unsigned char CalculateDeviceHighByte(int deviceNumber, int deviceCount);
};

#endif // _CONSOLE_INPUT_H_