#include <stdio.h>
#define __USE_GNU
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "appendlog.h"
int CreateLogFile(const char *sFile , StLogFile *pLogFile)
{
#ifdef __APPLE__
        int iFd = open(sFile , O_CREAT|O_EXCL|O_APPEND);
#else
        int iFd = open(sFile , O_CREAT|O_EXCL|O_APPEND);//简单处理先
        //int iFd = open(sFile , O_CREAT|O_EXCL|O_APPEND|O_DIRECT);
#endif
    if (iFd == -1)
        return ERROR_CREATE_FILE_ERROR ;
    pLogFile->iFd = iFd ;
    strncpy(pLogFile->sFileName , sFile , sizeof(pLogFile->sFileName)-1);
    pLogFile->ulCurSize = 0 ;
    memset(pLogFile->cWBuf , 0 , sizeof(pLogFile->cWBuf));
    return 0 ;
}

int WriteN(int iFd , char *cBuf , unsigned int uiSize , unsigned int *uiTotal)
{
    *uiTotal = 0 ;
    while(*uiTotal < uiSize)
    {
        int iWr =  (int)write(iFd , cBuf+*uiTotal, uiSize-*uiTotal);
        if(iWr < 0)
        {
            return ERROR_WRITE_FILE_ERROR ;
        }
        else
        {
            *uiTotal += (unsigned int)iWr ;
        }
    }
    return 0 ;
}

int AppendRecord(StLogFile *pLogFile , char *sKey , unsigned int uiKeySize ,char* sVal , unsigned int uiValSize)
{
    unsigned long ulLeft = BLOCK_SIZE - pLogFile->uiCurWBufSize ;
    unsigned int  uiRealNodeSize = sizeof(unsigned int) * 2 + uiKeySize + uiValSize ;
    unsigned int  uiMinSize = uiRealNodeSize+sizeof(StLogRecordNode) ;
    if( uiMinSize <= ulLeft)//FULL
    {
        StLogRecordNode *pRec =(StLogRecordNode *)(pLogFile->cWBuf + pLogFile->uiCurWBufSize) ;
        unsigned int uiOffset = 0 ;
        pRec->uiCheckSum  = 0 ; // in this version , don't care
        pRec->uiRecordSize = uiRealNodeSize ;
        pRec->cType       = FULL ;
        uiOffset += sizeof(StLogRecordNode);
        *((unsigned int *)((char *)pRec + uiOffset)) = uiKeySize ; uiOffset += sizeof(unsigned int); //don't care littend or bigend mode this version
        memcpy((char *)pRec + uiOffset , sKey , uiKeySize); uiOffset += uiKeySize ;
        *((unsigned int *)((char *)pRec + uiOffset)) = uiValSize ; uiOffset += sizeof(unsigned int); 
        memcpy((char *)pRec + uiOffset , sVal , uiValSize); uiOffset += uiValSize ;
        pLogFile->uiCurWBufSize += sizeof(StLogRecordNode) + uiRealNodeSize ;
    }
    else if((sizeof(StLogRecordNode)+1) <= ulLeft)//可以容纳下包头,多1字节，表示每个数据至少包含一个实际值
    {
        StLogRecordNode *pRec =(StLogRecordNode *)(pLogFile->cWBuf + pLogFile->uiCurWBufSize) ;
        unsigned int uiOffset = 0 ;
        pRec->uiCheckSum  = 0 ; // in this version , don't care
        pRec->uiRecordSize = uiRealNodeSize ;
        pRec->cType       = FIRST ;
        uiOffset += sizeof(StLogRecordNode);
        *((unsigned int *)((char *)pRec + uiOffset)) = uiKeySize ; uiOffset += sizeof(unsigned int); //don't care littend or bigend mode this version
        memcpy((char *)pRec + uiOffset , sKey , uiKeySize); uiOffset += uiKeySize ;
        *((unsigned int *)((char *)pRec + uiOffset)) = uiValSize ; uiOffset += sizeof(unsigned int); 
        memcpy((char *)pRec + uiOffset , sVal , uiValSize); uiOffset += uiValSize ;
        pLogFile->uiCurWBufSize += sizeof(StLogRecordNode) + uiRealNodeSize ;
    }
    else//不能容纳包头，重新开辟一个block
    {
        
    }
    
    if(pLogFile->uiCurWBufSize == BLOCK_SIZE)
    {
        unsigned int uiWr = 0 ;
        int iRet = WriteN(pLogFile->iFd , pLogFile->cWBuf , pLogFile->uiCurWBufSize  , &uiWr);
        if(iRet == 0)
        {
            pLogFile->uiCurWBufSize = 0 ;
            pLogFile->ulCurSize += uiWr ;
        }
        else
        {
            //暂不处理
            return ERROR_WRITE_FILE_ERROR ;
        }
    }
}