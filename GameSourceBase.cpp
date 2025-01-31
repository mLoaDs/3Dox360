#include "GameSourceBase.h"
#include "EmulationHelper.h"

GameSourceBase::GameSourceBase()
{
	m_SectorCount = 0;
	m_IsOpen = false;
}

void GameSourceBase::Open()
{
	if (!m_IsOpen)
	{
		this->m_IsOpen = true;
		this->OnOpen();

		this->ReadSectorCount();
		//this->m_GameId = 0;
		//this->m_GameName = 0;
	}
}

void GameSourceBase::Close()
{
	if (this->m_IsOpen)
	{
		this->m_IsOpen = false;
		this->OnClose();
	}
}

void GameSourceBase::ReadSector(void* destinationBuffer, int sectorNumber)
{
	if (this->m_IsOpen)
	{
		this->OnReadSector(destinationBuffer, sectorNumber);
	}
}

int GameSourceBase::GetSectorCount()
{
	if (this->m_IsOpen)
	{
		return this->m_SectorCount;
	}
	else
	{
		return 0;
	}
}

char* GameSourceBase::GetGameId()
{
	if (this->m_IsOpen)
	{
		return this->m_GameId;
	}
	else
	{
		return 0;
	}
}

char* GameSourceBase::GetGameName()
{
	if (this->m_IsOpen)
	{
		return this->m_GameName;
	}
	else
	{
		return 0;
	}
}

void GameSourceBase::ReadSectorCount()
{
	unsigned char* sectorZero = new unsigned char[2048];
	unsigned char* sectorPointer = sectorZero;
	
	this->ReadSector(sectorPointer, 0);
	this->m_SectorCount = EmulationHelper::GetSectorCount(sectorPointer);
}
