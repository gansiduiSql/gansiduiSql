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
	ss << value << " " << express.rightOperand.oprandName;

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

void RecordManager::insertValues(const std::string& tableName, const list<std::string>& values, const Table& table)
{
	//get the tail of the table record in the corresponding file
	ADDRESS tail = *((int *)(bmPtr->fetchARecord(tableName, 0)));
	int tableLength = table.getLength();
	
	int blockIndex = tail / BLOCKSIZE;
	//the record can't fit in the remain size of this block
	//set the tail to the start of next block
	if (blockIndex * BLOCKSIZE < tail + tableLength)
		tail = (blockIndex + 1) * BLOCKSIZE;

	BYTE* buffer = new BYTE[tableLength];	//a buffer to store the record
	ADDRESS offset = 0;
	vector<Data> tableVec = table.getTableVec();
	list<string>::const_iterator it = values.cbegin();
	BYTE* tmp = nullptr;
	for (auto iter : tableVec)	//foreach attribute in the table
	{
		stringstream ss;
		ss << *it;
		int attributeLength = iter.getLength();
		switch (iter.getType())
		{
		case INT:
			int intNum;
			ss >> intNum;
			memcpy(buffer + offset, &intNum, attributeLength);
			break;
		case CHAR:
			memcpy(buffer + offset, (*it).c_str(), attributeLength);
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
	}
	bmPtr->writeARecord(buffer, tableLength, tableName, tail);	//insert the record
	bmPtr->writeARecord((BYTE*)(tail + tableLength), 4, tableName, 0);	//update the tail of the record
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
	bmPtr->writeARecord((BYTE*)(4096), 4, tableName, 0);
}

void RecordManager::deleteValues(const std::string& tableName, std::list<Expression>& expressions)
{

}

void RecordManager::selectValues(const std::list<std::string>& attributeNames, const std::string& tableName, const Table& table, RECORDBUFFER& recordBuffer)
{
	map<string, int> attributeOffset; //map the attribute name to the offset in the each record
	map<string, TYPE> attributeType;
	map<string, int> attributeLength;
	
	constructMap(attributeOffset, attributeType, attributeLength, table);

	ADDRESS tail = *((int *)(bmPtr->fetchARecord(tableName, 0)));
	ADDRESS it = BLOCKSIZE;
	while (it < tail)
	{
		BYTE* buffer = bmPtr->fetchARecord(tableName, it);
		int index = 0;
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

		/*move it to the begin to the next record start
		*if a record can't fit in the remain size in this block
		*move the it to the begin of next block
		*cause by the record can't stored in two blocks
		*/
		it += table.getLength();
		int blockIndex = it / BLOCKSIZE;
		if (blockIndex*BLOCKSIZE - it < table.getLength())
			it = (blockIndex + 1)*BLOCKSIZE;
	}
}

void RecordManager::selectValues(const std::list<std::string>& attributeNames, const std::string& tableName, const Table& table, std::list<Expression>& expressions, RECORDBUFFER& recordBuffer)
{
	map<string, int> attributeOffset; //map the attribute name to the offset in the each record
	map<string, TYPE> attributeType;
	map<string, int> attributeLength;

	constructMap(attributeOffset, attributeType, attributeLength, table);

	ADDRESS tail = *((int *)(bmPtr->fetchARecord(tableName, 0)));
	ADDRESS it = BLOCKSIZE;
	while (it < tail)
	{
		BYTE* buffer = bmPtr->fetchARecord(tableName, it);
		int index = 0;
		bool flag = true;
		for (auto express : expressions)
		{
			string attributeName = express.leftOperand.oprandName;
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

		/*move it to the begin to the next record start
		*if a record can't fit in the remain size in this block
		*move the it to the begin of next block
		*cause by the record can't stored in two blocks
		*/
		it += table.getLength();
		int blockIndex = it / BLOCKSIZE;
		if (blockIndex*BLOCKSIZE - it < table.getLength())
			it = (blockIndex + 1)*BLOCKSIZE;
	}
}