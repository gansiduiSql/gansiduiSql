#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include "API.h"
#include "CatalogManager.h"

class Interpreter
{
public:
	Interpreter();
	~Interpreter();
	void execute(string sql);
private:
	API api;
	CatalogManager *ptrCatalogManager;
};

#endif


main()
{
	string s;
	cin >> s;

	interpreter in;
	in.inter(s);
}