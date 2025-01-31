#include "SimpleIni.h"
#include "IniFile.h"


IniFile g_iniFile;

//FIXME: SCREEN XPOS, YPOS, XSCALE, YSCALE should be floats!
//FIXME: Path for WIN32

IniFile::IniFile(void)
{
}

IniFile::~IniFile(void)
{
}

bool IniFile::Save(const string &szIniFileName)
{
	CSimpleIniA ini;
	SI_Error rc;
	ini.SetUnicode(true);
    ini.SetMultiKey(true);
    ini.SetMultiLine(true);

	//GENERAL SETTINGS
	ini.SetBoolValue("GENERAL SETTINGS", "SHOW DEBUG INFO", m_currentIniEntry.bShowDebugInfo);
	
	//VIDEO SETTINGS
	ini.SetBoolValue("VIDEO SETTINGS", "AUTOMATIC FRAME SKIP", m_currentIniEntry.bAutomaticFrameSkip);
	ini.SetBoolValue("VIDEO SETTINGS", "MANUAL FRAME SKIP", m_currentIniEntry.bManualFrameSkip);
	ini.SetLongValue("VIDEO SETTINGS", "NUMBER OF FRAME SKIPS", m_currentIniEntry.dwNumFrameSkips);
	ini.SetBoolValue("VIDEO SETTINGS", "SYNC AUDIO TO VIDEO", m_currentIniEntry.bSyncAudioToVideo);
	ini.SetBoolValue("VIDEO SETTINGS", "VERTICAL SYNC", m_currentIniEntry.bVSync);
	ini.SetLongValue("VIDEO SETTINGS", "FLICKER FILTER", m_currentIniEntry.dwFlickerFilter);
	ini.SetBoolValue("VIDEO SETTINGS", "SOFT DISPLAY FILTER", m_currentIniEntry.bSoftDisplayFilter);
	ini.SetLongValue("VIDEO SETTINGS", "TEXTURE FILTER", m_currentIniEntry.dwTextureFilter);
	ini.SetLongValue("VIDEO SETTINGS", "SCREEN XPOS", m_currentIniEntry.dwXPOS);
	ini.SetLongValue("VIDEO SETTINGS", "SCREEN YPOS", m_currentIniEntry.dwYPOS);
	ini.SetLongValue("VIDEO SETTINGS", "SCREEN XWIDTH", m_currentIniEntry.dwXWIDTH);
	ini.SetLongValue("VIDEO SETTINGS", "SCREEN YHEIGHT", m_currentIniEntry.dwYHEIGHT);

	//EMULATOR SETTINGS
	ini.SetBoolValue("EMULATOR SETTINGS", "HIDE NORMAL SCROLL 0", m_currentIniEntry.bHideNBG0);
	ini.SetBoolValue("EMULATOR SETTINGS", "HIDE NORMAL SCROLL 1", m_currentIniEntry.bHideNBG1);
	ini.SetBoolValue("EMULATOR SETTINGS", "HIDE NORMAL SCROLL 2", m_currentIniEntry.bHideNBG2);
	ini.SetBoolValue("EMULATOR SETTINGS", "HIDE NORMAL SCROLL 3", m_currentIniEntry.bHideNBG3);
	ini.SetBoolValue("EMULATOR SETTINGS", "HIDE ROTATION SCROLL 0", m_currentIniEntry.bHideRBG0);
	ini.SetBoolValue("EMULATOR SETTINGS", "HIDE VDP1", m_currentIniEntry.bHideVDP1);


	rc = ini.SaveFile(szIniFileName.c_str());
	
	OutputDebugStringA(szIniFileName.c_str());

    if (rc < 0) 
	{
		OutputDebugStringA(" failed to save!\n");
		return false;
	}

	OutputDebugStringA(" saved successfully!\n");
	return true;
}

bool IniFile::Load(const string &szIniFileName)
{
CSimpleIniA ini;
SI_Error rc;
ini.SetUnicode(true);
ini.SetMultiKey(true);
ini.SetMultiLine(true);

rc = ini.LoadFile(szIniFileName.c_str());

if (rc < 0) 
	{
		OutputDebugString("Failed to load ");
		OutputDebugString(szIniFileName.c_str());
		OutputDebugString("\n");
		return false;
	}

	OutputDebugStringA("Successfully loaded ");
	OutputDebugString(szIniFileName.c_str());
	OutputDebugString("\n");

	//GENERAL SETTINGS
	m_currentIniEntry.bShowDebugInfo = ini.GetBoolValue("GENERAL SETTINGS", "SHOW DEBUG INFO", NULL );

	//VIDEO SETTINGS
	m_currentIniEntry.bAutomaticFrameSkip = ini.GetBoolValue("VIDEO SETTINGS", "AUTOMATIC FRAME SKIP", NULL );
	m_currentIniEntry.bManualFrameSkip = ini.GetBoolValue("VIDEO SETTINGS", "MANUAL FRAME SKIP", NULL );
	m_currentIniEntry.dwNumFrameSkips = ini.GetLongValue("VIDEO SETTINGS", "NUMBER OF FRAME SKIPS", NULL );
	m_currentIniEntry.bSyncAudioToVideo = ini.GetBoolValue("VIDEO SETTINGS", "SYNC AUDIO TO VIDEO", NULL );
	m_currentIniEntry.bVSync = ini.GetBoolValue("VIDEO SETTINGS", "VERTICAL SYNC", NULL );
	m_currentIniEntry.dwFlickerFilter  = ini.GetLongValue("VIDEO SETTINGS", "FLICKER FILTER", NULL );
	m_currentIniEntry.bSoftDisplayFilter = ini.GetBoolValue("GENERAL SETTINGS", "SOFT DISPLAY FILTER", NULL );
	m_currentIniEntry.dwTextureFilter  = ini.GetLongValue("VIDEO SETTINGS", "TEXTURE FILTER", NULL );
	m_currentIniEntry.dwXPOS  = ini.GetLongValue("VIDEO SETTINGS", "SCREEN XPOS", NULL );
	m_currentIniEntry.dwYPOS  = ini.GetLongValue("VIDEO SETTINGS", "SCREEN YPOS", NULL );
	m_currentIniEntry.dwXWIDTH  = ini.GetLongValue("VIDEO SETTINGS", "SCREEN XWIDTH", NULL );
	m_currentIniEntry.dwYHEIGHT  = ini.GetLongValue("VIDEO SETTINGS", "SCREEN YHEIGHT", NULL );

	//EMULATOR SETTINGS
	m_currentIniEntry.bHideNBG0 = ini.GetBoolValue("EMULATOR SETTINGS", "HIDE NORMAL SCROLL 0", NULL );
	m_currentIniEntry.bHideNBG1 = ini.GetBoolValue("EMULATOR SETTINGS", "HIDE NORMAL SCROLL 1", NULL );
	m_currentIniEntry.bHideNBG2 = ini.GetBoolValue("EMULATOR SETTINGS", "HIDE NORMAL SCROLL 2", NULL );
	m_currentIniEntry.bHideNBG3 = ini.GetBoolValue("EMULATOR SETTINGS", "HIDE NORMAL SCROLL 3", NULL );
	m_currentIniEntry.bHideRBG0 = ini.GetBoolValue("EMULATOR SETTINGS", "HIDE ROTATION SCROLL 0", NULL );
	m_currentIniEntry.bHideVDP1 = ini.GetBoolValue("EMULATOR SETTINGS", "HIDE VDP1", NULL );
	
	return true;
}

bool IniFile::CreateAndSaveDefaultIniEntry()
{
	//GENERAL SETTINGS
	m_defaultIniEntry.bShowDebugInfo = false;

	//VIDEO SETTINGS
	m_defaultIniEntry.bAutomaticFrameSkip = true;
	m_defaultIniEntry.bManualFrameSkip = false;
	m_defaultIniEntry.dwNumFrameSkips = 3;
	m_defaultIniEntry.bSyncAudioToVideo = false;
	m_defaultIniEntry.bVSync = true;
	m_defaultIniEntry.dwFlickerFilter = 1;
	m_defaultIniEntry.bSoftDisplayFilter = false;
	m_defaultIniEntry.dwTextureFilter = 1; //1 = linear, 0 = point
	m_defaultIniEntry.dwXPOS = 15;
	m_defaultIniEntry.dwYPOS = 15;
	m_defaultIniEntry.dwXWIDTH = 333;
	m_defaultIniEntry.dwYHEIGHT = 33;

	//EMULATOR SETTINGS
	m_defaultIniEntry.bHideNBG0 = false;
	m_defaultIniEntry.bHideNBG1 = false;
	m_defaultIniEntry.bHideNBG2 = false;
	m_defaultIniEntry.bHideNBG3 = false;
	m_defaultIniEntry.bHideRBG0 = false;
	m_defaultIniEntry.bHideVDP1 = false;

	
	// our current ini is now the default ini
	m_currentIniEntry = m_defaultIniEntry;

	// save the default ini
	// FIXME! -> CD/DVD -> utility drive X:
	Save("D:\\3dox.ini");

return true;
}


bool IniFile::CheckForIniEntry()
{
	// try to load our ini file
	if(!Load("D:\\3dox.ini"))
	{
		// create a new one, if it doesn't exist
		CreateAndSaveDefaultIniEntry();
	}

return true;
}


bool IniFile::SaveTempRomFileName(const char *szFileName, const char *hash, const char *id)
{
	CSimpleIniA ini;
	SI_Error rc;

	DeleteFile("d:\\tmp3.lce");

	ini.SetUnicode(true);
    ini.SetMultiKey(true);
    ini.SetMultiLine(true);

	ini.SetValue("LAUNCHER", "ROM", szFileName, "#The filename that 3dox will load", true);
	ini.SetValue("LAUNCHER", "MD5", hash, "#MD5 checksum", true);
	ini.SetValue("LAUNCHER", "ID", id, "#First 8 bytes of the MD5 checksum", true);
	
	rc = ini.SaveFile("d:\\tmp.lce");
	
	OutputDebugStringA("d:\\tmp.lce");

    if (rc < 0) 
	{
		OutputDebugStringA(" failed to save!\n");
		return false;
	}

	OutputDebugStringA(" saved successfully!\n");
	return true;

}




