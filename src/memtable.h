#ifndef __MEMTABLE_INCLUDE
#define __MEMTABLE_INCLUDE
#include "skiplist.h"
typedef struct _StMemTable
{
	SkipList *pList ;
}StMemTable;
#endif
