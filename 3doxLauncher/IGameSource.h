#ifndef _I_GAME_SOURCE_H_
#define _I_GAME_SOURCE_H_

class IGameSource
{
public:
	virtual void Open()				= 0;
	virtual void Close()			= 0;
	virtual int GetSectorCount()	= 0;
	
	virtual char* GetGameId()				= 0;
	virtual char* GetGameName()				= 0;

	virtual void ReadSector(void* destinationBuffer, int sectorNumber) = 0;
};

#endif // _I_GAME_SOURCE_H_