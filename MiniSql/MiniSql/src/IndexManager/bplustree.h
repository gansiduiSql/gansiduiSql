/* @file IndexManager.cpp
* @brief Implementation of B+ Tree index on a minisql System
* @author lucas95123@outlook.com
* @version 1.0
* @date 2015/10/19
*/
#ifndef _BPLUSTREE_H_
#define _BPLUSTREE_H_
#include <string>
#include <iostream>
#include "../Definition.h"
using namespace std;

typedef string ElementType;
class BPlusTreeNode;
class Record;
class BPlusTreeLeaf;
typedef BPlusTreeNode* BPlusPointer;
typedef BPlusTreeLeaf* BPlusLeaf;
typedef int ADDRESS;

enum{
        LEFT,RIGHT
};

class BPlusTreeNode
{
protected:
    const int KEYNUM;//Number of keyvalues in the Node
    const int POINTERNUM;//Number of pointers in the Node
    BPlusPointer ptrToParent;//The pointer that points to the parent node
    ElementType* keyValue;//Pointer that points to an array that stores the key value
    int ELEMENTCOUNT;//Count of the element in the node
    bool isFull(){return ELEMENTCOUNT==KEYNUM; }
    bool containsKey(ElementType &s);//If the node contains &s, returns true, else, returns false
    int firstValueBiggerThan(ElementType &s); //returns -1 if not found, returns index if found

private:
    BPlusPointer* ptrToChild;//Pointer that points to an array that stores the pointer to childrens
    void alterParentNode();//after spliting nodes on insertion, alter the parent node of its child

public:
    BPlusTreeNode(int keyNumber);//constructer, allocate space for a node that contains keyNumber elements
    virtual ~BPlusTreeNode();//virtual distructor, delete dynamic allocatedconst ElementType &s ptrToChild and keyValue

	//Insertion
    virtual BPlusPointer addKey(const ADDRESS &p,ElementType &s);//recursively add key until to the left
	virtual void insertKey(BPlusPointer p, ElementType &s,const int &direction);//insert element in the node, if direction is LEFT, p lies left of &s, if RIGHT p lies right of &s also implement spliting nodes and correct pointers
	virtual void insertPtr(BPlusPointer p);//insert pointer to the next NON_NULL positon delete key from node, will recursive goes up
	//Deletion
    virtual BPlusPointer removeKey(ElementType &s);//recursively remove key until to the left
	virtual BPlusPointer deleteKey(BPlusPointer p);
	virtual void reDistributePtr(BPlusPointer sibPtr);//used when deleting, redistrubute the keyvalues
	BPlusPointer deleteKey(BPlusPointer p,ElementType &s);
	//Search
	virtual ADDRESS findKey(ElementType &s);
	virtual BPlusLeaf returnLeafNode(ElementType &s);
	virtual BPlusLeaf returnFirstLeafNode();
	virtual BPlusLeaf returnLastLeafNode();
	//General function
	int getKeyNum(){ return KEYNUM; }
	void setParentNode(BPlusPointer p);//set the ptrToPartent node
	ElementType getKeyValue(const int &index);//get the keyvalue of given index
	void alterKeyValue(const int &index,ElementType &s);//change the key value of the given index
	BPlusPointer getPtrToChild(const int &index);//get the keyvalue of given index
	int getElementCount();//return Element count
    virtual void traverse(int level);//traverse down the tree
	bool isEmpty(){ return ELEMENTCOUNT == 0; }
    virtual void makeEmpty();//makes the node empty
    int indexOf(BPlusPointer p);//index of a pointer returns -1 if not found
    int indexOf(ElementType &s);//index of an element returns -1 if not found
    BPlusPointer findSibling(BPlusPointer p);//used for nodes connecting non-leaf nodes
    BPlusLeaf findSibling(BPlusLeaf p);//used for nodes connecting leaf nodes  
};

class BPlusTreeLeaf : public BPlusTreeNode
{
private:
    ADDRESS* ptrToChild;
    BPlusTreeLeaf* ptrToSibling;

public:
    BPlusTreeLeaf(int keyNumber);
    ~BPlusTreeLeaf();
    BPlusPointer addKey(const ADDRESS &p, ElementType &s);
    BPlusPointer removeKey(ElementType &s); 
    BPlusPointer deleteKey(const ADDRESS &p);
	BPlusLeaf returnLeafNode(ElementType &s);
	BPlusLeaf returnFirstLeafNode();
	BPlusLeaf returnLastLeafNode();
	BPlusLeaf getPtrToSinling(){ return ptrToSibling; }
	void insertKey(const ADDRESS &p, ElementType &s, const int &direction);
    void deletePtr(const ADDRESS &p);
    void reDistributePtr(BPlusLeaf sibPtr);
    void makeEmpty();
	void traverse(int level);
    void insertPtrToSibling(BPlusLeaf p);
    int indexOf(const ADDRESS &s);
    int indexOf(ElementType &s);
    
	ADDRESS getPtrToChild(int index);
	ADDRESS findKey(ElementType &s);
};

class BPlusTreeIndex
{
private:
    BPlusPointer Root;
    int MAXKEYNUMBER;
	int offset;
	int length;
	TYPE type;
public:
    BPlusTreeIndex(int maxKeyNum, int attributeLength, int offserInRecord, TYPE type);
    ~BPlusTreeIndex();
    void addKey(ADDRESS p, ElementType &s);
    void removeKey(ElementType &s);
    ADDRESS findKey(ElementType &s);
	BPlusLeaf returnLeafNode(ElementType &s);
	BPlusLeaf returnFirstLeafNode();
    void traverseTree();
	int getMaxKeyNum(){ return MAXKEYNUMBER; }
	int getOffsetInRecord(){ return offset; }
	int getAttributeLength(){ return length; }
	TYPE getAttributeType(){ return type; }
};


#endif
