#ifndef _DEFINITION_H_
#define _DEFINITION_H_

#include <string>
#include <vector>

#define BLOCKNUM 10
#define BLOCKSIZE 4096
typedef unsigned char BYTE;
typedef int ADDRESS;
typedef std::vector<std::vector<std::string>> RECORDBUFFER;

struct Expression;
struct Operand;
class Table;
class Data;
class Block;
class ArrayList;

enum OPERATOR
{
	GREATER,
	LESS,
	EQUAL,
	NOTEQUAL,
	GREATER_AND_EQUAL,
	LESS_AND_EQUAL
};

struct Operand
{
	std::string oprandName;
	bool isAttribute;
};

struct Expression
{
	Operand leftOperand;
	Operand rightOperand;
	OPERATOR op;
};




enum TYPE
{
	INT,
	CHAR,
	FLOAT
};

class Table
{
public:
	Table(){}
	Table(const std::string& tableName, std::vector<Data>& tableVec);
	const std::string& getTableName()const { return tableName; }
	void setTableName(const std::string& tableName) { this->tableName = tableName; }
	const std::vector<Data>& getTableVec()const { return tableVec; }
	void setTableVec(const std::vector<Data>& tableVec) { this->tableVec = tableVec; }
	int getPrimaryKeyIndex()const { return primaryKeyIndex; }
	void setPrimaryKeyIndex(int primaryKeyIndex) { this->primaryKeyIndex = primaryKeyIndex; }
	int getLength()const { return length; }
	~Table();
private:
	std::string tableName;
	std::vector<Data> tableVec;
	int primaryKeyIndex;
	int length;
};

class Data
{
private:
	std::string attribute;
	TYPE type;
	int length;
	bool uniqueFlag;
	bool primaryFlag;
public:
	Data(){}
	Data(const std::string& attribute, TYPE type, int length, bool isUnique, bool isPrimary);
	const std::string& getAttribute()const { return attribute; }
	void setAttribute(const std::string& attr) { attribute = attr; }
	TYPE getType()const { return type; }
	void setType(TYPE type) { this->type = type; }
	int getLength()const{ return length; }
	void setLength(int len) { length = len; }
	bool isUnique()const { return uniqueFlag; }
	void setUnique(bool flag) { uniqueFlag = flag; }
	bool isPrimary()const { return primaryFlag; }
	void setPrimary(bool flag) { primaryFlag = flag; }

	~Data();
};
class Block
{
public:
	Block();
	~Block();
	void setBlockData(BYTE* data);
	BYTE* getBlockData();
	void setDirtyBit(bool isDirty);
	bool getDirtyBit();
	void setPinnedBit(bool pinnnedBit);
	bool getPinnnedBit();
	void setFileName(const std::string& fileName);
	std::string getFileName();
	void setTag(ADDRESS tag);
	ADDRESS getTag();
private:
	BYTE blockData[BLOCKSIZE];
	bool dirtyBit;
	bool isPinned;
	ADDRESS tag;
	std::string fileName;
};

class ArrayList
{
private:
	int header, tail;
	struct{
		int last;
		int next;
	} arraylist[BLOCKNUM];
public:
	ArrayList();
	~ArrayList();
	void moveTail(int index);
	int getHeader();
};

#endif