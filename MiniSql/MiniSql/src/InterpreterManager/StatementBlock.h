/*@brief
*@author tomstream@qq.com
*@date 2015/10/28
*@version 1.0
*/


/*
StatementBlock is an interface for specific statement block to contain its data. 
The StatementBlock holds the function check(), print() and execute(). 
The check function communicates with Catalog Manager and check the correctness of the data by comparing with the meta data information. 
The print function is used to debug. The execute function calls correspondent API functions.
*/
#ifndef _STATEMENTBLOCK_H_
#define _STATEMENTBLOCK_H_

#include "../Definition.h"
#include <memory>
#include <string>
#include <list>
#include <vector>

class Interpreter;
// an interface 
class StatementBlock{
public:
	virtual void execute() = 0;
	virtual void print() = 0;
	virtual void check() = 0;
	virtual ~StatementBlock() {};
};

class CreateTableBlock :public StatementBlock {
public:
	CreateTableBlock(Table table) :table(table) {};
	CreateTableBlock(Table table, std::string primaryKeyName) 
		:table(table),primaryKeyName(primaryKeyName) {};
	virtual void check();
	virtual void execute();
	virtual void print();

	virtual ~CreateTableBlock() {};
private:
	Table table;
	std::string primaryKeyName;
};

class CreateIndexBlock :public StatementBlock {
public:
	CreateIndexBlock(std::string indexName,std::string tableName, std::string attributeName) 
		:indexName(indexName),tableName(tableName),attributeName(attributeName) {};
	virtual void execute();
	virtual void check();
	virtual void print();
	virtual ~CreateIndexBlock() {};
private:
	std::string indexName;
	std::string tableName;
	std::string attributeName;
};

class InsertTableBlock :public StatementBlock {
public:
	InsertTableBlock(std::string tableName, std::vector<std::string> values) 
	:tableName(tableName),values(values) {};
	virtual void execute();
	virtual void check();
	virtual void print();
	virtual ~InsertTableBlock() {};
private:
	std::string tableName;
	std::vector<std::string> values;
};

class SelectBlock:public StatementBlock {
public:
	SelectBlock(Interpreter* const ip):star(false),doNothingFlag(false),ip(ip){}
	void setStar(bool star) { this->star = star; }
	void setTableName(std::string& tableName) { this->tableName = tableName; }
	void setAttributes(std::list<std::string>& attributes) { this-> attributes = attributes; }
	void setExpressions(std::list<Expression>& exps) { this->exps = exps; }
	virtual void execute();
	virtual void check();
	virtual void print();
	~SelectBlock(){}
private:
	bool star;
	bool doNothingFlag;
	std::list<std::string> attributes;
	std::string tableName;
	std::list<Expression> exps;
	Interpreter* const ip;
};


class QuitBlock :public StatementBlock {
public:
	QuitBlock(){}
	virtual void execute();
	virtual void check();
	virtual void print();

	virtual ~QuitBlock() {};
private:
};

class DropTableBlock :public StatementBlock {
public:
	DropTableBlock(std::string s) :tableName(s) {}
	virtual void execute();
	virtual void check();
	virtual void print();
	virtual ~DropTableBlock(){}
private:
	std::string tableName;
};

class DropIndexBlock :public StatementBlock {
public:
	DropIndexBlock(std::string s) :indexName(s) {}
	virtual void execute();
	virtual void check();
	virtual void print();

	virtual ~DropIndexBlock() {}
private:
	std::string indexName;
};


class DeleteBlock : public StatementBlock{
public:
	DeleteBlock(std::string tableName) :tableName(tableName), flag(false), doNothingFlag(false) {}
	DeleteBlock(std::string tableName, std::list<Expression> exps)
	 :tableName(tableName),exps(exps),flag(true),doNothingFlag(false){}
	 
	virtual void execute();
	virtual void check();
	virtual void print();
	virtual ~DeleteBlock(){}
private:
	std::string tableName;
	std::list<Expression> exps;
	bool flag;
	bool doNothingFlag;
};

class execBlock : public StatementBlock {
public:
	execBlock(std::string s,Interpreter* ip) :fileName(s),ip(ip){};
	virtual void execute();;
	virtual void print() {};
	virtual void check() {};
	virtual ~execBlock() {};
private:
	std::string fileName;
	Interpreter* const ip;
};


class CheckType {
public:
	CheckType(){}
	CheckType(const Table& table) :table(table) {};
	bool isType(const std::string& s, TYPE type);
	bool isString(const std::string& s);
	bool isFloat(const std::string& s);
	bool isInt(const std::string& s);
	bool isAttribute(const std::string& s);
	TYPE isWhatType(const std::string& s);
private:
	Table table;
};

template<typename T>
auto compareFunc(OPERATOR oper)->bool(*)(T, T) {
	switch (oper) {
	case GREATER:
		return [](T t1, T t2)->bool {return t1 > t2; };
	case LESS:
		return [](T t1, T t2)->bool {return t1 < t2; };
	case EQUAL:
		return [](T t1, T t2)->bool {return t1 == t2; };
	case NOTEQUAL:
		return [](T t1, T t2)->bool {return t1 != t2; };
	case GREATER_AND_EQUAL:
		return [](T t1, T t2)->bool {return t1 >= t2; };
	case LESS_AND_EQUAL:
		return [](T t1, T t2)->bool {return t1 <= t2; };
	}
	return nullptr;
}


bool compareExp(const std::string& left, const std::string& right, TYPE type, OPERATOR op);

#endif