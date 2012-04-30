#ifndef __INCLUDE_SKIPLIST_
#define __INCLUDE_SKIPLIST_
#define		MAX_HEIGHT		12
typedef struct _StValNode
{
	 char	*sKey ;
	 unsigned int uiKeySize ;
	 char   *sVal ;
	 unsigned int uiValSize ;
}StValNode;

typedef struct _StNode
{
	 StValNode stValNode ;
	struct _StNode *pNext ;
	struct _StNode *pDown ;
}StNode;

typedef struct _SkipList
{
	StNode	*pstHead[MAX_HEIGHT] ;
	int		iLevelCnt	;
	int     iTotCnt		;
	int		iSearchTm   ;
}SkipList ;

typedef int (*SkipList_func_equal)(char *p1 , char *p2)  ;

StNode * Find(SkipList*pList , char *sKey , StNode *pPath[], int iDel , SkipList_func_equal Compare) ;
int Insert(SkipList*pList , StValNode *pValNode , SkipList_func_equal Compare , int iRep) ;
int Delete(SkipList *pList , char *sKey , SkipList_func_equal Compare ) ;
SkipList * CreateSkipList();
void Destroy(SkipList *pList , SkipList_func_equal Compare  );
#endif

