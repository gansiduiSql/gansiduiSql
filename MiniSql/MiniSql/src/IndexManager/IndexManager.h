#ifndef _INDEXMANAGER_H_
#define _INDEXMANAGER_H_
#include "../Definition.h"
#include "bplustree.h"
#include "../BufferManager/BufferManager.h"
#include <list>
#include <map>
using namespace std;

class IndexManager
{
private:
	map<string, BPlusTree*> indexLibrary;
	BufferManager * bufferManager;
public:
	void createIndex(string indexName, string attributeName, string indexKey, 
					TYPE keyValueType, std::string fileName, ADDRESS firstRecordOffset);
	void dropIndex(string indexName); //delet/drop index indexfile and index in this function
	void deleteValues(string indexName, list<Expression> expressions); 
	void selectValues(string indexName, list<Expression> expressions, RECORDBUFFER recordBuffer);
};

#endif