#include "ekv_comm.h"
unsigned int GenCheckSum(void *pData , unsigned long uiSize)
{
	return 0 ;
}

//后续改为非阻塞
int WriteN(int iFd , char *cBuf , unsigned long uiSize , unsigned int *uiTotal)
{
    *uiTotal = 0 ;
    while(*uiTotal < uiSize)
    {
        int iWr =  (int)write(iFd , cBuf+*uiTotal, uiSize-*uiTotal);
        if(iWr < 0)
        {
            printf("write error:%s\n" , strerror(errno));
            return ERROR_WRITE_FILE_ERROR ;
        }
        else
        {
            *uiTotal += (unsigned int)iWr ;
        }
    }
    return 0 ;
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


int BSearch(void **pArr , unsigned int  uiSize , void *pKey , BSearchCompareFunc compareFunc  , int *iPos)
{
        unsigned int uiLow = 0 ;
        unsigned int uiHigh = uiSize ;
        while(uiLow < uiHigh)
        {
                unsigned int uiMid = (uiLow + uiHigh) / 2 ;
                int iRet = compareFunc(pKey , pArr[uiMid]);
                if(iRet == 0)
                {
                        if(iPos != NULL)
                                *iPos = uiMid ;
                        return  0;
                }
                else if(iRet < 0 )
                {
                        if(uiMid == uiHigh )
                                break ;
                        uiHigh = uiMid ;
                }
                else
                {
                        if(uiMid == uiLow)
                                break ;
                        uiLow = uiMid ;
                }
        }
        if(iPos != NULL)
        {
                *iPos = uiLow ;
                if(*iPos == 0 && compareFunc(pKey , pArr[uiLow]) < 0)
                {
                        *iPos = -1 ;
                }
        }
        return -1 ;
}

