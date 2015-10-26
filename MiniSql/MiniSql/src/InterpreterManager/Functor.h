#ifndef _FUNCTOR_H_
#define _FUNCTOR_H_

#include "../Definition.h"
#include <memory>
#include <string>
#include <list>
#include <functional>

std::string readWord(std::string::iterator& sIter, std::string::iterator end, std::function<bool(char)> f = [](char c)->bool {
	return isalnum(c) || c == '_';
});


std::list<Expression>
 readExp(std::string::iterator& sIter, std::string::iterator end);


class IsString {
public:
	IsString(std::string s) :s(s), iter(s.begin()) {}
	bool operator()(char c);

private:
	std::string s;
	std::string::iterator iter;
};

class IsVariableName {
public:
	IsVariableName() :i(0) {}
	bool operator()(char c);
private:
	int i;
};

class IsNum {
public:
	IsNum() :i(0) {}
	bool operator()(char c) {
		if (i == 0) {
			i++;
			if( '0'<c && c<='9')
				return true;
			else {
				throw GrammarError("It is not a number");
				return false;
			}
		}
		else {
			return '0'<=c && c <= '9';
		}
	}
private:
	int i;
};

auto IsAlnum = [](char c)->bool{
	return isalnum(c)||c=='_';
};

class IsChar{
public:
	IsChar(char c): c(c) ,flag(true){}
	bool operator()(char ch){
		if (flag && ch == c) {
			flag = false;
			return true;
		}return false;
	}
private:
	char c;
	bool flag;
};


class IsOperator{
public:
	IsOperator():c(0){}
	bool operator()(char ch){
		switch(c){
			case 0xff:
				return false;
			case 0:
				switch(ch){
					case '=':case '<':case '>':case '!':
						c = ch;return true;break;
					default: throw GrammarError("illegal operator:" + ch );
					break;
					
				}break;
			case '=':
				if (ch == '=') { c = 0xff; return true; }
				else throw GrammarError("illegal operator");
				break;
			case '>':
				if (ch == '=') { c = 0xff; return true; }
				else return false;
				break;
			case '<':
				if(ch=='='||ch=='>') { c = 0xff; return true; }
				else return false;
				break;
			case '!':
				if(ch=='=') { c = 0xff; return true; }
				else throw GrammarError("illegal operator");
				break;
			default: return false;break;
		}
		return false;
	}
private:
	char c;
};

bool isEnd(std::string::iterator& begin, std::string::iterator& end);
#endif