#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int BSearch(int *iArr , int iSize , int iKey , int **iPos)
{
	  
	  if(iSize >= 2)
	  {
	  	int *pSubArr ;
	  	int iSubSize ;
			if(iKey < iArr[iSize/2])
			{
				 pSubArr = iArr ;
				 iSubSize = iSize/2 ;
			}
			else
			{
				 pSubArr = iArr + (iSize/2) ;
				 iSubSize = iSize - (iSize/2) ;
			}
			return BSearch(pSubArr , iSubSize , iKey , iPos);
		}
		else if(iSize == 1)
		{
				*iPos = iArr ;
			 if(iArr[0] == iKey)
			 	 return 1 ; //ok
			 else
			 {
			 	 return 0 ; //fail
			 }
		}
		else
		{
			 return -1 ; // error search obj
		}
}

/*
 			                   1 , 15
 							           3 , 4 , 16
*/
void MergeDetail(int *pMin , int iMinSize , int *pMax , int iMaxSize , int *iMerge)
{
		//get the merge pos
			int i , j ;
			int *pStartAddr ;
			int iOffset ;
  		BSearch(pMin , iMinSize , pMax[0] ,&pStartAddr);
  		iOffset = pStartAddr - pMin ; 
  		printf("insert pos:%d val:%d\n" , iOffset , *pStartAddr);
  		for(i = 0 ; i <= iOffset ; ++i)
  		{
  				iMerge[i] = pMin[i] ;
  		}
  		++iOffset ;
  		for(j = 0 ; j < iMaxSize && iOffset < iMinSize;)
  		{
  				if(pMax[j] < pMin[iOffset])
  				{
  						iMerge[i++] = pMax[j];
  						++j ;
  				}
  				else
  				{
  						iMerge[i++]	= pMin[iOffset] ;
  						++iOffset ;
  				}
  		}
  		
  		if(j == iMaxSize)
  		{
  				for(;iOffset < iMinSize ;++iOffset)
  				{
  						iMerge[i++] = pMin[iOffset];
  				}
  		}
  		else
  		{
  				for(;j < iMaxSize ; ++j)
  				{
  						iMerge[i++] = pMax[j];
  				}
  		}
  		
}
void Merge(int *iArr1 , int iSize1 , int *iArr2 , int iSize2 , int *iMerge)
{
		
		if(iArr1[0] >= iArr2[0] && iArr1[0] <= iArr2[iSize2-1])
		{
				MergeDetail(iArr2 , iSize2 , iArr1 , iSize1 , iMerge);	
	  }
	  else if(iArr2[0] >= iArr1[0] && iArr2[0] <= iArr1[iSize1-1])
	  {
	  		MergeDetail(iArr1 , iSize1 , iArr2 , iSize2 , iMerge);
	  }
	  else
	  {
	  	 int i ;
	  	 int *pMin = iArr1;
	  	 int *pMax = iArr2;
	  	 int iMinSize = iSize1 ;
	  	 int iMaxSize = iSize2 ;
	  	 if(iArr2[iSize2-1] <= iArr1[0])
	  	 {
	  	 			pMin = iArr2 ;
	  	 			pMax = iArr1 ;
	  	 			iMaxSize = iSize1 ;
	  	 			iMinSize = iSize2 ;
	  	 }
	  	 for(i = 0 ; i < iMinSize ; ++i)
	  	 {
	  	 		iMerge[i] = pMin[i] ;
	  	 }
	  	 for(i = 0 ; i < iMaxSize ; ++i)
	  	 {
	  	 		iMerge[iMinSize + i] = pMax[i] ;	 	
	  	 }
	  }
}

void MergeSort(int *iArr , int iSize)
{
		 
}


int main()
{
	int i;
	int iArr1[]={1,15,16,17,19,22,23};
	int iArr2[]={3,4,16};
	int iSize1 = sizeof(iArr1)/sizeof(iArr1[0]) ;
	int iSize2 = sizeof(iArr2)/sizeof(iArr2[0]) ;
	int *iMerge = (int*)(malloc(iSize1 + iSize2));
	for(i = 0 ; i < (iSize1 + iSize2) ; ++i)
	{
			iMerge[i] = 0 ;
	}
	Merge(iArr1 , iSize1 , iArr2 , iSize2 , iMerge);
	for(i = 0 ; i < (iSize1 + iSize2) ; ++i)
	{
			printf("%d " , iMerge[i]);
	}
	printf("\n");
	return 0 ;
}
