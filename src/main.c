#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appendlog.h"
#define LOG_FILE "./test.dat"
int main()
{	
    int i ;
	StLogFile stLog ;
    StListNode *pNode ;
    int iSize ;
#if 1
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
#endif
    
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
	return 0;
}
