#include "RecordManager.h"
#include <sstream>
#include <map>

using namespace std;

/*template cmp function
*return true according to the given left, right and operator
*@param left	the left oprand
*@param right	the right oprand
*@param op		the operator
*return bool
*/
template<typename T>
bool cmp(T left, T right, OPERATOR op)
{
	switch (op)
	{
	case GREATER:
		return left > right;
		break;
	case LESS:
		return left < right;
		break;
	case EQUAL:
		return left == right;
		break;
	case NOTEQUAL:
		return left != right;
		break;
	case GREATER_AND_EQUAL:
		return left >= right;
		break;
	case LESS_AND_EQUAL:
		return left <= right;
		break;
	default:
		return false;
		break;
	}
}

/*return a bool value according to the given expression and compared value and the value type
*@param	express		the expression defined in definiton.h
*@param	value		the compared value
*@param	type		the type of the value, such int, char(n), float
*@return bool
*/
bool isTrue(const Expression express, const string& value, TYPE type)
{
	int intLeft, intRight;
	float floatLeft, floatRight;
	string strLeft, strRight;
	stringstream ss;
	ss << value << " " << express.rightOperand.operandName;

	switch (type)
	{
	case INT:
		ss >> intLeft >> intRight;
		return cmp(intLeft, intRight, express.op);
		break;
	case CHAR:
		ss >> strLeft >> strRight;
		return cmp(strLeft, strRight, express.op);
		break;
	case FLOAT:
		ss >> floatLeft >> floatRight;
		return cmp(floatLeft, floatRight, express.op);
		break;
	default:
		return false;
		break;
	}
}

void constructMap(map<string, int>& attributeOffset, map<string, TYPE>& attributeType, map<string, int>& attributeLength, const Table& table)
{
	int offset = 0;
	vector<Data> tableVec = table.getTableVec();

	for (auto iter : tableVec)
	{
		string attributeName = iter.getAttribute();
		attributeOffset[attributeName] = offset;
		attributeType[attributeName] = iter.getType();
		attributeLength[attributeName] = iter.getLength();
		offset += iter.getLength();
	}
}

/*default constructor
*get the pointer of the bufferManager
*@param void
*@return void
*/
RecordManager::RecordManager()
{
	bmPtr = BufferManager::getBufferManager();
}

RecordManager::~RecordManager()
{

}

/*insert values into a table 
*@param tableName the table name that you insert into
*@param values	a list values in order that you want insert into the table
*@param table	the info of the table you insert into
*@return void
*/
void RecordManager::insertValues(const std::string& tableName, const list<std::string>& values, const Table& table)
{
	//get the tail of the table record in the corresponding file
	ADDRESS tail = *((int *)(bmPtr->fetchARecord(tableName, 0)));
	int tableLength = table.getLength();
	
	/*check whether the insert value has the same value with the existed record
	*traverse all record in the file and check whether it is the same
	*if there is a same value in primary(unique) key, throw an exception
	*/
	vector<Data> tableVec = table.getTableVec();
	list<string>::const_iterator it = values.cbegin();
	RecordIterator iter(tableLength, tail);
	while (iter.hasNext())
	{
		stringstream ss;
		ss << *it;
		BYTE* buffer = bmPtr->fetchARecord(tableName, iter.value());
		int off = 0;
		for (auto& field : tableVec)
		{
			//the attribute is not a primary and unique key
			if (!field.isPrimary() && !field.isUnique())
				continue;
			int len = field.getLength();
			int intNum;
			float floatNum;
			char *ch = new char[len + 1];
			string s;
			switch (field.getType())
			{
			case INT:
				memcpy(&intNum, buffer + off, len);
				int insertNum;
				ss >> insertNum;
				if (insertNum == intNum)
					throw InsertException(field.getAttribute());
				break;
			case CHAR:
				memcpy(ch, buffer + off, len);
				ch[len] = '\0';
				s = string(ch);
				if (s == ss.str())
					throw InsertException(field.getAttribute());
				break;
			case FLOAT:
				memcpy(&floatNum, buffer + off, len);
				float insertNum;
				ss >> insertNum;
				if (floatNum == insertNum)
					throw InsertException(field.getAttribute());
				break;
			default:
				break;
			}
			delete[] ch;
		}
		iter = iter.next();
	}


	//the record can't fit in the remain size of this block
	//set the tail to the start of next block
	int blockIndex = tail / BLOCKSIZE;
	if ((blockIndex + 1) * BLOCKSIZE < tail + tableLength)
		tail = (blockIndex + 1) * BLOCKSIZE;

	BYTE* buffer = new BYTE[tableLength];	//a buffer to store the record
	ADDRESS offset = 0;
	it = values.cbegin();
	BYTE* tmp = nullptr;
	for (auto attribute : tableVec)	//foreach attribute in the table
	{
		stringstream ss;
		ss << *it;
		int intNum;
		string str = *it;
		int attributeLength = attribute.getLength();
		switch (attribute.getType())
		{
		case INT:
			ss >> intNum;
			memcpy(buffer + offset, &intNum, attributeLength);
			break;
		case CHAR:
			while (str.length() < attributeLength)
				str += " ";
			memcpy(buffer + offset, (str).c_str(), attributeLength);
			break;
		case FLOAT:
			float floatNum;
			ss >> floatNum;
			memcpy(buffer + offset, &floatNum, attributeLength);
			break;
		default:
			break;
		}
		offset += attributeLength;
		it++;
	}
	bmPtr->writeARecord(buffer, tableLength, tableName, tail);	//insert the record
	tail += tableLength;
	bmPtr->writeARecord((BYTE*)(&tail), 4, tableName, 0);	//update the tail of the record
	delete[] buffer;
}

/*delete all values in the given table
*only set the tail to BLOCKSIZE to devote that all values have been deleted
*actually the record still in the disk
*@param	tableName	the table you want to delete all values
*@return void
*/
void RecordManager::deleteValues(const std::string& tableName)
{
	//move the corresponding tail to the first block
	//the data is still in the file but use to tail to devote the deletion
	int tail = BLOCKSIZE;
	bmPtr->writeARecord((BYTE*)(&tail), 4, tableName, 0);
}

void RecordManager::deleteValues(const std::string& tableName, std::list<Expression>& expressions)
{

}

/*select the a specific attribute name from the table without the field of 'where'
*@param attributeName  a list of attribute name that you want to select
*@param tableName	the table name you select from
*@param table	the table info stored in catalog
*@param recordbuffer the buffer used to return the result, all attribute is in string type
*@return void
*/
void RecordManager::selectValues(const std::list<std::string>& attributeNames, const std::string& tableName, const Table& table, RECORDBUFFER& recordBuffer)
{
	map<string, int> attributeOffset; //map the attribute name to the offset in the each record
	map<string, TYPE> attributeType;  //map the attribute name to the type in each record
	map<string, int> attributeLength; //map the attribute name to the length in each record
	
	//construct the three map above
	constructMap(attributeOffset, attributeType, attributeLength, table);

	ADDRESS tail = *((int *)(bmPtr->fetchARecord(tableName, 0)));	//get the tail of the table
	RecordIterator it(table.getLength(), tail);
	while (it.hasNext())
	{
		BYTE* buffer = bmPtr->fetchARecord(tableName, it.value());	//fecth the record from bufferManager
		int index = 0;
		//foreach attribute that you want to select
		for (auto attributeName : attributeNames)
		{
			int off = attributeOffset[attributeName];
			TYPE type = attributeType[attributeName];
			int length = attributeLength[attributeName];
			stringstream ss;
			int intNum;
			char *ch = new char[length + 1];
			float floatNum;
			//swith the type of the attribute
			//push the into the recordbuffer in string type to return
			switch (type)
			{
			case INT:
				memcpy(&intNum, buffer + off, length);
				ss << intNum;
				recordBuffer[index].push_back(ss.str());
				break;
			case CHAR:
				memcpy(ch, buffer + off, length);
				recordBuffer[index].push_back(string(ch));
				break;
			case FLOAT:
				memcpy(&floatNum, buffer + off, length);
				ss << floatNum;
				recordBuffer[index].push_back(ss.str());
				break;
			default:
				break;
			}
		}
		index++;
		it = it.next();
	}
}

/*select the a specific attribute name from the table with the field of 'where'
*@param attributeName  a list of attribute name that you want to select
*@param tableName	the table name you select from
*@param table	the table info stored in catalog
*@param expressions a list of expression in the 'where' field and for simplity, all expression are join with logical and operator
*@param recordbuffer the buffer used to return the result, all attribute is in string type
*@return void
*/
void RecordManager::selectValues(const std::list<std::string>& attributeNames, const std::string& tableName, const Table& table, std::list<Expression>& expressions, RECORDBUFFER& recordBuffer)
{
	map<string, int> attributeOffset; //map the attribute name to the offset in the each record
	map<string, TYPE> attributeType;  //map the attribute name to the type in each record
	map<string, int> attributeLength; //map the attribute name to the length in each record

	//construct the three map above
	constructMap(attributeOffset, attributeType, attributeLength, table);
	
	//transverse all record in the file
	ADDRESS tail = *((int *)(bmPtr->fetchARecord(tableName, 0)));
	RecordIterator it(table.getLength(), tail);
	while (it.hasNext())
	{
		BYTE* buffer = bmPtr->fetchARecord(tableName, it.value());

		int index = 0;
		bool flag = true;
		for (auto express : expressions)
		{
			string attributeName = express.leftOperand.operandName;
			int off = attributeOffset[attributeName];
			TYPE type = attributeType[attributeName];
			int length = attributeLength[attributeName];
			stringstream ss;
			int intNum;
			string s;
			float floatNum;
			switch (type)
			{
			case INT:
				memcpy(&intNum, buffer + off, length);
				ss << intNum;
				flag = isTrue(express, ss.str(), type);
				break;
			case CHAR:
				memcpy(&s, buffer + off, length);
				flag = isTrue(express, s, type);
				break;
			case FLOAT:
				memcpy(&floatNum, buffer + off, length);
				ss << floatNum;
				flag = isTrue(express, ss.str(), type);
				break;
			default:
				break;
			}
			if (flag == false)
				break;
		}

		if (flag)
		{
			for (auto attributeName : attributeNames)
			{
				int off = attributeOffset[attributeName];
				TYPE type = attributeType[attributeName];
				int length = attributeLength[attributeName];
				stringstream ss;
				int intNum;
				string s;
				float floatNum;
				switch (type)
				{
				case INT:
					memcpy(&intNum, buffer + off, length);
					ss << intNum;
					recordBuffer[index].push_back(ss.str());
					break;
				case CHAR:
					memcpy(&s, buffer + off, length);
					recordBuffer[index].push_back(s);
					break;
				case FLOAT:
					memcpy(&floatNum, buffer + off, length);
					ss << floatNum;
					recordBuffer[index].push_back(ss.str());
					break;
				default:
					break;
				}
				index++;
			}
		}
		it = it.next();
	}
}