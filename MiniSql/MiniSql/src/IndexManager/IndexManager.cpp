/* @file IndexManager.cpp
* @brief Implementation of B+ Tree index on a minisql System
* @author lucas95123@outlook.com
* @version 1.0
* @date 2015/10/19
*/
#include "IndexManager.h"

/* @brief Constructor of IndexManager 
* @pre No index exists
*/
IndexManager::IndexManager()
{
	bufferManager = BufferManager::getBufferManager();
}

/* @brief Constructor of IndexManager
* @param indexName List of index that exists
*/
IndexManager::IndexManager(list<string> indexName)
{
	list<string>::iterator iter;
	for (iter = indexName.begin(); iter != indexName.end(); iter++)
		createIndexFromFile(*iter);
}

/* @brief Drop the index named string indexName, delete the BplusTree
*	in memory and call buffer manager to delete the Index file on disk
* @param indexName The name of the created index
* @pre Indexname exists
* @return void
* @throw IndexNotExistException
* @post The BplusTree in memory and Index file on disk will be deleted
*/
void IndexManager::dropIndex(const string indexName)throw(exception)
{
	/*Delete the BPlusTree in the index Library*/
	delete indexLibrary[indexName];
	ITER indexToDrop = indexLibrary.find(indexName);
	indexLibrary.erase(indexToDrop);

	/*delete the BPlusTree in the file*/
	bufferManager->deleteFile(indexName+"index");
}

/*@brief Traverse the whole File of the relation specified by fileName
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
{/*storing index wait to be completed*/
	/*Calculate the fan-out of BPlusTree so that each leaf node's size is a block*/
	int bPlusFanOut;
	switch (attribute.getType())
	{
	case CHAR:bPlusFanOut = (BLOCKSIZE - 4) / (attribute.getLength() + sizeof(ADDRESS)) + 1; break;
	case INT:bPlusFanOut = (BLOCKSIZE - 4) / (INT_STRING_SIZE + sizeof(ADDRESS)) + 1; break;
	case FLOAT:bPlusFanOut = (BLOCKSIZE - 4) / (FLOAT_INTEGER_SIZE+FLOAT_DECIMAL_SIZE + sizeof(ADDRESS)) + 1; break;
	default:
		exception ex;
		throw ex;
		break;
	}

	/*Create an index B+ Tree and insert it to index library*/
	BPlusTreeIndex *newIndex = new BPlusTreeIndex(bPlusFanOut, attribute.getType());
	indexLibrary[indexName] = newIndex;
	ADDRESS endOffset = getEndOffset(fileName);

	/*Get record from bufferManager and create index*/
	for (ADDRESS recordOffset = HEADER_BLOCK_OFFSET; recordOffset <= endOffset; recordOffset += recordLength)
	{
		BYTE* recordData = bufferManager->fetchARecord(fileName, recordOffset) + attribute.getOffset();/*point to the start of the attribute*/
		string recordString = "";
		string tmpRecordString = "";
		int tmpRecordInt = 0;
		float tmpRecordFloat = 0;
		stringstream ss;
		switch (attribute.getType())
		{
		case CHAR:
			memcpy(&tmpRecordString, recordData, attribute.getLength());
			recordString = tmpRecordString;
			newIndex->addKey(recordOffset, recordString);
			break;
		case INT:
			memcpy(&tmpRecordInt, recordData, sizeof(int));
			ss << tmpRecordInt;
			ss >> recordString;
			newIndex->addKey(recordOffset, toAlignedInt(recordString));
			break;
		case FLOAT:
			memcpy(&tmpRecordFloat, recordData, sizeof(float));
			ss << tmpRecordFloat;
			ss >> recordString;
			newIndex->addKey(recordOffset, toAlignedFloat(recordString));
			break;
		default:
			break;
		}
	}
	saveIndexToFile(indexName);
}

/* @brief create a B+ tree from existing index files at the ctor of IndexManager
* @param indexName 
*/
void IndexManager::createIndexFromFile(const string indexName)
{
	/*wait to be completed*/
}

/* @brief save the information of index to indexfile
* @param indexName
*/
void IndexManager::saveIndexToFile(const string indexName)
{
	/*create index file header
	* TYPE
	  Offset
	  {Offset 32bit,Key[]\0}[]
	*/
	BPlusTreeIndex* i = indexLibrary[indexName];
	/*Find the first leaf of b+ tree*/
	/*traverse to the end leaf*/
	/*for each node, store offset and keyvalue*/
}

void IndexManager::insertValues(const string indexName, const string indexKey, const ADDRESS recordOffset)
{
	if (indexLibrary[indexKey]->getAttributeType() == INT)
		indexLibrary[indexKey]->addKey(recordOffset, toAlignedInt(indexKey));
	else if (indexLibrary[indexKey]->getAttributeType() == FLOAT)
		indexLibrary[indexKey]->addKey(recordOffset, toAlignedFloat(indexKey));
	else
		indexLibrary[indexKey]->addKey(recordOffset, indexKey);
}

/* @brief delete the values specified by expression and indexName
* @param indexName The name of the index
* @param expression The expression that specify the deletion
* @param fileName The name of the file
* @param type The object type of attribute
* @pre Index exists
* @return void
* @throw IndexNotExistException
* @post Values statisfy the expression is deleted
*/
void IndexManager::deleteValues(const string indexName, list<Expression> expressions,
	const string fileName, const int recordLength, TYPE type)
{
	string upperbound = "";
	string lowerbound = "";
	bool equal = false;
	analysisExpression(lowerbound,upperbound,equal,expressions,type);

	if (equal)
	{
		deleteRecordFromFile(indexName, indexLibrary[indexName]->findKey(upperbound), recordLength, fileName);/*delete the record from data file*/
		indexLibrary[indexName]->removeKey(upperbound);/*remove it from the Index*/
	}
	else
	{
		BPlusLeaf endLeaf = indexLibrary[indexName]->returnLeafNode(upperbound);
		for (BPlusLeaf currentLeaf = indexLibrary[indexName]->returnLeafNode(lowerbound); currentLeaf < endLeaf; currentLeaf = currentLeaf->getPtrToSinling())
		{
			for (int i = 0; i < currentLeaf->getElementCount(); i++)
			{
				deleteRecordFromFile(indexName, currentLeaf->getPtrToChild(i), recordLength, fileName);/*deleted the record from data file*/
				/*waitToBeCompleted still don't know*/
			}
		}
	}
}

/* @brief Get the offset of the last record
* @return ADDRESS
*/
ADDRESS IndexManager::getEndOffset(const string fileName)
{
	*(ADDRESS *)bufferManager->fetchARecord(fileName, 0);
}

/* @brief Get the offset of the last record
* @return void
*/
void IndexManager::renewEndOffset(const string fileName, const int recordLength)
{
	ADDRESS renewedOffset = getEndOffset(fileName) - recordLength;
	bufferManager->writeARecord((BYTE *)&renewedOffset,recordLength,fileName,HEADER_BLOCK_OFFSET);
}

/* @brief Analyze given expression and set the lower or upperbound of this expression
* @param dstLowerBound 
* @param dstUpperBound
* @param dstEqual
* @param expression
* @return void
*/
void IndexManager::analysisExpression(string &dstLowerBound, string &dstUpperBound, bool &dstEqual, list<Expression> &expressions,const TYPE &type)
{
	list<Expression>::iterator expIter;
	for (expIter = expressions.begin(); expIter != expressions.end(); expIter++)
	{
		string bound; /*stores the bound in string, int is a string of 10 char, float is a string of 20 char*/
		switch (type)
		{
		case INT:
			expIter->rightOperand.oprandName=toAlignedInt(expIter->rightOperand.oprandName);
			break;
		case CHAR:
			break;
		case FLOAT:
			expIter->rightOperand.oprandName=toAlignedFloat(expIter->rightOperand.oprandName);
			break;
		default:
			break;
		}
		switch (expIter->op)
		{
		case GREATER:
		case GREATER_AND_EQUAL:
			if (dstUpperBound == "")
				dstUpperBound = bound;
			else if (bound > dstUpperBound)
				dstUpperBound = bound;
			break;
		case LESS:
		case LESS_AND_EQUAL:
			if (dstLowerBound == "")
				dstLowerBound = bound;
			else if (bound < dstLowerBound)
				dstLowerBound = bound;
			break;
		case EQUAL:
			dstEqual = true;
			break;
		default:
			break;
		}
		if (dstEqual)
		{
			dstLowerBound = bound;
			dstUpperBound = bound;
		}
		return;
	}
}

/* @brief Delete file on the DBdata file and renew the header block in file
* @param indexName Name
* @param recordOffset The offset of the record to be deleted
* @param recordLength Length of the record
* @param fileName Name of the DBdata file
* @return void
*/
void IndexManager::deleteRecordFromFile(const string indexName, const int recordOffset, const int recordLength, const string fileName)
{
	renewEndOffset(fileName, recordLength); /*renewedOffset=endOffset - recordLength*/
	bufferManager->writeARecord(bufferManager->fetchARecord(fileName, getEndOffset(fileName)), recordLength, fileName, recordOffset);/*Write the last record of data on the space of the deleted record*/
}

/* @brief  Casting Int to string, the string length INT_STRING_SIZE
* @param initial string
* @return string
*/
string IndexManager::toAlignedInt(string s)
{
	if (s.length() < INT_STRING_SIZE)
	{
		for (int i = s.length(); i <= INT_STRING_SIZE; i++);
		s = "0" + s;
		return s;
	}
	else return s;
}

/* @brief  Casting float to string, the string length FLOAT_STRING_SIZE
* @param initial string
* @return string
*/
string IndexManager::toAlignedFloat(string s)
{
	/*WaitToBeCompleted*/
}