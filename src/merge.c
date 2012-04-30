#include "merge.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
return 0 , find ! iPos will be the position
return -1 , not find . iPos will be the last postion which will be useful
*/
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

/*
 			                   1 , 15
 							           3 , 4 , 16
*/
void Merge(void **pArr1 , unsigned int uiSize1  , void  **pArr2 , unsigned int uiSize2 ,BSearchCompareFunc compareFunc ,  void **pMerge)
{
	int i = 0 ;
	int j  ;
	for(j = 0 ; j < uiSize2 ; )
	{
		int iRet = compareFunc(pArr1[i] , pArr2[j]) ;
		if(iRet <= 0)	
		{
			pMerge[i+j] = pArr1[i] ;
			if(++i >= uiSize1 )
				break ;
		}
		else
		{
			pMerge[i+j] = pArr2[j] ;
			++j ;
		}
	}	
	
	for( ; j < uiSize2 ; ++j)
		 	pMerge[i+j] = pArr2[j];		       	
	for( ; i < uiSize1 ; ++i )
			pMerge[i+j] = pArr1[i];
}


int CompareFunc(void *p1 , void *p2)
{
	int  i1  = *((int*)p1);
	int  i2 = *((int*)p2); 
	if(i1 < i2)
	  return -1 ;
	else if(i1 == i2)
	  return 0 ;
	else
	  return 1 ;
}

void PrintArray(int *iArr , int iSize , int * piKey)
{
	int i;	
 	for(i = 0 ; i < iSize ; ++i)
	{
		printf("%d " , iArr[i]);
	}
	printf("\n");
	scanf("%d" , piKey);

}

int main()
{
	#if 0
	int i;
	int iPos = 0 ;
	int iArr1[]={1,7 , 12 ,15,16,17,19,22,23 , 26};
	int iSize =  sizeof(iArr1)/sizeof(iArr1[0]) ;
	void **pAddr = malloc( iSize * sizeof(void *));
	for(i = 0 ; i < iSize ; ++i)
	{
		pAddr[i] = iArr1 + i ;
	}	
	PrintArray(iArr1 , iSize , &i);
	while(i != -1)
	{
		int iRet = BSearch( pAddr  , sizeof(iArr1)/sizeof(iArr1[0]) , (void *)&i , CompareFunc  ,&iPos);
		if(iRet == 0)
		{
			printf("bingo ! pos:%d\n" , iPos);
		}
		else
		{
			printf("bad luck ! pos:%d\n" , iPos);
		}
		PrintArray(iArr1 , iSize , &i);
	}
	#endif
	int i ;
	int iArr1[] = { 24 , 25};	
	int iArr2[]={1,2,3,4,5 , 8 , 9 , 23 };
	unsigned int iSize1 = sizeof(iArr1)/sizeof(iArr1[0]) ;
	unsigned int iSize2 = sizeof(iArr2)/sizeof(iArr2[0]) ;
	void **p1 = malloc(iSize1 * sizeof(void *));
	void **p2 = malloc(iSize2 * sizeof(void *));
	void **pMerge = (void **)malloc( sizeof(void *)* (iSize1 + iSize2));
	for(i = 0 ; i < iSize1 ; ++i)
        {
                p1[i] = iArr1 + i ;
        }
	for(i = 0 ; i < iSize2 ; ++i)
        {
                p2[i] = iArr2 + i ;
        }
	for(i = 0 ; i < (iSize1 + iSize2) ; ++i)
	{
			pMerge[i] = NULL ;
	}
	Merge(p1 , iSize1 , p2 , iSize2 ,CompareFunc ,  pMerge);
	for(i = 0 ; i < (iSize1 + iSize2) ; ++i)
	{
			printf("%d " , *((int *)pMerge[i]));
	}
	printf("\n");		
	return 0 ;
}
