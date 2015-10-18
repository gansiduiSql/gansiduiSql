#include <string>
#include <vector>

#define BLOCKNUM 10
#define BLOCKSIZE 4096
typedef unsigned char BYTE;
typedef int ADDRESS;
typedef std::vector<std::vector<std::string>> RECORDBUFFER;

enum OPERATOR
{
	GREATER,
	LESS,
	EQUAL,
	NOTEQUAL,
	GREATER_AND_EQUAL,
	LESS_AND_EQUAL
};

struct Expression
{
	Oprand leftOprand;
	Oprand rigthOprand;
	OPERATOR op;
};

/*class Expression
{
public:
	bool isTrue();
private:
	string attributeName;
	string value;
	TYPE 

};*/

class Oprand
{
	std::string oprandName;
	bool isAttribute;
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
	Table();
	~Table();
private:
	std::string tableName;
	int primaryKeyIndex;
	std::vector<Data> tableVec;
};

class Data
{
private:
	std::string attribute;
	TYPE type;
	int length;
	bool isUnique;
	bool isPrimay;
public:
	Data(std::string attribute, TYPE type, int length, bool isUnique, bool isPrimary);
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
	void setFileName(const string& fileName);
	string getFileName();
	void setTag(ADDRESS tag);
	ADDRESS getTag();
private:
	BYTE blockData[4096];
	bool dirtyBit;
	bool isPinned;
	ADDRESS tag;
	string fileName;
};

class ArrayList
{
private:
	int head, tail;
	struct{
		int last;
		int next;
	} arraylist[BLOCKNUM];
public:
	ArrayList();
	~ArrayList();
	void moveTail(int index);
	int getHead();
};