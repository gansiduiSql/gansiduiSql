#ifndef _BUFFERMANAGER_H_
#define _BUFFERMANAGER_H_

typedef struct{
	long long blockNum;
	long long blockOffset;
}ADDRESS;

class BufferManager
{
public:
	BufferManager();
	~BufferManager();
	
	
};

#endif