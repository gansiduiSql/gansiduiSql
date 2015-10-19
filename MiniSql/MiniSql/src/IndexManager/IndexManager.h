#ifndef _INDEXMANAGER_H_
#define _INDEXMANAGER_H_
#include "../Definition.h"
#include "bplustree.h"
#include <sstream>
#include "../BufferManager/BufferManager.h"
#include <list>
#include <map>
using namespace std;
typedef map<string, BPlusTreeIndex*>::iterator ITER;

class IndexManager
{
private:
	map<string, BPlusTreeIndex*> indexLibrary;
	BufferManager * bufferManager;
public:
	IndexManager();
	void createIndex(const string indexName, const Data attribute,const int recordLength, const string fileName);
	void dropIndex(const string indexName); //delet/drop index indexfile and index in this function
	void deleteValues(const string indexName, list<Expression> expressions); 
	void selectValues(const string indexName, list<Expression> expressions, RECORDBUFFER recordBuffer);

	void insertValues(const string indexName, const string key, const ADDRESS RecordOffset);
};

#endif