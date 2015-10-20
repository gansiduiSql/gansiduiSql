#include <iostream>
#include<fstream>
#include <string>
#include "bplustree.h"
#include "../Definition.h"
#define MAXSIZE 1000
using namespace std;

int ELEMENTNUM;
int main()
{

    string str[MAXSIZE];
    ifstream fin("D:\\QTworkspace\\build-BPlusTree-Desktop_Qt_5_4_1_MinGW_32bit-Debug\\data.txt");
   BPlusTreeIndex* Index=new BPlusTreeIndex(40, INT);

    for(int i=0;i<MAXSIZE;i++)
    {
         fin>>str[i];
        Index->addKey(i,str[i]);
    }
	//Index->traverseTree();
	BPlusLeaf tmp = Index->returnLeafNode("\"Davey,");
	BPlusLeaf tmp1 = Index->returnLeafNode("He");
	while (tmp->getPtrToSinling() != tmp1)
	{
		tmp->traverse(0);
		tmp = tmp->getPtrToSinling();
	}
	tmp->traverse(0);
	cout << ELEMENTNUM;
	fin.close();
	delete Index;
	system("pause");
}

