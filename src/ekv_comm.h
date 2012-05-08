#ifndef __EKV_COMM_H_
#define __EKV_COMM_H_

#define BLOCK_SIZE (32*1024)
#define MAX_NODE_SIZE (1024 * 1024)

unsigned int GenCheckSum(void *pData , unsigned long uiSize);
int WriteN(int iFd , char *cBuf , unsigned long uiSize , unsigned int *uiTotal);
long ReadN(int iFd , char *cBuf ,  long lSize);
#endif
