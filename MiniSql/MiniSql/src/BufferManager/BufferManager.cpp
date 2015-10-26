/*
it is the cpp file of the BufferManager of the minisql
*it administrate a buffer
*it can read a record and write a record
*the strategy of the buffer substitution is write back
*when it need a substitution, it use LRU(least recently used) to handle it
*@author wang_kejie@foxmail.com
*@date	2015/10/18
*@version 1.0
*/

#include "BufferManager.h"
#include <iostream>

using namespace std;

//default constructor
BufferManager::BufferManager()
{
	openedFilePtr = NULL;
	openedFileName = "";
}

//default deconstructor
BufferManager::~BufferManager()
{
	for (int i = 0; i < BLOCKNUM; i++)
	{
		if (blocks[i].getDirtyBit())
		{
			int *p = (int*)blocks[i].getBlockData();
			for (int i = 0; i < 16; i++)
				cout << p[i] << "\t";
			cout << endl;
			writeABlock(i);
		}
	}
	if (openedFilePtr != NULL)
		fclose(openedFilePtr);
}

/*return a pointer that points to static BufferManager*/
BufferManager* BufferManager::getBufferManager()
{
	static BufferManager bm;

	return &bm;
}

/*when create an index or create a table, create the dbfile in the disk
*set the record tail to be BLOCKSIZE(the start of the data segment)
*@param name the name of the file
*@return void
*@exception if fail to create the file and throw an exception
*/
void BufferManager::createFile(const string& name)
{
	//open the file
	string fileName = name + ".data";
	openedFilePtr = fopen(fileName.c_str(), "wb+");
	//fail to open this file
	if (openedFilePtr == NULL)
		throw OpenFileException(fileName);	//throw the openFile exception
	openedFileName = fileName;

	/*
	//write a empty block into the file
	//BYTE buffer[BLOCKSIZE];
	//fwrite(buffer, sizeof(buffer), 1, openedFilePtr);
	*/
	//set the tail to be the start of the data segment
	int tail = BLOCKSIZE;
	writeARecord((BYTE*)(&tail), sizeof(int), name, 0);
}

/*delete the corresponding file according to the given filename
*@param name the name of the given file
*@return void
*@exception	if fail to delete, throw an exception
*/
void BufferManager::deleteFile(const string& name)
{
	string fileName = name + ".data";

	if (openedFileName == fileName)
	{
		fclose(openedFilePtr);
		openedFileName = "";
		openedFilePtr = nullptr;
	}
	//clear the data in the blocks that contain the data in the deleted filed
	for (int i = 0; i < BLOCKNUM; i++)
	{
		if (blocks[i].getFileName() == fileName)
			blocks[i].clear();
	}
	if ((remove(fileName.c_str())) == -1)
		throw RemoveFileException(fileName);
}

/*fetch a record from the buffer if not hit, fecth it from the file into the buffer first
*@param name the name of the file without suffix name
*@param address	the address of the record in the file
*@return the header address of the record in the block
*/
BYTE* BufferManager::fetchARecord(const string& name, const ADDRESS& address)
{
	int blockIndex;
	string fileName = name + ".data";
	//miss and fecth a block from the file and return the corresponding block
	if ((blockIndex = hit(fileName, address / BLOCKSIZE)) == -1)
		blockIndex = fetchABlock(fileName, address / BLOCKSIZE);	//miss and we must get the data from disk(file)
	else //hit
	{	
		/*visiting this block and move it to tail
		*it devote that it is most recently used and will be substituted last
		*/
		substitutionQue.moveTail(blockIndex);
	}
	//blockoffset is the offset that the address in this block
	int blockOffset = address - (address / BLOCKSIZE) * BLOCKSIZE;
	
	return blocks[blockIndex].getBlockData() + blockOffset;
}

/*decide whether it hits and if it hits, return the block index, otherwise return -1
*@param fileName the file name of visited record
*@param tag the offset(tag) in the file of the visited record
*@return the hitted block index or -1 to devoted miss
*/
int BufferManager::hit(const string& fileName, const ADDRESS& tag)
{
	for (int i = 0; i < BLOCKNUM; i++)
	{
		//hit and return the hitted block index
		if ((blocks[i].getFileName() == fileName) && (blocks[i].getTag() == tag))
			return i;
	}
	//miss and return -1
	return -1;
}

/*write a record if it in buffer, write it into buffer otherwise fecth it from the file and write it
*@param record		the header address of the writing record
*@param recordLength	the length of the writing record
*@param address		the address that writing into the file
*/
void BufferManager::writeARecord(BYTE* record, int recordLength, const string& name, const ADDRESS& address)
{
	int blockIndex;
	string fileName = name + ".data";
	//miss
	if ((blockIndex = hit(fileName, address / BLOCKSIZE)) == -1)
		blockIndex = fetchABlock(fileName, address / BLOCKSIZE);
	else //hit
	{
		/*visiting this block and move it to tail
		*it devote that it is most recently used and will be substituted last
		*/
		substitutionQue.moveTail(blockIndex);
	}

	int blockOffset = address - (address / BLOCKSIZE) * BLOCKSIZE;
	memcpy(blocks[blockIndex].getBlockData() + blockOffset, record, recordLength);
	blocks[blockIndex].setDirtyBit(true);
}

/*miss and find the block and substitute it
*@param filename	the filename of visited record
*@param tag			the tag(the fileblockNum) of visited record
*@param buffer		a block of data stored in buffer
*return a block index that has been substitute
*if find a pinned block and move it to the tail
*/
int BufferManager::substitute(const string& fileName, const ADDRESS& tag, BYTE* buffer)
{
	/*decide which block to substitute*/
	int it = substitutionQue.getHeader();
	while (it != -1 && blocks[it].getPinnnedBit())
	{
		substitutionQue.moveTail(it);
		it = substitutionQue.getHeader();
	}
	if (it == -1)
		throw AllBlockPinned();

	//the block is dirty and write it into the 
	if (blocks[it].getDirtyBit())
		writeABlock(it);
	/*substitute the corrseponding block*/
	substitutionQue.moveTail(it);
	blocks[it].setBlockData(buffer);
	blocks[it].setDirtyBit(false);
	blocks[it].setFileName(fileName);
	blocks[it].setTag(tag);

	return  it;
}

/*fecth a block from the given fileName and address
*@param fileName	the file name of the block to fecth
*@param tag			the file block num(tag) of the file 
*@return the block that it store into
*/
int BufferManager::fetchABlock(const string& fileName, const ADDRESS& tag)
{
	//the open file is not the corresponding file and close it and open the needed file
	if (openedFileName != fileName&&openedFilePtr != NULL)
	{
		fclose(openedFilePtr);
		openedFilePtr = fopen(fileName.c_str(), "rb+");
	}
	//no file is open and open the needed file
	if (openedFilePtr == NULL)
		openedFilePtr = fopen(fileName.c_str(), "rb+");

	//read the corresponding file data into a 4K buffer
	BYTE buffer[BLOCKSIZE];
	fseek(openedFilePtr, tag*BLOCKSIZE, 0);

	fread(buffer, BLOCKSIZE, 1, openedFilePtr);
	//substitute the block
	int blockIndex = substitute(fileName, tag, buffer);

	return blockIndex;
}

/*write a block into the file
*using write back strategy that when a block will be substituted and it is dirty, write it back to the file
*@param blockIndex		the block that you want to write back
*/
void BufferManager::writeABlock(const int& blockIndex)
{
	string fileName = blocks[blockIndex].getFileName();
	FILE* fp = fopen(fileName.c_str(), "ab+");
	fseek(fp, blocks[blockIndex].getTag()*BLOCKSIZE, 0);
	fwrite(blocks[blockIndex].getBlockData(), BLOCKSIZE, 1, fp);
	fclose(fp);
}

/*set the block pinnned
*@param blockIndex	the block that you want to set it pinned
*/
void BufferManager::setBlockPinned(int blockIndex)
{
	blocks[blockIndex].setPinnedBit(true);
}

/*set the block not pinnned
*@param blockIndex	the block that you want to set it pinned
*/
void BufferManager::setBlockNotPinned(int blockIndex)
{
	blocks[blockIndex].setPinnedBit(false);
}