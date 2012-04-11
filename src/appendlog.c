#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#ifndef _WIN32
	#include <unistd.h>
#endif
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
	pLogFile->pTmpNodeBuf = (char *)malloc(MAX_NODE_SIZE);
    return 0 ;
}

//后续改为非阻塞
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

int AppendLog(StLogFile *pLogFile)
{
	if(pLogFile->uiCurWBufSize >= BLOCK_SIZE)
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

unsigned int uiGenCheckSum(char *pData , unsigned int uiSize)
{
	return 0 ;
}

void PackKvNode(char *pBuf , StLogNode *pNode)
{
	unsigned int uiStep = 0 ;
	*((unsigned int *)pBuf) = pNode->uiKeySize  ; uiStep += sizeof(unsigned int);
	memcpy(pBuf + uiStep , pNode->sKey , pNode->uiKeySize); uiStep += pNode->uiKeySize ;
	*((unsigned int *)(pBuf + uiStep)) = pNode->uiValSize ; uiStep += sizeof(unsigned int); 
	memcpy(pBuf + uiStep , pNode->sVal , pNode->uiValSize);
}

void PackLogData(StLogFile *pLogFile , char *pData , unsigned int uiSize , char cType)
{
    StLogRecordNode *pNode =(StLogRecordNode *)(pLogFile->cWBuf + pLogFile->uiCurWBufSize) ;
    pNode->usRecordSize = uiSize ;
    pNode->cType       = cType ;
	pLogFile->uiCurWBufSize += sizeof(StLogRecordNode) + uiSize ;
	pNode->uiCheckSum  = uiGenCheckSum(pData , uiSize);
	pLogFile->uiCurWBufSize += sizeof(StLogRecordNode);
	
	if(cType == FULL)//当为FULL的时候，Node已经在缓冲区之内了，减少内存copy
	{
		pLogFile->uiCurWBufSize += uiSize ;
		if(BLOCK_SIZE - pLogFile->uiCurWBufSize <= sizeof(StLogRecordNode))//如果剩下的空间小于一个包头，则直接跳过
		{
			pLogFile->uiCurWBufSize = BLOCK_SIZE ;
		}
	}
	else
	{
		memcpy(pLogFile->cWBuf + pLogFile->uiCurWBufSize , pData ,uiSize);
	}
}


int AppendRecord(StLogFile *pLogFile , StLogNode*pNode)
{
	unsigned int  ulLeft = BLOCK_SIZE - pLogFile->uiCurWBufSize ;
    unsigned int  uiRealNodeSize = sizeof(unsigned int) * 2 + pNode->uiKeySize + pNode->uiValSize ;
    unsigned int  uiMinSize = uiRealNodeSize+sizeof(StLogRecordNode) ;
	if(pNode->uiKeySize + pNode->uiValSize > MAX_NODE_SIZE-128)//留足空间，不要挑战边界
	{
		return ERROR_TOO_LARGE_NODE ;
	}
    if( uiMinSize <= ulLeft)//FULL
    {
		//暂不区分大、小端模式存储
		PackKvNode(pLogFile->cWBuf + pLogFile->uiCurWBufSize + sizeof(StLogRecordNode) , pNode);
		PackLogData(pLogFile , NULL , uiRealNodeSize , FULL);
		AppendLog(pLogFile);
    }
	else
    {
		int	 cType = FIRST ;
		char *pTmpBuf = pLogFile->pTmpNodeBuf ;
		unsigned int i = BLOCK_SIZE - pLogFile->uiCurWBufSize - sizeof(StLogRecordNode);
		unsigned int iCurPack = 0 ;
		PackKvNode(pTmpBuf , pNode);
		while(iCurPack < uiRealNodeSize)
		{
			if(iCurPack != 0)
			{
				i = uiRealNodeSize - iCurPack ;
				if( (i+sizeof(StLogRecordNode)) <= BLOCK_SIZE)//last
					cType = LAST ;
				else //middle
					cType = MIDDLE ;
			}
			PackLogData(pLogFile , pTmpBuf + iCurPack , i , cType);
			iCurPack += i ;
			AppendLog(pLogFile);	
		}
    }	

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

int GetLogData(StLogFile *pLogFile , char *sFile)
{
	int iFd = open(sFile , O_RDONLY);
	long lFileSize ;
	long lCur ;
	if(iFd == -1)
		return ERROR_OPEN_LOG_FILE ;
	seek(iFd , 0 ,SEEK_END); 
　　lFileSize = tell(iFd); 
	seek(iFd , 0 ,SEEK_SET);
	
	unsigned int uiNodeSize = 0 ;
	char cType ;
	for(lCur = 0 ; lCur <  lFileSize ;)
	{
		long lRead , lNeedRead;
		lNeedRead = (lFileSize - lCur) >= BLOCK_SIZE?BLOCK_SIZE:(lFileSize - lCur) ;
		lRead = ReadN(iFd , pLogFile->cWBuf , lNeedRead);
		if(lRead == lNeedRead)
		{
			StLogRecordNode *pNode = pLogFile->cWBuf ;
			if(pNode->cType == FULL)
			{
				unsigned short usPkgLen = pNode->usRecordSize ;
				
			}
			else if(pNode->cType == FIRST)
			{
				
			}else if(pNode->cType == SECOND)
			{
			
			}
			else if(pNode->cType == LAST)
			{
				
			}


			lCur += lRead ;
		}
		else
		{
			close(iFd);
			return ERROR_READ_LOG_FILE ;
		}
	}
	close(iFd);
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
		pLogFile->pTmpNodeBuf = 0 ;
	}
	return 0 ;
}