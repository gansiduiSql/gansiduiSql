/* @file IndexManager.h
* @brief Implementation of B+ Tree index on a minisql System
*
*
*
* @author lucas95123@outlook.com
* @version 1.0
* @date 2015/10/19
*/
#include "IndexManager.h"

/* @brief Constructor of IndexManager */
IndexManager::IndexManager()
{
	bufferManager = new BufferManager();
}
/* @brief Drop the index named string indexName, delete the BplusTree
*	in memory and call buffer manager to delete the Index file on disk
* @param indexName The name of the created index
* @pre Indexname exists
* @return void
* @throw IndexNotExistException
* @post The BplusTree in memory and Index file on disk will be deleted
*/
void IndexManager::dropIndex(string indexName)throw(exception)
{
	/*Delete the BPlusTree in the index Library*/
	delete indexLibrary[indexName];
	ITER indexToDrop = indexLibrary.find(indexName);
	indexLibrary.erase(indexToDrop);

	/*delete the BPlusTree in the file*/
	bufferManager->
}

/* @brief Traverse the whole File of the relation specified by fileName 
* and add lndex for each record
* @param indexName The name of the created Index
* @param attribute The attribute where Index is created on
* @param recordLength The Length of the entire record
* @param fileName The data file of the Relation
* @pre Indexname exists
* @return void
* @throw IndexNotExistException
* @post The BplusTree in memory and Index file on disk will be deleted
*/
void IndexManager::createIndex(const string indexName, const Data attribute, const int recordLength, const string fileName)
{
	/*Calculate the fan-out of BPlusTree so that each leaf node's size is a block*/
	int bPlusFanOut;
	switch (attribute.getType())
	{
	case CHAR:bPlusFanOut = (BLOCKSIZE - 4) / (attribute.getLength() + sizeof(ADDRESS)) + 1; break;
	case INT:bPlusFanOut = (BLOCKSIZE - 4) / (10 + sizeof(ADDRESS)) + 1; break;
	case FLOAT:bPlusFanOut = (BLOCKSIZE - 4) / (20 + sizeof(ADDRESS)) + 1; break;
	default:bPlusFanOut = (BLOCKSIZE - 4) / (20 + sizeof(ADDRESS)) + 1; break;
	}

	/*Create an index B+ Tree and insert it to index library*/
	BPlusTreeIndex *newIndex = new BPlusTreeIndex(bPlusFanOut, attribute.getType());
	indexLibrary[indexName] = newIndex;

	/*Get record from bufferManager and create index*/
	for (ADDRESS recordOffset = 0; ;recordOffset+=recordLength)
	{
		BYTE* recordData = bufferManager->fetchARecord(fileName, recordOffset);
		recordData += attribute.getOffset();/*point to the start of the attribute*/
		string recordString = "";
		string *tmpRecordString = new string;
		int *tmpRecordInt = new int;
		float *tmpRecordFloat = new float;
		stringstream ss;
		switch (attribute.getType())
		{
		case CHAR:
			memcpy(tmpRecordString, recordData, attribute.getLength());
			recordString = *tmpRecordString;
			break;
		case INT:
			memcpy(tmpRecordInt, recordData, sizeof(int));
			ss << tmpRecordInt;
			ss >> recordString;
			break;
		case FLOAT:
			memcpy(tmpRecordFloat, recordData, sizeof(float));
			ss << tmpRecordFloat;
			ss >> recordString;
			break;
		default:
			break;
		}
		newIndex->addKey(recordOffset, recordString);
	}
}