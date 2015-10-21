#ifndef _STATEMENTBLOCK_H_
#define _STATEMENTBLOCK_H_

#include <memory>
class Table;

// an interface 
class StatementBlock{
public:
	virtual void execute() = 0;
	virtual ~StatementBlock() {};
};

class CreateTableBlock :StatementBlock {
public:
	CreateTableBlock(std::shared_ptr<Table> p) :pTable(p) {};
	virtual void execute();

	virtual ~CreateTableBlock() {};
private:
	std::shared_ptr<Table> pTable;
};

class InsertTableBlock :StatementBlock {
public:
	InsertTableBlock(std::shared_ptr<Table> p) :pTable(p) {};
	virtual void execute();

	virtual ~InsertTableBlock() {};
private:
	std::shared_ptr<Table> pTable;
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
#endif