/*@brief
*@author tomstream@qq.com
*@date 2015/10/28
*@version 1.0
*/
#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <string>

class GrammarError :public std::exception{
public:
	GrammarError(std::string s):errorInfo("Grammar Error: "+s) {}
	const char *what()const { return errorInfo.c_str(); }
private:
	std::string errorInfo;
};


class AttributeError :public std::exception {
public:
	AttributeError(std::string s) :errorInfo("Attribute Error: "+s) {}
	const char *what()const { return errorInfo.c_str(); }
private:
	std::string errorInfo;
};

class EndOfString:public std::exception {
public:
	EndOfString() {};
	EndOfString(int state1, int state2) :state1(state1),state2(state2) {}
	const char *what()const { return "Grammar Error: unexpected end"; }
	int getState1() { return state1; }
	int getState2() { return state2; }
private:
	int state1;
	int state2;
};


class CatalogError : public std::exception {
public:
	CatalogError(std::string s):errorInfo("Catalog Error: " + s){}
	const char *what()const { return errorInfo.c_str(); }
private:
	std::string errorInfo;
};


class Quit {

};
#endif