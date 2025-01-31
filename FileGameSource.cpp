#include "FileGameSource.h"

FileGameSource::FileGameSource(const char* gameFilePath)
{
	char ext[5];
	this->m_GameRomReader = NULL;
	this->m_GameFilePath = gameFilePath;

	_splitpath(gameFilePath,NULL,NULL,NULL,ext);
	
	if (strcmp(ext,".bin") == 0)
	{
		this->m_imageDataType = 2352;
	}
	else
	{
		this->m_imageDataType = 2048;
	}
}

FileGameSource::~FileGameSource()
{}

const char* FileGameSource::GetGameFilePath()
{
	return m_GameFilePath;
}

void FileGameSource::SetGameFilePath(char* filePath)
{
	m_GameFilePath = filePath;
}

#define BUFSIZE 2048

void FileGameSource::OnOpen()
{
	unsigned long bytesRead = 0;

	m_GameRomReader = CreateFile(m_GameFilePath, 
		GENERIC_READ, 0, NULL, OPEN_EXISTING,
		  FILE_ATTRIBUTE_READONLY  , NULL);

	int result = GetLastError();
 

	DWORD fptr = SetFilePointer(m_GameRomReader, 0, NULL, FILE_BEGIN);
 

}

void FileGameSource::OnClose()
{
	CloseHandle(m_GameRomReader);
}

 
void FileGameSource::OnReadSector(void *destinationBuffer, int sectorNumber)
{
	if (this->m_GameRomReader == NULL)
		return; // No game loaded.

	unsigned long bytesRead = 0;

	if (this->m_imageDataType == 2352)
		SetFilePointer(m_GameRomReader, 2352  *  sectorNumber + 0x10, NULL, FILE_BEGIN);
	else
		SetFilePointer(m_GameRomReader, 2048  *  sectorNumber, NULL, FILE_BEGIN);
 
	unsigned char bytesToCopy[2048];	
	ReadFile(m_GameRomReader, &bytesToCopy, 2048, &bytesRead, NULL); 	
	memcpy(destinationBuffer, &bytesToCopy , 2048);

}