#ifndef _GAME_SOURCE_BASE_H_
#define _GAME_SOURCE_BASE_H_

#include "IGameSource.h"

class GameSourceBase : public IGameSource
{
public:
	GameSourceBase();
	virtual void OnOpen() {};
	virtual void OnClose() {};
	virtual void OnReadSector(void* destinationBuffer, int sectorNumber) {};

	virtual void Open();
	virtual void Close();
	virtual void ReadSector(void* destinationBuffer, int sectorNumber);
	int GetSectorCount();
	char* GetGameId();
	char* GetGameName();
	void ReadSectorCount();

private:
	int m_SectorCount;
	char* m_GameId;
	char* m_GameName;
	bool m_IsOpen;
};

#endif // _GAME_SOURCE_BASE_H_