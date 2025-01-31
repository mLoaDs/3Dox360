#include "stdafx.h"
#include "GameConsole.h"

const int ROM_SIZE = 1 * 1024 * 1024;
const int NVRAM_SIZE = 32 * 1024;
const int PBUS_DATA_MAX_SIZE = 16;

GameConsole* GameConsole::m_Instance = NULL;

GameConsole* GameConsole::GetInstance()
{
	if (m_Instance == NULL)
	{
		m_Instance = new GameConsole();
	}
	return m_Instance;
}

GameConsole::GameConsole()
{
	m_Instance = this;
	m_Initialized = false;
	m_IsFrameDone = 0;

	InitializeInstanceFields();

	int VDLFrameSize = sizeof(VDLFrame);

	m_Frame = new unsigned char[VDLFrameSize];
	m_PbusData = new unsigned char[PBUS_DATA_MAX_SIZE];
	m_NvRamCopy = new unsigned char[NVRAM_SIZE];
	m_BiosRomCopy = new unsigned char[ROM_SIZE];
}



GameConsole::~GameConsole()
{
	delete[] m_Frame;
	m_Frame = 0;

	delete[] m_PbusData;
	m_PbusData = 0;

	delete[] m_NvRamCopy;
	m_NvRamCopy = 0;

	delete[] m_BiosRomCopy;
	m_BiosRomCopy = 0;
}
 

void GameConsole::InitializeInstanceFields()
{
	if (!m_Initialized)
	{
		m_DxSound = NULL;
		m_Instance = NULL;
		m_DoFreeDoMultitask = false;

		m_BiosRomCopy = 0;
		m_Frame = 0;		
		m_PbusData = 0;
		m_NvRamFileName = 0;
		m_NvRamCopy = 0;

		m_CurrentSector = 0;
		m_ArmClock = 12500000;
		m_Initialized = true;

		m_IsFrameDone = 0;
	}
}

int GameConsole::IsFrameDone()
{
	return m_IsFrameDone;
}

void GameConsole::Start(char *biosRomFileName, IGameSource* gameSource, char* nvRamFileName)
{
	m_GameSource = gameSource;

	HANDLE biosFile = CreateFile(biosRomFileName, 
		GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	int result = 0;
	unsigned long bytesRead = 0;

	result = ReadFile(biosFile, m_BiosRomCopy, ROM_SIZE, &bytesRead,
		NULL);

	if (result == 0) { throw BadBiosRomException();	}

	CloseHandle(biosFile);

	result = 0;
	bytesRead = 0;

	HANDLE nvRamFile = CreateFile(nvRamFileName, 
		GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	result = ReadFile(nvRamFile, m_NvRamCopy, NVRAM_SIZE, &bytesRead,
		NULL);

	if (result == 0) { throw BadNvRamFileException(); }

	CloseHandle(nvRamFile);

	m_NvRamFileName = nvRamFileName;

	try
	{
		m_GameSource->Open();
	}
	catch(...)
	{
		throw BadGameRomException();
	}
 
	_freedo_Interface(FDP_INIT, externalInterface);
}

// Same as WorkerThread from 4DO.
void GameConsole::Update()
{
	m_IsFrameDone = 0;

	_freedo_Interface(FDP_DO_EXECFRAME_MT, (void*)this->m_Frame);
}

int GameConsole::GetNvRamSize()
{
	return NVRAM_SIZE;
}

void* GameConsole::GetCurrentFrame()
{
	return (void*)m_Frame;
}

char* GameConsole::GetNvRamFileName()
{
	return m_NvRamFileName;
}

void GameConsole::ReadRom(void* romPointer)
{
	memcpy(romPointer, (void*)m_BiosRomCopy, ROM_SIZE);
}

void GameConsole::ReadNvram(void* nvramPointer)
{
	unsigned char* sourcePtr = m_NvRamCopy;
	memcpy(nvramPointer, sourcePtr, NVRAM_SIZE);
}

void GameConsole::WriteNvram(void* nvramPointer)
{
	memcpy(m_NvRamCopy, nvramPointer, NVRAM_SIZE);
}

void* GameConsole::SwapFrame(void* currentFrame)
{
	// This get signaled in non-multi task mode at the end of each frame.
	// I'm not entirely certain why.

	// incrediclint: because EXT_FRAMETRIGGER_MT doesn't get triggered.
	// In single thread mode this singnals that a frame is done and ready
	// to be rendered to our buffer.
	
	// Frame done, callback to 3dox.cpp 
	// to copy it to it's buffer.
	// TODO: Should the console hold a copy of the frame
	// buffer rather then 3dox.cpp?
	this->FrameDone();

	// Return a pointer to our frame buffer.
	return (void*)m_Frame;
}

void GameConsole::PushSample(UINT dspSample)
{
	UINT rightSample = dspSample & 0xFFFF0000;
	UINT leftSample = dspSample & 0x0000FFFF;

	m_DxSound->SNDDXUpdateAudio(&leftSample, &rightSample, 1);
}

int GameConsole::GetPbusLength()
{
	// Ask input plugin for Pbus data.
	unsigned char pbusDataCopy[16] = {0};
	m_ConsoleInput.GetPbusData(pbusDataCopy);

	//////////////////
	// Copy the pbus data. We'll return it to the core soon when it asks.
	int copyLength = sizeof(pbusDataCopy) / sizeof(pbusDataCopy[0]);
	if (copyLength > 16)
		copyLength = 16;
	
	void* srcPtr = pbusDataCopy;
	memcpy(m_PbusData, srcPtr, copyLength);

	return (int)(copyLength);
}

void* GameConsole::GetPbusData()
{
	return m_PbusData;
}

void GameConsole::KPrint(void* value)
{
	OutputDebugString(LPCTSTR(&value));
}

void GameConsole::DebugPrint(void* value)
{
	OutputDebugString(LPCTSTR(value));
}

void GameConsole::FrameTrigger()
{
	// We got a signal that multi-task mode has completed a frame!

	// Done with this frame.
	_freedo_Interface(FDP_DO_FRAME_MT, (void*)m_Frame);

	this->FrameDone();

}

void GameConsole::Read2048(void* buffer)
{
	this->m_GameSource->ReadSector(buffer, this->m_CurrentSector);
}

int GameConsole::GetDiscSize()
{
	return this->m_GameSource->GetSectorCount();
}

void GameConsole::OnSector(int sectorNumber)
{
	m_CurrentSector = sectorNumber;
}

void GameConsole::Destroy()
{
	_freedo_Interface(FDP_DESTROY, (void*) 0);

	DWORD result = 0;
	DWORD bytesRead = 0;

	HANDLE nvRamFile = CreateFile(m_NvRamFileName, 
		GENERIC_WRITE, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	result = WriteFile(nvRamFile, m_NvRamCopy, NVRAM_SIZE, &bytesRead,
		NULL);

	if (result == 0) { throw BadNvRamFileException(); }

	CloseHandle(nvRamFile);
}

void GameConsole::SetFixMode(int value)
{
	m_FixMode = value;

	_freedo_Interface(FDP_SET_FIX_MODE, (void*)value);

}

void GameConsole::SetArmClock(int value)
{
	m_ArmClock = value;

	_freedo_Interface(FDP_SET_ARMCLOCK, (void*)value);
}

void*   GameConsole::externalInterface(int procedure, void* Data)
{
	switch (procedure)
	{
	case (int)EXT_READ_ROMS:
		GameConsole::GetInstance()->ReadRom(Data);
		break;

	case (int)EXT_READ_NVRAM:
		GameConsole::GetInstance()->ReadNvram(Data);
		break;

	case (int)EXT_WRITE_NVRAM:
		GameConsole::GetInstance()->WriteNvram(Data);
		break;

	case (int)EXT_SWAPFRAME:
		return GameConsole::GetInstance()->SwapFrame(Data);
		break;

	case (int)EXT_PUSH_SAMPLE:
		GameConsole::GetInstance()->PushSample((UINT)Data);
		break;

	case (int)EXT_GET_PBUSLEN:
		return (void*)GameConsole::GetInstance()->GetPbusLength();
		break;

	case (int)EXT_GETP_PBUSDATA:
		return GameConsole::GetInstance()->GetPbusData();
		break;

	case (int)EXT_DEBUG_PRINT:
		GameConsole::GetInstance()->DebugPrint(Data);
		break;

	case (int)EXT_FRAMETRIGGER_MT:
		GameConsole::GetInstance()->FrameTrigger();
		break;

	case (int)EXT_READ2048:
		GameConsole::GetInstance()->Read2048(Data);
		break;

	case (int)EXT_GET_DISC_SIZE:
		return (void*)GameConsole::GetInstance()->GetDiscSize();
		break;

	case (int)EXT_ON_SECTOR:
		GameConsole::GetInstance()->OnSector((int)Data);
		break;
		
	default:
		break;
	}
	return 0;
}
