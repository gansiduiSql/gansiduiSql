#ifndef _API_H_
#define _API_H_

#include <string>
#include <list>
#include <vector>
#include "..\Definition.h"
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
	void createTableCmd(const Table& table);
	void dropTableCmd(const std::string& tableName);
	void createIndexCmd(const std::string& indexName, const std::string& tableName, const std::string& attributeName);
	void dropIndexCmd(const std::string& indexName);
	void insertValuesCmd(const std::list<std::string>& values);
	void deleteValuesCmd(const std::string& tableName);
	void deleteValuesCmd(const std::string tableName, std::list<Expression>& expressions);
	void selectValuesCmd(const std::list<std::string>& attributeName, const std::string tableName);
	void selectValuesCmd(const std::list<std::string>& attributeName, const std::string tableName, std::list<Expression>& expressions);
	CatalogManager* getCatalogManagerPtr();
};


#endif