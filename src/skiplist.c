#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "skiplist.h"



StNode*CreateNode(StValNode *pValNode)
{
	  StNode *pNode = (StNode *)malloc(sizeof(StNode));
	  if(pValNode != NULL)
	  {
			pNode->stValNode.sKey = malloc(pValNode->uiKeySize);
			pNode->stValNode.sVal = malloc(pValNode->uiValSize);
			memcpy(pNode->stValNode.sKey , pValNode->sKey ,pValNode->uiKeySize);
			memcpy(pNode->stValNode.sVal , pValNode->sVal ,pValNode->uiValSize);
			pNode->stValNode.uiKeySize = pValNode->uiKeySize ;
			pNode->stValNode.uiValSize = pValNode->uiValSize ;
	  }
	  else
	  {
		   pNode->stValNode.sKey = NULL ;
		   pNode->stValNode.sVal = NULL ;
	  }
	  pNode->pDown = NULL ;
	  pNode->pNext = NULL ;
	  return pNode ;
}

StNode * Find(SkipList*pList , char *sKey , StNode *pPath[], int iDel , SkipList_func_equal Compare)
{
	StNode *pStart = NULL ;
	int i = 0 ;
	if(pList->iLevelCnt > 0 )
	{
		pStart = pList->pstHead[pList->iLevelCnt-1] ;
		i = pList->iLevelCnt - 1;
	}
	
	while(pStart != NULL)
	{
		if(iDel == 0)
		{
			while(pStart->pNext != NULL && Compare(sKey ,pStart->pNext->stValNode.sKey) >= 0)
				 pStart = pStart->pNext ;
			if(pPath != NULL)
			{
					pPath[i] = pStart ;
					--i ;
			}
		}
		else
		{
			while(pStart->pNext != NULL && Compare(sKey , pStart->pNext->stValNode.sKey) > 0)
				 pStart = pStart->pNext ;
			if(pPath != NULL)
			{
					pPath[i] = pStart ;
					--i ;
			}
		}

		
		if(pStart->pDown == NULL)
				break ;
		else
				pStart = pStart->pDown ;
		pList->iSearchTm++;
	}
	
	while(pStart != NULL )
	{
		if(Compare(sKey , pStart->stValNode.sKey) == 0) //bingo 
			return pStart ;
		else
		{
			if(pStart->pNext == NULL || Compare(sKey , pStart->pNext->stValNode.sKey) < 0 )
			{
				return NULL ;
			}
			pStart = pStart->pNext ;
		}
		pList->iSearchTm++;
	}
	
	return NULL ;
}

int GetHeight()
{
	int i  ;
	int iLevel = 0 ;
	
	i = (rand()%12 > 9 ? 1 : 0) ;
	while(i && iLevel < MAX_HEIGHT)
	{
		++iLevel;
		i = (rand()%12 > 9 ? 1 : 0) ;
	}
	return iLevel >= MAX_HEIGHT ? MAX_HEIGHT : (iLevel+1) ;
}

int Insert(SkipList*pList , StValNode *pValNode , SkipList_func_equal Compare)
{
	    StNode *pPath[MAX_HEIGHT+1] = {0} ;
		StNode *pNode = Find(pList , pValNode->sKey , pPath  , 0 , Compare) ;
		if(pNode == NULL)
		{
			int iHeight = GetHeight();
			int i  ;
			StNode *pUpNode = NULL ;
			if( iHeight > pList->iLevelCnt)//create new level
			{
				StNode *pNewNode = CreateNode(pValNode) ;
				pList->pstHead[pList->iLevelCnt] = CreateNode(NULL);
				pList->pstHead[pList->iLevelCnt]->pNext = pNewNode ;
				if(pList->iLevelCnt > 0)
				{
					pList->pstHead[pList->iLevelCnt]->pDown = pList->pstHead[pList->iLevelCnt-1] ;//head node
				}
				iHeight = ++pList->iLevelCnt ;
				pUpNode = pNewNode ;
				i = iHeight - 2 ;
			}
			else
			{
				i = iHeight - 1;
			}
			
			for(; i >= 0 ; --i)
			{
				StNode *pNewNode = CreateNode(pValNode) ;
				StNode *pNext = pPath[i]->pNext ;
				pPath[i]->pNext = pNewNode ;
				pNewNode->pNext = pNext ;
				if(pUpNode != NULL)//max level
				{
					pUpNode->pDown = pNewNode ;
				}
				pUpNode = pNewNode ; 
			}
			++pList->iTotCnt ;
			return 0;
		}
		else
		{
			return -1 ; //exist
		}
}

int Delete(SkipList *pList , char *sKey , SkipList_func_equal Compare )
{
	StNode *pPath[MAX_HEIGHT+1] = {0} ;
	StNode *pNode = Find(pList , sKey , pPath  , 1 ,Compare) ;
	if(pNode != NULL)
	{
		int i = 0; 
		while(pPath[i] != NULL)
		{
			pList->iSearchTm++;
			if(pPath[i]->pNext != NULL && Compare(pPath[i]->pNext->stValNode.sKey ,sKey) == 0)
			{
				StNode *pDel = pPath[i]->pNext ;
				pPath[i]->pNext = pPath[i]->pNext->pNext ;
				free(pDel->stValNode.sKey);
				free(pDel->stValNode.sVal);
				free(pDel);
				
				if(pPath[i]->stValNode.sKey == NULL && pPath[i]->pNext == NULL)
				{
					free(pList->pstHead[i]);
					pList->pstHead[i] = NULL ;
					--pList->iLevelCnt ;
				}
				++i ;
			}
			else
				break ;
		}
		--pList->iTotCnt ;
		return 0 ;
	}
	else
	{
		return -1 ;
	}
}

SkipList * CreateSkipList()
{
	int i ;
	SkipList *pList = (SkipList *)malloc(sizeof(SkipList));
	pList->iLevelCnt = 0 ; 
	pList->iTotCnt = 0 ;
	for(i = 0 ; i < MAX_HEIGHT ; ++i)
	{
		pList->pstHead[i] = NULL ;
	}
	return pList ;
}

void Destroy(SkipList *pList , SkipList_func_equal Compare  )
{
   while(pList->pstHead[0] != NULL && pList->pstHead[0]->pNext != NULL)
  {
	char * sKey = (char *)malloc(pList->pstHead[0]->pNext->stValNode.uiKeySize);
	memcpy(sKey , pList->pstHead[0]->pNext->stValNode.sKey , pList->pstHead[0]->pNext->stValNode.uiKeySize) ;
	Delete(pList , sKey , Compare );
	free(sKey);
  }
  if(pList != NULL)
	  free(pList);
}




/*
lev cnt:5
lev cnt:39
lev cnt:243
lev cnt:1426
lev cnt:8617
node:8616 level:5 space:10330


lev cnt:3
lev cnt:15
lev cnt:68
lev cnt:318
lev cnt:1097
lev cnt:2750
lev cnt:5302
lev cnt:7710
lev cnt:8627
node:8626 level:9 space:25890


*/


//http://wizard2.webdev.com/tcms/new_video/vpool_do_search.php?service=Video.PoolSearch&keywords=%E5%A5%87%E7%91%9E&page=1&limit=15&field=0&source=-1&order=Createtime&desc=desc&from=1&from=1
