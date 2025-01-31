#pragma once

struct VDLLine
//VDLP Line - one VDLP line per patent
{
	unsigned short line[320*4];//,line2[320*2*16];
	unsigned char xCLUTB[32];
	unsigned char xCLUTG[32];
	unsigned char xCLUTR[32];
	unsigned int xOUTCONTROLL;
	unsigned int xCLUTDMA;
	unsigned int xBACKGROUND;
};
struct VDLFrame
{
        VDLLine lines[240*4];
        unsigned int srcw,srch;
};

class FreedoCore
{
	private:
		enum ExternalFunction
		{
			EXT_READ_ROMS = 1,
			EXT_READ_NVRAM = 2,
			EXT_WRITE_NVRAM = 3,
			EXT_SWAPFRAME = 5, //frame swap (in mutlithreaded) or frame draw(single treaded)
			EXT_PUSH_SAMPLE = 6, //sends sample to the buffer
			EXT_GET_PBUSLEN = 7,
			EXT_GETP_PBUSDATA = 8,
			EXT_KPRINT = 9,
			EXT_DEBUG_PRINT = 10,
			EXT_FRAMETRIGGER_MT = 12, //multitasking
			EXT_READ2048 = 14, //for XBUS Plugin
			EXT_GET_DISC_SIZE = 15,
			EXT_ON_SECTOR = 16,
		};

	private:
		enum InterfaceFunction
		{
			FDP_FREEDOCORE_VERSION = 0,
			FDP_INIT = 1, //set ext_interface
			FDP_DESTROY = 2,
			FDP_DO_EXECFRAME = 3, //execute 1/60 of second
			FDP_DO_FRAME_MT = 4, //multitasking
			FDP_DO_EXECFRAME_MT = 5, //multitasking
			FDP_DO_LOAD = 6, //load state from buffer, returns !NULL if everything went smooth
			FDP_GET_SAVE_SIZE = 7, //return size of savestatemachine
			FDP_DO_SAVE = 8, //save state to buffer
			FDP_GETP_NVRAM = 9, //returns ptr to NVRAM 32K
			FDP_GETP_RAMS = 10, //returns ptr to RAM 3M
			FDP_GETP_ROMS = 11, //returns ptr to ROM 2M
			FDP_GETP_PROFILE = 12, //returns profile pointer, sizeof = 3M/4
			FDP_BUGTEMPORALFIX = 13, // JMK NOTE: Unused?
			FDP_SET_ARMCLOCK = 14,
			FDP_SET_TEXQUALITY = 15,
			FDP_GETP_WRCOUNT = 16, // JMK NOTE: Unused?
		};


	public:
		typedef void (*ReadRomDelegate)(void* romPointer);
	private:
		static ReadRomDelegate *privateReadRomEvent;
	public:
		static ReadRomDelegate *getReadRomEvent();
		static void setReadRomEvent(ReadRomDelegate *value);

		typedef void (*ReadNvramDelegate)(void* nvramPointer);
	private:
		static ReadNvramDelegate *privateReadNvramEvent;
	public:
		static ReadNvramDelegate *getReadNvramEvent();
		static void setReadNvramEvent(ReadNvramDelegate *value);

		typedef void (*WriteNvramDelegate)(void* nvramPointer);
	private:
		static WriteNvramDelegate *privateWriteNvramEvent;
	public:
		static WriteNvramDelegate *getWriteNvramEvent();
		static void setWriteNvramEvent(WriteNvramDelegate *value);

		typedef void* (*SwapFrameDelegate)(void* currentFrame);
	private:
		static SwapFrameDelegate *privateSwapFrameEvent;
	public:
		static SwapFrameDelegate *getSwapFrameEvent();
		static void setSwapFrameEvent(SwapFrameDelegate *value);

		typedef void (*PushSampleDelegate)(unsigned int dspSample);
	private:
		static PushSampleDelegate *privatePushSampleEvent;
	public:
		static PushSampleDelegate *getPushSampleEvent();
		static void setPushSampleEvent(PushSampleDelegate *value);

		typedef int (*GetPbusLengthDelegate)();
	private:
		static GetPbusLengthDelegate *privateGetPbusLengthEvent;
	public:
		static GetPbusLengthDelegate *getGetPbusLengthEvent();
		static void setGetPbusLengthEvent(GetPbusLengthDelegate *value);

		typedef void* (*GetPbusDataDelegate)();
	private:
		static GetPbusDataDelegate *privateGetPbusDataEvent;
	public:
		static GetPbusDataDelegate *getGetPbusDataEvent();
		static void setGetPbusDataEvent(GetPbusDataDelegate *value);

		typedef void (*KPrintDelegate)(void* value);
	private:
		static KPrintDelegate *privateKPrintEvent;
	public:
		static KPrintDelegate *getKPrintEvent();
		static void setKPrintEvent(KPrintDelegate *value);

		typedef void (*DebugPrintDelegate)(void* value);
	private:
		static DebugPrintDelegate *privateDebugPrintEvent;
	public:
		static DebugPrintDelegate *getDebugPrintEvent();
		static void setDebugPrintEvent(DebugPrintDelegate *value);

		typedef void (*FrameTriggerDelegate)();
	private:
		static FrameTriggerDelegate *privateFrameTriggerEvent;
	public:
		static FrameTriggerDelegate *getFrameTriggerEvent();
		static void setFrameTriggerEvent(FrameTriggerDelegate *value);

		typedef void (*Read2048Delegate)(void* buffer);
	private:
		static Read2048Delegate *privateRead2048Event;
	public:
		static Read2048Delegate *getRead2048Event();
		static void setRead2048Event(Read2048Delegate *value);

		typedef int (*GetDiscSizeDelegate)();
	private:
		static GetDiscSizeDelegate *privateGetDiscSizeEvent;
	public:
		static GetDiscSizeDelegate *getGetDiscSizeEvent();
		static void setGetDiscSizeEvent(GetDiscSizeDelegate *value);

		typedef void (*OnSectorDelegate)(int sectorNumber);
	private:
		static OnSectorDelegate *privateOnSectorEvent;
	public:
		static OnSectorDelegate *getOnSectorEvent();
		static void setOnSectorEvent(OnSectorDelegate *value);

	private:
		__declspec(dllimport) static void* FreeDoInterface(int procedure, void* datum);

	public:
		static int GetCoreVersion();

		static unsigned int GetSaveSize();

		static void* GetPointerNVRAM();

		static void* GetPointerRAM();

		static void* GetPointerROM();

		static void* GetPointerProfile();

		static int Initialize();

		static void Destroy();

		static void DoExecuteFrame(void* VDLFrame);

		static void DoExecuteFrameMultitask(void* VDLFrame);

		static void DoFrameMultitask(void* VDLFrame);

		static bool DoLoad(void* loadBuffer);

		static void DoSave(void* saveBuffer);

		static void* SetArmClock(int clock);

		static void* SetTextureQuality(int textureScalar);

	private:
		typedef void* (*ExternalInterfaceDelegate)(int procedure, void* Data);
		static ExternalInterfaceDelegate *externalInterfaceDelegate;

		static void* PrivateExternalInterface(int procedure, void* Data);
};
