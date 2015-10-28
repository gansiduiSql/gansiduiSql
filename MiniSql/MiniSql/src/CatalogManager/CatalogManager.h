#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_

#include "../Definition.h"
#include <string>
#include <set>
class BufferManager;

class CatalogManager{
public: 
	static CatalogManager *getCatalogManager();
	CatalogManager();
	~CatalogManager();
	void createTableCatalog(const Table& table);
	void deleteTableCatalog(const std::string& tableName);
	Table getTable(const std::string& tableName);
	std::string getIndexName(const std::string& attribute, const std::string& tableName);
	std::string getFileNameFromIndexName(const std::string& indexName, const std::string& fileName);
	void createIndexCatalog(const std::string& indexName, const std::string& tableName, const std::string& attributeName);
	void deleteIndexCatalog(const std::string& indexName);
private: 
	BufferManager* bm;
	std::set<std::string> tables;
	BYTE* saveData(const Data& data, BYTE* ptr);
	BYTE* readData(Data& data, BYTE* ptr);
	BYTE* readString(std::string& s, BYTE* ptr);
	BYTE* saveString(const std::string& s, BYTE* ptr);
	void regTable(const std::string& s);
	void deRegTable(const std::string& s);
	bool isTableExist(const std::string& s);
	void loadTable();
	bool isEnd(BYTE* ptr);
};

#endif
