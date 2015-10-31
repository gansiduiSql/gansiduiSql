/*create table student(
sid char(10),
sname char(20) unique,
credits int,
dept_name char(8),
gpa float,
primary key (sid)
);

create table inst(
iid char(10),
iname char(20),
salary int,
dept_name char(8),
primary key (iid)
);
*/

#include <iostream>
#include <cstdio>
#include <random>
#include <sstream>
#include <string>
#include <set>
#include <cmath>

using namespace std;

int main()
{
	FILE *sfp = fopen("student_insert.test", "w");
	FILE *ifp = fopen("inst_insert.test", "w");
	string s[7] = { "CS", "SoftWare", "Chinese", "English", "Math", "Physics", "History" };
	set<int> sid, iid;
	set<string> sname, iname;
	for (int i = 0; i<10000; i++)
	{
		//random an id
		int id1 = rand() % 10000000000;
		int id2 = rand() % 10000000000;
		while (sid.find(id1) != sid.end())
			id1 = rand() % 10000000000;
		while (iid.find(id2) != iid.end())
			id2 = rand() % 10000000000;

		sid.insert(id1);
		iid.insert(id2);
		//random a name
		string name1, name2;
		bool flag = true;
		do
		{
			int cnt = rand() % 15;

			for (int j = 0; j<15; j++)
			{
				int ch1 = rand() % 26;
				int ch2 = rand() % 26;
				if (flag)
				{
					name1 += ('A' + ch1);
					name2 += ('A' + ch2);
				}
				else
				{
					name1 += ('a' + ch1);
					name2 += ('a' + ch2);
				}
				flag = false;
			}
		} while (sname.find(name1) != sname.end() || iname.find(name2) != iname.end());

		sname.insert(name1);
		sname.insert(name2);
			//random a credit
		int credit = rand() % 160;

		//random a gpa
		float gpa = rand() % 5 + (rand() % 10) / 10.0 + (rand() % 10) / 100.0;

		//random a salary
		int salary = rand() % 100000 + 100000;
		int dept_name = rand() % 7;

		fprintf(sfp, "%s '%d', '%s', %d ,'%s',%.2f);\n", "insert into student values (", id1, name1.c_str(), credit, s[dept_name].c_str(), gpa);
		fprintf(ifp, "%s '%d', '%s', %d, '%s');\n", "insert into inst values (", id2, name2.c_str(), salary, s[dept_name].c_str());
	}
}