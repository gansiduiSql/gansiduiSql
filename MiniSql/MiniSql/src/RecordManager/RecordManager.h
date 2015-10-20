#ifndef _RECORDERMANAGER_H_
#define _RECORDERMANAGER_H_

#include <string>
#include <list>
#include "../BufferManager/BufferManager.h"
#include "../Definition.h"

class RecordManager
{
public:
	RecordManager();
	~RecordManager();
	void insertValues(const std::string& tableName, const std::list<std::string>& values, const Table& table);
	void deleteValues(const std::string& tableName);
	void deleteValues(const std::string& tableName, std::list<Expression>& expressions);
	void selectValues(const std::list<std::string>& attributeName, const std::string& tableName, const Table& table, RECORDBUFFER& recordBuffer);
	void selectValues(const std::list<std::string>& attributeName, const std::string& tableName, std::list<Expression>& expressions, RECORDBUFFER& recordBuffer);
private:
	BufferManager* bmPtr;
};

#endif