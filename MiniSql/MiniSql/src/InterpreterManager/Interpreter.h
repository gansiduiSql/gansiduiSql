#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include <string>
#include "..\API\API.h"
#include "..\CatalogManager\CatalogManager.h"

class Interpreter
{
public:
	Interpreter();
	~Interpreter();
	void execute(std::string sql);
private:
	API api;
	CatalogManager *ptrCatalogManager;
};

#endif