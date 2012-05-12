#include "memtable.h"

int memtable_init(StMemTable *pstTab , SkipList_func_equal Compare)
{
	pstTab->pList = CreateSkipList();
	pstTab->funcCompare = Compare ;
	return 0 ; 
}

int memtable_set(StMemTable *pstTab , StValNode *pNode)
{
	int iRet = Insert(pstTab->pList , pNode , pstTab->funcCompare , 1);
	return iRet ;
}

StValNode * memtable_find(StMemTable *pstTab , char *sKey)
{
    StNode *pNode = Find(pstTab->pList , sKey , NULL , 0 , pstTab->funcCompare );
	if(pNode != NULL)
	{
	     	return &(pNode->stValNode) ;
	}
	else
	{
		return NULL ;	
	}
}

int memtable_del(StMemTable *pstTab ,char *sKey)
{
        int iRet = Delete(pstTab->pList , sKey , pstTab->funcCompare);
        return iRet ;
}
