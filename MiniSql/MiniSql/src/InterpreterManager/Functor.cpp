#include "Functor.h"

using namespace std;
typedef std::string::iterator Iterator;

string readWord(Iterator& sIter, Iterator end, std::function<bool(char)> f)
{
	Iterator tIter = sIter;
	if (tIter == end)
		return "";
	while (*tIter == ' ' || *tIter == '\t' || *tIter == '\r' || *tIter == '\n') {
		tIter++;
		if (tIter == end)
			return "";
	}
	Iterator tmpIter = tIter;
	while (tIter != end && f(*tIter)) {
		tIter++;
	}
	sIter = tIter;
	return string(tmpIter, tIter);

}

