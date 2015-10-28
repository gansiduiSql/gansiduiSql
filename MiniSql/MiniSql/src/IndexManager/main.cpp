#include <iostream>
#include<fstream>
#include <string>
#include "bplustree.h"
#include "../Definition.h"
#define MAXSIZE 196
using namespace std;

int ELEMENTNUM;
int main()
{

    string str[MAXSIZE];
    ifstream fin("D:\\QTworkspace\\build-BPlusTree-Desktop_Qt_5_4_1_MinGW_32bit-Debug\\data.txt");
   BPlusTreeIndex* Index=new BPlusTreeIndex(8, INT);

    for(int i=0;i<MAXSIZE;i++)
    {
         fin>>str[i];
        Index->addKey(i,str[i]);
    }

	fin.close();
	Index->addKey(1036, "when");
	Index->addKey(1038, "when");
	cout<<"when"<< Index->findKey("when");
	Index->traverseTree();
	/*fin.open("D:\\QTworkspace\\build-BPlusTree-Desktop_Qt_5_4_1_MinGW_32bit-Debug\\data.txt");
	for (int i = 0; i<MAXSIZE; i++)
	{
		fin >> str[i];
		Index->removeKey(str[i]);
	}

	fin.close();*/
	cout << Index->returnLeafNode("-ffff_ffff")->indexOf("ffff_ffff");
	delete Index;
	system("pause");
}

