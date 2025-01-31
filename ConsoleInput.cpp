#include "ConsoleInput.h"

const int DEADZONE = 16000;
extern int running;
static XINPUT_STATE stateJoy;

enum InputButton
{
	INPUTBUTTON_UP = 0,
	INPUTBUTTON_DOWN,
	INPUTBUTTON_LEFT,
	INPUTBUTTON_RIGHT,
	INPUTBUTTON_A,
	INPUTBUTTON_B,
	INPUTBUTTON_C,
	INPUTBUTTON_X,
	INPUTBUTTON_P,
	INPUTBUTTON_L,
	INPUTBUTTON_R
};

ConsoleInput::ConsoleInput()
{
	 
}

ConsoleInput::~ConsoleInput()
{}


void ConsoleInput::GetPbusData(unsigned char* data)
{
	int deviceCount = 1;
 
	data[0x0] = 0x00;
	data[0x1] = 0x48;
	data[0x2] = CalculateDeviceLowByte(6, deviceCount);
	data[0x3] = CalculateDeviceHighByte(6, deviceCount);
	data[0x4] = CalculateDeviceLowByte(6, deviceCount);
	data[0x5] = CalculateDeviceHighByte(0, deviceCount);
	data[0x6] = CalculateDeviceLowByte(0, deviceCount);
	data[0x7] = CalculateDeviceHighByte(6, deviceCount);
	data[0x8] = CalculateDeviceLowByte(6, deviceCount);
	data[0x9] = CalculateDeviceHighByte(6, deviceCount);
	data[0xA] = CalculateDeviceLowByte(6, deviceCount);
	data[0xB] = CalculateDeviceHighByte(6, deviceCount);
	data[0xC] = 0x00;
	data[0xD] = 0x80;
	data[0xE] = CalculateDeviceLowByte(5, deviceCount);
	data[0xF] = CalculateDeviceHighByte(5, deviceCount);

	//return data;
}

int ConsoleInput::CheckDownButton(int deviceNumber, int key)
{
 
	if (deviceNumber > 3)
		return 0;

	XInputGetState( deviceNumber, &stateJoy );

	if( stateJoy.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB )
    {		
		running = 0;
    }

	switch (key)
	{
	case INPUTBUTTON_B:
		return stateJoy.Gamepad.wButtons & XINPUT_GAMEPAD_A ? 1 : 0;	 
	case INPUTBUTTON_C:
		return stateJoy.Gamepad.wButtons & XINPUT_GAMEPAD_B ? 1 : 0;	 
	case INPUTBUTTON_A:
		return stateJoy.Gamepad.wButtons & XINPUT_GAMEPAD_X ? 1 : 0;
	case INPUTBUTTON_UP:
		return stateJoy.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP || stateJoy.Gamepad.sThumbLY >  DEADZONE ? 1 : 0;
	case INPUTBUTTON_RIGHT:
		return stateJoy.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT || stateJoy.Gamepad.sThumbLX >  DEADZONE ? 1 : 0;
	case INPUTBUTTON_LEFT:
		return stateJoy.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT || stateJoy.Gamepad.sThumbLX < -DEADZONE ? 1 : 0;
	case INPUTBUTTON_DOWN:
		return stateJoy.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN || stateJoy.Gamepad.sThumbLY < -DEADZONE ? 1 : 0;
	case INPUTBUTTON_P:
		return stateJoy.Gamepad.wButtons & XINPUT_GAMEPAD_START ? 1 : 0;
	case INPUTBUTTON_X:
		return stateJoy.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? 1 : 0;
	case INPUTBUTTON_L:
		return stateJoy.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? 1 : 0;
	case INPUTBUTTON_R:
		return stateJoy.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD ? 1 : 0; 
	}
  


	return 0;
}

unsigned char ConsoleInput::CalculateDeviceLowByte(int deviceNumber, int deviceCount)
{
	unsigned char returnValue = 0;
 
 	if (deviceNumber >= deviceCount)
 	        return returnValue;

	returnValue |= 0x01 & 0; // unknown
	returnValue |= 0x02 & 0; // unknown
	returnValue |= CheckDownButton(deviceNumber, INPUTBUTTON_L) ? (unsigned char)0x04 : (unsigned char)0;
	returnValue |= CheckDownButton(deviceNumber, INPUTBUTTON_R) ? (unsigned char)0x08 : (unsigned char)0;
	returnValue |= CheckDownButton(deviceNumber, INPUTBUTTON_X) ? (unsigned char)0x10 : (unsigned char)0;
	returnValue |= CheckDownButton(deviceNumber, INPUTBUTTON_P) ? (unsigned char)0x20 : (unsigned char)0;
	returnValue |= CheckDownButton(deviceNumber, INPUTBUTTON_C) ? (unsigned char)0x40 : (unsigned char)0;
	returnValue |= CheckDownButton(deviceNumber, INPUTBUTTON_B) ? (unsigned char)0x80 : (unsigned char)0;
	return returnValue;
}

unsigned char ConsoleInput::CalculateDeviceHighByte(int deviceNumber, int deviceCount)
{
	unsigned char returnValue = 0;

	 if (deviceNumber >= deviceCount)
 	        return returnValue;

	returnValue |= CheckDownButton(deviceNumber, INPUTBUTTON_A)     ? (unsigned char)0x01 : (unsigned char)0;
	returnValue |= CheckDownButton(deviceNumber, INPUTBUTTON_LEFT)  ? (unsigned char)0x02 : (unsigned char)0;
	returnValue |= CheckDownButton(deviceNumber, INPUTBUTTON_RIGHT) ? (unsigned char)0x04 : (unsigned char)0;
	returnValue |= CheckDownButton(deviceNumber, INPUTBUTTON_UP)    ? (unsigned char)0x08 : (unsigned char)0;
	returnValue |= CheckDownButton(deviceNumber, INPUTBUTTON_DOWN)  ? (unsigned char)0x10 : (unsigned char)0;
	returnValue |= 0x20 & 0; // unknown
	returnValue |= 0x40 & 0; // unknown
	returnValue |= 0x80; // This last bit seems to indicate power and/or connectivity.
	return returnValue;
}