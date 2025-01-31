#ifndef _BIOS_ONLY_GAME_SOURCE_H_
#define _BIOS_ONLY_GAME_SOURCE_H_

#include "IGameSource.h"

class BiosOnlyGameSource : public IGameSource
{
public:
	BiosOnlyGameSource() {};
	~BiosOnlyGameSource() {};

	void Open() 
	{};

	void Close() 
	{};

	int GetSectorCount() 
	{ 
		return 0; 
	};

	char* GetGameId() 
	{ 
		return NULL;
	};

	char* GetGameName() 
	{ 
		return NULL;
	};

	void ReadSector(void* destinationBuffer, int sectorNumber) 
	{};
};

#endif // _BIOS_ONLY_GAME_SOURCE_H_