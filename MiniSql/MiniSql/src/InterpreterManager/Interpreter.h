#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include <string>
#include "..\API\API.h"
#include "..\CatalogManager\CatalogManager.h"
#include <list>
#include <utility>
#include <functional>



class Interpreter
{
public:
	Interpreter();
	~Interpreter();
	void execute(std::string sql);

private:
	typedef std::string::iterator Iterator;
	API api;
	CatalogManager *ptrCatalogManager;
	std::vector<std::string> split(std::string s, std::string::value_type c);
	Iterator createTableParser(Iterator& begin, Iterator end);
	Iterator createIndexParser(Iterator& begin, Iterator end);
	Iterator dropTableParser(Iterator& begin, Iterator end);
	Iterator dropIndexParser(Iterator& begin, Iterator end);
	Iterator selectParser(Iterator& begin, Iterator end);
	Iterator insertParser(Iterator& begin, Iterator end);
	Iterator deleteParser(Iterator& begin, Iterator end);
	Iterator quitParser(Iterator& begin, Iterator end);

	
};
#endif
