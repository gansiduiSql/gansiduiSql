/*@brief
*@author tomstream@qq.com
*@date 2015/10/28
*@version 1.0
*/


/*
As the highest-level module of our MiniSql database system, the interpreter owns its responsibility to interact with users of MiniSql
and exchange data with lower-level modules, i.e. Catalog Manager, Index Manager, Record Manager, by calling interface provided by API
module.

When a sql statement is passing, the interpreter takes the following three steps: 1. Examine the correctness of grammars and parse it
to a specific data structure containing statement information; 2. Check if the catalog information is correspondent to the information
stored; 3. Call execute function.


To be emphasized on, the interpreter is designed in a low-couple way owing to the polymorphism feature of C++ program language, which 
improves its flexibility.

*/
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
	Interpreter();;
	~Interpreter() {};
	void setRecordBuffer(RECORDBUFFER& rb) { this->rb = &rb; }
	void readInput(const std::string& s);
	void executeFile(const std::string& fileName);
	void executeSql(const std::string& sql);
	void parse(const std::string& sql);
	void print();
	void check();
	void execute();
	std::shared_ptr<StatementBlock>& gettPtr(){ return tptr; }
	std::vector<std::shared_ptr<StatementBlock>>& getVsb(){return vStatementBlock;}
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
	std::shared_ptr<StatementBlock> tptr;
};
#endif
