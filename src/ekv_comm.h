#ifndef __EKV_COMM_H_
#define __EKV_COMM_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "ekv_error.h"
#define BLOCK_SIZE (32*1024)
#define MAX_NODE_SIZE (1024 * 1024)

typedef int (*BSearchCompareFunc)(void *p1 , void *p2);
unsigned int GenCheckSum(void *pData , unsigned long uiSize);
int WriteN(int iFd , char *cBuf , unsigned long uiSize , unsigned int *uiTotal);
long ReadN(int iFd , char *cBuf ,  long lSize);
int BSearch(void **pArr , unsigned int  uiSize , void *pKey , BSearchCompareFunc compareFunc  , int *iPos);
#endif
