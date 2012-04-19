#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "skiplist.h"
#define MAX_CNT 1000000
#define MAX_KEY_SIZE 128
//char  pArrKey[MAX_CNT][MAX_KEY_SIZE];
//char  pArrVal[MAX_CNT][1024];

int pArrKey[MAX_CNT];
int CompareFunc(char *p1 , char *p2)
{
	 int i,j; 
	 if(p1 == NULL)
	 	 return -1 ;
	 if(p2 == NULL)
	 	 return  1 ;
	 //return strcmp(p1 , p2);
	  i = *((int*)p1)  ;
	  j =  *((int*)p2) ;
	  if(i < j)
		return -1 ;
	 else if(i == j) 
	       return 0 ;
	else
	 return 1;
}

char *pFILL = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbccccccccc";
//char *pFILL = "ssssssss";

int main()
{
	clock_t start, end;
	int i , j  ; 
	SkipList* pList = CreateSkipList();
	int iCurNode = 0 ;
	int iCntSpace = 0 ;
	StValNode stVal0 ;
	start = time(NULL);
	j = 0 ;
	for(i = 0 ; i < MAX_CNT ; ++i)
	{
		StValNode stVal ;
		char sKey[MAX_KEY_SIZE] ;
		char sVal[1024];
		int iVal = i+1 ;
		//snprintf(sKey , sizeof(sKey)-1 ,"key_%d" , iVal);
		*((int *)sKey) = iVal ;
		stVal.sKey = sKey ;
		stVal.uiKeySize = sizeof(int);//strlen(sKey);
		snprintf(sVal , sizeof(sVal)-1 ,"val_%d_val_%s" , iVal , pFILL );
		stVal.sVal = sVal ;
		stVal.uiValSize = strlen(sVal);
		int iRet = Insert(pList ,&stVal , CompareFunc);
		//printf("key:%s ret:%d\n" , sKey , iRet);
		if(iRet == 0)
		{
			pArrKey[j++] = iVal ;
			//memcpy(pArrKey[j++] , sKey ,  stVal.uiKeySize);
			//memcpy(pArrVal[i] , sVal ,  stVal.uiValSize+1);
			++iCurNode ;
			memcpy(&stVal0 , &stVal ,sizeof(stVal));
		}
		else
		{
			//printf("key:%s insert error\n" , stVal.sKey);
		}
	}
	end = time(NULL);
	printf("node:%d , lev:%d , cost:%d\n" , pList->iTotCnt , pList->iLevelCnt , (int)end - (int)start);


	//i =  Insert(pList , &stVal0 , CompareFunc);
	//printf("insert : %d\n" , i);	
	#if 1 
	i = 0 ;
	iCntSpace = 0;
	while(i != -1)
	{
		iCntSpace = 0 ;
	
		StNode *pStart = pList->pstHead[pList->iLevelCnt > 0? pList->iLevelCnt-1 : 0] ;
		StNode *pDown = NULL ;
		while(1)
		{
			int iLevCnt = 0 ;
			while(pStart != NULL)
			{
				if(pStart->stValNode.sKey == NULL)
				{
					//printf("-inf ");
					pDown = pStart->pDown ;
				}
				else
				{
					//printf("%d " ,pStart->iKey);
				}
				++iCntSpace ;
				++iLevCnt ;
				pStart = pStart->pNext ;
			}
			printf("\n");
			printf("lev:%d\n" , iLevCnt);
			iLevCnt = 0 ;
			if(pDown == NULL)
				break ;
			else
			   pStart = pDown ;
		}
		scanf("%d" , &i);
		printf("node:%d level:%d space:%d  cost:%d\n" ,  pList->iTotCnt , pList->iLevelCnt ,iCntSpace ,   (int)(end - start));
	} 
	//scanf("%d" , &i);
	Destroy(pList , CompareFunc);
	//scanf("%d", &i);
	#endif
	
	return 0 ;
}

