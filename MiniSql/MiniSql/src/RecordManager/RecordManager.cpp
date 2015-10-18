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

BYTE* toByte(string& value, TYPE type)
{
	
}

void RecordManager::insertValues(const std::string& tableName, const list<std::string>& values, const Table& table)
{
	int tail = *((int *)(bmPtr->fetchARecord(tableName, 0)));
	int length;
	
	BYTE* buffer = new BYTE[length];
	int offset = 0;
	vector<Data> tableVec = table.getTableVec();
	list<string>::const_iterator it = values.cbegin();
	for (auto iter : tableVec)
	{
		stringstream ss;
		ss << *it;
		int length = iter.getLength();
		switch (iter.getType())
		{
		case INT:
			int num;
			ss >> num;
			memcpy(buffer + offset, (BYTE*)(num), length);
			break;
		case CHAR:
			BYTE* tmp = new BYTE[length];
			ss >> tmp;
			memcpy(buffer + offset, tmp, length);
			break;
		case FLOAT:
			float num;
			ss >> num;
			memcpy(buffer + offset, (BYTE*)(num), length);
			break;
		default:
			break;
		}
		offset += length;
	}

}

void RecordManager::deleteValues(const std::string& tableName)
{

}

void RecordManager::deleteValues(const std::string& tableName, std::list<Expression>& expressions)
{

}

void RecordManager::selectValues(const std::list<std::string>& attributeName, const std::string& tableName)
{

}

void RecordManager::selectValues(const std::list<std::string>& attributeName, const std::string& tableName, std::list<Expression>& expressions)
{

}