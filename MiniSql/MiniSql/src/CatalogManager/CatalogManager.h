/*@brief
*@author tomstream@qq.com
*@date 2015/10/28
*@version 1.0
*/


/*
Catalog Manager undertakes its responsibility to manage meta table data, involving data about table definitions
and index reliance. Catalog Manager plays its special role in interacting with both API module and Interpreter 
module, guaranteeing the consistency between input statement with stored meta data and providing necessary functions
to help other modules to get catalog information.

All the file operations taken by Catalog Manager is implemented by calling the functions provided by Buffer Manager,
and Catalog Manager does no direct manipulation on the storage devices such as disks or SSDs.

*/
#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_

#include "../Definition.h"
#include <string>
#include <set>
#include <map>

class BufferManager;

class CatalogManager{
public: 
	static CatalogManager *getCatalogManager();
	
	~CatalogManager();
	std::set<std::string> getAllTables() { return tables; }
	void createTableCatalog(const Table& table);
	void deleteTableCatalog(const std::string& tableName);
	Table getTable(const std::string& tableName);
	std::string getIndexName(const std::string& attribute, const std::string& tableName);
	std::string getFileNameFromIndexName(const std::string& indexName, const std::string& fileName);
	void createIndexCatalog(const std::string& indexName, const std::string& tableName, const std::string& attributeName);
	std::vector<std::string> getIndexVecFromTableName(const std::string& tableName);
	void deleteIndexCatalog(const std::string& indexName);
	bool isIndexExist(const std::string& indexName);
	bool isIndexExist(const std::string& tableName, const std::string& indexName);
	bool isTableExist(const std::string& tableName);
	bool checkIndexTableAttribute(const std::string& indexName, const std::string& tableName, const std::string& attributeName);
private: 
	BufferManager* bm;
	std::set<std::string> tables;
	std::map<std::string, std::map<std::string,std::string>> indices;
	std::map<std::string, Table> tableLoaded;

	CatalogManager();
	void createIndex(const std::string& indexName, const std::string& tableName, const std::string& attributeName);
	BYTE* saveData(const Data& data, BYTE* ptr);
	BYTE* readData(Data& data, BYTE* ptr);
	BYTE* readString(std::string& s, BYTE* ptr);
	BYTE* saveString(const std::string& s, BYTE* ptr);
	void regTable(const std::string& s);
	void deRegTable(const std::string& s);
	void load();
	bool isEnd(BYTE* ptr);
};

#endif
