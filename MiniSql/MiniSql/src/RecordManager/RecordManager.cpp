#include "RecordManager.h"
#include <sstream>
#include <map>

using namespace std;

RecordManager::RecordManager()
{
	bmPtr = BufferManager::getBufferManager();
}

RecordManager::~RecordManager()
{

}

void RecordManager::insertValues(const std::string& tableName, const list<std::string>& values, const Table& table)
{
	ADDRESS tail = *((int *)(bmPtr->fetchARecord(tableName, 0)));
	int tableLength = table.getLength();

	BYTE* buffer = new BYTE[tableLength];
	ADDRESS offset = 0;
	vector<Data> tableVec = table.getTableVec();
	list<string>::const_iterator it = values.cbegin();
	BYTE* tmp = nullptr;
	for (auto iter : tableVec)
	{
		stringstream ss;
		ss << *it;
		int attributeLength = iter.getLength();
		switch (iter.getType())
		{
		case INT:
			int intNum;
			ss >> intNum;
			memcpy(buffer + offset, &intNum, attributeLength);
			break;
		case CHAR:
			tmp = new BYTE[attributeLength];
			ss >> tmp;
			memcpy(buffer + offset, tmp, attributeLength);
			delete[] tmp;
			break;
		case FLOAT:
			float floatNum;
			ss >> floatNum;
			memcpy(buffer + offset, &floatNum, attributeLength);
			break;
		default:
			break;
		}
		offset += attributeLength;
	}
	bmPtr->writeARecord(buffer, tableLength, tableName, tail);
	bmPtr->writeARecord((BYTE*)(tail + tableLength), 4, tableName, 0);
}

void RecordManager::deleteValues(const std::string& tableName)
{
	//move the corresponding tail to the first block
	//the data is still in the file but use to tail to devote the deletion
	bmPtr->writeARecord((BYTE*)(4096), 4, tableName, 0);
}

void RecordManager::deleteValues(const std::string& tableName, std::list<Expression>& expressions)
{

}

void RecordManager::selectValues(const std::list<std::string>& attributeNames, const std::string& tableName, const Table& table, RECORDBUFFER& recordBuffer)
{
	map<string, int> attributeOffset; //map the attribute name to the offset in the each record
	map<string, TYPE> attributeType;
	map<string, int> attributeLength;
	int offset = 0;
	vector<Data> tableVec = table.getTableVec();
	
	for (auto iter : tableVec)
	{
		string attributeName = iter.getAttribute();
		attributeOffset[attributeName] = offset;
		attributeType[attributeName] = iter.getType();
		attributeLength[attributeName] = iter.getLength();
		offset += iter.getLength();
	}

	ADDRESS tail = *((int *)(bmPtr->fetchARecord(tableName, 0)));
	ADDRESS it = BLOCKSIZE;
	while (it < tail)
	{
		BYTE* buffer = bmPtr->fetchARecord(tableName, it);
		int index = 0;
		for (auto attributeName : attributeNames)
		{
			int off = attributeOffset[attributeName];
			TYPE type = attributeType[attributeName];
			int length = attributeLength[attributeName];
			stringstream ss;
			int intNum;
			string s;
			float floatNum;
			switch (type)
			{
			case INT:
				memcpy(&intNum, buffer + off, length);
				ss << intNum;
				recordBuffer[index].push_back(ss.str());
				break;
			case CHAR:
				memcpy(&s, buffer + off, length);
				recordBuffer[index].push_back(s);
				break;
			case FLOAT:
				memcpy(&floatNum, buffer + off, length);
				ss << floatNum;
				recordBuffer[index].push_back(ss.str());
				break;
			default:
				break;
			}
			index++;
		}

		/*move it to the begin to the next record start
		*if a record can't fit in the remain size in this block
		*move the it to the begin of next block
		*cause by the record can't stored in two blocks
		*/
		it += table.getLength();
		int blockIndex = it / BLOCKSIZE;
		if (blockIndex*BLOCKSIZE - it < table.getLength())
			it = (blockIndex + 1)*BLOCKSIZE;
	}
}

void RecordManager::selectValues(const std::list<std::string>& attributeName, const std::string& tableName, std::list<Expression>& expressions, RECORDBUFFER& recordBuffer)
{

}