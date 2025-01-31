#ifndef _DISC_GAME_SOURCE_H_
#define _DISC_GAME_SOURCE_H_

#include "GameSourceBase.h"
#include "iosupport.h"

class DiscGameSource : public GameSourceBase
{
public:
	DiscGameSource();
	~DiscGameSource();

	void OnOpen();
	void OnClose();
	void OnReadSector(void* destinationBuffer, int sectorNumber);

private:
#ifdef _XBOX
	CIoSupport m_Drive;
	HANDLE m_Device;
#endif

};

#endif // _DISC_GAME_SOURCE_H_
