#ifndef __SSTABLE_INCLUDE_
#define __SSTABLE_INCLUDE_

#define MAX_KEY_SIZE 256

typedef struct _StTabNode
{
	unsigned int uiCrc32 ;
	StValNode *pNode ;
}StTabNode;

typedef struct _StSSTIndex
{
	char cMinKey[MAX_KEY_SIZE];
	char cMaxKey[MAX_KEY_SIZE];
	long lIndexStart ;
	long lIndexSize ;
	long lIndexCnt ;
	unsigned int uiCrc32;
	StTabNode  **pNode ;
}StSSTIndex;

typedef struct _StSstable
{
	int  iFileID ; 
	int  iFd ;
 	long lFileSize ;
	
}StSstable;




#endif
