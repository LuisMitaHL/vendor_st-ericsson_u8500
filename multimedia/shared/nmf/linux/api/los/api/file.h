/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef __INC_LOS_LIBIO_H
#define __INC_LOS_LIBIO_H

#include "type.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/*!
 * \defgroup LOS_FILE LOS Filesystem API
 */

/*!
 * \brief Origin enumeration, can be the beggining of the file, the end, or the current position
 *
 * \ingroup LOS_FILE
 */
typedef enum {
    LOS_SEEK_SET,   //!<  Beginning of file
    LOS_SEEK_CUR,   //!<  Current position of the file pointer
    LOS_SEEK_END    //!<  End of file
} t_los_origin;

/*!
 * \brief LOS file handle.
 * \ingroup LOS_FILE
 */
typedef struct t_los_file_desc t_los_file;


/*!
 * \brief LOS_fopen opens a file on the VFS
 *
 * Filename must indicate mount point used to access the file ("RAM:", "SMH:", ...).
 * By default, if correponding file system is not found or not gived SMH (smeihosting fs)
 * is used. For example, RAM:/home/user/....
 *
    C string containing a file access modes. It can be:
    - "r" Open a file for reading. The file must exist.
    - "w" Create an empty file for writing. If a file with the same name already exists its
	content is erased and the file is treated as a new empty file.
    - "a" Append to a file. Writing operations append data at the end of the file.
	The file is created if it does not exist.
    - "r+"    Open a file for update both reading and writing. The file must exist.
    - "w+"    Create an empty file for both reading and writing. If a file with the same
	name already exists its content is erased and the file is treated as a new empty file.
    - "a+"    Open a file for reading and appending. All writing operations are performed at
	the end of the file,
    	protecting the previous content to be overwritten. You can reposition (fseek, rewind)
	the internal pointer to anywhere in the file for reading,
    	but writing operations will move it back to the end of file.
	The file is created if it does not exist.

 *
 * \param[in] filename file name (string) of the file to open
 * \param[in] mode C string containing a file access modes
 *
 * \return (t_los_file *) pointer on the file instance
 *
 * \see <tt>man fopen</tt> for more information...
 *
 * \ingroup LOS_FILE
 * */
IMPORT_SHARED t_los_file* LOS_fopen 	( const char * filename, const char * mode );

/*!
 * \brief LOS_fread reads an opened "binary" file on the GFS
 *
 *
 * \param[in] ptr pointer to the file data which must be already allocated
 * \param[in] size
 * \param[in] count
 * \param[in] stream pointer on the file instance
 *
 * \return (size_t) size of data really copied to the buffer
 *
 * \see <tt>man fread</tt> for more information...
 *
 * \ingroup LOS_FILE
 * */
IMPORT_SHARED t_los_size 	LOS_fread 	( void * ptr, t_los_size size, t_los_size count, t_los_file * stream );

/*!
 * \brief LOS_fseek seeks an opened "binary" file on the GFS
 *
 * \param[in] stream pointer on the file instance
 * \param[in] offset
 * \param[in] origin
 *
 * \return (int) If successful, the function returns a zero value. Otherwise, it returns nonzero value.
 *
 * \see <tt>man fseek</tt> for more information...
 *
 * \ingroup LOS_FILE
 * */
IMPORT_SHARED int         LOS_fseek 	( t_los_file * stream, long int offset, t_los_origin origin);

/*!
 * \brief LOS_ftell returns the position of an opened "binary" file on the GFS
 *
 * \param[in] stream pointer on the file instance
 *
 * \return (int) the value returned corresponds to the number of bytes from the beginning of the file.
 *
 * \see <tt>man ftell</tt> for more information...
 *
 * \ingroup LOS_FILE
 * */
IMPORT_SHARED long int    LOS_ftell 	( t_los_file * stream );

/*!
 * \brief LOS_fwrite writes an array of count elements.
 *
 * LOS_fwrite writes an array of count elements, each one with a size of size bytes,
 * from the block of memory pointed by ptr to the current position in the stream.
 * The postion indicator of the stream is advanced by the total amount of bytes written.
 * The total amount of bytes written is (size * count).
 *
 * \param[in] ptr Pointer to the array of elements to be written.
 * \param[in] size Size in bytes of each element to be written.
 * \param[in] count Number of elements, each one with a size of size bytes.
 * \param[in] stream Pointer to a FILE object that specifies an output stream.
 *
 * \return The total number of elements successfully written is returned as a t_los_size object,
 *  which is an integral data type. If this number differs from the count parameter, it indicates an error.
 *
 * \see <tt>man fwrite</tt> for more information...
 *
 * \ingroup LOS_FILE
 * */
IMPORT_SHARED t_los_size LOS_fwrite 	( const void * ptr, t_los_size size, t_los_size count, t_los_file * stream );

/*!
 * \brief LOS_fclose closes already opened "binary" file on the GFS and free all memory allocated for it
 *
 * \param[in] stream pointer on the file instance
 *
 * \return (int) If the stream is successfully closed, a zero value is returned. On failure, EOF is returned.
 *
 * \see <tt>man fclose</tt> for more information...
 *
 * \ingroup LOS_FILE
 * */
IMPORT_SHARED int         LOS_fclose	( t_los_file * stream ); // 0 or EOF

#endif //__INC_LOS_LIBIO_H
