#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

#include "..\Definition.h"
#include <queue>
#include <string>
#include <cstdio>
#include <exception>

//open a file exception
class OpenFileException : public std::exception
{
private:
	std::string errLog;
public:
	OpenFileException(){ errLog = "fail to open file"; }
	OpenFileException(std::string fileName){ errLog = "fail to open " + fileName; }
	virtual const char* what()const{ return errLog.c_str(); }
};

//remove a file exception
class RemoveFileException : public std::exception
{
private:
	std::string errLog;
public:
	RemoveFileException(){ errLog = "fail to remove file"; }
	RemoveFileException(std::string fileName){ errLog = "fail to remove " + fileName; }
	virtual ~RemoveFileException(){}
	virtual const char* what()const{ return errLog.c_str(); }
};

//all block has been pinned
class AllBlockPinned : public std::exception
{
private:
	std::string errLog;
public:
	AllBlockPinned(){ errLog = "All block has been pinned!"; }
	virtual ~AllBlockPinned(){}
	virtual const char* what()const{ return errLog.c_str(); }
};

class BufferManager
{
public:
	BufferManager();
	~BufferManager();
	void createFile(const std::string& fileName);
	BYTE* fetchARecord(const std::string& name, const ADDRESS& address);
	void writeARecord(BYTE* record, int recordLength, const std::string& name, const ADDRESS& address);
	void deleteFile(const std::string& name);
	void setBlockPinned(int blockIndex);
	void setBlockNotPinned(int blockIndex);
	void getHeader(const std::string& fileName, ADDRESS& recordHeader, ADDRESS& freeListHeader);
	static BufferManager* getBufferManager();
private:
	void writeABlock(const int& blockIndex);
	int substitute(const std::string& fileName, const ADDRESS& tag, BYTE* buffer);
	int hit(const std::string& fileName, const ADDRESS& tag);
	int fetchABlock(const std::string& fileName, const ADDRESS& address);
private:
	FILE *openedFilePtr;
	std::string openedFileName;
	Block *blocks;
	ArrayList substitutionQue;
};

#endif