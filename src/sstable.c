#include "sstable.h"
#include "ekv_error.h"

int StSStableLevel_Init(StSStableLevel & pstTableLevel)
{
	pstTableLevel->uiCurTableOf0Level = 0 ;
	memset(pstTableLevel->sstable , 0 , sizeof(pstTableLevel->sstable));
	return 0 ;
}

int SSTable_Init(StSStable * pstSSTable , unsigned short usLevelID , unsigned int uiFileID)
{
		memset(pstSSTable , 0 , sizeof(StSStable));
		snprintf(pstSSTable->cFileName , sizeof(pstSSTable->cFileName)-1 , SSTABLE_FILE_NAME , usLevelID , uiFileID);
		pstSSTable->uiLevelID = usLevelID ;
		pstSSTable->uiFileID  = uiFileID ;
		return 0 ;
}

int SSTable_DumpOneSSTable(StSStable * pstSSTable , StMemTable *pstMemTable)
{
		int iRet = 0 ;
		int iFd = open(pstSSTable->cFileName , O_CREAT|O_EXCL|O_RDWR , 0666);
		if(iFd == -1)
			return ERROR_OPEN_SSTABLE_ERROR_FOR_DUMP ;
		else
		{
			//遍历memtable
			int iCnt = 0 ;
			StNode *pNode ;
			if(pstMemTable->pList->pstHead[0] == NULL || pstMemTable->pList->pstHead[0]->pNext == NULL)
			{
				iRet = ERROR_MEMTABLE_EMPTY ;
			}
			else
			{
				unsigned int uiWrBlockSize = 0 ;
				unsigned int uiIndexSize = 0 ;
				char *pWrBlock = (char *)malloc(MAX_NODE_SIZE  + BLOCK_SIZE);
				char *pIndex =(char *)malloc(sizeof(StIndexHead) * pstMemTable->pList->iTotCnt);
				memcpy(pstSSTable->stIndex.cMinKey , pstMemTable->pList->pstHead[0]->pNext->sKey , pstMemTable->pList->pstHead[0]->pNext->usKeyLen);
				memcpy(pstSSTable->stIndex.cMaxKey , pstMemTable->pList->pstHead[0]->pNext->sKey , pstMemTable->pList->pstHead[0]->pNext->usKeyLen);			
				
				while((pNode=pstMemTable->pList->pstHead[0]->pNext) != NULL)
				{
					int iWr = 0 ;
					unsigned int uiTotal = 0 ;
					unsigned int uiOffset = 0 ;
					unsigned int uiKvLen = 0 ;
					StValNode *pValNode =&(pNode->stValNode) ;		
					char *pKv = pWrBlock + uiWrBlockSize ;
					
					uiOffset = sizeof(int);
					*((unsigned int *)(pKv + uiOffset )) = pValNode->uiKeySize ; uiOffset += sizeof(int);
					memcpy(pKv + uiOffset , pValNode->sKey , pValNode->uiKeySize); uiOffset += pValNode->uiKeySize ;
					*((unsigned int *)(pKv + uiOffset )) = pValNode->uiValSize ; uiOffset += sizeof(int);
					memcpy(pKv + uiOffset , pValNode->sVal , pValNode->uiValSize); uiOffset += pValNode->uiValSize ;
					*((unsigned int *)pKv) = GenCheckSum(pKv+sizeof(int) , pValNode->uiKeySize + pValNode->uiValSize + sizeof(int) * 2);
					uiKvLen = pValNode->uiKeySize + pValNode->uiValSize + sizeof(int) * 3 ;
					
					

					//把要dump到文件的索引保存到内存pIndex
					(unsigned int *)(pIndex + uiIndexSize)= pValNode->uiKeySize ;
					memcpy(pIndex + uiIndexSize + sizeof(int) , pValNode->sKey , pValNode->uiKeySize) ;
					(unsigned int *)(pIndex + uiIndexSize + sizeof(int) +  pValNode->uiKeySize) = pstSSTable->uiFileSize + uiWrBlockSize ;
					uiIndexSize += (pValNode->uiKeySize + sizeof(int)*2) ;

					//保存该sstable文件的下界
					if(pstMemTable->funcCompare(pValNode->sKey , pstSSTable->stIndex.cMinKey) < 0)
					{
						memcpy(pstSSTable->stIndex.cMinKey , pValNode->sKey , pValNode->uiKeySize);
					}
					
					//保存该sstable文件的上界
					if(pstMemTable->funcCompare(pValNode->sKey , pstSSTable->stIndex.cMaxKey) > 0)
					{
						memcpy(pstSSTable->stIndex.cMaxKey , pValNode->sKey , pValNode->uiKeySize);
					}
					
					uiWrBlockSize += uiKvLen ;
					//写kv数据
					while(uiWrBlockSize >= BLOCK_SIZE)
					{
						iWr = WriteN(iFd , pWrBlock , BLOCK_SIZE , &uiTotal) ;
						if(iWr == 0)
						{
							uiWrBlockSize -= BLOCK_SIZE ;
							pstSSTable->uiFileSize += BLOCK_SIZE ;
						}
						else
						{
							iRet = ERROR_WRITE_DUMP_FILE ;
							break ;
						}
					}
					if(iWr < 0)
						break ;
					pNode = pNode->pNext ;
				}
				
				//flush kv data
				if(iWr == 0)
				{
					if(uiWrBlockSize > 0)
					{
						iWr = WriteN(iFd , pWrBlock , uiWrBlockSize , &uiTotal) ;
						if(iWr == 0)
							pstSSTable->uiFileSize += uiWrBlockSize ;
						else
						{
							iRet = ERROR_WRITE_DUMP_FILE ;
						}
					}
					
					//write index and meta data
					if(iWr == 0)
					{
						pstSSTable->stIndex.uiIndexSize = uiIndexSize ;
						pstSSTable->stIndex.uiIndexCnt = pstMemTable->pList->iTotCnt ;
						while(uiIndexSize >= BLOCK_SIZE)
						{
							iWr = WriteN(iFd , pIndex , BLOCK_SIZE , &uiTotal) ;
							if(iWr == 0)
							{
								uiIndexSize -= BLOCK_SIZE ;
								pstSSTable->uiFileSize += BLOCK_SIZE ;
							}
							else
							{
								iRet = ERROR_WRITE_DUMP_FILE ;
								break ;
							}
						}
						
						//flush index
						if(iWr == 0)
						{
							if(uiIndexSize > 0)
							{
								iWr = WriteN(iFd , pIndex , uiIndexSize , &uiTotal) ;
								if(iWr == 0)
									pstSSTable->uiFileSize += uiIndexSize ;
								else
								{
									iRet = ERROR_WRITE_DUMP_FILE ;
								}
							}

							//write meta data
							pstSSTable->stIndex.uiCrc32 = GenCheckSum((char *)&pstSSTable->stIndex , sizeof(StSSTIndex)-sizeof(int));
							if(iWr == 0)
							{
								iWr = WriteN(iFd , &pstSSTable->stIndex , sizeof(StSSTIndex) , &uiTotal) ;
								if(iWr == 0)
								{
									pstSSTable->uiFileSize += sizeof(StSSTIndex) ;
									pstSSTable->
								}
								else
								{
									iRet = ERROR_WRITE_DUMP_FILE ;
								}
							}
						}

					}

					
				}
				
				free(pWrBlock) ;
				free(pIndex)

			}
			close(iFd);
			return iRet ;
		}
}

int SSTable_Dump0Level(StSStableLevel *pstTableLevel , StMemTable *pstMemTable)
{
	StSStable *pstSSTable  ;
	pstSSTable = pstTableLevel->sstable + pstTableLevel->uiCurTableOf0Level  ;
	SSTable_Init(pstSSTable , 0 , pstTableLevel->uiCurTableOf0Level);
	if(SSTable_DumpOneSSTable(pstSSTable , pstMemTable) == 0)
	{
		if(++pstTableLevel->uiCurTableOf0Level ==  0_LEVEL_SIZE)
		{
			//start merging 0 level 
		}
	}
}

int SSTable_Load(StSStableMem *pTableMem , char *sFile)
{
	int iRet = 0 ;
	int iFd = open(sFile , O_CREAT|O_RDONLY , 0666);
	if(iFd == -1)
		return ERROR_OPEN_SSTABLE_ERROR_FOR_DUMP ;
	else
	{

		lseek(iFd , 0 , SEEK_END - sizeof(StSSTIndex));
	}
	close(iFd);
}

