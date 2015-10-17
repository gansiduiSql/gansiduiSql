#ifndef _CATALOGMANAGER_H_
#define _CATALOGMANAGER_H_

class CatalogManager
{
public:
	CatalogManager();
	~CatalogManager();
	void createTable(const Table& table);
	void deleteTable(const string& tableName);
	Table getTable(const string& tableName);
};

#endif