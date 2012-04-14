//
//  Header.h
//  Debug
//
//  Created by ernest on 12-4-8.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#ifndef __APPENDLOG_H_
#define __APPENDLOG_H_
#define BLOCK_SIZE (32*1024)
#define MAX_NODE_SIZE (1024 * 1024)
enum{
   ERROR_CREATE_FILE_ERROR = -1000,
   ERROR_WRITE_FILE_ERROR ,
   ERROR_TOO_LARGE_NODE ,
   ERROR_OPEN_LOG_FILE ,
   ERROR_FSTAT_LOG_FILE,
   ERROR_READ_LOG_FILE,
   ERROR_LOG_FORMAT ,
    ERROR_CREATE_NEW_NODE,
    ERROR_ALLOC_TEMP_MEM ,
};

enum{
    FULL=1 ,
    FIRST  ,
    MIDDLE ,
    LAST ,
};
typedef struct _StLogNode
{
	char *sKey ;
	unsigned int uiKeySize ;
	char* sVal ;
	unsigned int uiValSize ;
}StLogNode;
typedef struct _StLogRecordNode
{
    unsigned int uiCheckSum    ;
    unsigned short usRecordSize  ;
    char          cType ;
}StLogRecordNode;

typedef struct _StListNode
{
    StLogNode *pNode ;
    struct _StListNode *pNext ;
}StListNode;

typedef struct StLogNodeList
{
	StListNode *pHead ;
    StListNode *pTail ;
}_StLogNodeList;

typedef struct _StLogFile
{
    char sFileName[512];
    int iFd ;
    unsigned long ulCurSize ;
    char cWBuf[BLOCK_SIZE] ; 
    unsigned long ulCurWBufSize ;
	char *pTmpNodeBuf ;
    unsigned long ulTmpBufSize ;
    StLogNodeList stNodeList ;
}StLogFile;
int CreateLogFile(const char *sFile , StLogFile *pLogFile);
#endif
