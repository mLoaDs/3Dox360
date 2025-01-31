#ifndef _EMULATION_HELPER_
#define _EMULATION_HELPER_

struct VolumeHeader // 132 bytes
{
	unsigned char recordType; // 1 byte
	unsigned char syncBytes[5]; // 5 bytes
	unsigned char recordVersion; // 1 byte
	unsigned char flags; // 1 byte
	unsigned char comment[32]; // 32 bytes
	unsigned char label[32]; // 32 bytes
	unsigned int id; // 4 bytes
	unsigned int blockSize; // 4 bytes
	unsigned int blockCount; // 4 bytes
	unsigned int rootDirId; // 4 bytes
	unsigned int rootDirBlocks; // 4 bytes
	unsigned int rootDirBlockSize; // 4 bytes
	unsigned int lastRootDirCopy; // 4 bytes
	unsigned int rootDirCopies[8]; // 32 bytes
};

class EmulationHelper
{
public:
	static int GetSectorCount(void* sectorZeroByteZero);

	static void InitializeNvram(void* nvramByteZero);

private:
	static unsigned int ReverseBytes(unsigned int value);
};

#endif // _EMULATION_HELPER_