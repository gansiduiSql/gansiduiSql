#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include "..\Definition.h"
#include <queue>
#include <string>
#include <cstdio>

class BufferManager
{
public:
	BufferManager();
	~BufferManager();
	BYTE* fetchARecord(const std::string& name, const ADDRESS& address);
	void writeARecord(BYTE* record, int recordLength, const std::string& name, const ADDRESS& address);
	void setBlockPinned(int blockIndex);
	void getHeader(const std::string& fileName, ADDRESS& recordHeader, ADDRESS& freeListHeader);
	static BufferManager* getBufferManager();
private:
	void writeABlock(const int& blockIndex);
	int substitute(const std::string& fileName, const ADDRESS& tag, BYTE* buffer);
	int hit(std::string fileName, ADDRESS tag);
	int fetchABlock(const std::string& fileName, const ADDRESS& address);
private:
	FILE *openedFilePtr;
	std::string openedFileName;
	Block blocks[BLOCKNUM];
	ArrayList substitutionQue;
};

#endif