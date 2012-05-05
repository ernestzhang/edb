#ifndef __SSTABLE_INCLUDE_
#define __SSTABLE_INCLUDE_

#define MAX_KEY_SIZE 256
#define 0_LEVEL_SIZE 4
#define SSTABLE_FILE_NAME "sst_%d_%d"
typedef struct _StTabNode
{
	unsigned int uiCrc32 ;
	StValNode *pNode ;
}StTabNode;

typedef struct _StSSTIndex
{
	char cMinKey[MAX_KEY_SIZE];
	char cMaxKey[MAX_KEY_SIZE];
	unsigned int uiIndexSize ;
	unsigned int uiIndexCnt ;
	unsigned int uiCrc32;
}StSSTIndex;


typedef struct _StSStable
{
	char		 cFileName[256] ;
	unsigned     short uiLevelID ;
	unsigned	 int uiFileID   ;
 	unsigned int uiFileSize ;
	StSSTIndex   stIndex ;
}StSStable;

typedef struct _StIndexHead
{
	unsigned	int uiKeySize ;
	char		cKey[MAX_KEY_SIZE];
	unsigned	int uiOffset ;		
}StIndexHead ;

typedef struct _StSStableLevel
{
	unsigned int uiCurTableOf0Level ;
	StSStable  sstable[0_LEVEL_SIZE];
	
}StSStableLevel ;


typedef struct _StSStableIndexMem
{
	char *sKey ;
	unsigned int uiKeySize ;
	unsigned int uiDataOffset ;
}StSStableIndexMem;
typedef struct _StSStableMem
{
	unsigned int uiNodeNum ;
	StSStableIndexMem *pIndex ;
}StSStableMem;

#endif
