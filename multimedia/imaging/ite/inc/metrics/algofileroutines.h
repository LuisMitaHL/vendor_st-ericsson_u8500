/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Defines
 */

#ifndef __INCLUDE_ALGO_FILE_ROUTINES_H__
#define __INCLUDE_ALGO_FILE_ROUTINES_H__

 /*
 * Includes 
 */
#include "algotypedefs.h"

/** 
 * An enum TAlgoOrigin.
 * Origin enumeration, can be the beggining of the file, the end, or the current position. 
 */
typedef enum 
    {
    ALGO_SEEK_SET,   /**< enum value ALGO_SEEK_SET		 -   For Beginning of file. */
    ALGO_SEEK_CUR,   /**< enum value ALGO_SEEK_CUR		 -   Current position of the file pointer.*/
    ALGO_SEEK_END    /**< enum value ALGO_SEEK_END		 -   End of file.*/

    } TAlgoOrigin;


#ifdef __cplusplus
extern "C"
{
#endif  

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
void* ALGO_fopen 	( const char * aFilename, const char * aMode );

/**
 * ALGO_fclose closes already opened "binary" file on the GFS and free all memory allocated for it.
 *
 * @param aFile          [TAny*] Stream pointer on the file instance 
 *
 * @return               [TInt32] If the stream is successfully closed, a zero value is returned. 
 *                                On failure, EOF is returned.
 */
int         ALGO_fclose	( void * stream );

/**
 * Algo_fcloseall closes already opened all "binary" file on the GFS and free all memory allocated for it.
 *
 * @param                [None]
 *
 * @return               [TAny] None.
 */
int	Algo_fcloseall(void);

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
unsigned int 	ALGO_fread 	( void* aPtr, unsigned int aSize, unsigned int aCount, void* aStream );

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
unsigned int ALGO_fwrite 	( const void* aPtr, unsigned int aSize, unsigned int aCount, void* aStream );

/**
 * ALGO_fseek seeks an opened "binary" file on the GFS.
 *
 * @param aStream       [TAny*] Stream pointer on the file instance .
 * @param aOffset       [TInt64] Offset from the origin
 * @param aOrigin       [TAlgoOrigin] Origin from where offset should be counted.
 *
 * @return              [TInt32] If successful, the function returns a zero value. Otherwise, it returns nonzero value.
 */
int         ALGO_fseek 	( void* aStream, long int aOffset, TAlgoOrigin aOrigin);

/**
 * ALGO_ftell returns the position of an opened "binary" file on the GFS.
 *
 * @param aStream       [TAny*] Stream pointer on the file instance .
 *
 * @return              [TInt32] The value returned corresponds to the number of bytes from the beginning of the file.
 */
long int    ALGO_ftell 	( void* aStream );


#ifdef __cplusplus
}
#endif 

#endif	//__INCLUDE_ALGO_FILE_ROUTINES_H__


