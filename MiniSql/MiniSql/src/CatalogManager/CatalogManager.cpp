#include "CatalogManager.h"
#include "../BufferManager/BufferManager.h"
#include "../Definition.h"
#include <exception>
#include <cstring>
using namespace std;

CatalogManager::CatalogManager()
{
	bm = BufferManager::getBufferManager();
}

CatalogManager::~CatalogManager()
{
}

void CatalogManager::createTableCatalog(const Table& table)
{
	string tableName = table.getTableName();
	BYTE* buffer = bm->fetchARecord(tableName + ".log", 0);
	const char* cTableName = table.getTableName().c_str();
	const vector<Data>& tableVec = table.getTableVec();
	
	BYTE* tPtr = buffer;
	//store the table Name
	memcpy(tPtr, cTableName, strlen(cTableName) + 1);
	tPtr += strlen(cTableName) + 1;

	//store the Attribute
	for (auto &data : tableVec)
	{
		tPtr = saveData(data, tPtr);
	}
	*tPtr = 0xff;
}
void CatalogManager::deleteTableCatalog(const std::string& tableName)
{
	BYTE* buffer = bm->fetchARecord(tableName + ".log", 0);

	if (buffer == NULL)
		throw runtime_error("The table does not exist!");

	memset(buffer, 0, BLOCKSIZE);
}
Table CatalogManager::getTable(const std::string& tableName)
{
	BYTE* buffer = bm->fetchARecord(tableName + "log", 0);
	if (buffer == NULL)
		throw runtime_error("The table does not exist!");

	Table rTable;
	rTable.setTableName(tableName);
	
	BYTE* tPtr = buffer;
	while (*(tPtr++) == 0);

	vector<Data> tableVec;
	Data tmpData;
	int i = 0;
	while (!isEnd(tPtr))
	{
		tPtr=readData(tmpData,tPtr);
		if (tmpData.isPrimary())
		{
			rTable.setPrimaryKeyIndex(i);
		}
		tableVec.push_back(tmpData);
		i++;
	}

}
std::string	CatalogManager::getIndexName(const std::string& attribute, const std::string& fileName);
std::string	CatalogManager::getFileNameFromIndexName(const std::string& indexName, const std::string& fileName);
void CatalogManager::deleteIndexCatalog(const std::string& indexName)
{

}

BYTE* CatalogManager::saveData(const Data& data, BYTE* ptr)
{
	BYTE* tPtr = ptr;
	const char* attributeName = data.getAttribute().c_str();
	memcpy(tPtr, attributeName, strlen(attributeName)+1);
	tPtr += strlen(attributeName) + 1;
	unsigned int* iPtr = (unsigned int*)tPtr;
	*(iPtr++) = data.getType();
	*(iPtr++) = data.getLength();
	*(iPtr++) = data.isUnique();
	*(iPtr++) = data.isPrimary();
	return (BYTE*)iPtr;
}

BYTE* CatalogManager::readData(Data& data, BYTE* ptr)
{
	BYTE* tPtr = ptr;
	data.setAttribute(string((const char *)tPtr));
	while (tPtr++ != 0);
	int* iPtr = (int*)tPtr;
	data.setType((TYPE)*(iPtr++));
	data.setLength(*(iPtr++));
	data.setUnique((bool)*(iPtr++));
	data.setPrimary((bool)*(iPtr++));
	return (BYTE*)iPtr;

}

bool isEnd(BYTE* ptr)
{
	return *ptr == 0xff;
}