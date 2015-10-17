#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include "..\Defination.h"
#include <queue>

class BufferManager
{
public:
	BufferManager();
	~BufferManager();
	BYTE* fecthARecord(const ADDRESS& address);
	void writeABlock(const ADDRESS& address);
	void setBlockPinned(int blockIndex);
private:
	Block blocks[BLOCKNUM];
	std::queue<int> substitutionQue;
	void substitute(int blockIndex);
	int hit(ADDRESS tag);
	int fetchABlock(const ADDRESS& address);
};

#endif