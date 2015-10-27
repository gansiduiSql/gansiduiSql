#ifndef _STATEMENTBLOCK_H_
#define _STATEMENTBLOCK_H_

#include "../Definition.h"
#include <memory>
#include <string>
#include <list>


// an interface 
class StatementBlock{
public:
	virtual void execute() = 0;
	virtual ~StatementBlock() {};
};

class CreateTableBlock :StatementBlock {
public:
	CreateTableBlock(Table table) :table(table) {};
	virtual void execute();

	virtual ~CreateTableBlock() {};
private:
	Table table;
};

class CreateIndexBlock :StatementBlock {
public:
	CreateIndexBlock(std::string indexName,std::string tableName, std::string attributeName) 
		:indexName(indexName),tableName(tableName),attributeName(attributeName) {};
	virtual void execute();

	virtual ~CreateIndexBlock() {};
private:
	std::string indexName;
	std::string tableName;
	std::string attributeName;
};

class InsertTableBlock :StatementBlock {
public:
	InsertTableBlock(std::string tableName, std::list<std::string> values) 
	:tableName(tableName),values(values) {};
	virtual void execute();

	virtual ~InsertTableBlock() {};
private:
	std::string tableName;
	std::list<std::string> values;
};

class SelectBlock {
public:
	SelectBlock(){}
};


class QuitBlock :StatementBlock {
public:
	QuitBlock(){}
	virtual void execute();

	virtual ~QuitBlock() {};
private:
};

class DropTableBlock : StatementBlock {
public:
	DropTableBlock(std::string s) :tableName(s) {}
	virtual void execute();

	virtual ~DropTableBlock(){}
private:
	std::string tableName;
};

class DropIndexBlock : StatementBlock {
public:
	DropIndexBlock(std::string s) :indexName(s) {}
	virtual void execute();

	virtual ~DropIndexBlock() {}
private:
	std::string indexName;
};

class DropIndexBlock : StatementBlock {
public:
	DropIndexBlock(std::string s) :indexName(s) {}
	virtual void execute();

	virtual ~DropIndexBlock() {}
private:
	std::string indexName;
};

class DeleteBlock : StatementBlock{
public:
	DeleteBlock(std::string tableName):tableName(tableName),flag(false){}
	DeleteBlock(std::string tableName, std::list<Expression> exps)
	 :tableName(tableName),exps(exps),flag(true){}
	 
	virtual void execute();
	virtual ~DeleteBlock(){}
private:
	std::string tableName;
	std::list<Expression> exps;
	bool flag;
};
#endif