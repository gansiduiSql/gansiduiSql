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
* @pre indexfile already exists
*/
IndexManager::IndexManager(list<string> indexName)
{
	bufferManager = BufferManager::getBufferManager();
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
		//indexLibrary.erase(iter);
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
	case FLOAT:bPlusFanOut = (BLOCKSIZE - 4) / (1 + FLOAT_INTEGER_SIZE + FLOAT_DECIMAL_SIZE + 1 + sizeof(ADDRESS)); break;
	default:
		exception ex;
		throw ex;
		break;
	}

	/*Create an index B+ Tree and insert it to index library*/
	BPlusTreeIndex *newIndex = new BPlusTreeIndex(bPlusFanOut, attribute.getLength(), attribute.getOffset(), attribute.getType());
	indexLibrary[indexName] = newIndex;
	ADDRESS endOffset = getEndOffset(fileName);

	/*Get record from bufferManager and create index*/
	for (ADDRESS recordOffset = HEADER_BLOCK_OFFSET; recordOffset < endOffset; recordOffset += recordLength)
	{
		BYTE* recordData;
		if ((recordOffset / 4096 + 1) * 4096 - recordOffset < recordLength&&recordOffset % 4096 != 0)
			recordOffset = (recordOffset / 4096 + 1) * 4096;/*eliminate the tail of a block*/
		recordData = bufferManager->fetchARecord(fileName, recordOffset + attribute.getOffset());/*point to the start of the attribute*/
		string recordString = "";
		char* tmpRecordString = new char[attribute.getLength() + 1];
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
* @param indexName Name of the index
*/
void IndexManager::createIndexFromFile(const string &indexName)
{
	/*Read indexfile header*/
	try
	{
		INDEXFILEHEADER infh = *(INDEXFILEHEADER *)(bufferManager->fetchARecord(indexName, 0));
		indexLibrary[indexName] = new BPlusTreeIndex(infh.fanOut, infh.attributeLength, infh.offsetInRecord, infh.type);
		ADDRESS curser = HEADER_BLOCK_OFFSET;
		for (int i = 0; i < infh.elementCount; i++)
		{
			if ((curser / 4096 + 1) * 4096 - curser < infh.attributeLength + 1 + sizeof(ADDRESS) && curser % 4096 != 0)
				curser = (curser / 4096 + 1) * 4096;/*eliminate the tail of a block*/
			string keyValue = (char *)bufferManager->fetchARecord(indexName, curser + sizeof(ADDRESS));
			indexLibrary[indexName]->addKey(*(ADDRESS *)bufferManager->fetchARecord(indexName, curser), keyValue);
			curser += infh.attributeLength + 1 + sizeof(ADDRESS);
		}
	}
	catch (exception& ex)
	{
		throw ex;
	}
}

/* @brief save the information of index to indexfile
* @param indexName Name of the index
* @param type TYPE of the attribute where index is created
*/
void IndexManager::saveIndexToFile(const string &indexName, const TYPE &type)
{
	INDEXFILEHEADER infh;
	/*Generate indexfile header*/
	unsigned int i;
	for (i = 0; i < indexName.length(); i++)
		infh.indexName[i] = indexName[i];
	infh.indexName[i] = 0;/*Write the indexName to the header*/
	infh.attributeLength = indexLibrary[indexName]->getAttributeLength();
	infh.offsetInRecord = indexLibrary[indexName]->getOffsetInRecord();
	infh.type = type;/*Write the keyValue type to the header*/
	/*Travser leafnode and store data*/
	BPlusLeaf currentLeaf = indexLibrary[indexName]->returnFirstLeafNode();
	infh.fanOut = currentLeaf->getKeyNum();/*my fan out is defined by key number*/
	ADDRESS curser = HEADER_BLOCK_OFFSET;
	int  elementCount = 0;

	bufferManager->createFile(indexName);
	while (currentLeaf != NULL)/*traverse to the end leaf*/
	{
		for (int i = 0; i < currentLeaf->getElementCount(); i++)
		{/*for each node, store offset and keyvalue*/
			ADDRESS recordPointer = currentLeaf->getPtrToChild(i);
			ElementType keyValue = currentLeaf->getKeyValue(i);
			if ((curser / 4096 + 1) * 4096 - curser < (infh.attributeLength + 1 + sizeof(ADDRESS)) && curser % 4096 != 0)
				curser = (curser / 4096 + 1) * 4096;/*eliminate the tail of a block*/
			keyValue += '\0';
			bufferManager->writeARecord((BYTE *)&recordPointer, sizeof(recordPointer), indexName, curser);
			curser += sizeof(ADDRESS);
			bufferManager->writeARecord((BYTE *)keyValue.c_str(), keyValue.length(), indexName, curser);
			curser += infh.attributeLength + 1;
			elementCount++;
		}
		currentLeaf = currentLeaf->getPtrToSinling();/*Go to next node*/
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
void IndexManager::insertValues(const string &indexName, string indexKey, const ADDRESS &recordOffset)
{
	if (indexLibrary[indexName]->getAttributeType() == INT)
		indexLibrary[indexName]->addKey(recordOffset, toAlignedInt(indexKey));
	else if (indexLibrary[indexName]->getAttributeType() == FLOAT)
		indexLibrary[indexName]->addKey(recordOffset, toAlignedFloat(indexKey));
	else
	{
		while (indexKey.size() < indexLibrary[indexName]->getAttributeLength())
			indexKey+=" ";
		indexLibrary[indexName]->addKey(recordOffset, indexKey);
	}
}

/* @brief delete the values specified by expression and indexName
* @param indexName The name of the index
* @param expression The expression that specify the deletion
* @param fileName The name of the file
* @param recordLength Length of the record
* @pre Index exists
* @return void
* @throw IndexNotExistException
* @post Values statisfy the expression is deleted
*/
void IndexManager::deleteValues(const string &indexName, const list<string> &indexList, list<Expression> expressions,
	const string &fileName, const int& recordLength)
{
	BPlusTreeIndex* currentIndex = indexLibrary[indexName];
	bound upperbound;
	bound lowerbound;
	bool equal = false;
	bool lowerFlagAdded = false;
	bool upperFlagAdded = false;
	analysisExpression(lowerbound, upperbound, equal, expressions, currentIndex->getAttributeLength(),currentIndex->getAttributeType());
	if (equal)
	{
		deleteRecordFromFile(indexName, indexList, fileName, currentIndex->findKey(upperbound.value), recordLength);/*delete the record from data file*/
		currentIndex->removeKey(upperbound.value);/*remove it from the Index*/
	}
	else
	{
		if (currentIndex->findKey(lowerbound.value) == -1)/*If lower or upperbound not exist*/
		{
			currentIndex->addKey(-1, lowerbound.value);/*add a key as a flag and after process delete it*/
			lowerFlagAdded = true;
		}
		if (currentIndex->findKey(upperbound.value) == -1)
		{
			currentIndex->addKey(-1, upperbound.value);
			upperFlagAdded = true;
		}
		vector<string> keyContainer;
		BPlusLeaf currentLeaf = currentIndex->returnLeafNode(lowerbound.value);
		BPlusLeaf endLeaf = currentIndex->returnLeafNode(upperbound.value);
		if (currentLeaf == endLeaf)/*If the start Node and the same node is the same*/
		{
			int start = currentLeaf->indexOf(lowerbound.value);
			int end = currentLeaf->indexOf(upperbound.value);
			if (lowerbound.equal == false || lowerFlagAdded)
				start++;
			if (upperbound.equal == false || upperFlagAdded)
				end--;
			for (int i = start; i <= end; i++)/*Only have to process this node*/
			{
				deleteRecordFromFile(indexName, indexList, fileName, currentLeaf->getPtrToChild(i), recordLength);/*deleted the record from data file*/
				keyContainer.push_back(currentLeaf->getKeyValue(i));
			}
		}
		else/*if the start and end go through serveral nodes*/
		{
			if (lowerbound.equal&&!lowerFlagAdded)/*The delete the remaining keys from the head node*/
			for (int i = currentLeaf->indexOf(lowerbound.value); i < currentLeaf->getElementCount(); i++)/*from the key to the last node*/
			{
				deleteRecordFromFile(indexName, indexList, fileName, currentLeaf->getPtrToChild(i), recordLength);/*deleted the record from data file*/
				keyContainer.push_back(currentLeaf->getKeyValue(i));
			}
			else
			for (int i = currentLeaf->indexOf(lowerbound.value) + 1; i < currentLeaf->getElementCount(); i++)/*from the next key to the last node*/
			{
				deleteRecordFromFile(indexName, indexList, fileName, currentLeaf->getPtrToChild(i), recordLength);/*deleted the record from data file*/
				keyContainer.push_back(currentLeaf->getKeyValue(i));
			}
			currentLeaf = currentLeaf->getPtrToSinling();
			for (; currentLeaf != endLeaf; currentLeaf = currentLeaf->getPtrToSinling())
			{  /*delete every record except that of the head node and the last node from the file, but not from the tree, instead record every deleted key*/
				for (int i = 0; i < currentLeaf->getElementCount(); i++)
				{
					deleteRecordFromFile(indexName, indexList, fileName, currentLeaf->getPtrToChild(i), recordLength);/*deleted the record from data file*/
					keyContainer.push_back(currentLeaf->getKeyValue(i));
				}
			}

			if (upperbound.equal&&!upperFlagAdded)/*The delete the remaining keys from the end node*/
			for (int i = 0; i <= endLeaf->indexOf(upperbound.value); i++)/*from start to the upperbound key*/
			{
				deleteRecordFromFile(indexName, indexList, fileName, currentLeaf->getPtrToChild(i), recordLength);/*deleted the record from data file*/
				keyContainer.push_back(currentLeaf->getKeyValue(i));
			}
			else
			for (int i = 0; i < endLeaf->indexOf(upperbound.value); i++)/*from start to the previous key of upper bound*/
			{
				deleteRecordFromFile(indexName, indexList, fileName, currentLeaf->getPtrToChild(i), recordLength);/*deleted the record from data file*/
				keyContainer.push_back(currentLeaf->getKeyValue(i));
			}
		}
		for (unsigned int i = 0; i < keyContainer.size(); i++)
			currentIndex->removeKey(keyContainer[i]);/*remove all the keys from index*/
		if (lowerFlagAdded)/*If a lowerbound flag is added*/
			currentIndex->removeKey(lowerbound.value);/*delete it*/
		if (upperFlagAdded)/*if an upperbound flag is added*/
			currentIndex->removeKey(upperbound.value);/*delete it*/
	}
}

/* @brief delete the values specified by list of keys
* @param primaryIndexName The name of index on primary key
* @param primaryKeyValues The primary key value of records to be deleted
* @param indexList List of index on the table
* @param recordLength Length of the record
* @param fileName Name of the file
* @pre Index exists
* @return void
* @throw IndexNotExistException
* @post Values of the key list is deleted
*/
void IndexManager::deleteValues(const string &primaryIndexName, const list<string> &primaryKeyValues, const list<string> &indexList,
	const string &fileName, const int recordLength)
{
	int cnt = 0;
	for (auto keyValue : primaryKeyValues)
	{
		deleteRecordFromFile("", indexList, fileName, indexLibrary[primaryIndexName]->findKey(keyValue), recordLength);
		cout << cnt++;
	}
}

/* @brief delete all the values in Index by deletion of the whole tree
* @param indexName The name of the index
* @pre Index exists
* @return void
* @throw IndexNotExistException
* @post Values statisfy the expression is deleted
*/
void IndexManager::deleteValuesAll(const string &indexName)
{
	BPlusTreeIndex* oldIndex = indexLibrary[indexName];/*get the tree to be emptyed*/
	indexLibrary[indexName] = new BPlusTreeIndex(oldIndex->getMaxKeyNum(), oldIndex->getAttributeLength(), oldIndex->getOffsetInRecord(), oldIndex->getAttributeType());/*Construct a new tree*/
	delete oldIndex;/*delete the old tree*/
	int zero = 0;
	bufferManager->writeARecord((BYTE *)&zero, sizeof(int), indexName, 48);/*48 is the start offset of elementcount in indexfile Header*/
}

/* @brief select the values specified by expression and indexName
* @param fileName The name of the file
* @param indexName The name of the index
* @param expression The expression that specify the deletion
* @pre Index exists
* @return void
* @throw IndexNotExistException
* @post Values statisfy the expression is selected
*/
void IndexManager::selectValues(const string &indexName, const list<string> &attributes,Table& table, list<Expression> expressions, RECORDBUFFER &recordBuffer, const string &fileName)
{
	BPlusTreeIndex* currentIndex = indexLibrary[indexName];
	bound upperbound;
	bound lowerbound;
	bool equal = false;
	bool lowerFlagAdded = false;
	bool upperFlagAdded = false;
	int recordLength = table.getLength();
	analysisExpression(lowerbound, upperbound, equal, expressions, currentIndex->getAttributeLength(), currentIndex->getAttributeType());
	if (equal)
		pushToRecordbuffer(attributes, table, recordBuffer, currentIndex->findKey(upperbound.value), fileName);
	else
	{
		if (currentIndex->findKey(lowerbound.value) == -1)/*If lower or upperbound not exist*/
		{
			currentIndex->addKey(-1, lowerbound.value);/*add a key as a flag and after process delete it*/
			lowerFlagAdded = true;
		}
		if (currentIndex->findKey(upperbound.value) == -1)
		{
			currentIndex->addKey(-1, upperbound.value);
			upperFlagAdded = true;
		}
		BPlusLeaf currentLeaf = currentIndex->returnLeafNode(lowerbound.value);
		BPlusLeaf endLeaf = currentIndex->returnLeafNode(upperbound.value);
		if (currentLeaf == endLeaf)/*If the start Node and the same node is the same*/
		{
			int start = currentLeaf->indexOf(lowerbound.value);
			int end = currentLeaf->indexOf(upperbound.value);
			if (lowerbound.equal == false || lowerFlagAdded)
				start++;
			if (upperbound.equal == false || upperFlagAdded)
				end--;
			if (start!=end)
			for (int i = start; i <= end; i++)/*Only have to process this node*/
				pushToRecordbuffer(attributes,table, recordBuffer, currentLeaf->getPtrToChild(i), fileName);
		}
		else/*if the start and end go through serveral nodes*/
		{
			if (lowerbound.equal&&!lowerFlagAdded)/*The delete the remaining keys from the head node*/
			for (int i = currentLeaf->indexOf(lowerbound.value); i < currentLeaf->getElementCount(); i++)/*from the key to the last node*/
				pushToRecordbuffer(attributes,table, recordBuffer, currentLeaf->getPtrToChild(i), fileName);
			else
			for (int i = currentLeaf->indexOf(lowerbound.value) + 1; i < currentLeaf->getElementCount(); i++)/*from the next key to the last node*/
				pushToRecordbuffer(attributes,table, recordBuffer, currentLeaf->getPtrToChild(i), fileName);
			currentLeaf = currentLeaf->getPtrToSinling();
			for (; currentLeaf != endLeaf; currentLeaf = currentLeaf->getPtrToSinling())
			{  /*delete every record except that of the head node and the last node from the file, but not from the tree, instead record every deleted key*/
				for (int i = 0; i < currentLeaf->getElementCount(); i++)
					pushToRecordbuffer(attributes,table, recordBuffer, currentLeaf->getPtrToChild(i), fileName);
			}

			if (upperbound.equal&&!upperFlagAdded)/*The delete the remaining keys from the end node*/
			for (int i = 0; i <= endLeaf->indexOf(upperbound.value); i++)/*from start to the upperbound key*/
				pushToRecordbuffer(attributes,table, recordBuffer, currentLeaf->getPtrToChild(i), fileName);
			else
			for (int i = 0; i < endLeaf->indexOf(upperbound.value); i++)/*from start to the previous key of upper bound*/
				pushToRecordbuffer(attributes,table, recordBuffer, currentLeaf->getPtrToChild(i), fileName);
		}
		if (lowerFlagAdded)/*If a lowerbound flag is added*/
			currentIndex->removeKey(lowerbound.value);/*delete it*/
		if (upperFlagAdded)/*if an upperbound flag is added*/
			currentIndex->removeKey(upperbound.value);/*delete it*/
	}
}

/* @brief Get the offset of the next record of the last record
* @param fileName
* @return ADDRESS
*/
ADDRESS IndexManager::getEndOffset(const string &fileName)
{
	return *(ADDRESS *)bufferManager->fetchARecord(fileName, 0);
}

/* @brief Get the offset of the last record
* @param fileName
* @param recordLength
* @return ADDRESS
*/
ADDRESS IndexManager::getNextToEndOffset(const string &fileName, const int &recordLength)
{
	ADDRESS endOffset = *(ADDRESS *)bufferManager->fetchARecord(fileName, 0);
	if (endOffset % 4096 == 0)/*If the first record is reached*/
		return endOffset - (4096 - 4096 / recordLength*recordLength) - recordLength;/*eliminate the tail and minus the record length*/
	else
		return endOffset - recordLength;
}

/* @brief Get the offset of the last record and write it to header
* @param fileName
* @param recordLength
* @return ADDRESS address of the renewed Offset
*/
void IndexManager::renewEndOffset(const string &fileName, const int &recordLength)
{
	ADDRESS renewedOffset = getNextToEndOffset(fileName, recordLength);
	bufferManager->writeARecord((BYTE *)&renewedOffset, recordLength, fileName, 0);
}

/* @brief Analyze given expression and set the lower or upperbound of this expression
* @param dstLowerBound
* @param dstUpperBound
* @param dstEqual
* @param expression
* @return void
*/
void IndexManager::analysisExpression(bound &dstLowerBound, bound &dstUpperBound, bool &dstEqual, list<Expression> &expressions, const int &attributeLength, const TYPE &type)
{
	list<Expression>::iterator expIter;
	for (expIter = expressions.begin(); expIter != expressions.end(); expIter++)
	{
		switch (type)
		{
		case INT:
			expIter->rightOperand.operandName = toAlignedInt(expIter->rightOperand.operandName);
			break;
		case CHAR:
			while (expIter->rightOperand.operandName.size() < attributeLength)
				expIter->rightOperand.operandName += " ";
			break;
		case FLOAT:
			expIter->rightOperand.operandName = toAlignedFloat(expIter->rightOperand.operandName);
			break;
		default:
			break;
		}
		string bound = expIter->rightOperand.operandName; /*stores the bound in string, int is a string of 10 char, float is a string of 20 char*/
		switch (expIter->op)
		{
		case GREATER:
			if (dstLowerBound.value == "")
				dstLowerBound.value = bound;
			else if (bound > dstLowerBound.value)
				dstLowerBound.value = bound;
			dstLowerBound.equal = false;
			break;
		case GREATER_AND_EQUAL:
			if (dstLowerBound.value == "")
				dstLowerBound.value = bound;
			else if (bound > dstLowerBound.value)
				dstLowerBound.value = bound;
			dstLowerBound.equal = true;
			break;
		case LESS:
			if (dstUpperBound.value == "")
				dstUpperBound.value = bound;
			else if (bound < dstUpperBound.value)
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
	if (dstLowerBound.value != "" && dstUpperBound.value != "")
	{
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

/* @brief Delete the record on file and put the last record into the deleted position also delete the key in indexs other than the index where the query is on
* @param indexName
* @param fileName
* @param recordOffset The offset of the record to be deleted
* @param recordLength Length of the record
* @param fileName Name of the DBdata file
* @return void
*/
void IndexManager::deleteRecordFromFile(const string & keyIndexName, const list<string> &indexList, const string &fileName, const ADDRESS &recordOffset, const int &recordLength)
{
	if (recordOffset == getNextToEndOffset(fileName, recordLength))/*renew End offset and delete key in every index that is not key index*/
	{
		renewEndOffset(fileName, recordLength);
		for (auto indexName : indexList)
		{
			if (indexName == keyIndexName)
				continue;
			BPlusTreeIndex* currentIndex = indexLibrary[indexName];/*the current index*/
			BYTE* deleteRecordKey = bufferManager->fetchARecord(fileName, recordOffset + indexLibrary[indexName]->getOffsetInRecord());/*the key of record to be deleted*/
			string recordString = "";
			char* tmpRecordString = new char[currentIndex->getAttributeLength() + 1];
			int tmpRecordInt = 0;
			float tmpRecordFloat = 0;
			stringstream ss;
			switch (currentIndex->getAttributeType())
			{
			case CHAR:
				memcpy(tmpRecordString, deleteRecordKey, currentIndex->getAttributeLength());
				tmpRecordString[currentIndex->getAttributeLength()] = 0;
				recordString = tmpRecordString;
				currentIndex->removeKey(recordString);/*Delete the index*/
				break;
			case INT:
				memcpy(&tmpRecordInt, deleteRecordKey, sizeof(int));
				ss << tmpRecordInt;
				ss >> recordString;
				currentIndex->removeKey(toAlignedInt(recordString));/*Delete the index*/
				break;
			case FLOAT:
				memcpy(&tmpRecordInt, deleteRecordKey, sizeof(int));
				ss << tmpRecordInt;
				ss >> recordString;
				currentIndex->removeKey(toAlignedFloat(recordString));/*Delete the index*/
				break;
			default:
				break;
			}
		}
		return;
	}
	/*Insert the keyvalue of end record to every Index and for index that is not key index delete it*/
	ADDRESS renewedEndOffset = getNextToEndOffset(fileName, recordLength); /*renewedOffset*/
	for (auto indexName : indexList)/*For every index that exists in the table*/
	{
		BPlusTreeIndex* currentIndex = indexLibrary[indexName];/*the current index*/
		BYTE* deleteRecordKey = bufferManager->fetchARecord(fileName, recordOffset + indexLibrary[indexName]->getOffsetInRecord());/*the key of record to be deleted*/
		BYTE* endRecordKey = bufferManager->fetchARecord(fileName, renewedEndOffset + indexLibrary[indexName]->getOffsetInRecord());/*the key of end record*/
		string recordString = "";
		char* tmpRecordString = new char[currentIndex->getAttributeLength() + 1];
		int tmpRecordInt = 0;
		float tmpRecordFloat = 0;
		stringstream ss;
		switch (currentIndex->getAttributeType())
		{
		case CHAR:
			memcpy(tmpRecordString, endRecordKey, currentIndex->getAttributeLength());
			tmpRecordString[currentIndex->getAttributeLength()] = 0;
			recordString = tmpRecordString;
			currentIndex->addKey(recordOffset, recordString); /*Renew the offset of the last record*/
			if (indexName != keyIndexName)/*if the index is not key index*/
			{
				memcpy(tmpRecordString, deleteRecordKey, currentIndex->getAttributeLength());
				tmpRecordString[currentIndex->getAttributeLength()] = 0;
				recordString = tmpRecordString;
				currentIndex->removeKey(recordString);/*Delete the index*/
			}
			break;
		case INT:
			memcpy(&tmpRecordInt, endRecordKey, sizeof(int));
			ss << tmpRecordInt;
			ss >> recordString;
			currentIndex->addKey(recordOffset, toAlignedInt(recordString));
			if (indexName != keyIndexName)/*if the index is not key index*/
			{
				memcpy(&tmpRecordInt, deleteRecordKey, sizeof(int));
				ss << tmpRecordInt;
				ss >> recordString;
				currentIndex->removeKey(toAlignedInt(recordString));/*Delete the index*/
			}
			break;
		case FLOAT:
			memcpy(&tmpRecordFloat, endRecordKey, sizeof(float));
			ss << tmpRecordFloat;/*if the index is not key index*/
			ss >> recordString;
			currentIndex->addKey(recordOffset, toAlignedFloat(recordString));
			if (indexName != keyIndexName)
			{
				memcpy(&tmpRecordInt, deleteRecordKey, sizeof(int));
				ss << tmpRecordInt;
				ss >> recordString;
				currentIndex->removeKey(toAlignedFloat(recordString));/*Delete the index*/
			}
			break;
		default:
			break;
		}
	}
	bufferManager->writeARecord(bufferManager->fetchARecord(fileName, renewedEndOffset), recordLength, fileName, recordOffset);/*Write the last record of data on the space of the deleted record*/
	renewEndOffset(fileName, recordLength);/*renew the end offset in header*/
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
	while (s.length() < FLOAT_INTEGER_SIZE + FLOAT_DECIMAL_SIZE + 1)
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
	ITER iter = indexLibrary.find(indexName);
	if (iter != indexLibrary.end())
		indexLibrary[indexName]->traverseTree();
	else
		throw new exception;
}

/* @brief  Convert the data to coresponding string and push to record buffer
* @param table the relation
* @param recordData the pointer of the record
* @param recordLength the length of the record
* @return void
*/
void IndexManager::pushToRecordbuffer(const list<string> attributes, const Table &table, RECORDBUFFER &recordBuffer, const ADDRESS &address, const string &fileName)
{
	vector<Data> dataVec = table.getTableVec();
	vector<Data>::iterator iter;
	vector<string> singleRecordVec;
	for (iter = dataVec.begin(); iter != dataVec.end(); iter++)
	{
		bool hasAttribute = false;
		for (auto attr : attributes)
		{
			if (iter->getAttribute() == attr)
			{
				hasAttribute = true;
				break;
			}
		}
		if (hasAttribute)
		{
			BYTE* recordData = bufferManager->fetchARecord(fileName, address + iter->getOffset());/*point to the start of the attribute*/
			string recordString = "";
			char* tmpRecordChar = new char[iter->getLength() + 1];
			int tmpRecordInt = 0;
			float tmpRecordFloat = 0;
			stringstream ss;
			switch (iter->getType())
			{
			case CHAR:
				memcpy(tmpRecordChar, recordData, iter->getLength());
				tmpRecordChar[iter->getLength()] = 0;
				recordString = tmpRecordChar;
				break;
			case INT:
				memcpy(&tmpRecordInt, recordData, sizeof(int));
				ss << tmpRecordInt;
				ss >> recordString;
				break;
			case FLOAT:
				memcpy(&tmpRecordFloat, recordData, sizeof(float));
				ss << tmpRecordFloat;
				ss >> recordString;
				break;
			default:
				break;
			}
			delete[] tmpRecordChar;
			singleRecordVec.push_back(recordString);
		}
	}
	recordBuffer.push_back(singleRecordVec);
}

/*@brief Check if a key exists in indexFile specified by param indexName
* @param indexName name of the index
* @param keyValue key to check
* @return bool true:Key exists false:Key not exists
*/
bool IndexManager::keyExists(const string &indexName, string keyValue, const int length)
{
	while (keyValue.length() < length)
		keyValue += " ";
	if (indexLibrary[indexName]->findKey(keyValue) == -1)
		return false;
	else
		return true;
}