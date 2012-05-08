#ifndef __EKV_ERROR_H_
#define __EKV_ERROR_H_
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


   ERROR_OPEN_SSTABLE_ERROR_FOR_DUMP = -2000 , 
   ERROR_MEMTABLE_EMPTY ,
   ERROR_WRITE_DUMP_FILE ,
   ERROR_STAT_SSTABLE_FILE ,
	ERROR_READ_SSTABLE_FILE ,
	ERROR_INDEX_FILE ,
};
#endif
