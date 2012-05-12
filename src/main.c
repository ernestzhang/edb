#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appendlog.h"
#define LOG_FILE "./test.dat"

#define MAX_MERGE_CNT (100000*3)

int compareFunc(const void *p1 ,const  void *p2)
{
	StNodeVal *pNode1 = (StNodeVal *)p1 ;
	StNodeVal *pNode2 = (StNodeVal *)p2 ; 
	int iRet = memcmp(pNode1->sKey , pNode2->sKey , pNode1->uiKeySize<pNode2->uiKeySize?pNode1->uiKeySize:pNode2->uiKeySize) ;
	if(iRet == 0)
	{
		if( pNode1->uiKeySize >  pNode2->uiKeySize )
			return 1 ;
		else if (pNode1->uiKeySize ==  pNode2->uiKeySize)
			return 0 ;
		else
			return -1 ;
	}
	else
	{
		return iRet ;	
	}
}

int main()
{	
    int i ;
    StLogFile stLog ;
    StListNode *pNode ;
    StMemTable stMemTable1 ;
	StMemTable stMemTable2 ;
	StMemTable stMemTable3 ;
	StSStableLevel stTableLevel ;
	StSStableMem   stTableMem ;
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
		snprintf(sKey , sizeof(sKey)-1 , "%d_test_key_%d" , iRand , i);			
		snprintf(sVal , sizeof(sVal)-1 , "%d_ssssssssssssssssssssbbbbbbbbbbbbbbbbtest d_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestd_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestd_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestd_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestd_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestd_ssssssssssssssssssssbbbbbbbbbbbbbbbbtestwhy so many sb in......................._val_%d" , iRand , i);
		StLogNode stNode ;
		stNode.sKey = sKey;
		stNode.sVal = sVal;
		stNode.uiKeySize = (unsigned int)strlen(stNode.sKey);
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
		}
		else if(i >= iSplit && i < iSplit * 2)
		{
			memtable_set(&stMemTable2 , &stValNode);
		}
		else
		{
			memtable_set(&stMemTable3 , &stValNode);
		}
	
	}
	AppendLog(&stLog ,1);
    DestroyFile(&stLog);
	
	
	//dump file to sstable 	
	StSStableLevel_Init(&stTableLevel) ;
	SSTable_Dump0Level(&stTableLevel , &stMemTable3);
	iRet =  SSTable_Load(&stTableMem , "sst_0_0.dat") ;
	if(iRet == 0)
	{
		printf("load sstable file ok.\n");
	}
	else
	{
		printf("load sstable file error:%d.\n" , iRet );
		return -1 ; 
	}


	{
		StValNode stNode ;
		
		stNode.sKey = "abcdefg";
		stNode.uiKeySize = strlen(stNode.sKey) ;

		//find in mem table 1
		pNode = memtable_find(stMemTable1 , &stNode);
		if(pNode != NULL)
		{
			printf("find in memtab1 key:[%s] val:[%s]\n" , pNode->sKey , pNode->sVal);
			return 0 ;
		}
		
		printf("[%s]:can't find in memtab1  , start to find in memtable2.\n" , stNode.sKey);
		//find in mem table 2
		pNode = memtable_find(stMemTable2 , &stNode);
		if(pNode != NULL)
		{
			printf("find in memtab2 key:[%s] val:[%s]\n" , pNode->sKey , pNode->sVal);
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
					pData = (void *)((char *)pData + sizeof(int) + stNode.uiKeySize);
					printf("key:[%s] val:[%s]" , stNode.sKey , (void *)pData);
				}
			}
			else
			{
				printf("find:[%s] error:%d\n"  , stNode.sKey , iRet);
				return NULL;
			}
		}

	}
#if 0
    char *sKey[]={"11111111ernestzhang111111111111111111111111111111111111111111111111111" , "hadesmo" , "ivinlinsssssssssssssss" , "frankqian" , "stevenfu" , "ss", "sssssfsfsdf"};
    char *sVal[]={"1111111from wuhan...lllllllllll2b7777777777777777778" ,"from guangxi baizu.......hello world" , "FuJian boydddddssssssssssssssssdddddddddddddddddddssssssssssssssssdddd9" ,"nossssssssss", "enssss.....i don't know", "bbbbbbbb" , "ccccccccccccdddddddddddd7"};
    int iCr = CreateLogFile(LOG_FILE , &stLog);
    if(iCr != 0){
        printf("create file error:%d\n" , iCr);
        return -1 ;
    }
    //printf("%d %d\n" , strlen(sKey[0]) , strlen(sVal[0]));
    iSize =sizeof(sKey)/sizeof(sKey[0]) ;
    printf("size:%d\n" , iSize);
    for(i = 0 ; i < iSize ; ++i)
    {
        StLogNode stNode ;
        stNode.sKey = sKey[i];
        stNode.sVal = sVal[i];
        stNode.uiKeySize = (unsigned int)strlen(stNode.sKey);
        stNode.uiValSize = (unsigned int)strlen(stNode.sVal);
        AppendRecord(&stLog , &stNode);
    }
    AppendLog(&stLog ,1);
    DestroyFile(&stLog);

    
    {
        GetLogData(&stLog ,LOG_FILE);
        pNode = stLog.stNodeList.pHead ;
        while(pNode != NULL)
        {
            char *sKey ;
            char *sVal ;
            sKey = (char *)malloc(pNode->pNode->uiKeySize+1);
            memcpy(sKey ,pNode->pNode->sKey , pNode->pNode->uiKeySize);
            sKey[pNode->pNode->uiKeySize] = 0;
            sVal = (char *)malloc(pNode->pNode->uiValSize+1) ;
                memcpy(sVal , pNode->pNode->sVal , pNode->pNode->uiValSize);
            sVal[pNode->pNode->uiValSize] =0 ;
            printf("%s:%s\n" , sKey , sVal);
            pNode = pNode->pNext ;
        }
    
        DestroyFile(&stLog);
    }
#endif
	return 0;
}
