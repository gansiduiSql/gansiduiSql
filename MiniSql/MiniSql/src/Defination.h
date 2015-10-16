#include <string>

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
	string oprandName;
	bool isAttribute;
};

enum TYPE
{
	INT,
	CHAR,
	FLOAT
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