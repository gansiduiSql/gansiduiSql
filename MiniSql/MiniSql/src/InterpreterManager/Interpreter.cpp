#include "Interpreter.h"
#include "../Exception.h"
#include "Functor.h"
#include <sstream>
using namespace std;
typedef std::string::iterator Iterator;

Interpreter::Interpreter()
{
	ptrCatalogManager = api.getCatalogManagerPtr();
}

std::vector<std::string> Interpreter::split(std::string s, std::string::value_type c)
{
	stringstream ss(s);
	vector<string> ret;
	string tmp;
	while (getline(ss,tmp,c))
	{
		ret.push_back(tmp);
	}
	return move(ret);
}




void Interpreter::execute(string sql){
	string s;
	Iterator begin = s.begin();
	Iterator end = s.end();
	s =	readWord(begin, end);
	if (s == "select")
		selectParser(begin, end);
	else if (s == "insert")
		insertParser(begin, end);
	else if (s == "delete")
		deleteParser(begin, end);
	else if(s == "quit")
		quitParser(begin, end);
	else if(s == "create"){
		s = readWord(begin, end);
		if(s == "table")
			createTableParser(begin, end);
		else if(s=="index")
			createIndexParser(begin, end);
		else throw GrammarError("Syntax error in create opertion");
	}
	else if(s == "drop")
	{
		s = readWord(begin, end);
		if(s == "table")
			dropTableParser(begin, end);
		else if(s == "Index")
			dropIndexParser(begin, end);
		else throw GrammarError("Syntax error in drop operation");
	}
	else throw GrammarError("Undefined operation");
}
Iterator Interpreter::createTableParser(Iterator& begin, Iterator end){
	
}
Iterator Interpreter::createIndexParser(Iterator& begin, Iterator end);
Iterator Interpreter::dropTableParser(Iterator& begin, Iterator end);
Iterator Interpreter::dropIndexParser(Iterator& begin, Iterator end);
Iterator Interpreter::selectParser(Iterator& begin, Iterator end);
Iterator Interpreter::insertParser(Iterator& begin, Iterator end);
Iterator Interpreter::deleteParser(Iterator& begin, Iterator end);
Iterator Interpreter::quitParser(Iterator& begin, Iterator end);