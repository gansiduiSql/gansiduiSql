#include "CatalogManager.h"
#include "../Definition.h"

using namespace std;

CatalogManager::CatalogManager()
{
}

CatalogManager::~CatalogManager()
{
}

void CatalogManager::createTableCatalog(const Table& table)
{
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