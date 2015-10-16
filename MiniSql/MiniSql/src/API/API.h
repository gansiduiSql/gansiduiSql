#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include <string>
#include <list>
#include "RecorderManager.h"
#include "IndexManager.h"
#include "CatalogManager.h"

#include "Defination.h"


using namespace std;

class API
{
private:
	CatalogManager cm;
	RecorderManager rm;
	IndexManager im;
public:
	void createTable(const list<Data>& tableList);
	void dropTable(const string& tableName);
	void createIndex(const string& indexName, const string& tableName, const string& attributeName);
	void dropIndex(const string& indexName);
	void insertValues(const list<string>& values);
	void deleteValues(const string& tableName);
	void selectValues(const list<string> &attributeName, const string tableName);
	void selectValues(const list<string> &attributeName, const string tableName, list<Expression> &expressions);
	CatalogManager* getCatalogManagerPtr();
};


#endif