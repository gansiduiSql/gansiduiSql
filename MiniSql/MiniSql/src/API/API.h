#ifndef _API_H_
#define _API_H_

#include <string>
#include <list>
#include <vector>
#include <exception>
#include "..\Definition.h"
#include "..\CatalogManager\CatalogManager.h"
#include "..\RecordManager\RecordManager.h"
#include "..\IndexManager\IndexManager.h"

class PriOrUniqExistException : public exception
{
private:
	std::string errlog;
public:
	PriOrUniqExistException(std::string attributeName){ errlog = "The inserted values conflict in" + attributeName; }
	virtual const char* what(){ return errlog.c_str(); }
};

class IndexOnTheSameAttributeException : public exception
{
private:
	std::string errlog;
public:
	IndexOnTheSameAttributeException(){ errlog = "There is an index in this attribute in the table!"; }
	virtual const char* what(){ return errlog.c_str(); }
};
class API
{
private:
	CatalogManager* cmPtr;
	RecordManager* rmPtr;
	IndexManager* imPtr;
public:
	API();
	~API();
	static API* getAPIPtr();
	void createTableCmd(const Table& table);
	void dropTableCmd(const std::string& tableName);
	void createIndexCmd(const std::string& indexName, const std::string& tableName, const std::string& attributeName);
	void dropIndexCmd(const std::string& indexName);
	void insertValuesCmd(const std::list<std::string>& values, const Table& table);
	void deleteValuesCmd(const std::string& tableName);
	void deleteValuesCmd(const std::string tableName, std::list<Expression>& expressions);
	void selectValuesCmd(const std::list<std::string>& attributeName, const std::string& tableName, RECORDBUFFER& recordBuff);
	void selectValuesCmd(const std::list<std::string>& attributeName, const std::string& tableName, std::list<Expression>& expressions, RECORDBUFFER& recordBuff);
	CatalogManager* getCatalogManagerPtr();
};


#endif