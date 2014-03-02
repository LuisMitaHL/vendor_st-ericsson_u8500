/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file     algofileroutines.c
* \brief    Contains Algorithm Library File Routines
* \author   ST Ericsson
*/

/*
 * Defines
 */

/*
 * Includes 
 */
#ifndef WIN32
#include <los/api/los_api.h>
#else
#include <stdio.h>
#endif

#include "algoerror.h"
#include "algodebug.h"
#include "algofileroutines.h"
#include "algomemoryroutines.h"

typedef struct TAlgoFileListItem
{
    struct TAlgoFileListItem *iNext;
    struct TAlgoFileListItem *iPrevious;
    TAny *iData;
	TUint32 memHandle;
}TAlgoFileListItem_s;

static TUint32 sMemoryHandleList = 0;
static int sNbFopen = 0;
static int sNbFclose = 0;

TAny ListPushBack(TAny* aData)
{
	TUint32 memoryHandle = 0;
	TAlgoFileListItem_s* item = (TAlgoFileListItem_s*)NULL;
	TAlgoFileListItem_s* itemNew = (TAlgoFileListItem_s*)NULL;

	if(sMemoryHandleList == 0)
	{
		sMemoryHandleList = (TUint32)AlgoMalloc(sizeof(TAlgoFileListItem_s));
		if(0 == sMemoryHandleList)ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);
		item = (TAlgoFileListItem_s*)LOS_GetLogicalAddress(sMemoryHandleList);
		item->iPrevious = (struct TAlgoFileListItem *)NULL;
		item->iNext = (struct TAlgoFileListItem *)NULL;
		item->iData = aData;	
		item->memHandle = sMemoryHandleList;
	}
	else
	{
		item = (TAlgoFileListItem_s*)LOS_GetLogicalAddress(sMemoryHandleList);

		while(item->iNext != NULL)
		{
			item = item->iNext;
		}

		memoryHandle = (TUint32)AlgoMalloc(sizeof(TAlgoFileListItem_s));
		if(0 == memoryHandle)ALGO_Log_2("Error, can not allocate memory : fct %s, line %d\n", __FUNCTION__, __LINE__);
		itemNew = (TAlgoFileListItem_s*)(LOS_GetLogicalAddress(memoryHandle));
		item->iNext = itemNew;
		itemNew->iPrevious = item;
		itemNew->iNext = (struct TAlgoFileListItem *)NULL;
		itemNew->iData = aData;
		itemNew->memHandle = memoryHandle;
	}
}   

TAlgoFileListItem_s* ListPopBack()
{
	TAlgoFileListItem_s* item = (TAlgoFileListItem_s*)NULL;

	if(sMemoryHandleList != 0)
	{
		item = (TAlgoFileListItem_s*)LOS_GetLogicalAddress(sMemoryHandleList);
		while(item->iNext != NULL)
		{
			item = item->iNext;
		}

		if(item->iPrevious != NULL)
		{
			item->iPrevious->iNext = (struct TAlgoFileListItem *)NULL;
		}
		else
		{
			//ALGO_Log_0("Removing first element of FileList\n");
			sMemoryHandleList = 0;
		}
	}

	return item;
}

void ListRemoveItem(TAny* aData)
{
	TAlgoFileListItem_s* item = (TAlgoFileListItem_s*)NULL;

	if(sMemoryHandleList != 0)
	{
		item = (TAlgoFileListItem_s*)LOS_GetLogicalAddress(sMemoryHandleList);

		while( (item != NULL) && (item->iData != aData) )
		{
			item = item->iNext;
		}

		if(item == NULL)
		{
			ALGO_Log_0("Warning: imposssible to remove item\n");
			return;
		}

		if(item->iPrevious == NULL)
		{
			AlgoFree((TAny *)item->memHandle);
			sMemoryHandleList = 0;
		}
		else
		{
			if(item->iNext != NULL)
			{
				item->iPrevious->iNext = item->iNext;
				item->iNext->iPrevious = item->iPrevious;
				AlgoFree((TAny *)item->memHandle);
			}
			else
			{
				item->iPrevious->iNext = (struct TAlgoFileListItem *)NULL;
				AlgoFree((TAny *)item->memHandle);
			}
		}
	}
}

/**
 * ALGo_fopen opens a file on the VFS.
 * Filename must indicate mount point used to access the file ("RAM:", "SMH:", ...). 
 * By default, if correponding file system is not found or not gived SMH (smeihosting fs) 
 * is used. For example, RAM:/home/user/....
 * 
 * C string containing a file access modes. It can be:
 * - "r" Open a file for reading. The file must exist.
 * - "w" Create an empty file for writing. If a file with the same name already exists its 
 * content is erased and the file is treated as a new empty file.
 * - "a" Append to a file. Writing operations append data at the end of the file. 
 * The file is created if it does not exist.
 * - "r+"    Open a file for update both reading and writing. The file must exist.
 * - "w+"    Create an empty file for both reading and writing. If a file with the same 
 * name already exists its content is erased and the file is treated as a new empty file.
 * - "a+"    Open a file for reading and appending. All writing operations are performed at 
 * the end of the file,
 * protecting the previous content to be overwritten. You can reposition (fseek, rewind) 
 * the internal pointer to anywhere in the file for reading,
 * but writing operations will move it back to the end of file. 
 * The file is created if it does not exist.
 *
 * @param aFilename     [const TInt8*] Filename file name (string) of the file to open.
 * @param aMode         [const TInt8*] Mode C string containing a file access modes.
 *
 * @return              [TAny*] Pointer on the file instance.
 */
void* ALGO_fopen( const char * aFilename, const char * aMode )
{
#ifndef WIN32
	t_los_file* file = (t_los_file*)NULL;
	file = LOS_fopen(aFilename, aMode );
#else
	FILE* file = (FILE*)NULL;
	file = fopen(aFilename, aMode);
#endif
	if(file != NULL)
	{
		ListPushBack(file);
		sNbFopen++;
	}
#ifdef ALGO_FILE_TRACE_FILE_OPEN_CLOSE
	ALGO_Log_1(" sNbFopen = %d\n", sNbFopen);
#endif
	return (void*)file;
}

/**
 * ALGO_fclose closes already opened "binary" file on the GFS and free all memory allocated for it.
 *
 * @param aFile          [TAny*] Stream pointer on the file instance 
 *
 * @return               [TInt32] If the stream is successfully closed, a zero value is returned. 
 *                                On failure, EOF is returned.
 */
int ALGO_fclose( void * stream )
{
	sNbFclose++;
#ifdef ALGO_FILE_TRACE_FILE_OPEN_CLOSE
	ALGO_Log_2(" sNbFclose = %d with sNbFopen = %d\n", sNbFclose, sNbFopen);
#endif
	ListRemoveItem(stream);
#ifndef WIN32
	return LOS_fclose( (t_los_file *) stream );
#else
	return fclose( (FILE *) stream );
#endif
}

/**
 * Algo_fcloseall closes already opened all "binary" file on the GFS and free all memory allocated for it.
 *
 * @param                [None]
 *
 * @return               [TAny] None.
 */
int Algo_fcloseall(void)
{
	TAlgoFileListItem_s *item = ListPopBack();

	while(item != NULL)
	{
		sNbFclose++;
#ifdef ALGO_FILE_TRACE_FILE_OPEN_CLOSE
		ALGO_Log_2(" sNbFclose = %d with sNbFopen = %d\n", sNbFclose, sNbFopen);
#endif

#ifndef WIN32
		LOS_fclose( (t_los_file *) item->iData );
#else
		fclose( (FILE *) item->iData );
#endif

		AlgoFree((TAny *)item->memHandle);
		item = ListPopBack();
	}
	return 0;
}

/**
 * ALGO_fread reads an opened "binary" file on the GFS
 *
 * @param aPtr          [TAny*] Pointer to the file data which must be already allocated.
 * @param aSize         [TUint32] Size in bytes of each element to be read.
 * @param aCount        [TUint32] Number of elements, each one with a size of size bytes.
 * @param aStream       [TAny*] Stream pointer on the file instance.
 *
 * @return              [TUint32] Size of data really copied to the buffer.
 */
unsigned int ALGO_fread( void * aPtr, unsigned int aSize, unsigned int aCount, void * aStream )
{
#ifndef WIN32
	return 	(unsigned int)LOS_fread( aPtr, (t_los_size) aSize, (t_los_size) aCount, (t_los_file *) aStream );
#else
	return 	(unsigned int)fread( aPtr, (size_t) aSize, (size_t) aCount, (FILE *) aStream );
#endif
}

/**
 * ALGO_fwrite writes an array of count elements, each one with a size of size bytes, 
 * from the block of memory pointed by ptr to the current position in the stream.
 * The postion indicator of the stream is advanced by the total amount of bytes written.
 * The total amount of bytes written is (size * count).
 *
 * @param aPtr          [TAny*] Pointer to the array of elements to be written.
 * @param aSize         [TUint32] Size in bytes of each element to be written.
 * @param aCount        [TUint32] Number of elements, each one with a size of size bytes.
 * @param aStream       [TAny*] Pointer to a FILE object that specifies an output stream.
 *
 * @return              [TUint32] The total number of elements successfully written is returned as a 
 *                                t_ALGO_size object, which is an integral data type. If this number 
 *                                differs from the count parameter, it indicates an error.
 */
unsigned int ALGO_fwrite( const void * aPtr, unsigned int aSize, unsigned int aCount, void* aStream )
{
#ifndef WIN32
	return (unsigned int) LOS_fwrite(aPtr, (t_los_size) aSize, (t_los_size) aCount, (t_los_file *) aStream );
#else
	return (unsigned int) fwrite(aPtr, (size_t) aSize, (size_t) aCount, (FILE *) aStream );
#endif
}

/**
 * ALGO_fseek seeks an opened "binary" file on the GFS.
 *
 * @param aStream       [TAny*] Stream pointer on the file instance .
 * @param aOffset       [TInt64] Offset from the origin
 * @param aOrigin       [TAlgoOrigin] Origin from where offset should be counted.
 *
 * @return              [TInt32] If successful, the function returns a zero value. Otherwise, it returns nonzero value.
 */
int ALGO_fseek( void * aStream, long int aOffset, TAlgoOrigin aOrigin )
{
#ifndef WIN32
	t_los_origin los_origin = LOS_SEEK_SET;
	switch(aOrigin)
	{
		case ALGO_SEEK_SET:
			los_origin = LOS_SEEK_SET; break;
		case ALGO_SEEK_CUR:
			los_origin = LOS_SEEK_CUR; break;
		case ALGO_SEEK_END:
			los_origin = LOS_SEEK_END; break;
	}
	return LOS_fseek( (t_los_file *) aStream, aOffset, los_origin);
#else
	int stdio_origin = SEEK_SET;
	switch(aOrigin)
	{
		case ALGO_SEEK_SET:
			stdio_origin = SEEK_SET; 
            break;
		case ALGO_SEEK_CUR:
			stdio_origin = SEEK_CUR; 
            break;
		case ALGO_SEEK_END:
			stdio_origin = SEEK_END; 
            break;
	}

	return fseek( (FILE *) aStream, aOffset, (int) stdio_origin );
#endif
}

/**
 * ALGO_ftell returns the position of an opened "binary" file on the GFS.
 *
 * @param aStream       [TAny*] Stream pointer on the file instance .
 *
 * @return              [TInt32] The value returned corresponds to the number of bytes from the beginning of the file.
 */
long int ALGO_ftell( void * aStream )
{
#ifndef WIN32
	return LOS_ftell( (t_los_file *) aStream );
#else
	return ftell( (FILE *) aStream );
#endif
}



