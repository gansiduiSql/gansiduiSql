#include "Definition.h"
#include <algorithm>
using namespace std;

Table::Table(const std::string& tableName, std::vector<Data>& tableVec)
	:tableName(tableName), tableVec(tableVec)
{
	int i = 0;
	length = 0;
	int offset = 0;
	for (auto &data : this->tableVec)
	{
		if (data.isPrimary())
			this->primaryKeyIndex = i;
		this->length += data.getLength();
		data.setOffset(offset);
		offset += data.getLength();
		i++;
	}
}

void Table::setTableVec(const std::vector<Data>& tableVec)
{ 
	this->tableVec = tableVec; 
	this->length = 0;
	int offset = 0;
	for (auto& data : this->tableVec)
	{
		this->length += data.getLength();
		data.setOffset(offset);
		offset += data.getLength();
	}
}

bool Table::isAttribute(const std::string & s)
{
	auto iter = std::find_if(tableVec.begin(), tableVec.end(), [=](const Data& d)->bool {
		return d.getAttribute() == s;
	});
	return iter != tableVec.end();
}

Data Table::getData(const std::string & s)
{
	auto iter = std::find_if(tableVec.begin(), tableVec.end(), [=](const Data& d)->bool {
		return d.getAttribute() == s;
	});
	return *iter;
}

void Data::setType(TYPE type)
{ 
	this->type = type; 
	if (type == INT)
		this->length = sizeof(int);
	else if (type == FLOAT)
		this->length = sizeof(float);
}

ArrayList::ArrayList()
{
	header = 0;
	tail = BLOCKNUM - 1;
	for (int i = 0; i < BLOCKNUM; i++)
	{
		arraylist[i].next = i + 1;
		arraylist[i].last = i - 1;
	}
	arraylist[tail].next = -1;
}

ArrayList::~ArrayList(){}

int ArrayList::getHeader()
{
	return header;
}

void ArrayList::moveTail(int index)
{
	if (index == tail)
		return;
	int last = arraylist[index].last;
	int next = arraylist[index].next;

	arraylist[tail].next = index;
	arraylist[last].next = next;
	arraylist[next].last = last;
	arraylist[index].next = -1;
	arraylist[index].last = tail;
	tail = index;
	if (index == header)
		header = next;
}

OPERATOR reverseOperator(OPERATOR op)
{
	switch (op) {
	case GREATER: return LESS; break;
	case LESS:return GREATER; break;
	case EQUAL: return EQUAL; break;
	case NOTEQUAL:return NOTEQUAL; break;
	case GREATER_AND_EQUAL:return LESS_AND_EQUAL; break;
	case LESS_AND_EQUAL:return GREATER_AND_EQUAL; break;
	default:return UNDEFINED;
	}
}

TYPE stringToTYPE(std::string s)
{
	if (s == "int")
		return INT;
	else if (s == "float")
		return FLOAT;
	else if (s == "char")
		return CHAR;
	else 
		return TYPE::UNDEFINEDTYPE;
}

OPERATOR stringToOperator(std::string s)
{
	if (s == "==")
		return EQUAL;
	else if (s == ">")
		return GREATER;
	else if (s == "<")
		return LESS;
	else if (s == "!=" || s == "<>")
		return NOTEQUAL;
	else if (s == ">=")
		return GREATER_AND_EQUAL;
	else if (s == "<=")
		return LESS_AND_EQUAL;
	else return OPERATOR::UNDEFINED;
}

void Expression::swap()
{
	std::swap(this->leftOperand, this->rightOperand);
	op = reverseOperator(op);
}
