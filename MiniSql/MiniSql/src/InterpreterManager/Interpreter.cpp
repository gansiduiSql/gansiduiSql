#include "Interpreter.h"
#include "../Exception.h"
#include "../CatalogManager/CatalogManager.h"
#include "..\API\API.h"
#include "StatementBlock.h"
#include "Functor.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <fstream>
using namespace std;
typedef std::string::iterator Iterator;



std::vector<std::string> Interpreter::split(std::string s, std::string::value_type c){
	stringstream ss(s);
	vector<string> ret;
	string tmp;
	while (getline(ss,tmp,c)){
		ret.push_back(tmp);
	}
	return move(ret);
}


void Interpreter::readInput(const string & s)
{
	static bool inQuota = false;
	auto iter1 = s.begin();
	auto iter2 = iter1;
	for (; iter2 != s.end(); iter2++) {
		if (*iter2 == '\'') {
			inQuota = !inQuota;
		}
		if (!inQuota) {
			if (*iter2 == ';') {
				tmpStoredSql += string(iter1, iter2);
				try {
					parse(tmpStoredSql);
				}
				catch (exception& e) {
					printOut(e.what());
				}
				check();execute();
				iter1 = iter2 + 1;
				tmpStoredSql.clear();
			}
		}
	}
	tmpStoredSql += string(iter1, iter2);
}

void Interpreter::executeFile(const std::string & fileName)
{
	ifstream is(fileName);
	if (!is.is_open()) {
		throw runtime_error("file(" + fileName + ") can not be open");
	}
	string s;
	bool inQuota = false;
	while (getline(is, s)) {
		auto iter1 = s.begin();
		auto iter2 = iter1;
		for (; iter2 != s.end(); iter2++) {
			if (*iter2 == '\'') {
				inQuota = !inQuota;
			}
			if (!inQuota) {
				if (*iter2 == ';') {
					tmpStoredSql += string(iter1, iter2);
					try {
						parse(tmpStoredSql);
					}
					catch (exception& e) {
						printOut(e.what());
						throw runtime_error("error occur when run the file");
					}
					//check();execute();
					iter1 = iter2 + 1;
					tmpStoredSql.clear();
				}
			}
		}
		tmpStoredSql += string(iter1, iter2);
	}
}

void Interpreter::executeSql(const std::string & sql) {
	parse(sql); 
	check(); 
	//print(); 
	execute();
}

void Interpreter::parse(const string& sql){
	
	string str = sql + ' ';
	Iterator begin = str.begin();
	Iterator end = str.end();
	string s =	readWord(begin, end);
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
	else if (s == "drop") {
		s = readWord(begin, end);
		if (s == "table")
			dropTableParser(begin, end);
		else if (s == "index")
			dropIndexParser(begin, end);
		else throw GrammarError("Syntax error in drop operation");
	}
	else if (s == "execfile")
		execfileParser(begin, end);
	else throw GrammarError("Undefined operation: "+s);


}

void Interpreter::print()
{
	for (auto& vsb : vStatementBlock) {
		vsb->print();
	}
}

void Interpreter::check()
{
	bool flag = true;
	string s;
	for (auto& vsb : vStatementBlock) {
		try {
			vsb->check();
		}
		catch (exception& e) {
			s += string(e.what())+" ";
			flag = false;
		}
	}

	if (!flag) {
		vStatementBlock.clear();
		throw CatalogError(s);
	}
}

void Interpreter::execute()
{
	try {
		for (auto& vsb : vStatementBlock) {
			vsb->execute();		
		}
	}catch(exception& e){
		vStatementBlock.clear();
		throw runtime_error(e.what());
	}
	vStatementBlock.clear();
}

void Interpreter::createTableParser(Iterator& begin, Iterator end){
	string s;
	Table table;
	string primaryName;

	s = readWord(begin, end, IsVariableName()); //read tableName
	table.setTableName(s);

	s = readWord(begin, end, IsString("("));

	int state = 0;
	for (;;){
		Data tmpData;
		//set attribute name
		s = readWord(begin, end, IsVariableName());
		//check primary key
		if (s == "primary") {
			readWord(begin, end, IsString("key"));
			readWord(begin, end, IsString("("));
			primaryName = readWord(begin, end, IsVariableName());
			readWord(begin, end, IsString(")"));
		}
		else {
			tmpData.setAttribute(s);

			//read type
			s = readWord(begin, end);
			TYPE type = stringToTYPE(s);
			if (type == UNDEFINEDTYPE)
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
				}
				catch (invalid_argument e) {
					throw GrammarError("The length of a string should be a integer!");
				}
				readWord(begin, end, IsString(")"));
			}
			else {
				tmpData.setType(type);
			}
			s = readWord(begin, end, IsChar('u'));
			if (s.length() > 0) {
				readWord(begin, end, IsString("nique"));
				tmpData.setUnique(true);
			}
			table.pushData(tmpData);
		}
		//check if the end is comming
		s = readWord(begin, end, IsChar(')'));
		if (s == ")") {
			break;
		}else {
			s = readWord(begin, end, IsString(","));
		}
	}

	readToEnd(begin,end);
	shared_ptr<StatementBlock> pSB(new CreateTableBlock(table,primaryName));
	vStatementBlock.push_back(pSB);
}


/*
@func	createIndexParser
@format create index [INDEX] on [TABLE]([ATTRIBUTE])
*/
void Interpreter::createIndexParser(Iterator& begin, Iterator end) {
	string indexName = readWord(begin, end, IsVariableName());	//read index
	readWord(begin, end, IsString("on"));	//read on
	string tableName = readWord(begin, end, IsVariableName());	//read tableName
	readWord(begin, end, IsString("("));	//read left brace
	string attributeName = readWord(begin, end, IsVariableName());	//read attribute
	readWord(begin, end, IsString(")"));	//read right brace 
	
	readToEnd(begin, end);
	shared_ptr<StatementBlock> pSB(new CreateIndexBlock(indexName, tableName, attributeName));
	vStatementBlock.push_back(pSB);
}

/*
@func dropTableParser
@format drop [TABLE]
*/
void Interpreter::dropTableParser(Iterator& begin, Iterator end) {
	string s = readWord(begin, end, IsVariableName()); //read table
	shared_ptr<StatementBlock> pSB(new DropTableBlock(s));

	readToEnd(begin, end);
	vStatementBlock.push_back(pSB);
}

/*
@func dropIndexParser
@format drop index [INDEX]
*/
void Interpreter::dropIndexParser(Iterator& begin, Iterator end) {
	string s = readWord(begin, end, IsVariableName()); //read index 
	shared_ptr<StatementBlock> pSB(new DropIndexBlock(s));
	readToEnd(begin, end);
	vStatementBlock.push_back(pSB);
}

/*
func: selectParser
format select [ATTRIBUTES] from [TABLE] (where [EXPRESSIONS])
*/
void Interpreter::selectParser(Iterator& begin, Iterator end) {
	list<string> attributeList;
	
	SelectBlock sb(this);

	string s = readWord(begin, end, IsChar('*')); //read *
	if (s.length() != 0) {
		sb.setStar(true);
	}else {
		while (true) {
			s = readWord(begin, end, IsVariableName());//read attribute
			attributeList.push_back(s);
			s = readWord(begin, end, IsChar(','));
			if (s.length() == 0) {
				sb.setAttributes(attributeList);
				break;
			}
		}
	}

	readWord(begin, end, IsString("from")); // read from
	s = readWord(begin, end, IsVariableName()); // read table
	sb.setTableName(s);
	try {
		s = readWord(begin, end);
	}catch (EndOfString) {
		shared_ptr<StatementBlock> pSB(new SelectBlock(sb));
		vStatementBlock.push_back(pSB);
		return;
	}
	if (s != "where")throw GrammarError("illegal where");//read where
	list<Expression> expList = readExp(begin, end);// read exps
	sb.setExpressions(expList);

	readToEnd(begin, end);
	shared_ptr<StatementBlock> pSB(new SelectBlock(sb));
	vStatementBlock.push_back(pSB);
}

/*
@func insertParser
@format insert into [TABLE] values([VALUE TUPLE])
*/
void Interpreter::insertParser(Iterator& begin, Iterator end){
	vector<string> values;
	readWord(begin, end, IsString("into")); //read into
	string tableName = readWord(begin, end, IsVariableName()); //read tableName
	readWord(begin, end, IsString("values"));//read values
	readWord(begin, end, IsString("(")); //read left brace
	while(true){
		string s = readWord(begin, end, IsChar('\''));
		if(s.length()!=0){
			s ="\'"+ readWord(begin, end, IsCharArray());
			//readWord(begin, end, IsString("\'"));
		}else{
			s = readWord(begin, end);
		}
		values.push_back(s);
		s = readWord(begin, end, IsChar(')'));
		if(s.length()!=0)break;
		readWord(begin, end, IsString(","));
	}
	readToEnd(begin, end);
	shared_ptr<StatementBlock> pSB(new InsertTableBlock(tableName, values));
	vStatementBlock.push_back(pSB);
}

/*
@func deleteParser
@format delete from [TABLE] (where [EXPS])
*/
void Interpreter::deleteParser(Iterator& begin, Iterator end){
	readWord(begin, end, IsString("from"));  //read from
	string s = readWord(begin, end, IsVariableName());  //read table
	string tableName = s;
	
	try{
		s = readWord(begin, end);
	}catch(EndOfString e){
		shared_ptr<StatementBlock> pSB(new DeleteBlock(tableName));
		vStatementBlock.push_back(pSB);
		return;
	}
	if(s!="where")
		throw GrammarError("illegal delete operation");
	auto exps = readExp(begin, end);

	readToEnd(begin, end);
	shared_ptr<StatementBlock> pSB(new DeleteBlock(tableName,exps));
	vStatementBlock.push_back(pSB);
	
}

/*
@func quit
@format quit
*/
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

void Interpreter::execfileParser(Iterator & begin, Iterator end)
{
	string s = readWord(begin, end, IsCharArray());
	readToEnd(begin, end);
	shared_ptr<StatementBlock> pSB(new execBlock(s,this));
	vStatementBlock.push_back(pSB);
}
