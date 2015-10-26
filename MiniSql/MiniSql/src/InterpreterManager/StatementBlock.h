#ifndef _STATEMENTBLOCK_H_
#define _STATEMENTBLOCK_H_

#include <memory>
#include <string>
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
	DeleteBlock(std::string tableName, std::shared_ptr<std::list<Expression>> pel)
	 :tableName(tableName),pExpList(pel),flag(true){}
	 
	virtual void execute();
	virtual ~DeleteBlock(){}
private:
	std::string tableName;
	std::shared_ptr<std::list<Expression>> pExpList;
	bool flag;
};
#endif