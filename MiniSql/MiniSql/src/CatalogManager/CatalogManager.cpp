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
	BYTE* buffer = bm->fecthARecord(tableName + ".log", 0);
	BYTE* tPtr = buffer;

	if (buffer == NULL)
	{
		throw runtime_error("The table does not exist!");
	}
	const vector<Data>& tableVec = table.getTableVec();
	for (auto &data : tableVec)
	{
		tPtr = saveData(data, tPtr);
	}
	*tPtr = 0xff;
}
void CatalogManager::deleteTableCatalog(const std::string& tableName)
{

}
Table CatalogManager::getTable(const std::string& tableName)
{

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