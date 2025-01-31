#ifndef _GAME_CONSOLE_H_
#define _GAME_CONSOLE_H_

#include "stdafx.h"
#include "ConsoleInput.h"
#include "IGameSource.h"
#include "DxSound.h"

namespace Console_State
{
	enum State
	{
		Stopped = 0,
		Paused,
		Running
	};
}
typedef Console_State::State ConsoleState;

//===========================================================================
//	Class Declaration
//
class GameConsole
{
public:
	//=======================================================================
	//	Public Member Functions
	//

	//-----------------------------------------------------------------------
    // Destructor.
    //.......................................................................
	~GameConsole();


    //-----------------------------------------------------------------------
    // Returns a pointer to the current GameSource. 
    //.......................................................................
	IGameSource* GetGameSource() { return m_GameSource; }

	
	//-----------------------------------------------------------------------
    // Returns a pointer to the only instance of GameConsole.
    //.......................................................................
	static GameConsole* GetInstance();


    //-----------------------------------------------------------------------
    // Returns a pointer to the GameConsole's current frame data.
    //.......................................................................
	void* GetCurrentFrame();


    //-----------------------------------------------------------------------
    // Function pointer that should be set to an external function to
	// process the this GameConsole's frame data once a frame is complete.
    //.......................................................................
	void (*FrameDone)();


	//-----------------------------------------------------------------------
    // Returns the current name of our NvRam file.
    //.......................................................................
	char* GetNvRamFileName();

	
    //-----------------------------------------------------------------------
    // Returns the size of the NvRam.
    //.......................................................................	
	int GetNvRamSize();


    //-----------------------------------------------------------------------
    // Pause the emulator!
    //.......................................................................
	void Pause() {}


    //-----------------------------------------------------------------------
    // Resume the emulator!
    //.......................................................................
	void Resume() {}

	
	//-----------------------------------------------------------------------
    // Sets the freedo core's arm clock speed.
    //.......................................................................
	void SetArmClock(int value);


	//-----------------------------------------------------------------------
    // Sets the Fix mode for some games (timing hacks)
    //.......................................................................
	void SetFixMode(int value);

	//-----------------------------------------------------------------------
    // Start the emulator!
    //.......................................................................
	void Start(char *biosRomFileName, IGameSource* gameSource, char* nvRamFileName);


    //-----------------------------------------------------------------------
    // Stop the emulator!
    //.......................................................................
	void Stop() {}

	//-----------------------------------------------------------------------
	// Has the core finished a frame?
	//.......................................................................
	int IsFrameDone();

    //-----------------------------------------------------------------------
    // Update the emulator!
    //.......................................................................	
	void Update();  

	// KILL
	void Destroy();

	DxSound* m_DxSound;

private:
    //=======================================================================
    //	Private Member Functions
    //

	//-----------------------------------------------------------------------
    // Constructor.
    //.......................................................................
	GameConsole();


    //-----------------------------------------------------------------------
    // Initializes this GameConsole instance.
    //.......................................................................
	void InitializeInstanceFields();


    //-----------------------------------------------------------------------
    // Prints debug output from the core.
    //.......................................................................
	void DebugPrint(void* value);


    //-----------------------------------------------------------------------
    // Triggered when a frame is completed by the freedo core while in
	// multi-task mode.
    //.......................................................................
	void FrameTrigger();


    //-----------------------------------------------------------------------
    // Retuns the sector count of our GameSource.
    //.......................................................................
	int GetDiscSize();


    //-----------------------------------------------------------------------
    // Returns a pointer to our PbusData;
    //.......................................................................
	void* GetPbusData();


    //-----------------------------------------------------------------------
    // Copies the PBusData from our input plugin to our PBus data buffer and
	// returns the length of our PbusData.
    //.......................................................................
	int GetPbusLength();


    //-----------------------------------------------------------------------
    // I dunno!
    //.......................................................................
	void KPrint(void* value);


    //-----------------------------------------------------------------------
    // Sets our current sector.
    //.......................................................................
	void OnSector(int sectorNumber);


    //-----------------------------------------------------------------------
    // Sends the audio data recieved from the freedo core to our sound 
	// device.
    //.......................................................................
	void PushSample(UINT dspSample);


    //-----------------------------------------------------------------------
    // Copies the data of the current sector of our GameSource to the freedo
	// core's data buffer.
    //.......................................................................
	void Read2048(void* buffer);


    //-----------------------------------------------------------------------
    // Copies our NvRam to the freedo core's NvRam buffer.
    //.......................................................................
	void ReadNvram(void* nvramPointer);


	//-----------------------------------------------------------------------
    // Copies our bios rom to the freedo core's rom buffer. 
    //.......................................................................
	void ReadRom(void* romPointer);


    //-----------------------------------------------------------------------
    // Triggered when the freedo core has completed a frame while NOT in
	// multi-task mode. The core passes us a pointer to the frame it it has 
	// completed and expects us to give it a pointer to a frame buffer for
	// it to fill.
    //.......................................................................
	void* SwapFrame(void* currentFrame);


    //-----------------------------------------------------------------------
    // Copies the freedo core's NvRam buffer to our NvRam buffer.
    //.......................................................................
	void WriteNvram(void* nvramPointer);
 
private:
    //=======================================================================
    //	Private Event Handlers
    //

	//-----------------------------------------------------------------------
    // Callback for the freedo core. Responds to requests made by the freedo
	// core.
    //.......................................................................
	static void*   externalInterface(int procedure, void* Data);

public:
	int m_IsFrameDone;
private:
    //=======================================================================
    //	Private Data Members
    //
	static GameConsole* m_Instance; // Pointer to this instace.

	IGameSource* m_GameSource; // Pointer to a gamesource that's passed in.
	ConsoleInput m_ConsoleInput; // Input class.
	
	bool m_Initialized; // Is this instance initialized?
	bool m_DoFreeDoMultitask; // Should we use multitask mode?
 
	char* m_NvRamFileName; // Name of the NVRam file we'll be using.

	unsigned char* m_BiosRomCopy; // Holds a copy of the bios rom.
	unsigned char* m_Frame; // Frame buffer data storage.
	unsigned char* m_PbusData; // Pbus Data storage.(input)
	unsigned char* m_NvRamCopy; // Holds a copy of the NvRam.

	int m_CurrentSector; // Current sector we are using.
	int m_ArmClock; // Clock speed of the arm core.

	int m_FixMode; // Fix mode timing hacks for some games
 
};

#endif // _GAME_CONSOLE_H_