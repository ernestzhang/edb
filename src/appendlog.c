#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h> 
#ifndef _WIN32
	#include <unistd.h>
#endif
#include "appendlog.h"

int CreateLogFile(const char *sFile , StLogFile *pLogFile)
{
    memset(pLogFile , 0 , sizeof(StLogFile));
#ifdef __APPLE__
        int iFd = open(sFile , O_CREAT|O_EXCL|O_RDWR|O_APPEND , 0666);
#else
        int iFd = open(sFile , O_CREAT|O_EXCL|O_RDWR|O_APPEND , 0666);//简单处理先
        //int iFd = open(sFile , O_CREAT|O_EXCL|O_APPEND|O_DIRECT);
#endif
    if (iFd == -1)
    {
        printf("open file error:%s\n" , strerror(errno));
        return ERROR_CREATE_FILE_ERROR ;
    }
    pLogFile->iFd = iFd ;
    strncpy(pLogFile->sFileName , sFile , sizeof(pLogFile->sFileName)-1);
	pLogFile->pTmpNodeBuf = (char *)malloc(MAX_NODE_SIZE);
    if(pLogFile->pTmpNodeBuf == NULL)
        return  ERROR_ALLOC_TEMP_MEM ;
    return 0 ;
}



int AppendLog(StLogFile *pLogFile , int iFlush)
{
	if(pLogFile->ulCurWBufSize >= BLOCK_SIZE || iFlush == 1)
    {
        unsigned int uiWr = 0 ;
        int iRet = WriteN(pLogFile->iFd , pLogFile->cWBuf , pLogFile->ulCurWBufSize  , &uiWr);
        if(iRet == 0)
        {
            pLogFile->ulCurWBufSize = 0 ;
            pLogFile->ulCurSize += uiWr ;
        }
        else
        {
            //暂不处理
            return ERROR_WRITE_FILE_ERROR ;
        }
    }
    return 0;
}





void PackKvNode(char *pBuf , StLogNode *pNode)
{
	unsigned int uiStep = 0 ;
	*((unsigned short *)pBuf) = pNode->uiKeySize  ; uiStep += sizeof(unsigned short);
	memcpy(pBuf + uiStep , pNode->sKey , pNode->uiKeySize); uiStep += pNode->uiKeySize ;
	*((unsigned short *)(pBuf + uiStep)) = pNode->uiValSize ; uiStep += sizeof(unsigned short); 
	memcpy(pBuf + uiStep , pNode->sVal , pNode->uiValSize);
}

void PackLogData(StLogFile *pLogFile , char *pData , unsigned long uiSize , char cType)
{
    StLogRecordNode *pNode =(StLogRecordNode *)(pLogFile->cWBuf + pLogFile->ulCurWBufSize) ;
    pNode->usRecordSize = uiSize ;
    pNode->cType       = cType ;
	pNode->uiCheckSum  = GenCheckSum(pData , uiSize);
	pLogFile->ulCurWBufSize += sizeof(StLogRecordNode);
	
	if(cType != FULL)//当为FULL的时候，Node已经在缓冲区之内了，减少内存copy
	{
		memcpy(pLogFile->cWBuf + pLogFile->ulCurWBufSize , pData ,uiSize);
    }    
    pLogFile->ulCurWBufSize += uiSize ;
    if(BLOCK_SIZE - pLogFile->ulCurWBufSize <= sizeof(StLogRecordNode))//如果剩下的空间小于一个包头，则直接跳过
    {
        pLogFile->ulCurWBufSize = BLOCK_SIZE ;
    }

}


int AppendRecord(StLogFile *pLogFile , StLogNode*pNode)
{
	unsigned long  ulLeft = BLOCK_SIZE - pLogFile->ulCurWBufSize ;
    unsigned long  uiRealNodeSize = sizeof(unsigned short) * 2 + pNode->uiKeySize + pNode->uiValSize ;
    unsigned long  uiMinSize = uiRealNodeSize+sizeof(StLogRecordNode) ;
	if(pNode->uiKeySize + pNode->uiValSize > MAX_NODE_SIZE-128)//留足空间，不要挑战边界
	{
		return ERROR_TOO_LARGE_NODE ;
	}
    if( uiMinSize <= ulLeft)//FULL
    {
		//暂不区分大、小端模式存储
        //当为FULL的时候，直接copy到指定位置，避免多次内存copy
		PackKvNode(pLogFile->cWBuf + pLogFile->ulCurWBufSize + sizeof(StLogRecordNode) , pNode);
		PackLogData(pLogFile , pLogFile->cWBuf + pLogFile->ulCurWBufSize + sizeof(StLogRecordNode) , uiRealNodeSize , FULL);
    }
	else
    {
		char	 cType = FIRST ;
		char *pTmpBuf = pLogFile->pTmpNodeBuf ;
		unsigned long i = BLOCK_SIZE - pLogFile->ulCurWBufSize - sizeof(StLogRecordNode);
		unsigned long iCurPack = 0 ;
		PackKvNode(pTmpBuf , pNode);
		while(iCurPack < uiRealNodeSize)
		{
            int iWr ;
			if(iCurPack != 0)
			{
				i = uiRealNodeSize - iCurPack ;
				if( (i+sizeof(StLogRecordNode)) <= BLOCK_SIZE)//last
					cType = LAST ;
				else //middle
                {
                    cType = MIDDLE ;
                    i = BLOCK_SIZE - sizeof(StLogRecordNode);
                }
					
			}
			PackLogData(pLogFile , pTmpBuf + iCurPack , i , cType);
			iCurPack += i ;
			if(0 != (iWr=AppendLog(pLogFile , 0)))
            {
                return iWr ;
            }
		}
    }
    return AppendLog(pLogFile , 0);

}

long ReadN(int iFd , char *cBuf ,  long lSize)
{
	long lTotal = 0 ;
	long iRead ;
	while(lTotal < lSize)
	{
		iRead = read(iFd , cBuf + lTotal  , lSize - lTotal);
		if(iRead < 0)
		{
			return ERROR_READ_LOG_FILE ;
		}
		else
		{
			 lTotal += iRead ;
		}
	}
	return lTotal ;
}

int IsUnValidDataRecord(StLogRecordNode *pNode , unsigned long ulSize)
{
    if(pNode->usRecordSize == 0 || pNode->usRecordSize > BLOCK_SIZE - sizeof(StLogRecordNode))
    {
        return -1 ;
    }
    if(pNode->uiCheckSum != GenCheckSum(pNode + 1 , pNode->usRecordSize))
    {
        return -1 ;
    }
    return 0 ;
}

StLogNode *CreateNewNode(StLogFile *pLogFile , StLogNode *pData)
{
    StLogNode *pNode = (StLogNode *)malloc(sizeof(StLogNode));
    if(pNode != NULL)
    {
        StListNode *pListNode ;
        pNode->sKey = (char *)malloc(pData->uiKeySize);
        pNode->sVal = (char *)malloc(pData->uiValSize);
        if(pNode->sKey == NULL || pNode->sVal == NULL)
        {
            free(pNode);
            return NULL ;
        }
        memset(pNode->sKey , 0 , pData->uiKeySize);
        memset(pNode->sVal , 0 , pData->uiValSize);
        memcpy(pNode->sKey , pData->sKey , pData->uiKeySize);
        memcpy(pNode->sVal , pData->sVal , pData->uiValSize);
        pNode->uiKeySize = pData->uiKeySize ;
        pNode->uiValSize = pData->uiValSize ;
        //printf("%s %s\n" , pNode->sKey , pNode->sVal);
        pListNode = (StListNode *)malloc(sizeof(StListNode));
        if(pListNode == NULL)
        {
            free(pNode);
            return NULL ;
        }
        pListNode->pNode = pNode ;
        pListNode->pNext = NULL ;
        if(pLogFile->stNodeList.pHead == NULL)
        {
            pLogFile->stNodeList.pHead = pListNode ;
            pLogFile->stNodeList.pHead->pNext = NULL ;
            pLogFile->stNodeList.pTail = pListNode ;
        }
        else
        {
            pLogFile->stNodeList.pTail->pNext = pListNode ;
            pLogFile->stNodeList.pTail = pListNode ;
        }
    }
    return pNode ;
}

int ParseLogData(StLogFile *pLogFile , void *pData , unsigned long ulSize , char cType)
{
    StLogNode stData , *pNode ;
    stData.uiKeySize =  *((unsigned short *)pData);
    stData.sKey = (char *)pData + sizeof(unsigned short) ;
    stData.uiValSize =  *((unsigned short *)((char *)pData + sizeof(short) + stData.uiKeySize));
    stData.sVal = (char *)pData + sizeof(short) + stData.uiKeySize + sizeof(short);
    pNode = CreateNewNode(pLogFile , &stData);
    if(pNode == NULL)
        return ERROR_CREATE_NEW_NODE ;
    if(cType != FULL)
        pLogFile->ulTmpBufSize = 0 ;
    return  0;
}

//处理本次读取的block，并放入tmp buf 中
int ProcessLogFile(StLogFile*pLogFile)
{
    StLogRecordNode *pNode ;
    unsigned long ulCurSize = 0 ;//当前处理的量
    if(pLogFile->ulCurWBufSize <= sizeof(StLogRecordNode))
        return ERROR_LOG_FORMAT ;
    while (ulCurSize < pLogFile->ulCurWBufSize) 
    {
        
        pNode = (StLogRecordNode *)(pLogFile->cWBuf + ulCurSize);
        if(IsUnValidDataRecord(pNode , pNode->usRecordSize))
        {
            return ERROR_LOG_FORMAT ;
        }
    
        if(pNode->cType == FULL)
        {
            if(pLogFile->ulTmpBufSize > 0 )
                return  ERROR_LOG_FORMAT ;
            ParseLogData(pLogFile , pNode + 1 , pNode->usRecordSize , pNode->cType);
            printf("full:%d\n" , pNode->usRecordSize + sizeof(StLogRecordNode));
        }
        else if(pNode->cType == FIRST)
        {
            if(pLogFile->ulTmpBufSize > 0)
                return ERROR_LOG_FORMAT ;
            memcpy(pLogFile->pTmpNodeBuf , pNode + 1 , pNode->usRecordSize);
            pLogFile->ulTmpBufSize += pNode->usRecordSize ;
            
            printf("first:%d\n" , pNode->usRecordSize + sizeof(StLogRecordNode));
        }
        else if(pNode->cType == MIDDLE)
        { 
            if(pLogFile->ulTmpBufSize == 0)
                return  ERROR_LOG_FORMAT ;
            memcpy(pLogFile->pTmpNodeBuf + pLogFile->ulTmpBufSize , pNode + 1 , pNode->usRecordSize);
            pLogFile->ulTmpBufSize += pNode->usRecordSize ;
            
            printf("middle:%d\n" , pNode->usRecordSize + sizeof(StLogRecordNode));
        }
        else if(pNode->cType == LAST)
        {
            if(pLogFile->ulTmpBufSize == 0)
                return  ERROR_LOG_FORMAT ;
            memcpy(pLogFile->pTmpNodeBuf + pLogFile->ulTmpBufSize , pNode + 1 , pNode->usRecordSize);
            pLogFile->ulTmpBufSize += pNode->usRecordSize ;
            ParseLogData(pLogFile , pLogFile->pTmpNodeBuf , pLogFile->ulTmpBufSize , pNode->cType);
            printf("last:%d\n" , pNode->usRecordSize + sizeof(StLogRecordNode));
        }
        ulCurSize += pNode->usRecordSize + sizeof(StLogRecordNode) ;
        //如果剩余的空间不足一个包头，则为填充值
        if(ulCurSize+sizeof(StLogRecordNode) >= BLOCK_SIZE)
        {
            break ;
        }
    }

    return 0;
}

int GetLogData(StLogFile *pLogFile , char *sFile)
{
    
    struct stat _fst ;
	int iFd = open(sFile , O_RDONLY);
	long lFileSize ;
	long lCur ;
    memset(pLogFile , 0 , sizeof(StLogFile));
	if(iFd == -1)
		return ERROR_OPEN_LOG_FILE ;
    strncpy(pLogFile->sFileName , sFile , sizeof(pLogFile->sFileName)-1);
    pLogFile->iFd = iFd ;
	pLogFile->pTmpNodeBuf = (char *)malloc(MAX_NODE_SIZE);
    if(pLogFile->pTmpNodeBuf == NULL)
        return  ERROR_ALLOC_TEMP_MEM ;
    memset(&_fst  , 0 , sizeof(_fst));
	if(0 != fstat(iFd , &_fst))
    {
        return ERROR_FSTAT_LOG_FILE ;
    }
    
    lFileSize = _fst.st_size ;
	for(lCur = 0 ; lCur <  lFileSize ;)
	{
		long lRead , lNeedRead;
		lNeedRead = (lFileSize - lCur) >= BLOCK_SIZE?BLOCK_SIZE:(lFileSize - lCur) ;
		lRead = ReadN(iFd , pLogFile->cWBuf , lNeedRead);
		if(lRead == lNeedRead)
		{
            //处理本次所读取的数据
            int iRet ;
            pLogFile->ulCurWBufSize = lRead ;
			lCur += lRead ;
            if( (iRet=ProcessLogFile(pLogFile)) < 0 )
            {
                return iRet ;
            }
		}
		else
		{
			close(iFd);
			return ERROR_READ_LOG_FILE ;
		}
	}
	close(iFd);
    return 0;
}

int DestroyFile(StLogFile *pLogFile)
{
	if(pLogFile->iFd != -1)
	{
		close(pLogFile->iFd);
		pLogFile->iFd = -1 ;
	}
	if(pLogFile->pTmpNodeBuf != NULL)
	{
		free(pLogFile->pTmpNodeBuf);
		pLogFile->pTmpNodeBuf = NULL ;
	}
	return 0 ;
}


