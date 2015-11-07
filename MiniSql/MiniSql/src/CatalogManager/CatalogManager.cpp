/*@brief
*@author tomstream@qq.com
*@date 2015/10/28
*@version 1.0
*/

#include "CatalogManager.h"
#include "../BufferManager/BufferManager.h"
#include "../Definition.h"
#include "../Exception.h"
#include <cstring>
using namespace std;
#define FIX_LENGTH 16
/*
@name: getCatalogManager
@brief: return a unique ptr to CatalogManager
*/
CatalogManager * CatalogManager::getCatalogManager()
{
	static CatalogManager cm;
	return &cm;
}

/*
@name: CatalogManager
@brief: get ptr to bufferManager and load table info and index info
*/
CatalogManager::CatalogManager()
{
	bm = BufferManager::getBufferManager();
	load();
}

/*
@name:	createIndex
@brief: create index on tablename and its attribute, and name it indexName
@in:	indexName tableName attributeName
@out:	void
*/
void CatalogManager::createIndex(const std::string & indexName, const std::string & tableName, const std::string & attributeName)
{
	BYTE* buffer = bm->fetchARecord("index.index", 0);
	BYTE* tPtr = buffer;
	string sIndexName, sAttributeName, sTableName;
	while (!isEnd(tPtr))
	{
		tPtr = readString(sIndexName, tPtr);
		tPtr = readString(sTableName, tPtr);
		tPtr = readString(sAttributeName, tPtr);
		if (sIndexName == indexName/* || sTableName == tableName&&sAttributeName == attributeName*/)
			throw CatalogError("cannot create the index(" + indexName + ") ");
	}
	tPtr = saveString(indexName, tPtr);
	tPtr = saveString(tableName, tPtr);
	tPtr = saveString(attributeName, tPtr);
	*tPtr = 0xff;

	bm->writeARecord(buffer, BLOCKSIZE, "index.index", 0);
	indices[indexName][tableName] = attributeName;
}

/*
@name:	 ~CatalogManager
@brief:	destructor
@in:	none
@out:	none
*/
CatalogManager::~CatalogManager()
{
}

/*
@name:	createTableCatalog	
@brief:	create table and its catalog
@in:	table
@out:	void
*/
void CatalogManager::createTableCatalog(const Table& table)
{
	string tableName = table.getTableName();
	regTable(tableName);

	//create a table data file
	bm->createFile(tableName);
	int tail = BLOCKSIZE;
	bm->writeARecord((BYTE*)&tail, sizeof(int), tableName, 0);
	//create a table log data file
	bm->createFile(tableName + ".log");
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
	bm->writeARecord(buffer, BLOCKSIZE, tableName + ".log", 0);
	tableLoaded[table.getTableName()] = table;
}

/*
@name:	deleteTableCatalog
@brief:	delete table and its catalog
@in:	tableName
@out:	void
*/
void CatalogManager::deleteTableCatalog(const std::string& tableName)
{
	
	deRegTable(tableName);
	bm->deleteFile(tableName + ".log");
	tableLoaded.erase(tableName);
}
Table CatalogManager::getTable(const std::string& tableName)
{
	if (tableLoaded.find(tableName) != tableLoaded.end()) 
		return tableLoaded[tableName];
	if (!isTableExist(tableName))
		throw CatalogError("The table(" + tableName + ") does not exist");
	BYTE* buffer = bm->fetchARecord(tableName + ".log", 0);
	if (buffer == NULL)
		throw CatalogError("The table does not exist!");
	BYTE* tPtr = buffer;
	tPtr += FIX_LENGTH;

	vector<Data> tableVec;
	Data tmpData;
	while (!isEnd(tPtr))
	{
		tPtr=readData(tmpData,tPtr);
		tableVec.push_back(tmpData);
	}

	Table ret(tableName, tableVec);
	tableLoaded[tableName] = ret;
	return ret;
}


/*
@name:	getIndexName
@brief:	get an index name from attribute and tableName
@in:	attribute name , table name
@out:	table name(string)
@throw:	CatalogError
*/
std::string	CatalogManager::getIndexName(const std::string& attribute, const std::string& tableName)
{
	for (auto& map1 : indices) {
		for (auto& map2 : map1.second) {
			if (map2.first == tableName&& map2.second == attribute) {
				return map1.first;
			}
		}
	}
	throw CatalogError("This index does not exist.");
	return "";
}
std::string	CatalogManager::getFileNameFromIndexName(const std::string& indexName, const std::string& tableName)
{
	BYTE* buffer = bm ->fetchARecord("index.index", 0);
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
	throw CatalogError("This file does not exist.");
	return "";
}
/*
@name:	createIndexCatalog
@brief:	create index catalog
@in:	indexName tableName attributeName
@out:	void
@throw:	
*/
void CatalogManager::createIndexCatalog(const std::string & indexName, const std::string & tableName, const std::string & attributeName)
{
	createIndex(indexName, tableName, attributeName);
}

/*
@name:	getIndexVecFromTableName
@brief:	give a tableName and return a vector involving all indexName on this table
@in:	tableName
@out:	container of indexName(std::vector<std::string>)
@throw:	none
*/
vector<std::string> CatalogManager::getIndexVecFromTableName(const std::string & tableName)
{
	vector<string> vRet;
	for (auto &content1 : indices) {
		for (auto& content2 : content1.second) {
			if (content2.first == tableName) {
				vRet.push_back(content1.first);
				break;
			}
		}
	}
	return vRet;
}

/*
@name:	delete index catalog
@brief:	delete index catalog
@in:	indexName
@out:	void
*/
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
		if(headPtr!=deletedPtr)
			memcpy(deletedPtr, headPtr, FIX_LENGTH * 3);
		*headPtr = 0xff;
		indices.erase(indexName);
	}
	else
	throw CatalogError("This index does not exist.");

	bm->writeARecord(buffer, BLOCKSIZE, "index.index", 0);
	indices.erase(indexName);
}
/*
@name:	isIndexExist
@brief:	check if a index exists giving an indexName
@in:	indexName
@out:	bool
@throw: none
*/
bool CatalogManager::isIndexExist(const std::string & indexName)
{
	return indices.find(indexName) != indices.end();
}

/*
@name:	isIndexExist
@brief:	giving a table name and its attribute name and check if the index exists
@in:	tableName attrName
@out:	bool
@throw:	none
*/
bool CatalogManager::isIndexExist(const std::string & tableName, const std::string & attrName)
{
	for (auto& map1 : indices) {
		for (auto& map2 : map1.second) {
			if (map2.first == tableName&& map2.second == attrName) {
				return true;
			}
		}
	}
	return false;
}
/*
@name:	checkIndexTableAttribute
@brief:	check if an index on an attribute of a table exists
@in:	indexName tableName attibuteName
@out:	bool
@throw:	none
*/
bool CatalogManager::checkIndexTableAttribute(const std::string & indexName, const std::string & tableName, const std::string & attributeName)
{
	if (indices.find(indexName) != indices.end()) {
		auto& tmp = indices[indexName];
		if (tmp.find(tableName) != tmp.end())
			return true;
	}
	return false;
}
/*
@name:	saveData
@brief:	save the data to a buffer
@in:	data:the data to write ptr:the start ptr to write in the buffer
@out:	BYTE* (address just after the address written data on)
@throw:	none
*/
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
/*
@name:	readData
@brief:	read the data to a buffer
@in:	data:the data to write ptr:the start ptr to read in the buffer
@out:	BYTE* (address just after the address read data on)
@throw:	none
*/
BYTE* CatalogManager::readData(Data& data, BYTE* ptr)
{
	//attributeNameCannotMoreThan16bytes
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
/*
@name:	load
@brief:	load table catalog and index catalog
@in:	none
@out:	none
@throw:	none
*/
void CatalogManager::load()
{
	BYTE* buffer = bm->fetchARecord("table.table", 0);
	BYTE* tPtr = buffer;
	string tableName;
	while (!isEnd(tPtr)) {
		tPtr = readString(tableName, tPtr);
		tables.insert(tableName);
	}
	buffer = bm->fetchARecord("table.table", 0);
	 buffer = bm->fetchARecord("index.index", 0);
	tPtr = buffer;
	string sIndexName, sAttributeName, sTableName;
	while (!isEnd(tPtr))
	{
		tPtr = readString(sIndexName, tPtr);
		tPtr = readString(sTableName, tPtr);
		tPtr = readString(sAttributeName, tPtr);
		indices[sIndexName][sTableName] = sAttributeName;
	}
}
/*
@name:	isEnd
@brief:	check if the ptr point to end flag
@in:	ptr(the address to check)
@out:	
*/
bool CatalogManager::isEnd(BYTE* ptr)
{
	return *ptr == 0xff;
}

/*
@name:	readString
@brief:	read a string
@in:	s(the string read), ptr(current position)
@out:	ptr(next ptr)
*/
BYTE* CatalogManager::readString(std::string& s, BYTE* ptr)
{
	s = string((char*)ptr);
	return ptr + FIX_LENGTH;
}

/*
@name:	saveString
@brief:	save a string
@in:	s(the string to save), ptr(current position)
@out:	ptr(next ptr)
*/
BYTE * CatalogManager::saveString(const std::string & s, BYTE* ptr)
{
	strcpy((char*)ptr, s.c_str());
	return ptr + FIX_LENGTH;
}

/*
@name:	regTable
@brief:	register table
@in:	s(table name)
@out:	void
*/
void CatalogManager::regTable(const std::string & s)
{
	if (isTableExist(s))throw CatalogError(s+":table name does exist");
	tables.insert(s);
	BYTE* buffer = bm->fetchARecord("table.table", 0);
	BYTE* tPtr = buffer;
	while(!isEnd(tPtr)){
		tPtr += FIX_LENGTH;
	}
	strcpy((char *)tPtr, s.c_str());
	tPtr += FIX_LENGTH;
	*tPtr = 0Xff;
	bm->writeARecord(buffer, BLOCKSIZE,  "table.table", 0);
}

/*
@name:	deRegTable
@brief:	deRegister table
@in:	s(table name)
@out:	void
*/
void CatalogManager::deRegTable(const std::string & s)
{
	if (!isTableExist(s))throw CatalogError(s + ":table name does not exist");
	tables.erase(s);
	BYTE* buffer = bm->fetchARecord("table.table", 0);
	BYTE* tPtr = buffer;
	BYTE* deletePtr = nullptr;
	string tableName;
	while (!isEnd(tPtr)) {
		tPtr = readString(tableName, tPtr);
		if (s == tableName) {
			deletePtr = tPtr - FIX_LENGTH;
		}
	}
	BYTE* endPtr = tPtr - FIX_LENGTH;
	strcpy((char *)deletePtr, (char *)endPtr);
	*endPtr = 0xff;
	bm->writeARecord(buffer, BLOCKSIZE, "table.table", 0);
}

inline bool CatalogManager::isTableExist(const std::string & s)
{
	return tables.find(s) != tables.end();
}

