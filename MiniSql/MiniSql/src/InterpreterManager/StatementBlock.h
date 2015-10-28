#ifndef _STATEMENTBLOCK_H_
#define _STATEMENTBLOCK_H_

#include "../Definition.h"
#include <memory>
#include <string>
#include <list>
#include <vector>


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
	SelectBlock():star(false){}
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
	std::list<std::string> attributes;
	std::string tableName;
	std::list<Expression> exps;
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
	DeleteBlock(std::string tableName):tableName(tableName),flag(false){}
	DeleteBlock(std::string tableName, std::list<Expression> exps)
	 :tableName(tableName),exps(exps),flag(true){}
	 
	virtual void execute();
	virtual void check();
	virtual void print();
	virtual ~DeleteBlock(){}
private:
	std::string tableName;
	std::list<Expression> exps;
	bool flag;
};

class CheckType {
public:
	CheckType():CheckType(nullptr){}
	CheckType(Table* table) :pTable(table) {};
	bool isType(const std::string& s, TYPE type);
	bool isString(const std::string& s);
	bool isFloat(const std::string& s);
	bool isInt(const std::string& s);
	bool isAttribute(const std::string& s);
	TYPE isWhatType(const std::string& s);
private:
	Table* pTable;
};



#endif