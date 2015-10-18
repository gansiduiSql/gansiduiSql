#include "RecordManager.h"

using namespace std;

RecordManager::RecordManager()
{
	bmPtr = BufferManager::getBufferManager();
}

RecordManager::~RecordManager()
{

}

void RecordManager::insertValues(const std::string& tableName, const list<std::string>& values)
{

}

void RecordManager::deleteValues(const std::string& tableName)
{

}

void RecordManager::deleteValues(const std::string& tableName, std::list<Expression>& expressions)
{

}

void RecordManager::selectValues(const std::list<std::string>& attributeName, const std::string& tableName)
{

}

void RecordManager::selectValues(const std::list<std::string>& attributeName, const std::string& tableName, std::list<Expression>& expressions)
{

}