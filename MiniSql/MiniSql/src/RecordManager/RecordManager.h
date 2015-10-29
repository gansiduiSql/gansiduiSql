/*RecordManager.h
*@class RecorManager contains a list of interface for select, insert, delete
*@class RecordIterator is an iteartor for a successive visit of the record
*@exception the exception when insert a value conflict to the primary(unique) value in the record
*@author wang_kejie@foxmail.com
*@date 2015/10/18
*@version 1.0
*/

#ifndef _RECORDERMANAGER_H_
#define _RECORDERMANAGER_H_

#include <string>
#include <list>
#include "../BufferManager/BufferManager.h"
#include "../Definition.h"
#include <exception>

//the exception when insert a value conflict to the primary(unique) value in the record
class InsertException : public std::exception
{
private:
	std::string errlog;
public:
	InsertException(const std::string attributeName){ errlog = "Insert error! " + attributeName + " is a primary(unique) key!"; }
	virtual const char* what(){ return errlog.c_str(); }
};

class RecordIterator
{
public:
	RecordIterator(int len, int tail){ nowOffset = BLOCKSIZE; recordLength = len; this->tail = tail; }
	RecordIterator(int off, int len, int tail){ nowOffset = off; recordLength = len; this->tail = tail; }
	void setTail(int tail){ this->tail = tail; }
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
	ADDRESS insertValues(const std::string& tableName, const std::list<std::string>& values, const Table& table);
	void deleteValues(const std::string& tableName);
	void deleteValues(const std::string& tableName, const Table& table, std::list<Expression>& expressions);
	void selectValues(const std::list<std::string>& attributeNames, const std::string& tableName, const Table& table, RECORDBUFFER& recordBuffer);
	void selectValues(const std::list<std::string>& attributeNames, const std::string& tableName, const Table& table, std::list<Expression>& expressions, RECORDBUFFER& recordBuffer);
	void dropTable(const std::string tableName);
	//for test
	BufferManager* getBmPtr(){ return bmPtr; }
private:
	BufferManager* bmPtr;
};

#endif