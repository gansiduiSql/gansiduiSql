#include "StatementBlock.h"
#include "../CatalogManager/CatalogManager.h"
#include "../Exception.h"
#include <iostream>
#include <algorithm>

using namespace std;

void CreateTableBlock::check()
{
}

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

void CreateIndexBlock::check()
{
	auto pcb = CatalogManager::getCatalogManager();
	if (pcb->isIndexExist(attributeName))
		throw CatalogError("index(" + indexName + ") exists");
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

void InsertTableBlock::check()
{
	auto pcb = CatalogManager::getCatalogManager();
	auto table = pcb->getTable(tableName);
	if (table.getTableName() != tableName)
		throw CatalogError("Table name(" + tableName + ") does not exist");
	auto& tableVec = table.getTableVec();
	if (tableVec.size() != values.size())
		throw CatalogError("The number of attributes does not match");
	CheckType ct;
	string s;
	int size = tableVec.size();
	for (int i = 0; i < size; i++) {
		TYPE type = tableVec[i].getType();
		if (!ct.isType(values[i], type)) {
			throw CatalogError("The type does not match");
			if (type == CHAR && values[i].length() == tableVec[i].getLength() + 2) {
				values[i] = string(values[i].begin() + 1, values[i].end() - 1);
			}throw CatalogError("The length of string does not match");
		}
	}
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

void QuitBlock::check()
{
	//no need to check
}

void QuitBlock::print()
{
	cout << "quit" << endl;
}

void DropTableBlock::execute()
{
}

void DropTableBlock::check()
{
	auto pcb = CatalogManager::getCatalogManager();
	if (!pcb->isTableExist(tableName))
		throw CatalogError("The table does not exist");
}

void DropTableBlock::print()
{
	cout << "drop " << tableName << endl;
}

void DropIndexBlock::execute()
{
}

void DropIndexBlock::check()
{
	auto pcb = CatalogManager::getCatalogManager();
	if (!pcb->isIndexExist(indexName))
		throw CatalogError("The index does not exist");
}

void DropIndexBlock::print()
{
	cout << "drop " << indexName << endl;
}

void DeleteBlock::execute()
{
	
}

void DeleteBlock::check()
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

void SelectBlock::check()
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

bool CheckType::isType(const std::string & s, TYPE type) {
	switch (type) {
	case INT:
		return isInt(s);
		break;
	case FLOAT:
		return isFloat(s);
		break;
	case CHAR:
		return isString(s);
		break;
	default:return false;
	}
}

bool CheckType::isString(const std::string & s) {
	return *(s.begin()) == '\''&&*(s.end() - 1) == '\'';
}

bool CheckType::isFloat(const std::string & s) {
	try {
		stof(s);
	}
	catch (exception& e) {
		return false;
	}return true;
}

bool CheckType::isInt(const std::string & s) {
	try {
		stoi(s);
	}
	catch (exception& e) {
		return false;
	}return true;
}
/*
bool CheckType::isAttribute(const std::string & s)
{
	vector<Data> v = pTable->getTableVec();
	auto iter = std::find_if(v.begin(), v.end(), [=](vector<Data>::iterator d) {
		return d->getAttribute() == s;
	});
	return iter != v.end();
}
*/