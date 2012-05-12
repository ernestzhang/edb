#ifndef __SSTABLE_INCLUDE_
#define __SSTABLE_INCLUDE_
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "memtable.h"

#define MAX_KEY_SIZE 1024
#define _0_LEVEL_SIZE 4
#define SSTABLE_FILE_NAME "sst_%d_%d.dat"

#if 0
typedef struct _StTabNode
{
	unsigned int uiCrc32 ;
	StValNode *pNode ;
}StTabNode;
#endif
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
	unsigned 	int uiKvSize ;
}StIndexHead ;

typedef struct _StSStableLevel
{
	unsigned int uiCurTableOf0Level ;
	StSStable  sstable[_0_LEVEL_SIZE];
	
}StSStableLevel ;


typedef struct _StSStableMem
{
	unsigned int uiNodeNum ;
	int iFd ;
	char **pIndex ;
}StSStableMem;

typedef int (*IndexCompareFuncDef)(const void *p1 , const void *p2);
int StSStableLevel_Init(StSStableLevel * pstTableLevel);
int SSTable_Init(StSStable * pstSSTable , unsigned short usLevelID , unsigned int uiFileID);
int SSTable_DumpOneSSTable(StSStable * pstSSTable , StMemTable *pstMemTable);
int SSTable_Dump0Level(StSStableLevel *pstTableLevel , StMemTable *pstMemTable);
int SSTable_Load(StSStableMem *pTableMem , char *sFile);
int IndexCompareFunc(const void *p1 , const void *p2);


int  SSTable_Find(StSStableMem *pTableMem , char *sKey , unsigned int uiKeySize , void **pData , unsigned int* uiValSize);

#endif
