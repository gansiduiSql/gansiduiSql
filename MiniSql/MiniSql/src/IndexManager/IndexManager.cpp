/* @file IndexManager.h
* @brief Implementation of B+ Tree index on a minisql System
*
* 
*
* @author lucas95123@outlook.com
* @version 1.0
* @date 2015/10/19
*/
#include "IndexManager.h"

/* @brief Drop the index named string indexName, delete the BplusTree 
*	in memory and call buffer manager to delete the Index file on disk
* @param indexName The name of the created index  
* @pre Indexname exists
* @return void
* @throw IndexNotExistException 
* @post The BplusTree in memory and Index file on disk will be deleted
*/
void IndexManager::dropIndex(string indexName)throw(exception)
{
	indexLibrary
}

/* @brief Drop the index named string indexName, delete the BplusTree
*	in memory and call buffer manager to delete the Index file on disk
* @param indexName The name of the created index
* @pre Indexname exists
* @return void
* @throw IndexNotExistException
* @post The BplusTree in memory and Index file on disk will be deleted
*/
void IndexManager::createIndex(string indexName, string attributeName, string indexKey,
	TYPE keyValueType, std::string fileName, ADDRESS firstRecordOffset)
{
	//fetch a record from buffer manager and get size of record
	//initiate a B+Tree with 4096/recordSize Nodes;

	BPlusTree *Index=new BPlusTree(4096/recordSize);
	indexLibrary[indexName] = Index;
	//find the attribute where index is created on
	//if TYPE == int or float
	//alter the value to string before insertion
	//else if TYPE == string
	//just insert it

}