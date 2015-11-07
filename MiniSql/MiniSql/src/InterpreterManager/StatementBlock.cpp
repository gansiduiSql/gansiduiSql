/*@brief
*@author tomstream@qq.com
*@date 2015/10/28
*@version 1.0
*/

#include "StatementBlock.h"
#include "../CatalogManager/CatalogManager.h"
#include "../Exception.h"
#include "../API/API.h"
#include "Interpreter.h"
#include <iostream>
#include <set>
#include <algorithm>
#include <sstream>
#include <regex>

using namespace std;

void CreateTableBlock::check()
{
	auto pcb = CatalogManager::getCatalogManager();
	if (pcb->isTableExist(table.getTableName()))
		throw CatalogError("The table have already exist");
	set<string> attrSet;
	auto tableVec = table.getTableVec();
	for (auto &data : tableVec) {
		attrSet.insert(data.getAttribute());
	}
	if (attrSet.size() != tableVec.size())
		throw GrammarError("exist duplicated attributes");
	string s =primaryKeyName;
	auto iter = find_if(tableVec.begin(), tableVec.end(), [=](const Data& d)->bool {
		return d.getAttribute() == s;
		; });
	if (iter == tableVec.end())
		throw GrammarError("The name of primary key attribute does not exist");
	iter->setPrimary(true);
	table.setPrimaryKeyIndex(iter - tableVec.begin());
	table.setTableVec(tableVec);
}

void CreateTableBlock::execute()
{
	auto api = API::getAPIPtr();
	api->createTableCmd(table);
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
		if (c.isPrimary() == true)
			cout << " PRIMARY";
			cout << endl;
	}
	cout <<" Primary Key Name:"<< primaryKeyName << endl;
}

void CreateIndexBlock::execute()
{
	auto api = API::getAPIPtr();
	api->createIndexCmd(indexName, tableName, attributeName);
}

void CreateIndexBlock::check()
{
	auto pcb = CatalogManager::getCatalogManager();
	if (pcb->isIndexExist(indexName))
		throw CatalogError("index(" + indexName + ") exists");
	if (!pcb->isTableExist(tableName))
		throw CatalogError("table(" + tableName + ") does not exist");
	auto table = pcb->getTable(tableName);
	if (!table.isAttribute(attributeName))
		throw CatalogError("attribute("+ attributeName +") does not exist");
	auto d = table.getData(attributeName);
	if (d.isUnique() || d.isPrimary())
		return;
	else throw CatalogError("attribute(" + attributeName + ") is not unique or primary key");

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
	auto api = API::getAPIPtr();
	auto cp = CatalogManager::getCatalogManager();
	list<string> v;
	for (auto& content : values) {
		v.push_back(content);
	}
	api->insertValuesCmd(v, cp->getTable(tableName));
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
		if (!ct.isType(values[i], type)) 
			throw CatalogError("The type does not match");
		if (type == CHAR) {
			if (values[i].length() <= tableVec[i].getLength() + 2)
				values[i] = string(values[i].begin() + 1, values[i].end() - 1);
			else
				throw CatalogError("The length of string does not match");
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
	throw Quit();
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
	auto api = API::getAPIPtr();
	api->dropTableCmd(tableName);
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
	auto api = API::getAPIPtr();
	api->dropIndexCmd(indexName);
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
	if (doNothingFlag)return;
	auto api = API::getAPIPtr();
	if (!flag)
		api->deleteValuesCmd(tableName);
	else api->deleteValuesCmd(tableName, exps);
}

void DeleteBlock::check()
{
	auto pcb = CatalogManager::getCatalogManager();
	if(!pcb->isTableExist(tableName))
		throw CatalogError("The table does not exist");
	if (!flag)return;

	Table table = pcb->getTable(tableName);
	CheckType ct(table);
	list<Expression> tmpExps;
	for (auto &exp : exps) {
		string leftName = exp.leftOperand.operandName;
		string rightName = exp.rightOperand.operandName;
		TYPE typeLeft = ct.isWhatType(leftName);
		TYPE typeRight = ct.isWhatType(rightName);
		if (typeLeft != typeRight)throw CatalogError("unmatched type");
		bool b1 = ct.isType(leftName, typeLeft), b2 = ct.isType(rightName, typeRight);
		
		exp.leftOperand.isAttribute = !b1;
		exp.rightOperand.isAttribute = !b2;
		//if they are all normal data
		if (b1&&b2) {
			if (compareExp(leftName, rightName, typeLeft, exp.op)){
			}
			else doNothingFlag = true;
		}
		else {
			if (b1&&typeLeft == CHAR)
				exp.leftOperand.operandName = string(leftName.begin() + 1, leftName.end() - 1);
			if (b2&&typeRight == CHAR)
				exp.rightOperand.operandName = string(rightName.begin() + 1, rightName.end() - 1);
			if (b1&&!b2)
				exp.swap();
			tmpExps.push_back(exp);
		}
	}
	if (tmpExps.empty()) {
		flag = true;
	}
	exps = tmpExps;
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
	if (doNothingFlag)return;
	auto api = API::getAPIPtr();
	if (exps.size() == 0)
		api->selectValuesCmd(attributes, tableName, ip->getRecordBuffer());
	else
		api->selectValuesCmd(attributes, tableName, exps, ip->getRecordBuffer());
}

void SelectBlock::check()
{
	auto pcb = CatalogManager::getCatalogManager();
	Table table = pcb->getTable(tableName);
	vector<Data> tableVec = table.getTableVec();
	
	if (star) {
		//select *
		attributes.clear();
		for (auto& data : tableVec) {
			attributes.push_back(data.getAttribute());
		}
	}
	else {
		for (auto& attr : attributes) {
			if (!table.isAttribute(attr))
				throw CatalogError("The attribute does not exist");
		}
	}
	if (exps.size() == 0)return;
	CheckType ct(table);
	
	std::list<Expression> expTmp;
	for (auto &exp : exps) {
		string leftName = exp.leftOperand.operandName;
		string rightName = exp.rightOperand.operandName;
		TYPE typeLeft = ct.isWhatType(leftName);
		TYPE typeRight = ct.isWhatType(rightName);
		if (typeLeft != typeRight)throw CatalogError("unmatched type");
		bool b1 = ct.isType(leftName, typeLeft), b2 = ct.isType(rightName, typeRight);

		exp.leftOperand.isAttribute = !b1;
		exp.rightOperand.isAttribute = !b2;
		//if they are all normal data
		if (b1&&b2) {
			if (compareExp(leftName, rightName, typeLeft, exp.op)) {
			}
			else doNothingFlag = true;
		}
		else {
			if (b1&&typeLeft == CHAR)
				exp.leftOperand.operandName = string(leftName.begin() + 1, leftName.end() - 1);
			if (b2&&typeRight == CHAR)
				exp.rightOperand.operandName = string(rightName.begin() + 1, rightName.end() - 1);
			if (b1&&!b2)
				exp.swap();
			expTmp.push_back(exp);
		}
	}
	exps = expTmp;
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
	if (std::regex_match(s, std::regex("^-?([1-9]\\d*\\.\\d*|0\\.\\d*)$")))
		return true;
	else return false;
}

bool CheckType::isInt(const std::string & s) {
	if (std::regex_match(s, std::regex("^-?([1-9]\\d*|0)$")))
		return true;
	return false;
}

bool CheckType::isAttribute(const std::string & s)
{
	return table.isAttribute(s);
}

TYPE CheckType::isWhatType(const std::string & s)
{	
	if (isInt(s))
		return INT;
	if (isString(s)) 
		return CHAR;
	if (isFloat(s))
		return FLOAT;
	vector<Data> v = table.getTableVec();
	auto iter = std::find_if(v.begin(), v.end(), [=](const Data& d) {
		return d.getAttribute() == s;
	});
	if(iter == v.end())
		throw CatalogError("no type matches " + s);
	return iter->getType();
}

bool compareExp(const std::string& left, const std::string& right, TYPE type, OPERATOR op)
{
	switch (type) {
	case INT:
		return compareFunc<int>(op)(stoi(left),stoi(right));
	case FLOAT:
		return compareFunc<float>(op)(stof(left), stof(right));
	case CHAR:
		return compareFunc<std::string>(op)(left, right);
	}

	return false;
}
void execBlock::execute() 
{
	string file = fileName;
	
	ip->gettPtr() = ip->getVsb()[0];
	ip->getVsb().clear();
	ip->executeFile(fileName);
	ip->getVsb().push_back(ip->gettPtr());
}
