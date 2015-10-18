#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_

#include "Definition.h"
#include <string>

class CatalogManager
{
public:
	CatalogManager();
	~CatalogManager();
	void createTableCatalog(const Table& table);
	void deleteTableCatalog(const std::string& tableName);
	Table getTable(const std::string& tableName);
	std::string getIndexName(std::string& attribute, std::string& fileName);
	std::string getFileNameFromIndexName(const std::string& indexName, const std::string& fileName);
	void deleteIndexCatalog(const std::string& indexName);
private: 
	BufferManager* bm;
	void saveData(const Data& data, BYTE* ptr);
};



#endif
