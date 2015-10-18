#include <iostream>
#include<fstream>
#include <string>
#include "bplustree.h"
#include <windows.h>
#define MAXSIZE 40
using namespace std;

int ELEMENTNUM;
int main()
{
  /*  LARGE_INTEGER freq;
    LARGE_INTEGER counterBegin,counterEnd;*/

    string str[MAXSIZE];
    ifstream fin("D:\\QTworkspace\\build-BPlusTree-Desktop_Qt_5_4_1_MinGW_32bit-Debug\\data.txt");
    BPlusTree Index(6);
  /*  QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counterBegin);*/

    for(int i=0;i<MAXSIZE;i++)
    {
         fin>>str[i];
        Index.addKey(i,str[i]);
    }
	fin.close();

	fin.open("D:\\QTworkspace\\build-BPlusTree-Desktop_Qt_5_4_1_MinGW_32bit-Debug\\data.txt");
	for (int i = 0; i<15; i++)
	{
		fin >> str[i];
		Index.removeKey(str[i]);
	}
	fin >> str[i];
	Index.removeKey(str[i]);

//    Index.removeKey("He");
  //  QueryPerformanceCounter(&counterEnd);
    Index.traverseTree();
 //   cout<<"Total Element:"<<ELEMENTNUM;
  //  QueryPerformanceCounter(&counterEnd);
 //    printf("time:%lf\n", (double)(counterEnd.QuadPart-counterBegin.QuadPart)/(double)freq.QuadPart);


	 system("pause");
}

