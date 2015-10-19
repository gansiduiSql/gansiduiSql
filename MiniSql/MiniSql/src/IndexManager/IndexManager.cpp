/* @file IndexManager.h
* @brief Implementation of B+ Tree index on a minisql System
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
	bufferManager;
}

/* @brief Traverse the whole File of the relation specified by fileName
* and add lndex for each record
* @param indexName The name of the created Index
* @param attribute The attribute where Index is created on
* @param  recordLength The Length of the entire record
* @param fileName The data file of the Relation
* @pre Indexname exists
* @return void
* @throw IndexNotExistException
* @post A index is created and store in .index file
*/
void IndexManager::createIndex(const string indexName, const Data attribute, const int recordLength, const string fileName)
{
	/*Calculate the fan-out of BPlusTree so that each leaf node's size is a block*/
	int bPlusFanOut;
	switch (attribute.getType())
	{
	case CHAR:bPlusFanOut = (BLOCKSIZE - 4) / (attribute.getLength() + sizeof(ADDRESS)) + 1; break;
	case INT:bPlusFanOut = (BLOCKSIZE - 4) / (INT_STRING_SIZE + sizeof(ADDRESS)) + 1; break;
	case FLOAT:bPlusFanOut = (BLOCKSIZE - 4) / (FLOAT_STRING_SIZE + sizeof(ADDRESS)) + 1; break;
	default:
		exception ex;
		throw ex;
		break;
	}

	/*Create an index B+ Tree and insert it to index library*/
	BPlusTreeIndex *newIndex = new BPlusTreeIndex(bPlusFanOut, attribute.getType());
	indexLibrary[indexName] = newIndex;

	/*Get record from bufferManager and create index*/
	for (ADDRESS recordOffset = 0; recordOffset <= endOffset; recordOffset += recordLength)
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
		delete tmpRecordFloat;
		delete tmpRecordString;
		delete tmpRecordInt;
	}
}

/* @brief delete the values specified by expression and indexName
* @param IndexName The name of the index
* @param Expression The expression that specify the deletion
* @pre Index exists
* @return void
* @throw IndexNotExistException
* @post Values statisfy the expression is deleted
*/
void IndexManager::deleteValues(const string indexName, list<Expression> expressions, TYPE type)
{
	list<Expression>::iterator expIter;
	string upperbound = "";
	string lowerbound = "";
	bool equal = false;

	for (expIter = expressions.begin(); expIter != expressions.end(); expIter++)
	{
		string bound; /*stores the bound in string, int is a string of 10 char, float is a string of 20 char*/
		switch (type)
		{
		case INT:
			if (expIter->rigthOprand.oprandName.length() <= INT_STRING_SIZE)
			{
				bound = expIter->rigthOprand.oprandName;
				for (int i = expIter->rigthOprand.oprandName.length(); i <= INT_STRING_SIZE; i++);
				bound = "0" + bound;
			}
			break;
		case CHAR:
			break;
		case FLOAT:
			if (expIter->rigthOprand.oprandName.length() <= FLOAT_STRING_SIZE)
			{
				bound = expIter->rigthOprand.oprandName;
				for (int i = expIter->rigthOprand.oprandName.length(); i <= FLOAT_STRING_SIZE; i++);
				bound = "0" + bound;
			}
			break;
		default:
			break;
		}
		switch (expIter->op)
		{
		case GREATER:
		case GREATER_AND_EQUAL:
			if (upperbound == "")
				upperbound = bound;
			else if (bound > upperbound)
				upperbound = bound;
			break;
		case LESS:
		case LESS_AND_EQUAL:
			if (lowerbound == "")
				lowerbound = bound;
			else if (bound < lowerbound)
				lowerbound = bound;
			break;
		case EQUAL:
			equal = true;
			break;
		default:
			break;
		}
		if (equal)
		{
			lowerbound = bound;
			upperbound = bound;
			break;
		}
	}
	if (equal)
	{
		indexLibrary[indexName]->findKey("bound");
		indexLibrary[indexName]->removeKey("Bound");
	}
}