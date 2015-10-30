#include "Functor.h"
#include "../Exception.h"
#include "../Definition.h"
#include <iostream>

using namespace std;
typedef std::string::iterator Iterator;

string readWord(Iterator& sIter, Iterator end, std::function<bool(char)> f)
{
	Iterator tIter = sIter;
	if (isEnd(tIter,end))
		return "";
	char c = *tIter;
	while (c == ' ' || c == '\t' || c == '\r' ||c == '\n') {
		tIter++;
		if (isEnd(tIter, end))
			return "";
		c = *tIter;
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
			 OPERATOR opr = stringToOperator(s);
			 tmpExp.op = opr;
			 s = readWord(begin, end, IsAlnum);
			 tmpExp.rightOperand.operandName = s;
			 ret.push_back(tmpExp);
			 s = readWord(begin, end, IsString("and"));
		 }
	 }catch (const EndOfString& e){
	 }
	 return move(ret);
 }

bool IsAlnum(char c) {
	return isalnum(c) || c == '_'||c=='.'||c=='\'';
}

void readToEnd(std::string::iterator& begin, std::string::iterator end){
	for (; begin != end; begin++) {
		if(*begin!=' '&&*begin!='\t'&&*begin!='\n'&&*begin!='\r')
			throw GrammarError("redundant words found at the tail");
	}
}

bool IsVariableName::operator()(char c) {
	if (i == 0) {
		i++;
		if (isalpha(c) || c == '_')
			return true;
		else
			throw GrammarError("illegal name");
		return false;

	}
	else {
		if(isalnum(c) || c == '_')
			return true;
		else
			return false;
	}
}

bool IsString::operator()(char c) {
	if (i == s.length())return false;
	if (s[i] != c) {
		throw GrammarError("dismatch: " + s);
		return false;
	}
	i++;
	return true;
}

bool isEnd(std::string::iterator& begin, std::string::iterator end) {
	int i = end - begin;
	if (end == begin) {
		int b;
		throw EndOfString();
		return true;
	}
	else
		return false;
}