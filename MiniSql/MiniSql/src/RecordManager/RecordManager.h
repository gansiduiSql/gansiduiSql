#ifndef _RECORDERMANAGER_H_
#define _RECORDERMANAGER_H_

#include <string>
#include <list>
#include "../BufferManager/BufferManager.h"
#include "../Definition.h"
#include <exception>

class InsertException : public std::exception
{
private:
	std::string errlog;
public:
	InsertException(string attributeName){ errlog = "Insert error! " + attributeName + " is a primary(unique) key!"; }
};

class RecordIterator
{
public:
	RecordIterator(int len, int tail){ nowOffset = BLOCKSIZE; recordLength = len; this->tail = tail; }
	RecordIterator(int off, int len, int tail){ nowOffset = off; recordLength = len; this->tail = tail; }
	int setTail(int tail){ this->tail = tail; }
	RecordIterator& next(){
		nowOffset += recordLength;
		if (nowOffset%BLOCKSIZE + recordLength > BLOCKSIZE)
			nowOffset = (nowOffset / BLOCKSIZE + 1)*BLOCKSIZE;
		return *this;
	}
	bool hasNext(){
		if ((nowOffset + recordLength) <= tail)
			return true;
		return false;
	}
	int value(){ return nowOffset; }
private:
	int nowOffset;
	int recordLength;
	int tail;
};

class RecordManager
{
public:
	RecordManager();
	~RecordManager();
	void insertValues(const std::string& tableName, const std::list<std::string>& values, const Table& table);
	void deleteValues(const std::string& tableName);
	void deleteValues(const std::string& tableName, const Table& table, std::list<Expression>& expressions);
	void selectValues(const std::list<std::string>& attributeNames, const std::string& tableName, const Table& table, RECORDBUFFER& recordBuffer);
	void selectValues(const std::list<std::string>& attributeNames, const std::string& tableName, const Table& table, std::list<Expression>& expressions, RECORDBUFFER& recordBuffer);
	
	//for test
	BufferManager* getBmPtr(){ return bmPtr; }
private:
	BufferManager* bmPtr;
};

#endif