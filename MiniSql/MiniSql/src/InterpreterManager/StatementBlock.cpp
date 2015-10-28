#include "StatementBlock.h"
#include <iostream>
using namespace std;


void CreateTableBlock::execute()
{
}

void CreateTableBlock::print()
{
	cout <<"Table Name:"<< table.getTableName()<<endl;
	auto v = table.getTableVec();
	for (auto& c : v) {
		cout << "Attribute:" << c.getAttribute() << " " << " TYPE:"
			<< c.getType() << " length:" << c.getLength();
		if (c.isUnique() == true)
			cout << " UNIQUE";
			cout << endl;
	}
	cout <<" Primary Key Name:"<< primaryKeyName << endl;
}

void CreateIndexBlock::execute()
{
}

void CreateIndexBlock::print()
{
	cout << "indexName:" << indexName
		<< " tableName:" << tableName
		<< " attributeName:" << attributeName
		<< endl;
}

void InsertTableBlock::execute()
{
}

void InsertTableBlock::print()
{
	cout << "tableName:" << tableName << endl;
	cout << "values:";
	for (auto & s : values) {
		cout << s << " ";
	}cout << endl;
}

void QuitBlock::execute()
{
}

void QuitBlock::print()
{
	cout << "quit" << endl;
}

void DropTableBlock::execute()
{
}

void DropTableBlock::print()
{
	cout << "drop " << tableName << endl;
}

void DropIndexBlock::execute()
{
}

void DropIndexBlock::print()
{
	cout << "drop " << indexName << endl;
}

void DeleteBlock::execute()
{
	
}

void DeleteBlock::print()
{
	cout << "Delete from " << tableName << " ";
	if (flag) {
		if (exps.size() != 0) {
			for (auto& exp : exps) {
				cout << "[" << exp.leftOperand.operandName << " " << exp.op << " " << exp.rightOperand.operandName << "]";
			}
		}
	}
	cout << endl;
}

void SelectBlock::execute()
{
}

void SelectBlock::print()
{
	cout << "select ";
	if (star)cout << "* ";\
	else {
		for (auto & s : attributes) {
			cout << s << ",";
		}
	}
	cout << "from " << tableName << " ";
	if (exps.size() != 0) {
		for (auto& exp : exps) {
			cout << "[" << exp.leftOperand.operandName << " " << exp.op << " " << exp.rightOperand.operandName << "]";
		}
	}
	cout << endl;
}
