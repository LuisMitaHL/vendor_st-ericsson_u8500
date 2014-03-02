/****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/
#if ( defined(AIQ_MEMTRACE_ENABLE) || defined(__SYMBIAN32__) )

#include "AIQ_CommonTypes.h"
#include "AIQ_Memory.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct AIQ_MemoryChunk {
   AIQ_U32          startCode;
   AIQ_U32          size;
} AIQ_MemoryChunk;

typedef struct  {
   FILE            *LogFile;
   AIQ_U32          ui32_nbAllocations;
   AIQ_U32          ui32_nbDesallocations;
   AIQ_U32          ui32_allocatedSize;
   AIQ_U32          ui32_maxAllocatedSize;
} AIQ_MemoryContext;

/* Global variable storing parameters of memory manager */
AIQ_MemoryContext memoryContext = {NULL,0,0,0,0};

/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API AIQ_Error AIQINT_InitMemTrace(char *fileName)
{
   /* Open Output log file */
   if ((fileName != NULL) && (memoryContext.LogFile == NULL))
   {
      memoryContext.LogFile = fopen(fileName,"w");
      if (memoryContext.LogFile == NULL)
      {
         return(AIQ_SYSTEM_ERROR);
      }
      fprintf(memoryContext.LogFile, "Address\tAction\tSize (Byte)\tId\tLabel\n");
   }

   /* Initialize counters */
   memoryContext.ui32_nbAllocations    = 0;
   memoryContext.ui32_nbDesallocations = 0;
   memoryContext.ui32_allocatedSize    = 0;
   memoryContext.ui32_maxAllocatedSize = 0;

   return AIQ_NO_ERROR;
}

/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API void* AIQINT_malloc(AIQ_U32 size,AIQ_U32 identifier,char* description)
{
   AIQ_U8*           address = NULL;
   AIQ_MemoryChunk  *p_chunk = NULL;

   p_chunk            = (AIQ_MemoryChunk*)malloc(size+sizeof(AIQ_MemoryChunk));
   p_chunk->startCode = 0x12345678;
   p_chunk->size      = size;
   address            = (AIQ_U8*)p_chunk + sizeof(AIQ_MemoryChunk);

   memoryContext.ui32_nbAllocations++;
   memoryContext.ui32_allocatedSize += size;
   if(memoryContext.ui32_allocatedSize > memoryContext.ui32_maxAllocatedSize)
   {
      memoryContext.ui32_maxAllocatedSize = memoryContext.ui32_allocatedSize;
   }

   if (memoryContext.LogFile != NULL)
   {
      fprintf(memoryContext.LogFile,
              "%lu\tallocate\t%lu\t%lu\t%s\n",
              (AIQ_U32)address,
              size,
              (AIQ_U32)identifier,
              description);
	  fflush(memoryContext.LogFile);
   }

   return (void*)address;
}


/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API void* AIQINT_calloc(AIQ_U32 nobj,AIQ_U32 size,AIQ_U32 identifier,char* description)
{
   AIQ_U8*           address  = NULL;
   AIQ_MemoryChunk  *p_chunk  = NULL;
   AIQ_U32           fullSize = nobj*size;

   p_chunk            = (AIQ_MemoryChunk*)malloc(fullSize+sizeof(AIQ_MemoryChunk));
   p_chunk->startCode = 0x12345678;
   p_chunk->size      = fullSize;
   address            = (AIQ_U8*)p_chunk + sizeof(AIQ_MemoryChunk);

   memoryContext.ui32_nbAllocations++;
   memoryContext.ui32_allocatedSize += fullSize;

   if (memoryContext.LogFile != NULL)
   {
      fprintf(memoryContext.LogFile,
              "%lu\tallocate\t%lu\t%lu\t%s\n",
              (AIQ_U32)address,
              fullSize,
              (AIQ_U32)identifier,
              description);
	  fflush(memoryContext.LogFile);
   }

   memset((void*)address,0,fullSize);

   return (void*)address;
}

/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API void AIQINT_free(void* address)
{
   AIQ_MemoryChunk  *p_chunk  = NULL;
   AIQ_U8           *addr     = (AIQ_U8*)address;


   addr   -= sizeof(AIQ_MemoryChunk);
   p_chunk = (AIQ_MemoryChunk*)addr;

   if(p_chunk->startCode != 0x12345678)
   {
      /* this address was not allocated by the memory tracer */
      if (memoryContext.LogFile != NULL)
      {
         fprintf(memoryContext.LogFile,"%lu\tfree called, but this addres was not allocated by AIQ memory tracer!\n",(AIQ_U32)address);
		 fflush(memoryContext.LogFile);
      }
      free(address);
   }

   if (memoryContext.LogFile != NULL)
   {
      fprintf(memoryContext.LogFile,"%lu\tfree\n",(AIQ_U32)address);
	  fflush(memoryContext.LogFile);
   }
   memoryContext.ui32_nbDesallocations++;
   memoryContext.ui32_allocatedSize -= p_chunk->size;
   free(addr);

}

/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API void AIQINT_MemTraceInsertComment(char* comment)
{
   if (memoryContext.LogFile != NULL)
   {
      fprintf(memoryContext.LogFile,"%s\n",comment);
   }
}

/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API void AIQINT_MemTraceStatus(char* comment)
{
   if (memoryContext.LogFile != NULL)
   {
      fprintf(memoryContext.LogFile,"%s, memory status: %ld bytes currently allocated\n",
              comment,
              memoryContext.ui32_allocatedSize);
   }
}

/****************************************************************************
* 
* 
* 
* 
*****************************************************************************/
AIQ_COMMON_API void AIQINT_DeInitMemTrace(void)
{
   AIQ_S32  si32_MemoryLeaks = 0;

   si32_MemoryLeaks = memoryContext.ui32_nbAllocations - memoryContext.ui32_nbDesallocations;

   if(memoryContext.LogFile != NULL)
   {
      if(memoryContext.ui32_allocatedSize != 0)
      {
         fprintf(memoryContext.LogFile,"%ld bytes leaking\n",memoryContext.ui32_allocatedSize);
      }
      else
      {
         fprintf(memoryContext.LogFile,"no memory leak reported\n");
      }
      if (si32_MemoryLeaks != 0)
      {
         fprintf(memoryContext.LogFile,
                 "Error: %ld blocks not desallocated (%ld blocks allocated, %ld blocks desallocated), max allocated size %ld\n",
                 si32_MemoryLeaks,
                 memoryContext.ui32_nbAllocations,
                 memoryContext.ui32_nbDesallocations,
                 memoryContext.ui32_maxAllocatedSize);
      }
      else
      {
         fprintf(memoryContext.LogFile,"Same number of allocation & desallocation reported\n");
      }
      fclose(memoryContext.LogFile);
      memoryContext.LogFile = NULL;
   }
}


#endif // #if (defined(AIQ_MEMTRACE_ENABLE))
