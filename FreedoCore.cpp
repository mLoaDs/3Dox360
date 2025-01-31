#include "FreedoCore.h"

FreedoCore::ReadRomDelegate *FreedoCore::getReadRomEvent()
{
	return privateReadRomEvent;
}

void FreedoCore::setReadRomEvent(ReadRomDelegate *value)
{
	privateReadRomEvent = value;
}

FreedoCore::ReadNvramDelegate *FreedoCore::getReadNvramEvent()
{
	return privateReadNvramEvent;
}

void FreedoCore::setReadNvramEvent(ReadNvramDelegate *value)
{
	privateReadNvramEvent = value;
}

FreedoCore::WriteNvramDelegate *FreedoCore::getWriteNvramEvent()
{
	return privateWriteNvramEvent;
}

void FreedoCore::setWriteNvramEvent(WriteNvramDelegate *value)
{
	privateWriteNvramEvent = value;
}

FreedoCore::SwapFrameDelegate *FreedoCore::getSwapFrameEvent()
{
	return privateSwapFrameEvent;
}

void FreedoCore::setSwapFrameEvent(SwapFrameDelegate *value)
{
	privateSwapFrameEvent = value;
}

FreedoCore::PushSampleDelegate *FreedoCore::getPushSampleEvent()
{
	return privatePushSampleEvent;
}

void FreedoCore::setPushSampleEvent(PushSampleDelegate *value)
{
	privatePushSampleEvent = value;
}

FreedoCore::GetPbusLengthDelegate *FreedoCore::getGetPbusLengthEvent()
{
	return privateGetPbusLengthEvent;
}

void FreedoCore::setGetPbusLengthEvent(GetPbusLengthDelegate *value)
{
	privateGetPbusLengthEvent = value;
}

FreedoCore::GetPbusDataDelegate *FreedoCore::getGetPbusDataEvent()
{
	return privateGetPbusDataEvent;
}

void FreedoCore::setGetPbusDataEvent(GetPbusDataDelegate *value)
{
	privateGetPbusDataEvent = value;
}

FreedoCore::KPrintDelegate *FreedoCore::getKPrintEvent()
{
	return privateKPrintEvent;
}

void FreedoCore::setKPrintEvent(KPrintDelegate *value)
{
	privateKPrintEvent = value;
}

FreedoCore::DebugPrintDelegate *FreedoCore::getDebugPrintEvent()
{
	return privateDebugPrintEvent;
}

void FreedoCore::setDebugPrintEvent(DebugPrintDelegate *value)
{
	privateDebugPrintEvent = value;
}

FreedoCore::FrameTriggerDelegate *FreedoCore::getFrameTriggerEvent()
{
	return privateFrameTriggerEvent;
}

void FreedoCore::setFrameTriggerEvent(FrameTriggerDelegate *value)
{
	privateFrameTriggerEvent = value;
}

FreedoCore::Read2048Delegate *FreedoCore::getRead2048Event()
{
	return privateRead2048Event;
}

void FreedoCore::setRead2048Event(Read2048Delegate *value)
{
	privateRead2048Event = value;
}

FreedoCore::GetDiscSizeDelegate *FreedoCore::getGetDiscSizeEvent()
{
	return privateGetDiscSizeEvent;
}

void FreedoCore::setGetDiscSizeEvent(GetDiscSizeDelegate *value)
{
	privateGetDiscSizeEvent = value;
}

FreedoCore::OnSectorDelegate *FreedoCore::getOnSectorEvent()
{
	return privateOnSectorEvent;
}

void FreedoCore::setOnSectorEvent(OnSectorDelegate *value)
{
	privateOnSectorEvent = value;
}

int FreedoCore::GetCoreVersion()
{
	return static_cast<int>(FreeDoInterface(static_cast<int>(FDP_FREEDOCORE_VERSION), static_cast<void*>(0)));
}

unsigned int FreedoCore::GetSaveSize()
{
	return static_cast<unsigned int>(FreeDoInterface(static_cast<int>(FDP_GET_SAVE_SIZE), static_cast<void*>(0)));
}

void* FreedoCore::GetPointerNVRAM()
{
	return FreeDoInterface(static_cast<int>(FDP_GETP_NVRAM), static_cast<void*>(0));
}

void* FreedoCore::GetPointerRAM()
{
	return FreeDoInterface(static_cast<int>(FDP_GETP_RAMS), static_cast<void*>(0));
}

void* FreedoCore::GetPointerROM()
{
	return FreeDoInterface(static_cast<int>(FDP_GETP_ROMS), static_cast<void*>(0));
}

void* FreedoCore::GetPointerProfile()
{
	return FreeDoInterface(static_cast<int>(FDP_GETP_PROFILE), static_cast<void*>(0));
}

int FreedoCore::Initialize()
{
	return static_cast<int>(FreeDoInterface(static_cast<int>(FDP_INIT), Marshal::GetFunctionPointerForDelegate(externalInterfaceDelegate)));
}

void FreedoCore::Destroy()
{
	FreeDoInterface(static_cast<int>(FDP_DESTROY), static_cast<void*>(0));
}

void FreedoCore::DoExecuteFrame(void* VDLFrame)
{
	FreeDoInterface(static_cast<int>(FDP_DO_EXECFRAME), VDLFrame);
}

void FreedoCore::DoExecuteFrameMultitask(void* VDLFrame)
{
	FreeDoInterface(static_cast<int>(FDP_DO_EXECFRAME_MT), VDLFrame);
}

void FreedoCore::DoFrameMultitask(void* VDLFrame)
{
	FreeDoInterface(static_cast<int>(FDP_DO_FRAME_MT), VDLFrame);
}

bool FreedoCore::DoLoad(void* loadBuffer)
{
	return FreeDoInterface(static_cast<int>(FDP_DO_LOAD), loadBuffer)->ToInt32() != 0;
}

void FreedoCore::DoSave(void* saveBuffer)
{
	FreeDoInterface(static_cast<int>(FDP_DO_SAVE), saveBuffer);
	return;
}

void* FreedoCore::SetArmClock(int clock)
{
	// TODO: Untested!
	return FreeDoInterface(static_cast<int>(FDP_SET_ARMCLOCK), void*(clock));
}

void* FreedoCore::SetTextureQuality(int textureScalar)
{
	// TODO: Untested!
	return FreeDoInterface(static_cast<int>(FDP_SET_TEXQUALITY), void*(textureScalar));
}

ExternalInterfaceDelegate *FreedoCore::externalInterfaceDelegate = new ExternalInterfaceDelegate(PrivateExternalInterface);

void* FreedoCore::PrivateExternalInterface(int procedure, void* Data)
{
	switch (procedure)
	{
		case static_cast<int>(EXT_READ_ROMS):
			if (ReadRomEvent != 0)
				ReadRomEvent(Data);
			break;

		case static_cast<int>(EXT_READ_NVRAM):
			if (ReadNvramEvent != 0)
				ReadNvramEvent(Data);
			break;

		case static_cast<int>(EXT_WRITE_NVRAM):
			if (WriteNvramEvent != 0)
				WriteNvramEvent(Data);
			break;

		case static_cast<int>(EXT_SWAPFRAME):
			if (SwapFrameEvent != 0)
				return SwapFrameEvent(Data);
			break;

		case static_cast<int>(EXT_PUSH_SAMPLE):
			if (PushSampleEvent != 0)
				PushSampleEvent(static_cast<unsigned int>(Data));
			break;

		case static_cast<int>(EXT_GET_PBUSLEN):
			if (GetPbusLengthEvent != 0)
				return static_cast<void*>(GetPbusLengthEvent());
			break;

		case static_cast<int>(EXT_GETP_PBUSDATA):
			if (GetPbusDataEvent != 0)
				return GetPbusDataEvent();
			break;

		case static_cast<int>(EXT_KPRINT):
			if (KPrintEvent != 0)
				KPrintEvent(Data);
			break;

		case static_cast<int>(EXT_DEBUG_PRINT):
			if (DebugPrintEvent != 0)
				DebugPrintEvent(Data);
			break;

		case static_cast<int>(EXT_FRAMETRIGGER_MT):
			if (FrameTriggerEvent != 0)
				FrameTriggerEvent();
			break;

		case static_cast<int>(EXT_READ2048):
			if (Read2048Event != 0)
				Read2048Event(Data);
			break;

		case static_cast<int>(EXT_GET_DISC_SIZE):
			if (GetDiscSizeEvent != 0)
				return static_cast<void*>(GetDiscSizeEvent());
			break;

		case static_cast<int>(EXT_ON_SECTOR):
			if (OnSectorEvent != 0)
				OnSectorEvent(static_cast<int>(Data));
			break;

		default:
			break;
	}
	return void*(0);
}
