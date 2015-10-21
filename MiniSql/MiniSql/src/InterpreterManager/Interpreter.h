#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_


#include "..\API\API.h"
#include "..\CatalogManager\CatalogManager.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

class StatementBlock;

class Interpreter
{
public:
	Interpreter();
	~Interpreter();
	void execute(std::string sql);

private:
	typedef std::string::iterator Iterator;
	
	std::vector<std::string> split(std::string s, std::string::value_type c);
	void createTableParser(Iterator& begin, Iterator end);
	void createIndexParser(Iterator& begin, Iterator end);
	void dropTableParser(Iterator& begin, Iterator end);
	void dropIndexParser(Iterator& begin, Iterator end);
	void selectParser(Iterator& begin, Iterator end);
	void insertParser(Iterator& begin, Iterator end);
	void deleteParser(Iterator& begin, Iterator end);
	void quitParser(Iterator& begin, Iterator end);
	
	std::vector<std::shared_ptr<StatementBlock>> vStatementBlock;
	CatalogManager *ptrCatalogManager;
	API api;
};
#endif
