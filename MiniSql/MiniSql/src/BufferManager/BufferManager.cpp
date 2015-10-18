#include "BufferManager.h"

using namespace std;

//default constructor
BufferManager::BufferManager()
{
	openedFilePtr = NULL;
	openedFileName = "";
	/*initial the substitution queue to contain all blocks
	*all these block can be substituted
	*/
	/*head = 0;
	tail = 5;
	for (int i = 0; i < BLOCKNUM; i++)
	{
		substitutionQue[i].next = i + 1;
		substitutionQue[i].last = i - 1;
	}
	substitutionQue[BLOCKNUM - 1].next = -1;*/
}

//default deconstructor
BufferManager::~BufferManager()
{
	for (int i = 0; i < BLOCKNUM; i++)
		if (blocks[i].getDirtyBit())
			writeABlock(i);
	if (openedFilePtr != NULL)
		fclose(openedFilePtr);
}

/*return a pointer that points to static BufferManager*/
BufferManager* BufferManager::getBufferManager()
{
	static BufferManager bm;

	return &bm;
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
	{
		//all blocks are pinned
		//throw an exception
	}

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

/*decide whether it hits and if it hits, return the block index, otherwise return -1
*@param fileName	the file name of visited record
*@param tag    the offset(tag) in the file of the visited record
*@return the hitted block index or -1 to devoted miss
*/
int BufferManager::hit(string fileName, ADDRESS tag)
{
	for (int i = 0; i < BLOCKNUM; i++)
	{
		//hit and return the hitted block index
		if (fileName == blocks[i].getFileName() && tag == blocks[i].getTag())
			return i;
	}
	//miss and return -1 to devoted miss
	return -1;
}

/*fecth a block from the given fileName and address
*@param fileName	the file name of the block to fecth
*@param tag			the file block num(tag) of the file 
*@return the block that it store into
*/
int BufferManager::fetchABlock(const string& fileName, const ADDRESS& tag)
{
	if (openedFileName != fileName&&openedFilePtr != NULL)
	{
		fclose(openedFilePtr);
		openedFilePtr = fopen(fileName.c_str(), "rb");
	}
	if (openedFilePtr == NULL)
		openedFilePtr = fopen(fileName.c_str(), "rb");

	//read the corresponding file data into a 4K buffer
	BYTE buffer[BLOCKSIZE];
	fseek(openedFilePtr, tag*BLOCKSIZE, 0);
	fread(buffer, BLOCKSIZE, 1, openedFilePtr);
	//substitute the block
	int blockIndex = substitute(fileName, tag, buffer);

	return blockIndex;
}

/*fetch a record from the buffer if not hit, fecth it from the file into the buffer first
*@param name	the name of the file without suffix name
*@param address	the address of the record in the file
*@return the header address of the record in the block
*/
BYTE* BufferManager::fetchARecord(const string& name, const ADDRESS& address)
{
	//hit and return the corresponding block
	int blockIndex;
	string fileName = name + ".data";
	//miss and fecth a block from the file and return the corresponding block
	if ((blockIndex = hit(fileName, address / BLOCKSIZE)) == -1)
	{
		//miss and we must get the data from disk(file)
		string fileName = name + ".data";
		blockIndex = fetchABlock(fileName, address / BLOCKSIZE);
	}
	
	int blockOffset = address - (address / BLOCKSIZE) * BLOCKSIZE;
	substitutionQue.moveTail(blockIndex);

	return blocks[blockIndex].getBlockData() + blockOffset;
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

	int blockOffset = address - (address / BLOCKSIZE) * BLOCKSIZE;
	memcpy(blocks[blockIndex].getBlockData() + blockOffset, record, recordLength);
	blocks[blockIndex].setDirtyBit(true);
}

/*write a block into the file
*using write back strategy that when a block will be substituted and it is dirty, write it back to the file
*@param blockIndex		the block that you want to write back
*/
void BufferManager::writeABlock(const int& blockIndex)
{
	string fileName = blocks[blockIndex].getFileName();
	FILE* fp = fopen(fileName.c_str(), "wb");
	fseek(fp, blocks[blockIndex].getTag()*BLOCKSIZE, 0);
	fwrite(blocks[blockIndex].getBlockData(), BLOCKSIZE, 1, fp);
	fclose(fp);
}


/*set the block pinnned
*@param blockIndex		the block that you want to set it pinned
*/
void BufferManager::setBlockPinned(int blockIndex)
{
	blocks[blockIndex].setPinnedBit(true);
}