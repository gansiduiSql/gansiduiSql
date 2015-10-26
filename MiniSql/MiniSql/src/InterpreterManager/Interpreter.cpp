#include "Interpreter.h"
#include "../Exception.h"
#include "../CatalogManager/CatalogManager.h"
#include "StatementBlock.h"
#include "Functor.h"
#include <sstream>
#include <memory>
using namespace std;
typedef std::string::iterator Iterator;

Interpreter::Interpreter(){
	ptrCatalogManager = CatalogManager::getCatalogManager();
}

std::vector<std::string> Interpreter::split(std::string s, std::string::value_type c){
	stringstream ss(s);
	vector<string> ret;
	string tmp;
	while (getline(ss,tmp,c)){
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
	else if(s == "drop"){
		s = readWord(begin, end);
		if(s == "table")
			dropTableParser(begin, end);
		else if(s == "Index")
			dropIndexParser(begin, end);
		else throw GrammarError("Syntax error in drop operation");
	}
	else throw GrammarError("Undefined operation: "+s);
}
void Interpreter::createTableParser(Iterator& begin, Iterator end){
	string s;
	shared_ptr<Table> pTable(new Table);
	Data tmpData;

	s = readWord(begin, end, IsVariableName());
	pTable->setTableName(s);

	s = readWord(begin, end, IsString("{"));

	int state = 0;
	for (;;){
		//set attribute name
		s = readWord(begin, end, IsVariableName());
		tmpData.setAttribute(s);
		
		//read type
		s = readWord(begin, end);
		TYPE type = stringToTYPE(s);
		if (type == UNDEFINED)
			throw GrammarError("undefined type: " + s);
		else if (type == CHAR) {
			readWord(begin, end, IsString("("));
			s = readWord(begin, end, IsNum());
			try {
				int i = stoi(s);
				if (i <= 0)
					throw GrammarError("Cannot define the length of a string less than 1");
				tmpData.setType(CHAR);
				tmpData.setLength(i);
			}catch(invalid_argument e){
				throw GrammarError("The length of a string should be a integer!");
			}
			readWord(begin, end, IsString(")"));
		}else {
			tmpData.setType(type);
		}
		pTable->pushData(tmpData);

		//check if the end is comming
		s = readWord(begin, end, IsChar('}'));
		if (s == "}") {
			break;
		}else {
			s = readWord(begin, end, IsString(","));
		}
	}
	shared_ptr<StatementBlock> pSB(new CreateTableBlock(pTable));
	vStatementBlock.push_back(pSB);
}
void Interpreter::createIndexParser(Iterator& begin, Iterator end) {
	readWord(begin, end, IsVariableName());
}
void Interpreter::dropTableParser(Iterator& begin, Iterator end) {
	auto s = readWord(begin, end, IsNum());
	shared_ptr<StatementBlock> pSB(new DropTableBlock(s));
	vStatementBlock.push_back(pSB);
}
void Interpreter::dropIndexParser(Iterator& begin, Iterator end) {
	auto s = readWord(begin, end, IsNum());
	shared_ptr<StatementBlock> pSB(new DropIndexBlock(s));
	vStatementBlock.push_back(pSB);
}
void Interpreter::selectParser(Iterator& begin, Iterator end);
void Interpreter::insertParser(Iterator& begin, Iterator end);
void Interpreter::deleteParser(Iterator& begin, Iterator end){
	readWord(begin, end, IsString("from"));
	auto s = readWord(begin, end, IsVariableName());
	auto tableName = s;
	
	try{
		s = readWord(begin, end);
	}catch(EndOfString e){
		shared_ptr<StatementBlock> pSB(new DeleteBlock(tableName));
		return;
	}
	if(s!="where")
		throw GrammarError("illegal delete operation");
	
}
void Interpreter::quitParser(Iterator& begin, Iterator end) {
	try {
		readWord(begin, end);
	}catch(EndOfString e){
		shared_ptr<StatementBlock> pSB(new QuitBlock);
		vStatementBlock.push_back(pSB);
		return;
	}
	throw GrammarError("Words are forbidden to be followed by quit");
}