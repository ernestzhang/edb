#include "memtable.h"
int memtable_init(StMemTable *pstTab)
{
	pstTab->pList = CreateSkipList();
	return 0 ; 
}

int memtable_set();
