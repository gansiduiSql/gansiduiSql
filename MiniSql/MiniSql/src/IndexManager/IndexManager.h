/* @file IndexManager.h
* @brief Header of B+ Tree index on a minisql System
* @author lucas95123@outlook.com
* @version 1.0
* @date 2015/10/19
*/
#ifndef _INDEXMANAGER_H_
#define _INDEXMANAGER_H_

#include "../BufferManager/BufferManager.h"
#include "../Definition.h"
#include "bplustree.h"
#include <sstream>
#include <list>
#include <map>

#define INT_STRING_SIZE 10
#define FLOAT_STRING_SIZE 20
#define HEADER_BLOCK_OFFSET 1

using namespace std;
typedef map<string, BPlusTreeIndex*>::iterator ITER;

class IndexManager
{
private:
	map<string, BPlusTreeIndex*> indexLibrary;
	BufferManager * bufferManager;
	ADDRESS getEndOffset(const string fileName);
	void renewEndOffset(const string fileName, const int recordLength);
public:
	IndexManager();
	void createIndex(const string indexName, const Data attribute, const int recordLength, const string fileName);/*create Index of a relation*/
	void dropIndex(const string indexName); /*delet/drop index indexfile and index in this function*/
	void deleteValues(const string indexName, list<Expression> expressions, const string fileName, const int recordLength, TYPE type);
	void selectValues(const string indexName, list<Expression> expressions, RECORDBUFFER recordBuffer);
	void insertValues(const string indexName, const string indexKey, const ADDRESS RecordOffset);
};

#endif