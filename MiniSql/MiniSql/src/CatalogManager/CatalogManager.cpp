#include "CatalogManager.h"
#include "../BufferManager/BufferManager.h"
#include "../Definition.h"
#include "../Exception.h"
#include <cstring>
using namespace std;
#define FIX_LENGTH 16
/*
@name: getCatalogManager
@function: return a unique ptr to CatalogManager
*/
CatalogManager * CatalogManager::getCatalogManager()
{
	static CatalogManager cm;
	return &cm;
}

/*
@name: CatalogManager
@func: get ptr to bufferManager and load table info and index info
*/
CatalogManager::CatalogManager()
{
	bm = BufferManager::getBufferManager();
	load();
}


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

CatalogManager::~CatalogManager()
{
}

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
}
void CatalogManager::deleteTableCatalog(const std::string& tableName)
{
	tableLoaded.erase(tableName);
	deRegTable(tableName);
	bm->deleteFile(tableName + ".log");
}
Table CatalogManager::getTable(const std::string& tableName)
{
	if (tableLoaded.find(tableName) != tableLoaded.end()) 
		return tableLoaded[tableName];
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

	Table ret(tableName, tableVec);
	tableLoaded[tableName] = ret;
	return ret;
}
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

void CatalogManager::createIndexCatalog(const std::string & indexName, const std::string & tableName, const std::string & attributeName)
{
	string s;
	if (isIndexExist(indexName))
		throw CatalogError("this index exists");
	s = this->getIndexName(attributeName, tableName);
	if (s[0] == '$')
			deleteIndexCatalog(s);

	createIndex(indexName, tableName, attributeName);
}
vector<std::string> CatalogManager::getIndexVecFromTableName(const std::string & tableName)
{
	vector<string> vRet;
	if (indices.find(tableName) == indices.end())
		return vRet;
	auto mapTables = indices[tableName];
	for (auto& content : mapTables) {
		vRet.push_back(content.first);
	}
	return vRet;
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
		indices.erase(indexName);
	}
	else
	throw CatalogError("This index does not exist.");

	bm->writeARecord(buffer, BLOCKSIZE, "index.index", 0);
	indices.erase(indexName);
}

bool CatalogManager::isIndexExist(const std::string & indexName)
{
	return indices.find(indexName) != indices.end();
}

bool CatalogManager::isIndexExist(const std::string & tableName, const std::string & attrName)
{
	for (auto& map1 : indices) {
		for (auto& map2 : map1.second) {
			if (map1.first[0]!='$'&& map2.first == tableName&& map2.second == attrName) {
				return true;
			}
		}
	}
	return false;
}

bool CatalogManager::checkIndexTableAttribute(const std::string & indexName, const std::string & tableName, const std::string & attributeName)
{
	if (indices.find(indexName) != indices.end()) {
		auto& tmp = indices[indexName];
		if (tmp.find(tableName) != tmp.end())
			return true;
	}
	return false;
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

bool CatalogManager::isEnd(BYTE* ptr)
{
	return *ptr == 0xff;
}

BYTE* CatalogManager::readString(std::string& s, BYTE* ptr)
{
	s = string((char*)ptr);
	return ptr + FIX_LENGTH;
}

BYTE * CatalogManager::saveString(const std::string & s, BYTE* ptr)
{
	strcpy((char*)ptr, s.c_str());
	return ptr + FIX_LENGTH;
}

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

