#ifndef _API_H_
#define _API_H_

#include <string>
#include <list>
#include "..\Defination.h"
#include "..\CatalogManager\CatalogManager.h"
#include "..\RecordManager\RecordManager.h"
#include "..\IndexManager\IndexManager.h"

class API
{
private:
	CatalogManager cm;
	RecordManager rm;
	IndexManager im;
public:
	void createTable(const std::list<Data>& tableList);
	void dropTable(const std::string& tableName);
	void createIndex(const std::string& indexName, const std::string& tableName, const std::string& attributeName);
	void dropIndex(const std::string& indexName);
	void insertValues(const std::list<std::string>& values);
	void deleteValues(const std::string& tableName);
	void selectValues(const std::list<std::string>& attributeName, const std::string tableName);
	void selectValues(const std::list<std::string>& attributeName, const std::string tableName, std::list<Expression>& expressions);
	CatalogManager* getCatalogManagerPtr();
};


#endif