#include "Definition.h"

using namespace std;

Table::Table(const std::string& tableName, std::vector<Data>& tableVec)
	:tableName(tableName), tableVec(tableVec)
{
	int i = 0;
	length = 0;
	for (auto &data : this->tableVec)
	{
		if (data.isPrimary())
			this->primaryKeyIndex = i;
		this->length += data.getLength();
		i++;
	}
}

ArrayList::ArrayList()
{
	header = 0;
	tail = BLOCKNUM - 1;
	for (int i = 0; i < BLOCKNUM; i++)
	{
		arraylist[i].next = i + 1;
		arraylist[i].last = i - 1;
	}
	arraylist[tail].next = -1;
}

ArrayList::~ArrayList(){}

int ArrayList::getHeader()
{
	return header;
}

void ArrayList::moveTail(int index)
{
	if (index == tail)
		return;
	int last = arraylist[index].last;
	int next = arraylist[index].next;

	arraylist[tail].next = index;
	arraylist[last].next = next;
	arraylist[next].last = last;
	arraylist[index].next = -1;
	arraylist[index].last = tail;
	tail = index;
	if (index == header)
		header = next;
}