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
	Table(const std::string& tableName, std::vector<Data>& tableVec, int primaryKeyIndex)
		:tableName(tableName), tableVec(tableVec), primaryKeyIndex(primaryKeyIndex)
	{}
	~Table();
private:
	std::string tableName;
	std::vector<Data> tableVec;
	int primaryKeyIndex;
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
	Data(std::string attribute, TYPE type, int length, bool isUnique, bool isPrimary);
	std::string getAttribute() { return attribute; }
	void setAttribute(std::string& attr) { attribute = attr; }
	int getLength() { return length; }
	void setLength(int len) { length = len; }
	bool isUnique() { return uniqueFlag; }
	void setUnique(bool flag) { uniqueFlag = flag; }
	bool isPrimary() { return primaryFlag; }
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
	void setFileName(const string& fileName);
	string getFileName();
	void setTag(ADDRESS tag);
	ADDRESS getTag();
private:
	BYTE blockData[BLOCKSIZE];
	bool dirtyBit;
	bool isPinned;
	ADDRESS tag;
	string fileName;
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