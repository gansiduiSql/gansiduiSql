#ifndef _RECORDERMANAGER_H_
#define _RECORDERMANAGER_H_

class RecordManager
{
public:
	RecordManager();
	~RecordManager();
	void insertValues(std::string tableName, list<std::string> values);
	void deleteValues(std::string tableName);
	void deleteValues(std::string tableName, std::list<Expression> expressions);
	void selectValues(std::list<std::string> attributeName, const std::string tableName);
	void selectValues(std::list<std::string> attributeName, const std::string tableName, std::list<Expression> expressions);
};

#endif