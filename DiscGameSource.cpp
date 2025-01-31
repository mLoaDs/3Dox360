#include "DiscGameSource.h"

DiscGameSource::DiscGameSource()
{
}

DiscGameSource::~DiscGameSource()
{}

void DiscGameSource::OnOpen()
{ 
#ifdef _XBOX
	m_Device = m_Drive.OpenCDROM2(); 
#endif
}

void DiscGameSource::OnClose()
{
#ifdef _XBOX
	m_Drive.CloseCDROM(m_Device);
#endif
}

void DiscGameSource::OnReadSector(void* destinationBuffer, int sectorNumber)
{
#ifdef _XBOX
	m_Drive.ReadSector(m_Device, sectorNumber, (char*)destinationBuffer);
#endif
}
