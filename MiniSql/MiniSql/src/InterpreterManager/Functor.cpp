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
	while (tIter != end && f(*tIter)) {
		tIter++;
	}
	sIter = tIter;
	return string(tmpIter, tIter);

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