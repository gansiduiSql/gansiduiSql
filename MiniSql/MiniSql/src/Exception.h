#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <exception>
#include <string>

class GrammarError :public std::exception{
public:
	GrammarError(std::string s):errorInfo(s) {}
	const char *what()const { return errorInfo.c_str(); }
private:
	std::string errorInfo;
};


class AttributeError :public std::exception {
public:
	AttributeError(std::string s) :errorInfo(s) {}
	const char *what()const { return errorInfo.c_str(); }
private:
	std::string errorInfo;
};



#endif