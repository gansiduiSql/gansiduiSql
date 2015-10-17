#ifndef _INDEXMANAGER_H_
#define _INDEXMANAGER_H_

class IndexManager
{
public:
	void createIndex(std::string indexName, std::string attributeName, std::string indexKey, 
					TYPE keyValueType, string fileName, ADDRESS fisrtRecordOffset);
	void dropIndex(std::string indexName); //delet/drop index indexfile and index in this function
	void deleteValues(std::string indexName, std::list<Expression> expressions); 
	void selectValues(std::string indexName, std::list<Expression> expressions, RECORDBUFFER recordBuffer);
};

#endif