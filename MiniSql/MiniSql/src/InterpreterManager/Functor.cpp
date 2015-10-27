#include "Functor.h"
#include "../Exception.h"

using namespace std;
typedef std::string::iterator Iterator;

string readWord(Iterator& sIter, Iterator end, std::function<bool(char)> f)
{
	Iterator tIter = sIter;
	if (isEnd(tIter,end))
		return "";
	while (*tIter == ' ' || *tIter == '\t' || *tIter == '\r' || *tIter == '\n') {
		tIter++;
		if (isEnd(tIter, end))
			return "";
	}
	Iterator tmpIter = tIter;
	while (!isEnd(tIter, end) && f(*tIter)) {
		tIter++;
	}
	sIter = tIter;
	return string(tmpIter, tIter);

}

std::list<Expression>
 readExp(std::string::iterator& begin, std::string::iterator end){
	 string s;
	 list<Expression> ret;
	 try{
		 for(;;){
			 Expression tmpExp;
			 s = readWord(begin, end, IsAlnum);
			 tmpExp.leftOperand.operandName = s;
			 
			 s = readWord(begin, end, IsOperator());
			 s = readWord(begin, end, IsAlnum);
			 tmpExp.rightOperand.operandName = s;
			 ret.push_back(tmpExp);
			 s = readWord(begin, end, IsString("and"));
		 }
	 }catch (const EndOfString& e){
	 }
	 return move(ret);
 }

void readToEnd(std::string::iterator& begin, std::string::iterator end){
	try{
		readWord(begin, end, [](char)->bool{return false;});
	}catch (EndOfString& e){
		return;
	}
	throw GrammarError("redundant words found at the tail");
}

bool IsVariableName::operator()(char c) {
	if (i == 0) {
		i++;
		if (!isalpha(c) && !c == '_')
			throw GrammarError("illegal name");
	}
	else {
		return isalnum(c) || c == '_';
	}

}

bool IsString::operator()(char c) {
	if (iter == s.end() || *iter != c) {
		throw GrammarError("dismatch: " + s);
		return false;
	}
	iter++;
	return true;
}

bool isEnd(std::string::iterator& begin, std::string::iterator& end) {
	if (begin == end) {
		throw EndOfString();
		return true;
	}
	else
		return false;
}