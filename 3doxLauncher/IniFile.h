#pragma once
#include "Global.h"



struct IniFileEntry
{
	//debug output
	bool bShowDebugInfo;

	//automatic frame skip method
	bool bAutomaticFrameSkip;

	//manual frame skip method (throttlespeed)
	bool bManualFrameSkip;

	//number of frames to skip
	dword dwNumFrameSkips;

	//sync audio to video
	bool bSyncAudioToVideo;

	//vertical synchronization
	bool bVSync;

	//flicker filter
	dword dwFlickerFilter;

	//soft display filter
	bool bSoftDisplayFilter;

	//texture filter
	DWORD dwTextureFilter;

	//screen xpos
	dword dwXPOS;
	//screen ypos
	dword dwYPOS;
	//screen xscale
	dword dwXWIDTH;
	//screen yscale
	dword dwYHEIGHT;

	//hide normal scroll 0
	bool bHideNBG0;
	//hide normal scroll 1
	bool bHideNBG1;
	//hide normal scroll 2
	bool bHideNBG2;
	//hide normal scroll 3
	bool bHideNBG3;
	//hide rotation scroll 0
	bool bHideRBG0;
	//hide VDP1
	bool bHideVDP1;
};


class IniFile
{
public:
	IniFile();
	~IniFile();

	bool Save(const string &szIniFileName);
	bool SaveTempRomFileName(const char *szFileName, const char *hash, const char *id);
	bool Load(const string &szIniFileName);
	bool CreateAndSaveDefaultIniEntry();
	bool CheckForIniEntry();

	IniFileEntry m_currentIniEntry;

private:
	IniFileEntry m_defaultIniEntry;

	string szRomFileName;
	
};

extern IniFile g_iniFile;





