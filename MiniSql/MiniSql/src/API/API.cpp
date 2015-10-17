#include "API.h"

using namespace std;

void API::createTableCmd(const Table& table)
{

}

void API::dropTableCmd(const string& tableName)
{

}

void API::createIndexCmd(const string& indexName, const string& tableName, const string& attributeName)
{

}

void API::dropIndexCmd(const string& indexName)
{

}

void API::insertValuesCmd(const list<string>& values)
{

}

void API::deleteValuesCmd(const string& tableName)
{

}

void API::selectValuesCmd(const list<string> &attributeName, const string tableName)
{

}

void API::selectValuesCmd(const list<string> &attributeName, const string tableName, list<Expression> &expressions)
{

}

CatalogManager* API::getCatalogManagerPtr()
{

}