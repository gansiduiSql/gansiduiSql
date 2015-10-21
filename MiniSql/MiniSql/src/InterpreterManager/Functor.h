#ifndef _FUNCTOR_H_
#define _FUNCTOR_H_

#include <string>
#include <functional>

std::string readWord(std::string::iterator& sIter, std::string::iterator end, std::function<bool(char)> f = [](char c)->bool {
	return isalnum(c) || c == '_';
});

auto IsComma = [](char c)->bool {
	return ',';
};
auto IsLeftBrace = [](char c)->bool {
	return '{';
}
auto IsRightBrace = [](char c)->bool {
	return '}';
}

class IsString {
public:
	IsString(std::string s) :s(s), iter(s.begin()) {}
	bool operator()(char c) {
		if (iter == s.end() || *iter != c)
			return false;
		iter++;
		return true;
	}

private:
	std::string s;
	std::string::iterator iter;
};

class IsVariableName {
public:
	IsVariableName() :i(0) {}
	bool operator()(char c) {
		if (i == 0) {
			i++;
			return isalpha(c) || c == '_';
		}
		else {
			return isalnum(c) || c == '_';
		}

	}
private:
	int i;
};


#endif