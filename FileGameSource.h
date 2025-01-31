#ifndef _FILE_GAME_SOURCE_H_
#define _FILE_GAME_SOURCE_H_

#include "GameSourceBase.h"
#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif

class FileGameSource : public GameSourceBase
{
public:
	FileGameSource(const char* gameFilePath);
	~FileGameSource();

	const char* GetGameFilePath();
	void SetGameFilePath(char* filePath);

	void OnOpen();
	void OnClose();
	void OnReadSector(void* destinationBuffer, int sectorNumber);

	unsigned char *rom;

	int m_imageDataType;		// 2048 for .iso, 2352 for .bin/cue

private:
	// BinaryReader m_GameRomReader = NULL;
	HANDLE m_GameRomReader;
	const char* m_GameFilePath;

};

#endif //_FILE_GAME_SOURCE_H_