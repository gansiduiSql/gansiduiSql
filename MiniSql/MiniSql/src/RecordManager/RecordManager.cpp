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
	for (auto iter : tableVec)
	{
		stringstream ss;
		ss << *it;
		int attributeLength = iter.getLength();
		switch (iter.getType())
		{
		case INT:
			int num;
			ss >> num;
			memcpy(buffer + offset, (BYTE*)(num), attributeLength);
			break;
		case CHAR:
			BYTE* tmp = new BYTE[attributeLength];
			ss >> tmp;
			memcpy(buffer + offset, tmp, attributeLength);
			delete[] tmp;
			break;
		case FLOAT:
			float num;
			ss >> num;
			memcpy(buffer + offset, (BYTE*)(num), attributeLength);
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

RECORDBUFFER RecordManager::selectValues(const std::list<std::string>& attributeName, const std::string& tableName, const Table& table)
{
	map<string, int> attributeOffset;
	int offset = 0;
	vector<Data> tableVec = table.getTableVec();
	
	for (auto iter : tableVec)
	{
		string attributeName = iter.getAttribute();
		attributeOffset[attributeName] = offset;
		offset += iter.getLength();
	}

	ADDRESS tail = *((int *)(bmPtr->fetchARecord(tableName, 0)));
	ADDRESS it = BLOCKSIZE;
	while (it < tail)
	{
		BYTE* buffer = bmPtr->fetchARecord(tableName, it);

	}
}

RECORDBUFFER RecordManager::selectValues(const std::list<std::string>& attributeName, const std::string& tableName, std::list<Expression>& expressions)
{

}