#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appendlog.h"
#define LOG_FILE "./test.dat"

#define MAX_MERGE_CNT 100000

int compareFunc(void *p1 , void *p2)
{
	
}

int main()
{	
    int i ;
    StLogFile stLog ;
    StListNode *pNode ;
    StMemTable stMemTable ;
    int iCr = CreateLogFile(LOG_FILE , &stLog);
	if(iCr != 0){
	        printf("create file error:%d\n" , iCr);
        	return -1 ;
    }
	
	memtable_init(&stMemTable , compareFunc );
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
        	stNode.uiValSize = (unsigned int)strlen(stNode.sVal);
        	AppendRecord(&stLog , &stNode);
		
	}
	AppendLog(&stLog ,1);
    	DestroyFile(&stLog);
	
	
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
