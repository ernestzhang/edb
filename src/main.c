#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appendlog.h"
#include "skiplist.h"
#include "sstable.h"
#include "memtable.h"
#define LOG_FILE "./test.dat"

#define MAX_MERGE_CNT (10*3)

int compareFunc(char *p1 , char *p2)
{
	if(p1 == NULL)
		return -1 ;
	if(p2 == NULL)
		return 1 ;
	return 	strcmp(p1 , p2) ;
}

int main()
{	
    int i ;
    StLogFile stLog ;
    StMemTable stMemTable1 ;
	StMemTable stMemTable2 ;
	StMemTable stMemTable3 ;
	StSStableLevel stTableLevel ;
	StSStableMem   stTableMem ;
	int iFileSize = 0 ;
	int iRet ;
	int iSplit =  MAX_MERGE_CNT/3 ;
    int iCr = CreateLogFile(LOG_FILE , &stLog);
	if(iCr != 0){
	        printf("create file error:%d\n" , iCr);
        	return -1 ;
    }
	
	memtable_init(&stMemTable1 , compareFunc);
	memtable_init(&stMemTable2 , compareFunc);
	memtable_init(&stMemTable3 , compareFunc);
	for(i = 0 ; i < MAX_MERGE_CNT ; ++i)
	{
		int iRand = rand()*rand();
		char sKey[MAX_KEY_SIZE]	;
		char sVal[2048];
		//iRand = 1 ;
		snprintf(sKey , sizeof(sKey)-1 , "%d_test_key_" , iRand );			
		snprintf(sVal , sizeof(sVal)-1 , "%d_ssssssssssssssssssssbbbbbbbbbbbbbbbbtest d_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestd_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestd_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestd_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestd_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestd_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestwhy so many sb in......................._val_%d" , iRand , i);
		StLogNode stNode ;
		stNode.sKey = sKey;
		stNode.sVal = sVal;
		stNode.uiKeySize = (unsigned int)strlen(stNode.sKey) + 1;
		stNode.uiValSize = (unsigned int)strlen(stNode.sVal) + 1;
		AppendRecord(&stLog , &stNode);

		StValNode stValNode ;
		stValNode.sKey = sKey ;
		stValNode.uiKeySize = stNode.uiKeySize ;
		stValNode.sVal = sVal ;
		stValNode.uiValSize = stNode.uiValSize ;
		
		if(i < iSplit)
		{
			memtable_set(&stMemTable1 , &stValNode);
			if(i == iSplit/ 2)
				printf("t1:%s\n" , stValNode.sKey);
		}
		else if(i >= iSplit && i < iSplit * 2)
		{
			memtable_set(&stMemTable2 , &stValNode);
			if(i == iSplit + iSplit/2)
                        	printf("t1:%s\n" , stValNode.sKey);
		}
		else
		{
			memtable_set(&stMemTable3 , &stValNode);
			iFileSize += stValNode.uiKeySize + sizeof(int) * 3 + stValNode.uiKeySize + stValNode.uiValSize + sizeof(int) * 2 ;
			if(i == iSplit*2 + iSplit/2)
                                printf("t1:%s\n" , stValNode.sKey);			
		}
	
	}
	iFileSize += sizeof(StSSTIndex);
        printf("file size:%d\n" , iFileSize);
	AppendLog(&stLog ,1);
    DestroyFile(&stLog);
	
	//dump file to sstable 	
	StSStableLevel_Init(&stTableLevel) ;
	iRet = SSTable_Dump0Level(&stTableLevel , &stMemTable3);
	if(iRet == 0)
	{
		printf("dump sstable ok .\n");
	}
	else
	{
		printf("dump sstable error:%d .\n" , iRet);
	}
	iRet =  SSTable_Load(&stTableMem , "sst_0_0.dat") ;
	{
		for(i = 0 ; i < stMemTable3.pList->iTotCnt ; ++i)
		{
			char *pIndexNode = stTableMem.pIndex[i];
			StValNode *pValNode  = memtable_find(&stMemTable3 ,pIndexNode+sizeof(int)) ; 
			if(pValNode == NULL)
			{
				printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxx========%d key:%s\n" ,i, pIndexNode + sizeof(int));
				return 0 ;	
			}
			if(strcmp(pIndexNode + sizeof(int) , "-450124004_test_key_") == 0 )
			{
				printf("got it ....\n");
			}
			//if(i < 10 || i > stMemTable3.pList->iTotCnt -10)
			{
				unsigned int uiOffset = *((unsigned int*)(pIndexNode+sizeof(int) + pValNode->uiKeySize));
				unsigned int uiNodeSize = *((unsigned int*)(pIndexNode+sizeof(int)*2 + pValNode->uiKeySize));
				printf("%s %s offset:%d node size:%d\n" , pIndexNode + sizeof(int) , pValNode->sKey , uiOffset , uiNodeSize);	
			}	
		}
	}
	if(iRet == 0)
	{
		printf("load sstable file ok. index cnt:%d\n\n" , stTableMem.uiNodeNum);
	}
	else
	{
		printf("load sstable file error:%d.\n" , iRet );
		return -1 ; 
	}

	{
		StValNode stNode ;
		StValNode *pNode ;	
		stNode.sKey = stTableMem.pIndex[5]+sizeof(int);
		stNode.uiKeySize = strlen(stNode.sKey) + 1;

		//find in mem table 1
		pNode = memtable_find(&stMemTable1 , stNode.sKey);
		if(pNode != NULL)
		{	
			printf("find in memtab1 key:[%s] val:[%s]\n" , pNode->sKey , pNode->sVal);
			return 0 ;
		}
		
		printf("[%s]:can't find in memtab1  , start to find in memtable2.\n" , stNode.sKey);
		//find in mem table 2
		pNode = memtable_find(&stMemTable2 , stNode.sKey);
		if(pNode != NULL)
		{
			printf("find in memtab2 key:[%s] val:[%s]\n" , pNode->sKey ,pNode->sVal);
			return 0 ;
		}
		else
		{

			int iRet ;
			void *pData = NULL ;
			unsigned int uiNodeSize = 0 ;
			printf("[%s]:can't find in memtab2  , start to find in sstable.\n" , stNode.sKey);
			iRet = SSTable_Find(&stTableMem , stNode.sKey , stNode.uiKeySize , &pData , &uiNodeSize) ;
			if(iRet == 0)
			{
				if(pData == NULL)
				{
					printf("can't find:[%s] \n" , stNode.sKey);
				}
				else
				{
					pData = (void *)((char *)pData + sizeof(int) + stNode.uiKeySize + sizeof(int));
					printf("key:[%s] val:[%s]" , stNode.sKey , (char *)pData);
				}
			}
			else
			{
				printf("find:[%s] error:%d\n"  , stNode.sKey , iRet);
				return -1;
			}
		}

	}
	return 0;
}
