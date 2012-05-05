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