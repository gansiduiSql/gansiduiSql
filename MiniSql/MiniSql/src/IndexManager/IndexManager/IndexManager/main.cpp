#include "../../IndexManager.h"

int ELEMENTNUM;
int main(void)
{
	Data sid;
	sid.setAttribute("sid");
	sid.setLength(sizeof(int));
	sid.setOffset(16);
	sid.setType(INT);
	sid.setUnique(1);
	sid.setPrimary(1);
	list<string> tmp;
	tmp.push_back("studentIndex");
	IndexManager * IM = new IndexManager(tmp);
//	IM->createIndex("studentIndex", sid, 20, "CreateIndexTestINT");
//	IM->dropIndex("studentIndex");
	IM->traverseTree("studentIndex");
 
	Expression exp2;
	exp2.leftOperand.operandName = "sid";
	exp2.leftOperand.isAttribute = true;
	exp2.op = LESS_AND_EQUAL;
	exp2.rightOperand.operandName = "6";
	exp2.rightOperand.isAttribute = false;
	list<Expression> lstExp;
	//lstExp.push_back(exp1);
	lstExp.push_back(exp2);
	IM->deleteValues("studentIndex", lstExp , "CreateIndexTestINT", 20, INT);
	IM->traverseTree("studentIndex");
	//IM->dropIndex("studentIndex");
	//IM->insertValues("studentIndex", "1000", 0x5e20);
	//IM->traverseTree("studentIndex");
	delete IM;
}