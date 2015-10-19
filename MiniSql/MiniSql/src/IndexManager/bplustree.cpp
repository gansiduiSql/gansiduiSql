/// ///////////////////////////////////
/// BPlusTree implementation
/// Summary: BPlusTree with single value index
/// Author: KUAN LU
/// Data: 2015/10/18
/// Department: Zhejiang University College of Computer Science
/// //////////////////////////////////
#include "bplustree.h"
extern int ELEMENTNUM;
/// //////////////////////////////////
/// \brief BPlusTree Node
/// \param BPusTree Node
/// ///////////////////////////////////
BPlusTreeNode::BPlusTreeNode(int keyNumber) :KEYNUM(keyNumber), POINTERNUM(keyNumber + 1)
{
	ELEMENTCOUNT = 0;
	ptrToChild = new BPlusPointer[POINTERNUM];
	keyValue = new ElementType[KEYNUM];
	for (int i = 0; i < POINTERNUM; i++)
		ptrToChild[i] = NULL;
	ptrToParent = NULL;
}

BPlusTreeNode::~BPlusTreeNode()
{
	delete[] ptrToChild;
	delete[] keyValue;
}

BPlusPointer BPlusTreeNode::addKey(RecordPointer p, ElementType s)
{
	int i = firstValueBiggerThan(s);				 //Find the index of the first value bigger than s
	if (i == -1)                                                 //If every Value is smaller than s,  or if the node is empty
	{
		int j;
		for (j = 0; j < POINTERNUM;)				 //Find the index of the first none-null index
		{
			if (ptrToChild[++j] == NULL)
				break;
		}
		ptrToChild[j]->addKey(p, s);			    //Addkey recursively
	}
	else if (i == KEYNUM)
	{
		ptrToChild[POINTERNUM - 1]->addKey(p, s);
	}
	else
	{
		if (s == keyValue[i])								//若第i项和s相等
			ptrToChild[i + 1]->addKey(p, s);		//递归向下
		else															 //第i项大于s
			ptrToChild[i]->addKey(p, s);				//递归向下查询
	}
	return this->ptrToParent == NULL ? this : this->ptrToParent;
}

void BPlusTreeNode::insertKey(BPlusPointer p, ElementType s, int direction)
{
	if (isFull())															  //若该node已满
	{
		bool newParentCreated = false;
		if (ptrToParent == NULL)
		{
			BPlusPointer newFatherNode = new BPlusTreeNode(KEYNUM); //创建一个新的父节点
			ptrToParent = newFatherNode;
			newParentCreated = true;
		}
		BPlusPointer newNode = new BPlusTreeNode(KEYNUM);							 //创建一个新Node
		newNode->setParentNode(ptrToParent);

		ElementType* tmpKeyValue = new ElementType[KEYNUM + 1];          //创建两个多一个位置的数组
		BPlusPointer* tmpPtr = new BPlusPointer[POINTERNUM + 1];

		for (int i = 0; i < KEYNUM; i++)                 //将Key全部移出
			tmpKeyValue[i] = keyValue[i];

		for (int i = 0; i<POINTERNUM; i++)              //将指针全部移出
			tmpPtr[i] = ptrToChild[i];

		int i = firstValueBiggerThan(s);        //找到插入位点

		for (int j = KEYNUM; j>i; j--)                   //腾出Key插入位置
			tmpKeyValue[j] = keyValue[j - 1];

		for (int j = POINTERNUM; j > i + 1; j--)             //腾出Pointer插入位置
			tmpPtr[j] = ptrToChild[j - 1];

		tmpKeyValue[i] = s;                           //插入Key
		tmpPtr[i + 1] = p;                              //插入Pointer

		this->makeEmpty();

		for (int i = 0; i < POINTERNUM / 2; i++)
			this->insertKey(tmpPtr[i], tmpKeyValue[i], LEFT);
		this->insertPtr(tmpPtr[POINTERNUM / 2]);

		if (newParentCreated)
			ptrToParent->insertPtr(this);
		ptrToParent->insertKey(newNode, tmpKeyValue[POINTERNUM / 2], RIGHT);

		for (int i = POINTERNUM / 2 + 1; i < POINTERNUM; i++)
			newNode->insertKey(tmpPtr[i], tmpKeyValue[i], LEFT);
		newNode->insertPtr(tmpPtr[POINTERNUM]);

		this->alterParentNode();
		newNode->alterParentNode();

		delete[] tmpKeyValue;
		delete[] tmpPtr;
	}
	else
	{
		int i = firstValueBiggerThan(s);
		for (int j = KEYNUM - 1; j > i; j--)
			keyValue[j] = keyValue[j - 1];
		if (direction == RIGHT)
		{
			for (int j = POINTERNUM - 1; j > i + 1; j--)
				ptrToChild[j] = ptrToChild[j - 1];
			ptrToChild[i + 1] = p;
		}
		else if (direction == LEFT)
		{
			for (int j = POINTERNUM - 1; j > i; j--)
				ptrToChild[j] = ptrToChild[j - 1];
			ptrToChild[i] = p;
		}
		ELEMENTCOUNT++;
		keyValue[i] = s;
	}
}

BPlusPointer BPlusTreeNode::removeKey(ElementType s)
{
	int i = firstValueBiggerThan(s);     //找到最小的比s大的元素的index
	BPlusPointer returnedPointer;
	if (i == -1)                                                         //若所有元素比s小或者Node为空
	{
		int j;
		for (j = 0; j < POINTERNUM;)               //找到第一个不为0的指针的index
		{
			if (ptrToChild[++j] == NULL)
				break;
		}
		returnedPointer = ptrToChild[j]->removeKey(s);             //递归向下删除
	}
	else if (i == KEYNUM)
	{
		returnedPointer = ptrToChild[POINTERNUM - 1]->removeKey(s);
	}
	else
	{
		if (s == keyValue[i])                              //若第i项和s相等
			returnedPointer = ptrToChild[i + 1]->removeKey(s);     //递归向下
		else                                                           //第i项大于s
			returnedPointer = ptrToChild[i]->removeKey(s);           //递归向下查询
	}
	if (ptrToParent == NULL)
		return returnedPointer;
	else if (ptrToParent->isEmpty())
		return ptrToParent->ptrToChild[0];
	else return ptrToParent;
}

BPlusLeaf BPlusTreeNode::returnLeafNode(ElementType s)
{
	int i = firstValueBiggerThan(s);     //找到最小的比s大的元素的index
	BPlusPointer returnedPointer;
	if (i == -1)                                                         //若所有元素比s小或者Node为空
	{
		int j;
		for (j = 0; j < POINTERNUM;)               //找到第一个不为0的指针的index
		{
			if (ptrToChild[++j] == NULL)
				break;
		}
		return ptrToChild[j]->returnLeafNode(s);             //递归向下删除
	}
	else if (i == KEYNUM)
	{
		return ptrToChild[POINTERNUM - 1]->returnLeafNode(s);
	}
	else
	{
		if (s == keyValue[i])                              //若第i项和s相等
			return ptrToChild[i + 1]->returnLeafNode(s);     //递归向下
		else                                                           //第i项大于s
			return ptrToChild[i]->returnLeafNode(s);           //递归向下查询
	}
}

void BPlusTreeNode::insertPtr(BPlusPointer p)
{
	int i = 0;
	while (ptrToChild[i] != NULL)
		i++;
	ptrToChild[i] = p;
	return;
}

void BPlusTreeNode::reDistributePtr(BPlusPointer sibPtr)
{
	BPlusPointer* tmpPtr = new BPlusPointer[POINTERNUM * 2];
	ElementType* tmpKeyValue = new ElementType[KEYNUM * 2];
	int SIBELEMENT = sibPtr->ELEMENTCOUNT;
	int THISELEMENT = this->ELEMENTCOUNT;
	int TOTALELEMENT = SIBELEMENT + THISELEMENT;
	bool sibBiggerThanThis;

	if (sibPtr->keyValue[0] < this->keyValue[0])
		sibBiggerThanThis = false;

	if (sibBiggerThanThis)
	{
		for (int i = 0; i < THISELEMENT; i++)                 //将Key全部移出
			tmpKeyValue[i] = this->keyValue[i];
		for (int i = 0; i < SIBELEMENT; i++)
			tmpKeyValue[i + THISELEMENT] = sibPtr->keyValue[i];
		for (int i = 0; i < THISELEMENT + 1; i++)                 //将指针全部移出
			tmpPtr[i] = this->ptrToChild[i];
		for (int i = 0; i < SIBELEMENT + 1; i++)
			tmpPtr[i + THISELEMENT + 1] = sibPtr->ptrToChild[i];
	}
	else
	{
		for (int i = 0; i < SIBELEMENT; i++)                 //将Key全部移出
			tmpKeyValue[i] = sibPtr->keyValue[i];
		for (int i = 0; i < THISELEMENT; i++)
			tmpKeyValue[i + SIBELEMENT] = this->keyValue[i];
		for (int i = 0; i < SIBELEMENT + 1; i++)                 //将指针全部移出
			tmpPtr[i] = sibPtr->ptrToChild[i];
		for (int i = 0; i < THISELEMENT + 1; i++)
			tmpPtr[i + SIBELEMENT + 1] = this->ptrToChild[i];
	}

	int count = 0;
	int MIDPOINT = TOTALELEMENT / 2;
	sibPtr->makeEmpty();
	this->makeEmpty();
	if (sibBiggerThanThis)
	{
		for (; count < MIDPOINT; count++)
			this->insertKey(tmpPtr[count], tmpKeyValue[count], LEFT);
		this->insertPtr(tmpPtr[MIDPOINT]);

		ptrToParent->alterKeyValue(ptrToParent->indexOf(sibPtr) - 1, tmpKeyValue[MIDPOINT]);
		sibPtr->insertKey(tmpPtr[MIDPOINT + 1], tmpPtr[MIDPOINT + 2]->keyValue[0], LEFT);

		for (count = MIDPOINT + 1; count < TOTALELEMENT; count++)
			sibPtr->insertKey(tmpPtr[count + 1], tmpKeyValue[count], LEFT);
		sibPtr->insertPtr(tmpPtr[TOTALELEMENT + 1]);
	}
	else
	{
		for (; count < MIDPOINT; count++)
			sibPtr->insertKey(tmpPtr[count], tmpKeyValue[count], LEFT);
		sibPtr->insertPtr(tmpPtr[MIDPOINT]);

		ptrToParent->alterKeyValue(ptrToParent->indexOf(this) - 1, tmpKeyValue[MIDPOINT]);
		this->insertKey(tmpPtr[MIDPOINT + 1], tmpPtr[MIDPOINT + 2]->keyValue[0], LEFT);

		for (count = MIDPOINT + 1; count < TOTALELEMENT; count++)
			this->insertKey(tmpPtr[count + 1], tmpKeyValue[count], LEFT);
		this->insertPtr(tmpPtr[TOTALELEMENT + 1]);
	}

	delete[] tmpKeyValue;
	delete[] tmpPtr;
}

BPlusPointer BPlusTreeNode::deleteKey(BPlusPointer p)
{
	int position = indexOf(p);
	int i;
	for (i = position; i < POINTERNUM - 1; i++)
		ptrToChild[i] = ptrToChild[i + 1];
	ptrToChild[i] = NULL;
	if (position == 0)
	{
		for (i = position; i < KEYNUM - 1; i++)
			keyValue[i] = keyValue[i + 1];
		keyValue[i] = "";
	}
	else
	{
		for (i = position - 1; i < KEYNUM - 1; i++)
			keyValue[i] = keyValue[i + 1];
		keyValue[i] = "";
	}
	ELEMENTCOUNT--;

	if (ptrToParent == NULL || ptrToParent->isEmpty())
	{
		ptrToParent = NULL;
	}
	else if (ELEMENTCOUNT < KEYNUM / 2)
	{
		BPlusPointer sibPtr = ptrToParent->findSibling(this);
		if (sibPtr->ELEMENTCOUNT + this->ELEMENTCOUNT < KEYNUM)//if number of element fits into its sibling
		{
			if (sibPtr->keyValue[0] < keyValue[0]) //sib is on the left
			{
				sibPtr->insertKey(ptrToChild[0], ptrToChild[0]->keyValue[0], RIGHT);
				for (int i = 0; i < ELEMENTCOUNT; i++)
					sibPtr->insertKey(ptrToChild[i + 1], keyValue[i], RIGHT);
			}
			else
			{
				sibPtr->insertKey(ptrToChild[KEYNUM / 2 - 1], sibPtr->ptrToChild[0]->keyValue[0], LEFT);
				for (int i = 0; i < ELEMENTCOUNT; i++)
					sibPtr->insertKey(ptrToChild[i], keyValue[i], LEFT);
			}
			this->alterParentNode();
			sibPtr->alterParentNode();
			ptrToParent->deleteKey(this);
			return this;
		}
		else//else if number of element doesn't fits into its sibling
		{
			reDistributePtr(sibPtr);
			this->alterParentNode();
			sibPtr->alterParentNode();
		}
	}
	return this;
}

BPlusPointer BPlusTreeNode::deleteKey(BPlusPointer p, ElementType s)
{
	int i = indexOf(p);
	if (i != 0)
		keyValue[indexOf(p) - 1] = s;
	return this;
}

RecordPointer BPlusTreeNode::findKey(ElementType s)
{
	int i = firstValueBiggerThan(s);						 //找到最小的比s大的元素的index
	if (i == -1)                                                         //若所有元素比s小或者Node为空
	{
		int j;
		for (j = 0; j < POINTERNUM;)						 //找到第一个不为0的指针的index
		{
			if (ptrToChild[++j] == NULL)
				break;
		}
		return ptrToChild[j]->findKey(s);             //递归向下删除
	}
	else if (i == KEYNUM)
	{
		return ptrToChild[POINTERNUM - 1]->findKey(s);
	}
	else
	{
		if (s == keyValue[i])                              //若第i项和s相等
			return ptrToChild[i + 1]->findKey(s);    //递归向下
		else                                                           //第i项大于s
			return ptrToChild[i]->findKey(s);           //递归向下查询
	}
}

void BPlusTreeNode::traverse(int level)
{
	cout << '|';
	for (int i = 0; i < KEYNUM; i++)
	{
		cout << keyValue[i] << "|";
	}
	cout << " level:" << level++;
	cout << " Element count=" << ELEMENTCOUNT << endl;
	for (int i = 0; i < POINTERNUM; i++)
	{
		if (ptrToChild[i] != NULL)
			ptrToChild[i]->traverse(level);
	}
}

bool BPlusTreeNode::containsKey(ElementType s)
{
	int i;
	for (i = 0; i < KEYNUM; i++)
	{
		if (keyValue[i] == s)
			return true;
	}
	return false;
}

int BPlusTreeNode::indexOf(ElementType s)
{
	int i;
	for (i = 0; i < KEYNUM; i++)
	if (keyValue[i] == s)
		return i;
	return -1;
}

int BPlusTreeNode::indexOf(BPlusPointer p)
{
	int i;
	for (i = 0; i < POINTERNUM; i++)
	if (ptrToChild[i] == p)
		return i;
	return -1;
}

void BPlusTreeNode::makeEmpty()
{
	ELEMENTCOUNT = 0;
	for (int i = 0; i < KEYNUM; i++)
		keyValue[i] = "";
	for (int i = 0; i < POINTERNUM; i++)
		ptrToChild[i] = NULL;
}

BPlusPointer BPlusTreeNode::findSibling(BPlusPointer p)
{
	int i = indexOf(p);
	if (i == POINTERNUM)
		return ptrToChild[POINTERNUM - 1];
	else if (i == 0)
		return ptrToChild[1];
	else if (ptrToChild[i + 1] == NULL)
		return ptrToChild[i - 1];
	else
		return ptrToChild[i - 1]->ELEMENTCOUNT > ptrToChild[i + 1]->ELEMENTCOUNT ? ptrToChild[i + 1] : ptrToChild[i - 1];
}

BPlusLeaf BPlusTreeNode::findSibling(BPlusLeaf p)
{
	int i = indexOf(p);
	if (i == POINTERNUM)
		return (BPlusLeaf)ptrToChild[POINTERNUM - 1];
	else if (i == 0)
		return (BPlusLeaf)ptrToChild[1];
	else if (ptrToChild[i + 1] == NULL)
	{
		return (BPlusLeaf)ptrToChild[i - 1];
	}
	return ptrToChild[i - 1]->ELEMENTCOUNT > ptrToChild[i + 1]->ELEMENTCOUNT ? (BPlusLeaf)ptrToChild[i + 1] : (BPlusLeaf)ptrToChild[i - 1];
}

int BPlusTreeNode::firstValueBiggerThan(ElementType s)
{
	int i;
	for (i = 0; i < KEYNUM; i++)
	{
		if (keyValue[i].compare(s) == 1 || keyValue[i] == "")
			return i;
	}
	return i;
}

void BPlusTreeNode::alterParentNode()
{
	for (int i = 0; ptrToChild[i] != NULL&&i < POINTERNUM; i++)
		ptrToChild[i]->ptrToParent = this;
}

void BPlusTreeNode::alterKeyValue(int index, ElementType s)
{
	keyValue[index] = s;
}

ElementType BPlusTreeNode::getKeyValue(int index)
{
	return this->keyValue[index];
}

BPlusPointer BPlusTreeNode::getPtrToChild(int index)
{
	return this->ptrToChild[index];
}

int BPlusTreeNode::getElementCount()
{
	return ELEMENTCOUNT;
}

void BPlusTreeNode::setParentNode(BPlusPointer p)
{
	this->ptrToParent = p;
}

////////////////////////////////////////////////////////////////////////////////////////
/// \brief BPlusTreeLeaf
/// \param BPlusTreeLeaf
///
////////////////////////////////////////////////////////////////////////////////////////
BPlusTreeLeaf::BPlusTreeLeaf(int keyNumber) :BPlusTreeNode(keyNumber)
{
	ptrToChild = new RecordPointer[KEYNUM];
	for (int i = 0; i < KEYNUM; i++)
	{
		ptrToChild[i] = NULL;
	}
	ptrToSibling = NULL;
}

BPlusTreeLeaf::~BPlusTreeLeaf()
{
	delete[] ptrToChild;
}

BPlusPointer BPlusTreeLeaf::addKey(RecordPointer p, ElementType s)
{
	insertKey(p, s, LEFT);
	return this->ptrToParent == NULL ? this : this->ptrToParent;;
}

void BPlusTreeLeaf::insertKey(RecordPointer p, ElementType s, int direction)
{
	bool newParentCreated = false;
	if (isFull())                             //若该node已满
	{
		if (ptrToParent == NULL)
		{
			BPlusPointer newFatherNode = new BPlusTreeNode(KEYNUM); //创建一个新的父节点
			ptrToParent = newFatherNode;
			newParentCreated = true;
		}
		BPlusLeaf newNode = new BPlusTreeLeaf(KEYNUM);  //创建一个新 Leaf Node
		newNode->setParentNode(this->ptrToParent);          //指向父节点

		ElementType* tmpKeyValue = new ElementType[KEYNUM + 1];          //创建两个多一个位置的数组
		RecordPointer* tmpPtr = new RecordPointer[KEYNUM + 1];

		for (int i = 0; i < KEYNUM; i++)                 //将Key全部移出
		{
			tmpKeyValue[i] = keyValue[i];
			tmpPtr[i] = ptrToChild[i];
		}
		int i = firstValueBiggerThan(s);        //找到插入位点
		for (int j = KEYNUM; j>i; j--)                   //腾出Key插入位置
		{
			tmpKeyValue[j] = keyValue[j - 1];
			tmpPtr[j] = ptrToChild[j - 1];
		}
		tmpKeyValue[i] = s;                           //插入Key
		tmpPtr[i] = p;                              //插入Pointer

		this->makeEmpty();
		for (int i = 0; i < POINTERNUM / 2; i++)
			this->insertKey(tmpPtr[i], tmpKeyValue[i], LEFT);
		newNode->ptrToSibling = this->ptrToSibling;
		this->ptrToSibling = newNode;                 //对于Leaf Sibling为下一个

		if (newParentCreated)
			ptrToParent->insertPtr(this);
		ptrToParent->insertKey(newNode, tmpKeyValue[POINTERNUM / 2], RIGHT);

		for (int i = POINTERNUM / 2; i<POINTERNUM; i++)
			newNode->insertKey(tmpPtr[i], tmpKeyValue[i], LEFT);

		delete[] tmpKeyValue;
		delete[] tmpPtr;
	}
	else
	{                                                           //叶节点的插入步骤
		if (containsKey(s))
			ptrToChild[indexOf(s)] = p;
		else
		{
			int i = firstValueBiggerThan(s);
			for (int j = KEYNUM - 1; j>i; j--)
			{
				ptrToChild[j] = ptrToChild[j - 1];
				keyValue[j] = keyValue[j - 1];
			}
			ELEMENTCOUNT++;
			ptrToChild[i] = p;
			keyValue[i] = s;
		}
	}
}

BPlusLeaf BPlusTreeLeaf::returnLeafNode(ElementType s)
{
	return this;
}

void BPlusTreeLeaf::insertPtrToSibling(BPlusLeaf p)
{
	ptrToSibling = p;
}

BPlusPointer BPlusTreeLeaf::removeKey(ElementType s)
{
	BPlusPointer returnedPointer;
	if (containsKey(s))
		returnedPointer = deleteKey(ptrToChild[this->indexOf(s)]);
	else
		returnedPointer = this;
	if (ptrToParent == NULL || ptrToParent->isEmpty())
		return returnedPointer;
	else return ptrToParent;
}

BPlusPointer BPlusTreeLeaf::deleteKey(RecordPointer p)
{
	int position = indexOf(p);
	int i;
	for (i = position; i < KEYNUM - 1; i++)
	{
		keyValue[i] = keyValue[i + 1];
		ptrToChild[i] = ptrToChild[i + 1];
	}
	keyValue[i] = "";
	ptrToChild[i] = 0;
	ELEMENTCOUNT--;
	if (ptrToParent == NULL || ptrToParent->isEmpty())
	{
		if (ptrToParent != NULL)
			delete ptrToParent;
		ptrToParent = NULL;
	}
	else if (ELEMENTCOUNT >= KEYNUM / 2)
	{
		if (position == 0)//if the first element is deleted, the value must be deleted in it's parent
		{
			ptrToParent->deleteKey(this, keyValue[0]);
		}
		//else do nothing
	}
	else
	{
		BPlusLeaf sibPtr = ptrToParent->findSibling(this);
		if (sibPtr->getElementCount() + this->getElementCount() <= KEYNUM) //if number of element fits into its sibling
		{
			for (int i = 0; i < ELEMENTCOUNT; i++)
				sibPtr->insertKey(ptrToChild[i], keyValue[i], LEFT);
			if (sibPtr->keyValue[0] < keyValue[0])
				ptrToParent->alterKeyValue(ptrToParent->indexOf(sibPtr), sibPtr->keyValue[0]);
			else
				ptrToParent->alterKeyValue(ptrToParent->indexOf(sibPtr) - 1, sibPtr->keyValue[0]);
			if (ptrToParent->indexOf(this) != 0)
			{
				BPlusLeaf leftLeafNode = (BPlusLeaf)(ptrToParent->getPtrToChild(ptrToParent->indexOf(this) - 1));
				leftLeafNode->insertPtrToSibling(this->ptrToSibling);
			}
			ptrToParent->deleteKey(this);
			return sibPtr;
		}
		else            //else if number of element doesn't fits into its sibling
			reDistributePtr(sibPtr);  //redistribute pointer and update tags along the road
	}
	return this;
}

RecordPointer BPlusTreeLeaf::findKey(ElementType s)
{
	for (int i = 0; keyValue[i] != ""; i++)
	{
		if (keyValue[i] == s)
			return ptrToChild[i];
	}
	return -1;
}

void BPlusTreeLeaf::reDistributePtr(BPlusLeaf sibPtr)
{
	RecordPointer* tmpPtr = new RecordPointer[POINTERNUM * 2];
	ElementType* tmpKeyValue = new ElementType[KEYNUM * 2];
	int SIBELEMENT = sibPtr->getElementCount();
	int THISELEMENT = ELEMENTCOUNT;
	int TOTALELEMENT = SIBELEMENT + THISELEMENT;
	bool sibBiggerThanThis;

	string st = sibPtr->getKeyValue(0);
	if (st.compare(keyValue[0]) > 0)
		sibBiggerThanThis = true;

	if (sibBiggerThanThis)
	{
		for (int i = 0; i < THISELEMENT; i++)                 //将Key全部移出
		{
			tmpKeyValue[i] = keyValue[i];
			tmpPtr[i] = ptrToChild[i];
		}
		for (int i = 0; i < SIBELEMENT; i++)
		{
			tmpKeyValue[i + THISELEMENT] = sibPtr->getKeyValue(i);
			tmpPtr[i + THISELEMENT + 1] = sibPtr->getPtrToChild(i);
		}
	}
	else
	{
		for (int i = 0; i < SIBELEMENT; i++)                 //将Key全部移出
		{
			tmpKeyValue[i] = sibPtr->getKeyValue(i);
			tmpPtr[i] = sibPtr->getPtrToChild(i);
		}
		for (int i = 0; i < THISELEMENT; i++)
		{
			tmpKeyValue[i + SIBELEMENT] = keyValue[i];
			tmpPtr[i + SIBELEMENT + 1] = ptrToChild[i];
		}
	}

	int count = 0;
	int MIDPOINT = TOTALELEMENT / 2;
	sibPtr->makeEmpty();
	makeEmpty();

	if (sibBiggerThanThis)
	{
		for (; count < MIDPOINT; count++)
			this->insertKey(tmpPtr[count], tmpKeyValue[count], LEFT);
		for (; count < TOTALELEMENT; count++)
			sibPtr->insertKey(tmpPtr[count], tmpKeyValue[count], LEFT);
		ptrToParent->alterKeyValue(ptrToParent->indexOf(this), sibPtr->getKeyValue(0));
	}
	else
	{
		for (; count < MIDPOINT; count++)
			sibPtr->insertKey(tmpPtr[count], tmpKeyValue[count], LEFT);
		for (; count < TOTALELEMENT; count++)
			this->insertKey(tmpPtr[count], tmpKeyValue[count], LEFT);
		ptrToParent->alterKeyValue(ptrToParent->indexOf(sibPtr), this->getKeyValue(0));
	}
}

void BPlusTreeLeaf::makeEmpty()
{
	int i;
	ELEMENTCOUNT = 0;
	for (i = 0; i < KEYNUM; i++)
	{
		keyValue[i] = "";
		ptrToChild[i] = NULL;
	}
}

RecordPointer BPlusTreeLeaf::getPtrToChild(int index)
{
	return ptrToChild[index];
}

int BPlusTreeLeaf::indexOf(RecordPointer s)
{
	int i;
	for (i = 0; i < KEYNUM; i++)
	if (ptrToChild[i] == s)
		return i;
	return -1;
}

int BPlusTreeLeaf::indexOf(ElementType s)
{
	int i;
	for (i = 0; i < KEYNUM; i++)
	if (keyValue[i] == s)
		return i;
	return -1;
}

void BPlusTreeLeaf::traverse(int level)
{
	cout << '|';
	for (int i = 0; i < KEYNUM; i++)
	{
		cout << keyValue[i] << "|";
	}
	cout << " level:" << level++;
	cout << " Element count=" << ELEMENTCOUNT << endl;
	ELEMENTNUM += ELEMENTCOUNT;
}
////////////////////////////////////////////////////////////////////////////////////////
/// \brief BPlusTreeIndex
/// \param BPlusTreeIndex
///
////////////////////////////////////////////////////////////////////////////////////////
BPlusTreeIndex::BPlusTreeIndex(int maxKeyNum,TYPE type) :MAXKEYNUMBER(maxKeyNum)
{
	Root = new BPlusTreeLeaf(MAXKEYNUMBER);
}

BPlusTreeIndex::~BPlusTreeIndex()
{
	delete Root;
}

void BPlusTreeIndex::addKey(RecordPointer p, ElementType s)throw(exception)
{
	try
	{
		Root = Root->addKey(p, s);
	}
	catch (exception ex)
	{
		throw ex;
	}
}

void BPlusTreeIndex::removeKey(ElementType s)throw(exception)
{
	try
	{
		Root = Root->removeKey(s);
	}
	catch (exception ex)
	{
		throw ex;
	}
}

RecordPointer BPlusTreeIndex::findKey(ElementType s)
{
	return Root->findKey(s);
}

BPlusLeaf BPlusTreeIndex::returnLeafNode(ElementType s)
{
	return Root->returnLeafNode(s);
}

void BPlusTreeIndex::traverseTree()
{
	Root->traverse(0);
}