#include "RecordManager.h"
#include <sstream>

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
	bmPtr->writeARecord((BYTE*)(4096), 4, tableName, 0);
}

void RecordManager::deleteValues(const std::string& tableName, std::list<Expression>& expressions)
{

}

RECORDBUFFER RecordManager::selectValues(const std::list<std::string>& attributeName, const std::string& tableName, const Table& table)
{

}

RECORDBUFFER RecordManager::selectValues(const std::list<std::string>& attributeName, const std::string& tableName, std::list<Expression>& expressions)
{

}