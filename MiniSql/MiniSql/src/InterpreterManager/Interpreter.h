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
@ATTENTION before using the interpreter, PLEASE setRecordBuffer
@exception Quit():which is not derived from std::exception, so when catch this exception, the program must go to end.
*/

class Interpreter
{
public:
	Interpreter(std::function<void(std::string)> printOut):printOut(printOut) {};
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
	std::function<void(std::string)> printOut;
	RECORDBUFFER* rb; 
};
#endif
