#include "CatalogManager.h"
#include "../BufferManager/BufferManager.h"
#include "../Definition.h"
#include <exception>
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
	if (buffer == NULL)
	{
		
	}
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