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

/* @brief Constructor of IndexManager. In API Catalog read
* catalog file first and construct IM with indexNames
* @param indexName List of index that exists
*/
IndexManager::IndexManager(list<string> indexName)
{
	list<string>::iterator iter;
	for (iter = indexName.begin(); iter != indexName.end(); iter++)
		createIndexFromFile(*iter);
}

IndexManager::~IndexManager()
{
	/*store and then delete*/
	for (ITER iter = indexLibrary.begin(); iter != indexLibrary.end(); iter++)
	{
		saveIndexToFile(iter->first, iter->second->getAttributeType());
		delete iter->second;
		indexLibrary.erase(iter);
	}
}
/* @brief Drop the index named string indexName, delete the BplusTree
*	in memory and call buffer manager to delete the Index file on disk
* @param indexName The name of the created index
* @pre Indexname exists
* @return void
* @throw IndexNotExistException
* @post The BplusTree in memory and Index file on disk will be deleted
*/
void IndexManager::dropIndex(const string &indexName)
{
	/*Delete the BPlusTree in the index Library*/
	delete indexLibrary[indexName];
	ITER indexToDrop = indexLibrary.find(indexName);
	indexLibrary.erase(indexToDrop);

	/*delete the BPlusTree in the file*/
	bufferManager->deleteFile(indexName);
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
void IndexManager::createIndex(const string &indexName, Data &attribute, const int &recordLength, const string &fileName)
{/*storing index wait to be completed*/
	/*Calculate the fan-out of BPlusTree so that each leaf node's size is a block*/
	int bPlusFanOut;
	switch (attribute.getType())
	{
	case CHAR:bPlusFanOut = (BLOCKSIZE - 4) / (attribute.getLength() + sizeof(ADDRESS)); break;
	case INT:bPlusFanOut = (BLOCKSIZE - 4) / (1 + INT_STRING_SIZE + sizeof(ADDRESS)); break;
	case FLOAT:bPlusFanOut = (BLOCKSIZE - 4) / (1 + FLOAT_INTEGER_SIZE + FLOAT_DECIMAL_SIZE+1 + sizeof(ADDRESS)); break;
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
	for (ADDRESS recordOffset = HEADER_BLOCK_OFFSET; recordOffset < endOffset; recordOffset += recordLength)
	{
		BYTE* recordData = bufferManager->fetchARecord(fileName, recordOffset + attribute.getOffset());/*point to the start of the attribute*/
		string recordString = "";
		char* tmpRecordString = new char[attribute.getLength()+1];
		int tmpRecordInt = 0;
		float tmpRecordFloat = 0;
		stringstream ss;
		switch (attribute.getType())
		{
		case CHAR:
			memcpy(tmpRecordString, recordData, attribute.getLength());
			tmpRecordString[attribute.getLength()] = 0;
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
	saveIndexToFile(indexName, attribute.getType());
}

/* @brief create a B+ tree from existing index files at the ctor of IndexManager
* @param indexName
*/
void IndexManager::createIndexFromFile(const string &indexName)
{
	/*Read indexfile header*/
	INDEXFILEHEADER infh = *(INDEXFILEHEADER *)(bufferManager->fetchARecord(indexName, 0));
	indexLibrary[indexName] = new BPlusTreeIndex(infh.fanOut, infh.type);
	ADDRESS curser = HEADER_BLOCK_OFFSET;
	for (int i = 0; i < infh.elementCount; i++)
	{
		string keyValue = *(string *)bufferManager->fetchARecord(indexName, curser + sizeof(int));
		indexLibrary[indexName]->addKey(*(int *)bufferManager->fetchARecord(indexName, curser), keyValue);
		curser += sizeof(keyValue);
	}
}

/* @brief save the information of index to indexfile
* @param indexName
*/
void IndexManager::saveIndexToFile(const string &indexName, const TYPE &type)
{
	INDEXFILEHEADER infh;
	/*Generate indexfile header*/
	unsigned int i;
	for (i = 0; i < indexName.length(); i++)
		infh.indexName[i] = indexName[i];
	infh.indexName[i] = 0;
	infh.type = type;
	/*Travser leafnode and store data*/
	BPlusLeaf currentLeaf = indexLibrary[indexName]->returnFirstLeafNode();
	infh.fanOut = currentLeaf->getKeyNum();/*my fan out is defined by key number*/
	ADDRESS curser = HEADER_BLOCK_OFFSET;
	int  elementCount;
	for (; currentLeaf->getPtrToSinling() != NULL; currentLeaf = currentLeaf->getPtrToSinling())/*traverse to the end leaf*/
	{
		for (int i = 0; i < currentLeaf->getElementCount(); i++)
		{/*总觉得这段有些些许不对*/ 	/*for each node, store offset and keyvalue*/
			RecordPointer recordPointer=currentLeaf->getPtrToChild(i);
			ElementType keyValue = currentLeaf->getKeyValue(i);
			bufferManager->writeARecord((BYTE *)&recordPointer, sizeof(recordPointer), indexName, curser);
			curser += sizeof(RecordPointer);
			bufferManager->writeARecord((BYTE *)&keyValue, sizeof(keyValue), indexName, curser);
			curser += sizeof(currentLeaf->getKeyValue(i));
			elementCount++;
		}
	}
	/*add elementCount infomation to indexfileHeader*/
	infh.elementCount = elementCount;
	/*Write indexfile header*/
	bufferManager->writeARecord((BYTE *)&infh, sizeof(INDEXFILEHEADER), indexName, 0);
}

/* @brief update the B+ tree index after insertion
* @param indexName The name of the index
* @param indexKey The keyValue
* @param the recordOffset of the value
* @pre Index exists
* @return void
* @throw IndexNotExistException
* @post The value is inserted to the B+ tree index
*/
void IndexManager::insertValues(const string &indexName, const string &indexKey, const ADDRESS &recordOffset)
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
void IndexManager::deleteValues(const string &indexName, list<Expression> expressions,
	const string &fileName, const int& recordLength,const TYPE &type)
{
	bound upperbound;
	bound lowerbound;
	bool equal = false;
	analysisExpression(lowerbound, upperbound, equal, expressions, type);
	if (equal)
	{
		deleteRecordFromFile(fileName, indexLibrary[indexName]->findKey(upperbound.value), recordLength);/*delete the record from data file*/
		indexLibrary[indexName]->removeKey(upperbound.value);/*remove it from the Index*/
	}
	else
	{
		vector<string> keyContainer;
		BPlusLeaf currentLeaf = indexLibrary[indexName]->returnLeafNode(lowerbound.value);
		BPlusLeaf endLeaf = indexLibrary[indexName]->returnLeafNode(upperbound.value);
		int elementCount = 0;

		if (lowerbound.equal)/*The delete the remaining keys from the head node*/
		for (int i = currentLeaf->indexOf(lowerbound.value); i < currentLeaf->getElementCount() - 1; i++)/*from the key to the last node*/
		{
			deleteRecordFromFile(fileName, currentLeaf->getPtrToChild(i), recordLength);/*deleted the record from data file*/
			keyContainer[elementCount] = currentLeaf->getKeyValue(i);
			elementCount++;
		}
		else
		for (int i = currentLeaf->indexOf(lowerbound.value) + 1; i < currentLeaf->getElementCount() - 1; i++)/*from the next key to the last node*/
		{
			deleteRecordFromFile(fileName, currentLeaf->getPtrToChild(i), recordLength);/*deleted the record from data file*/
			keyContainer[elementCount] = currentLeaf->getKeyValue(i);
			elementCount++;
		}

		for (; currentLeaf != endLeaf; currentLeaf = currentLeaf->getPtrToSinling())
		{  /*delete every record except that of the head node and the last node from the file, but not from the tree, instead record every deleted key*/
			for (int i = 0; i < currentLeaf->getElementCount(); i++)
			{
				deleteRecordFromFile(fileName, currentLeaf->getPtrToChild(i), recordLength);/*deleted the record from data file*/
				keyContainer[elementCount] = currentLeaf->getKeyValue(i);
				elementCount++;
			}
		}

		if (upperbound.equal)/*The delete the remaining keys from the end node*/
		for (int i = 0; i <= endLeaf->indexOf(upperbound.value); i++)/*from start to the upperbound key*/
		{
			deleteRecordFromFile(fileName, currentLeaf->getPtrToChild(i), recordLength);/*deleted the record from data file*/
			keyContainer[elementCount] = currentLeaf->getKeyValue(i);
			elementCount++;
		}
		else
		for (int i = 0; i < endLeaf->indexOf(upperbound.value); i++)/*from start to the previous key of upper bound*/
		{
			deleteRecordFromFile(fileName, currentLeaf->getPtrToChild(i), recordLength);/*deleted the record from data file*/
			keyContainer[elementCount] = currentLeaf->getKeyValue(i);
			elementCount++;
		}

		for (unsigned int i = 0; i < keyContainer.size(); i++)
			indexLibrary[indexName]->removeKey(keyContainer[i]);
	}
}

/* @brief select the values specified by expression and indexName
* @param fileName The name of the file
* @param indexName The name of the index
* @param expression The expression that specify the deletion
* @param recordBuffer The buffer to put the selected value
* @param type The object type of attribute
* @pre Index exists
* @return void
* @throw IndexNotExistException
* @post Values statisfy the expression is selected
*/
void IndexManager::selectValues(const string &indexName, list<Expression> expressions, RECORDBUFFER recordBuffer, const string &fileName, const int &recordLength, const TYPE &type)
{
	bound upperbound;
	bound lowerbound;
	bool equal = false;
	analysisExpression(lowerbound, upperbound, equal, expressions, type);
	if (equal)
	{
		bufferManager->fetchARecord(fileName, indexLibrary[indexName]->findKey(upperbound.value));
	}
	else
	{
		BPlusLeaf currentLeaf = indexLibrary[indexName]->returnLeafNode(lowerbound.value);
		BPlusLeaf endLeaf = indexLibrary[indexName]->returnLeafNode(upperbound.value);
		/*The remaining part of beginLeaf*/
		if (lowerbound.equal)
		for (int i = currentLeaf->indexOf(lowerbound.value); i < currentLeaf->getElementCount() - 1; i++)/*from the key to the last node*/
		{
			
		}
		else
		for (int i = currentLeaf->indexOf(lowerbound.value) + 1; i < currentLeaf->getElementCount() - 1; i++)/*from the next key to the last node*/
		{
			
		}

		for (; currentLeaf != endLeaf; currentLeaf = currentLeaf->getPtrToSinling())
		{  
			for (int i = 0; i < currentLeaf->getElementCount(); i++)
			{
				
			}
		}
		/*The remaining part of endLeaf*/
		if (upperbound.equal)
		for (int i = 0; i <= endLeaf->indexOf(upperbound.value); i++)
		{
			
		}
		else
		for (int i = 0; i < endLeaf->indexOf(upperbound.value); i++)
		{
			
		}
	}
}

/* @brief Get the offset of the last record
* @return ADDRESS
*/
ADDRESS IndexManager::getEndOffset(const string &fileName)
{
	return *(ADDRESS *)bufferManager->fetchARecord(fileName, 0);
}

/* @brief Get the offset of the last record
* @return void
*/
void IndexManager::renewEndOffset(const string &fileName, const int &recordLength)
{
	ADDRESS renewedOffset = getEndOffset(fileName) - recordLength;
	bufferManager->writeARecord((BYTE *)&renewedOffset, recordLength, fileName, HEADER_BLOCK_OFFSET);
}

/* @brief Analyze given expression and set the lower or upperbound of this expression
* @param dstLowerBound
* @param dstUpperBound
* @param dstEqual
* @param expression
* @return void
*/
void IndexManager::analysisExpression(bound &dstLowerBound, bound &dstUpperBound, bool &dstEqual, list<Expression> &expressions, const TYPE &type)
{
	list<Expression>::iterator expIter;
	for (expIter = expressions.begin(); expIter != expressions.end(); expIter++)
	{
		switch (type)
		{
		case INT:
			expIter->rightOperand.oprandName = toAlignedInt(expIter->rightOperand.oprandName);
			break;
		case CHAR:
			break;
		case FLOAT:
			expIter->rightOperand.oprandName = toAlignedFloat(expIter->rightOperand.oprandName);
			break;
		default:
			break;
		}
		string bound = expIter->rightOperand.oprandName; /*stores the bound in string, int is a string of 10 char, float is a string of 20 char*/
		switch (expIter->op)
		{
		case GREATER:
			if (dstLowerBound.value == "")
				dstLowerBound.value = bound;
			else if (bound < dstLowerBound.value)
				dstLowerBound.value = bound;
			dstLowerBound.equal = false;
			break;
		case GREATER_AND_EQUAL:
			if (dstLowerBound.value == "")
				dstLowerBound.value = bound;
			else if (bound < dstLowerBound.value)
				dstLowerBound.value = bound;
			dstLowerBound.equal = true;
			break;
		case LESS:
			if (dstUpperBound.value == "")
				dstUpperBound.value = bound;
			else if (bound > dstUpperBound.value)
				dstUpperBound.value = bound;
			dstUpperBound.equal = false;
			break;
		case LESS_AND_EQUAL:
			if (dstUpperBound.value == "")
				dstUpperBound.value = bound;
			else if (bound < dstUpperBound.value)
				dstUpperBound.value = bound;
			dstUpperBound.equal = true;
			break;
		case EQUAL:
			dstEqual = true;
			break;
		default:
			break;
		}
		if (dstEqual)
		{
			dstLowerBound.value = bound;
			dstUpperBound.value = bound;
			return;
		}
	}
	/*Judging whether the expression is valid*/
	if (dstLowerBound.value == dstUpperBound.value&&dstLowerBound.equal == true && dstUpperBound.equal == true)
	{
		dstEqual = true;
		return;
	}
	if (type == CHAR && (dstLowerBound.value > dstUpperBound.value || dstLowerBound.value == dstUpperBound.value && (dstLowerBound.equal == false || dstUpperBound.equal == false)))
	{
		exception ex;
		throw ex;
	}
	if (type == INT || type == FLOAT)
	{
		if (dstLowerBound.value != ""&&dstUpperBound.value != "")
		{
			if (dstLowerBound.value[0] == '-' && dstUpperBound.value[0] != '-' && dstLowerBound.value[0] != '-' && dstUpperBound.value[0] == '-')
			{
				string tmp;
				tmp = dstLowerBound.value;
				dstLowerBound.value = dstUpperBound.value;
				dstUpperBound.value = tmp;
			}
			else if (dstLowerBound.value[0] == '-' && dstUpperBound.value[0] == '-')
			{
				if (dstLowerBound.value > dstUpperBound.value);
				else
				{
					exception ex;
					throw ex;
				}
			}
			else if (dstLowerBound.value > dstUpperBound.value || dstLowerBound.value == dstUpperBound.value && (dstLowerBound.equal == false || dstUpperBound.equal == false))
			{
				exception ex;
				throw ex;
			}
		}
	}
	if (dstLowerBound.value == "")
	{
		dstLowerBound.value = "-ffff_ffff";
		dstLowerBound.equal = true;
	}
	if (dstUpperBound.value == "")
	{
		dstUpperBound.value = "ffff_ffff";
		dstUpperBound.equal = true;
	}
	return;
}

/* @brief Delete file on the DBdata file and renew the header block in file
* @param indexName Name
* @param recordOffset The offset of the record to be deleted
* @param recordLength Length of the record
* @param fileName Name of the DBdata file
* @return void
*/
void IndexManager::deleteRecordFromFile(const string &fileName, const int &recordOffset, const int &recordLength)
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
	bool positive = true;
	if (s[0] == '-')
	{
		positive = false;
		s = s.substr(1);
	}
	if (s.length() < INT_STRING_SIZE)
	{
		for (int i = s.length(); i < INT_STRING_SIZE; i++)
			s = "0" + s;
	}
	if (positive)
		return "+" + s;
	else
		return "-" + s;
}

/* @brief  Casting float to string, the decimal part length of FLOAT_DECIMAL_LENGTH
* the integer part length FLOAT_INTEGER_LENGTH
* @param initial string
* @return string
*/
string IndexManager::toAlignedFloat(string s)
{
	bool positive = true;
	if (s[0] == '-')
	{
		positive = false;
		s = s.substr(1);
	}
	if (s.find('.') == -1)
		s = s + '.';
	while (s.find_first_of('.') <= FLOAT_DECIMAL_SIZE)
		s = "0" + s;
	while (s.length()<FLOAT_INTEGER_SIZE + FLOAT_DECIMAL_SIZE + 1)
		s = s + "0";
	if (positive)
		return "+" + s;
	else
		return "-" + s;
}

/* @brief  Traverse the B+ tree
* @param indexName The name of B+ Tree
* @return void
*/
void IndexManager::traverseTree(const string& indexName)
{
	indexLibrary[indexName]->traverseTree();
}