#include "CatalogManager.h"
#include "../BufferManager/BufferManager.h"
#include "../Definition.h"
#include <exception>
#include <cstring>
using namespace std;
#define FIX_LENGTH 64

CatalogManager * CatalogManager::getCatalogManager()
{
	static CatalogManager cm;
	return &cm;
}

CatalogManager::CatalogManager()
{
	bm = BufferManager::getBufferManager();
	bm->createFile("index.index");
}

CatalogManager::~CatalogManager()
{
}

void CatalogManager::createTableCatalog(const Table& table)
{
	string tableName = table.getTableName();
	//create a table data file
	bm->createFile(tableName);
	int tail = BLOCKSIZE;
	bm->writeARecord((BYTE*)&tail, sizeof(int), tableName, 0);
	//create a table log data file
	bm->createFile(tableName + "log");
	BYTE* buffer = bm->fetchARecord(tableName + ".log", 0);
	
	const char* cTableName = table.getTableName().c_str();
	const vector<Data>& tableVec = table.getTableVec();
	
	BYTE* tPtr = buffer;
	//store the table Name
	memcpy(tPtr, cTableName, strlen(cTableName) + 1);
	tPtr += FIX_LENGTH;

	//store the Attribute
	for (auto &data : tableVec)
	{
		tPtr = saveData(data, tPtr);
	}
	*tPtr = 0xff;
}
void CatalogManager::deleteTableCatalog(const std::string& tableName)
{
	bm->deleteFile(tableName + ".log");
}
Table CatalogManager::getTable(const std::string& tableName)
{
	BYTE* buffer = bm->fetchARecord(tableName + ".log", 0);
	
	if (buffer == NULL)
		throw runtime_error("The table does not exist!");
	
	BYTE* tPtr = buffer;
	tPtr += FIX_LENGTH;

	vector<Data> tableVec;
	Data tmpData;

	while (!isEnd(tPtr))
	{
		tPtr=readData(tmpData,tPtr);
		tableVec.push_back(tmpData);
	}
	return Table(tableName, tableVec);
}
std::string	CatalogManager::getIndexName(const std::string& attribute, const std::string& tableName)
{
	BYTE* buffer = bm->fetchARecord("index.index", 0);
	BYTE* tPtr = buffer;
	string sIndexName,sAttributeName,sTableName;
	while (!isEnd(tPtr))
	{
		tPtr = readString(sIndexName, tPtr);
		tPtr = readString(sTableName, tPtr);
		tPtr = readString(sAttributeName, tPtr);
		if (sTableName == tableName&&sAttributeName == attribute)
			return sIndexName;
	}
	throw runtime_error("This index does not exist.");
	return "";
}
std::string	CatalogManager::getFileNameFromIndexName(const std::string& indexName, const std::string& tableName)
{
	BYTE* buffer = bm->fetchARecord("index.index", 0);
	BYTE* tPtr = buffer;
	string sIndexName, sAttributeName, sTableName;
	while (!isEnd(tPtr))
	{
		tPtr = readString(sIndexName, tPtr);
		tPtr = readString(sTableName, tPtr);
		tPtr = readString(sAttributeName, tPtr);
		if (sTableName == tableName&&indexName == sIndexName)
			return tableName;
	}
	throw runtime_error("This file does not exist.");
	return "";
}
void CatalogManager::deleteIndexCatalog(const std::string& indexName)
{
	BYTE* buffer = bm->fetchARecord("index.index", 0);
	BYTE* tPtr = buffer, *deletedPtr, *headPtr;
	string sIndexName, sAttributeName, sTableName;
	bool flag = false;
	headPtr = deletedPtr= 0;
	while (!isEnd(tPtr))
	{
		headPtr = tPtr;
		tPtr = readString(sIndexName, tPtr);
		tPtr = readString(sTableName, tPtr);
		tPtr = readString(sAttributeName, tPtr);
		if (!flag && sIndexName == indexName)
		{
			deletedPtr = headPtr;
			flag = true;
			break;
		}
	}
	if (flag)
	{
		while (!isEnd(tPtr))
		{
			headPtr = tPtr;
			tPtr += FIX_LENGTH * 3;
		}
		if(headPtr==tPtr)
			memcpy(deletedPtr, headPtr, FIX_LENGTH * 3);
		*headPtr = 0xff;
	}
	else
	throw runtime_error("This index does not exist.");
}

BYTE* CatalogManager::saveData(const Data& data, BYTE* ptr)
{
	BYTE* tPtr = ptr;
	const char* attributeName = data.getAttribute().c_str();
	memcpy(tPtr, attributeName, strlen(attributeName)+1);
	tPtr += FIX_LENGTH;
	unsigned int* iPtr = (unsigned int*)tPtr;
	*(iPtr++) = data.getType();
	*(iPtr++) = data.getLength();
	*(iPtr++) = data.isUnique();
	*(iPtr++) = data.isPrimary();
	return (BYTE*)iPtr;
}

BYTE* CatalogManager::readData(Data& data, BYTE* ptr)
{
	//attributeNameCannotMoreThan64bytes
	BYTE* tPtr = ptr;
	data.setAttribute(string((const char *)tPtr));
	tPtr += FIX_LENGTH;
	int* iPtr = (int*)tPtr;
	data.setType((TYPE)*(iPtr++));
	data.setLength(*(iPtr++));
	data.setUnique((bool)*(iPtr++));
	data.setPrimary((bool)*(iPtr++));
	return (BYTE*)iPtr;
}

bool CatalogManager::isEnd(BYTE* ptr)
{
	return *ptr == 0xff;
}

BYTE* CatalogManager::readString(std::string& s, BYTE* ptr)
{
	s = string((char*)ptr);
	return ptr + FIX_LENGTH;
}