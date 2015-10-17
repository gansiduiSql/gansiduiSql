#ifndef _BPLUSTREE_H_
#define _BPLUSTREE_H_
#define LEFT 0xba
#define RIGHT 0xbb
#define POINTERNUM 187
#define KEYNUM 186
#include <string>
#include <iostream>

using namespace std;
typedef string ElementType;
class BPlusTreeNode;
class Record;
class BPlusTreeLeaf;
typedef BPlusTreeNode* BPlusPointer;
typedef BPlusTreeLeaf* BPlusLeaf;
typedef string* RecordPointer;

class BPlusTreeNode
{
protected:
    //const int KEYNUM;									 //Number of keyvalues in the Node
    //const int POINTERNUM;                          //Number of pointers in the Node
    BPlusPointer ptrToParent;						 //The pointer that points to the parent node
    //ElementType* keyValue;						 //Pointer that points to an array that stores the key value
    ElementType keyValue[KEYNUM];
    int ELEMENTCOUNT;                               //Count of the element in the node
    bool isFull(){return ELEMENTCOUNT==KEYNUM; }
    bool containsKey(ElementType s);      //If the node contains s, returns true, else, returns false
    int leastValueBiggerThan(ElementType s); //returns -1 if not found, returns index if found

private:
    //BPlusPointer* ptrToChild;                       //Pointer that points to an array that stores the pointer to childrens
    BPlusPointer ptrToChild[POINTERNUM];
    void alterParentNode();                       //after spliting nodes on insertion, alter the parent node of its child

public:
    //BPlusTreeNode(int keyNumber);                                                                                    //constructer, allocate space for a node that contains keyNumber elements
    BPlusTreeNode();
    virtual ~BPlusTreeNode();                                                                                                  //virtual distructor, delete dynamic allocated ptrToChild and keyValue
    virtual BPlusPointer addKey(RecordPointer p, ElementType s);                          //recursively add key until to the left
    virtual BPlusPointer removeKey(ElementType s);                                                     //recursively remove key until to the left
    virtual void insertKey(BPlusPointer p, ElementType s, int direction);                 //insert element in the node, if direction is LEFT, p lies left of s, if RIGHT p lies right of s
                                                                                                                                                      //also implement spliting nodes and correct pointers
    virtual void insertPtr(BPlusPointer p);                                                    //insert pointer to the next NON_NULL positon
    virtual BPlusPointer deleteKey(BPlusPointer p);                                                //delete key from node, will recursive goes up
    BPlusPointer deleteKey(BPlusPointer p, ElementType s);
    virtual void reDistributePtr(BPlusPointer sibPtr);                                   //used when deleting, redistrubute the keyvalues
    virtual void traverse(int level);                                                              //traverse down the tree
    virtual void makeEmpty();                                                                    //makes the node empty
    int indexOf(BPlusPointer p);																	//index of a pointer returns -1 if not found
    int indexOf(ElementType s);                                                                 //index of an element returns -1 if not found
    BPlusPointer findSibling(BPlusPointer p);											     //used for nodes connecting non-leaf nodes
    BPlusLeaf findSibling(BPlusLeaf p);                                                       //used for nodes connecting leaf nodes
    void setParentNode(BPlusPointer p);                                                   //set the ptrToPartent node
    void alterKeyValue(int index, ElementType s);                                   //change the key value of the given index
    ElementType getKeyValue(int index);                                                 //get the keyvalue of given index
    BPlusPointer getPtrToChild(int index);                                                  //get the keyvalue of given index
    int getElementCount();                                                                           //return Element count
    bool isEmpty(){return ELEMENTCOUNT==0; }
};

class BPlusTreeLeaf : public BPlusTreeNode
{
private:
    //RecordPointer* ptrToChild;
    RecordPointer  ptrToChild[KEYNUM];
    BPlusPointer ptrToSibling;

public:
    //BPlusTreeLeaf(int keyNumber);
    BPlusTreeLeaf();
    ~BPlusTreeLeaf();
    BPlusPointer addKey(RecordPointer p, ElementType s);
    BPlusPointer removeKey(ElementType s);
    void insertKey(RecordPointer p, ElementType s, int direction);
    BPlusPointer deleteKey(RecordPointer p);
    void deletePtr(RecordPointer p);
    void reDistributePtr(BPlusLeaf sibPtr);
    void makeEmpty();
    RecordPointer getPtrToChild(int index);
    void insertPtrToSibling(BPlusLeaf p);
    int indexOf(RecordPointer s);
    int indexOf(ElementType s);
    void traverse(int level);
};

class BPlusTree
{
    BPlusPointer Root;
    int MAXKEYNUMBER;
public:
    BPlusTree(int maxKeyNum);
    ~BPlusTree();
    bool addKey(RecordPointer p, ElementType s);
    bool removeKey(ElementType s);
    ElementType* findKey(ElementType s);
    void traverseTree();
};


#endif
