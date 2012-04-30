#ifndef __MEMTABLE_INCLUDE
#define __MEMTABLE_INCLUDE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "skiplist.h"
typedef struct _StMemTable
{
	SkipList *pList ;
	SkipList_func_equal funcCompare ;
}StMemTable;

int memtable_init(StMemTable *pstTab , SkipList_func_equal Compare);
int memtable_set(StMemTable *pstTab , StValNode *pNode);
StValNode * memtable_find(StMemTable *pstTab , char *sKey );
int memtable_del(StMemTable *pstTab ,char *sKey  );

#endif
