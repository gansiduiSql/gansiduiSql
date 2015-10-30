#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_

#include "../Definition.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

class StatementBlock;

/*
@name Interpreter
@func parse check and execute sql sentences
@ATTENTION before using the interpreter, PLEASE set RecordBuffer by calling setRecordBuffer
@exception Quit():which is not derived from std::exception, so when catch this exception, the program must go to end.
*/

/*
@puiblic function:
@executeSql:	[in]sql (string) [out]none : execute Sql sentence
@parse:		[in]sql (string) [out]none: parse Sql sentence
@check:		[in]none [out]none : after parse, call this func to check the data by communication with 
				CatalogManager
@execute:	[in]none [out]none : after check, call this func to execute by call API
*/

class Interpreter
{
public:
	Interpreter(){};
	~Interpreter() {};
	void setRecordBuffer(RECORDBUFFER& rb) { this->rb = &rb; }
	void readInput(const std::string& s);
	void executeFile(const std::string& fileName);
	void executeSql(const std::string& sql);
	void parse(const std::string& sql);
	void print();
	void check();
	void execute();
	RECORDBUFFER& getRecordBuffer() { return *rb; }
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
	void execfileParser(Iterator& begin, Iterator end);
	
	std::vector<std::shared_ptr<StatementBlock>> vStatementBlock;
	std::string tmpStoredSql;
	RECORDBUFFER* rb; 
};
#endif
