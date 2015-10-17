#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include "..\Defination.h"
#include <queue>

class BufferManager
{
public:
	BufferManager();
	~BufferManager();
	BYTE* fecthARecord(const ADDRESS& ADDRESS);
	void writeARecord(BYTE* record, int recordLength, const ADDRESS& ADDRESS);
	void writeABlock(const ADDRESS& ADDRESS);
	void setBlockPinned(int blockIndex);
private:
	FILE *openedFilePtr;
	std::string openedFileName;
	Block blocks[BLOCKNUM];
	std::queue<int> substitutionQue;
	void substitute(int blockIndex);
	int hit(ADDRESS tag);
	int fetchABlock(const ADDRESS& ADDRESS);
};

#endif