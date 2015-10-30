/*@brief 
*@author wang_kejie@foxmail.com
*@date 2015/10/28
*@version 1.0
*/

#include "API.h"

using namespace std;

API::API()
{
	cmPtr = CatalogManager::getCatalogManager();
	rmPtr = RecordManager::getRecordMangerPtr();
	list<string> indexNames;
	for (auto tableName : cmPtr->getAllTables())
	{
		vector<string> indexName = cmPtr->getIndexVecFromTableName(tableName);
		for (auto name : indexName)
			indexNames.insert(indexNames.end(), name);
	}
	imPtr = IndexManager::getIndexManagerPtr(indexNames);
}

API::~API(){}

API* API::getAPIPtr()
{
	static API api;
	
	return &api;
}


/*@brief Create a table for the given table info
*it should create the table info via catalog manager
*it should create the index on the primary via index manager
*it should write this index info into catalog via catalog manager
*@param table The table info
*@return void
*/
void API::createTableCmd(const Table& table)
{
	string tableName = table.getTableName();
	//create table in the catalog
	cmPtr->createTableCatalog(table);

	vector<Data> tableVec = table.getTableVec();
	//for each field on the table
	for (auto field : tableVec)
	{
		if (field.isPrimary())
		{
			//create an index on the primary key
			string tableName = table.getTableName();
			string attributeName = field.getAttribute();
			imPtr->createIndex(tableName + attributeName + "idx", field, table.getLength(), tableName);
			//write the index info into catalog
			cmPtr->createIndexCatalog(tableName + attributeName + "idx", tableName, attributeName);
		}
	}
}


/*@brief drop the table
*it should drop the record file on the disk via recordManager
*it should drop the all index file on this table on the disk via indexManager
*it should delete the index info and table info via catalogManager
*@param tableName The tableName it will drop
*@return void
*/
void API::dropTableCmd(const string& tableName)
{
	rmPtr->dropTable(tableName);
	for (auto indexname : cmPtr->getIndexVecFromTableName(tableName))
	{
		imPtr->dropIndex(indexname);
		cmPtr->deleteIndexCatalog(indexname);
	}
	cmPtr->deleteTableCatalog(tableName);
}


/*@brief create an index for the given tableName on the the given arrtibuteName
*it should create an index via index manager
*it should write the index info into catalog via catalog manager
*@param indexName The index name you create
*@param tableName The table name you create index on
*@param attributeName The attributeName you create index on
*@return void
*/
void API::createIndexCmd(const string& indexName, const string& tableName, const string& attributeName)
{
	Table table = cmPtr->getTable(tableName);
	for (auto field : table.getTableVec())
	{
		//the attribute you create index on
		if (field.getAttribute() == attributeName)
		{
			if (cmPtr->isIndexExist(tableName, attributeName))
			{
				string name = cmPtr->getIndexName(attributeName, tableName);
				if (name[0] == '$')
				{
					imPtr->dropIndex(name);
					cmPtr->deleteIndexCatalog(name);
					//create the index via index manager
					imPtr->createIndex(indexName, field, table.getLength(), tableName);
					//write the index info into catalog
					cmPtr->createIndexCatalog(indexName, tableName, attributeName);
				}
				else
				{
					throw IndexOnTheSameAttributeException();
				}
			}
			else
			{
				//create the index via index manager
				imPtr->createIndex(indexName, field, table.getLength(), tableName);
				//write the index info into catalog
				cmPtr->createIndexCatalog(indexName, tableName, attributeName);
			}
		}
	}
}


/*@brief drop the index named the give index name
*it should delete the index file via index manager
*it should delete the index info via catalog manager
*@param indexName The index name you drop
*rreturn void
*/
void API::dropIndexCmd(const string& indexName)
{
	//drop the index file
	imPtr->dropIndex(indexName);
	//delete the index info from catalog
	cmPtr->deleteIndexCatalog(indexName);
}


/*@brief insert values into a table
*it should check whether it is confilic in the primary or unique key via index manager
*if it is conflic, throw an exception
*if not, insert the values into the record via recorder manager
*and insert the index values intot the index via index manager
*@param values The inserted values
*@param  The insert table infos
*@return void
*/
void API::insertValuesCmd(const list<string>& values, const Table& table)
{
	list<string>::const_iterator it = values.cbegin();
	for (auto field : table.getTableVec())
	{
		if (field.isPrimary() || field.isUnique())
		{
			if (!cmPtr->isIndexExist(table.getTableName(), field.getAttribute()))
			{
				string indexName = "$" + table.getTableName() + "$" + field.getAttribute();
				imPtr->createIndex(indexName, field, table.getLength(), table.getTableName());
				cmPtr->createIndexCatalog(indexName, table.getTableName(), field.getAttribute());
				if (imPtr->keyExists("$" + table.getTableName() + "$" + field.getAttribute(), *it, field.getLength()))
					throw PriOrUniqExistException(field.getAttribute());
			}
			else
			{
				if (imPtr->keyExists(cmPtr->getIndexName(field.getAttribute(), table.getTableName()), *it, field.getLength()))
					throw PriOrUniqExistException(field.getAttribute());
			}
		}
		it++;
	}

	ADDRESS address = rmPtr->insertValues(table.getTableName(), values, table);

	it = values.cbegin();
	for (auto field : table.getTableVec())
	{
		if (field.isPrimary() || field.isUnique())
			imPtr->insertValues(cmPtr->getIndexName(field.getAttribute(), table.getTableName()),*it, address);
		it++;
	}
}

/*@brief delete all values from the table
*it should delete all value from record via record manager
*it should delete the index via index manager
*it should delete the index log via catalog manager
*@param tableName The table name you delete from
*@return void
*/
void API::deleteValuesCmd(const string& tableName)
{
	rmPtr->deleteValues(tableName);
	for (auto indexname : cmPtr->getIndexVecFromTableName(tableName))
	{
		imPtr->deleteValuesAll(indexname);
		cmPtr->deleteIndexCatalog(indexname);
	}
}


void API::deleteValuesCmd(const std::string tableName, std::list<Expression>& expressions)
{
	list<string> primaryValues;

	list<string> indexNames;
	for (auto tableName : cmPtr->getAllTables())
	{
		vector<string> indexName = cmPtr->getIndexVecFromTableName(tableName);
		for (auto name : indexName)
			indexNames.insert(indexNames.end(), name);
	}

	std::set<string> attributeNameSet;
	bool flag = true;
	for (auto express : expressions)
	{
		if (express.leftOperand.isAttribute)
			attributeNameSet.insert(express.leftOperand.operandName);
		if (express.rightOperand.isAttribute)
			attributeNameSet.insert(express.rightOperand.operandName);
	}
	if (attributeNameSet.size() > 1)
		rmPtr->deleteValues(tableName, cmPtr->getTable(tableName), expressions, primaryValues);
	else
	{
		vector<Data> vec = cmPtr->getTable(tableName).getTableVec();
		for (auto &field : vec)
		{
			if (field.isPrimary() || field.isUnique())
			{
				if (*(attributeNameSet.begin()) == field.getAttribute())
				{
					imPtr->deleteValues(cmPtr->getIndexName(field.getAttribute(), tableName), indexNames, expressions, tableName, cmPtr->getTable(tableName).getLength());
					return;
				}
			}
		}
		rmPtr->deleteValues(tableName, cmPtr->getTable(tableName), expressions, primaryValues);
	}

	string primaryName;
	//get the primary attributeName
	for (auto field : cmPtr->getTable(tableName).getTableVec())
	{
		if (field.isPrimary())
		{
			primaryName = field.getAttribute();
			break;
		}
	}

	imPtr->deleteValues(primaryName, primaryValues, indexNames, tableName, cmPtr->getTable(tableName).getLength());
}

/*@brief select a specific list attributes from the table(select xxx, yyy from zzz(tableName))
*@param attributeName a list attribute names you select from
*@param tableName the table name you select from
*@param recordBuff the record buffer you store in the selected result
*@return void
*/
void API::selectValuesCmd(const list<string> &attributeName, const string& tableName, RECORDBUFFER& recordBuff)
{
	rmPtr->selectValues(attributeName, tableName, cmPtr->getTable(tableName), recordBuff);
}


/*
*/
void API::selectValuesCmd(const list<string> &attributeName, const string& tableName, list<Expression> &expressions, RECORDBUFFER& recordBuff)
{
	std::set<string> attributeNameSet;
	bool flag = true;
	for (auto express : expressions)
	{
		if (express.leftOperand.isAttribute)
			attributeNameSet.insert(express.leftOperand.operandName);
		if (express.rightOperand.isAttribute)
			attributeNameSet.insert(express.rightOperand.operandName);
	}
	if (attributeNameSet.size() > 1)
		rmPtr->selectValues(attributeName, tableName, cmPtr->getTable(tableName), expressions, recordBuff);
	else
	{
		vector<Data> vec = cmPtr->getTable(tableName).getTableVec();
		for (auto &field : vec)
		{
			if (field.isPrimary() || field.isUnique())
			{
				if (*(attributeNameSet.begin()) == field.getAttribute())
				{
					imPtr->selectValues(cmPtr->getIndexName(field.getAttribute(), tableName), cmPtr->getTable(tableName), expressions, recordBuff, tableName);
					return;
				}
			}
		}
		rmPtr->selectValues(attributeName, tableName, cmPtr->getTable(tableName), expressions, recordBuff);
	}
}

//return the pointer point to the catalog manager
CatalogManager* API::getCatalogManagerPtr()
{
	return cmPtr;
}