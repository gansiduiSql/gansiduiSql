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
using namespace std;

#define INT_STRING_SIZE 10
#define FLOAT_INTEGER_SIZE 10 //Integer part length of a float number
#define FLOAT_DECIMAL_SIZE 9//decimal part length of a float number
#define HEADER_BLOCK_OFFSET 1*BLOCKSIZE//record offset

typedef map<string, BPlusTreeIndex*>::iterator ITER;

typedef struct{
	BYTE indexName[32];
	TYPE type;
	int attributeLength;
	int offsetInRecord;
	int fanOut;
	int elementCount;
}INDEXFILEHEADER;

typedef struct{
	string value;
	bool equal;
}bound;

class IndexManager
{
private:
	map<string, BPlusTreeIndex*> indexLibrary;
	BufferManager * bufferManager;
	ADDRESS getEndOffset(const string &fileName);
	ADDRESS renewEndOffset(const string &fileName, const int &recordLength);
	void deleteRecordFromFile(const string& indexName, const string &fileName, const ADDRESS &recordOffset, const int &recordLength);
	void analysisExpression(bound &dstLowerBound, bound &dstUpperBound, bool &dstEqual, list<Expression> &expressions, const TYPE &type);/*Function tested*/
	void createIndexFromFile(const string &indexName);
	void saveIndexToFile(const string &indexName, const TYPE &type);
	void pushToRecordbuffer(const Table &table, RECORDBUFFER &recordBuffer, const ADDRESS &address, const string &fileName);
	string toAlignedInt(string s);/*Function tested*/
	string toAlignedFloat(string s);/*Function tested*/
public:
	IndexManager();/*Tested Working Normally*/
	IndexManager(list<string> indexName);/*Tested Working Normally*/
	~IndexManager();/*Tested Working Normally*/
	void createIndex(const string &indexName, Data &attribute, const int &recordLength, const string &fileName);/*create Index of a relation*//*Tested Working Normally for CreateIndex on primary-key and for CreateIndex for unique key, Cautious, if you create an unique index on an integer or float, do not use IM to process where A<xx A>xx query*/
	void dropIndex(const string &indexName); /*delet/drop index indexfile and index in this function*//*Tested Working Normally*/
	void deleteValues(const string &indexName, list<Expression> expressions, const string &fileName, const int &recordLength);/*BUG OF INCOMPLETE RECORD, Deleteing values works fine but writing the file will cause extra data to be added*/
	void selectValues(const string &indexName, Table& table, list<Expression> expressions, RECORDBUFFER &recordBuffer, const string &fileName);
	void insertValues(const string &indexName, const string &indexKey, const ADDRESS &recordOffset);/*insert indexkey to bplus tree after insertion with RM*//*Tested Working Normally*/
	void traverseTree(const string &indexName);/*Tested Working Normally*/
	static IndexManager* getIndexManagerPtr(){ static IndexManager im; return &im; }
};

#endif