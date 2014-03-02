/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "FILE-IO"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include <fileio.nmf>
#include <fileio_types.idt.h>
#include <stdio.h>
#include "osi_trace.h"

/*
 * Constructor
 */
METH(fileio)()
{
    IN0("");
    OUT0("");
    return;
}

/*
 * Local stuff
 */
#define HASH_ENTRY(id) {id,#id}

typedef struct
{
    t_fileio_error id;
    const char* name;
} t_fileio_error_entry;

static const t_fileio_error_entry KFioErrorCodesList[] =
{
    HASH_ENTRY(FILE_IO_OK),
    HASH_ENTRY(FILE_IO_BAD_PARAMETER),
    HASH_ENTRY(FILE_IO_OPEN_FAILED),
    HASH_ENTRY(FILE_IO_SEEK_FAILED),
    HASH_ENTRY(FILE_IO_READ_FAILED),
    HASH_ENTRY(FILE_IO_WRITE_FAILED)
};

#define KFioErrorCodesListSize (sizeof(KFioErrorCodesList)/sizeof(KFioErrorCodesList[0]))

/*
 * Destructor
 */
METH(~fileio)()
{
    IN0("");
    OUT0("");
    return;
}

/*
 * Test method
 */
void fileio::test(void)
{
    ASYNC_IN0("");
    response.fileIoResponse(FILE_IO_OK);
    ASYNC_OUT0("");
    return;
}

/*
 * Size method
 */
void fileio::size(const char* aFileName, t_uint32* aSizePtr)
{
    ASYNC_IN0("");
    FILE* theFile = NULL;

    // Sanity check
    if( aFileName == NULL || aSizePtr == NULL)
    {
        // bad parameter
        MSG2("Bad parameter: aFileName=%p, aSizePtr=%p\n", aFileName, aSizePtr);
        response.fileIoResponse(FILE_IO_BAD_PARAMETER);
        ASYNC_OUT0("");
        return;
    }

    // Open the file
    MSG1("Opening file '%s' for reading\n", aFileName);
    theFile = fopen( aFileName, "rb");
    if(theFile==NULL)
    {
        // could not open the file
        MSG1("File '%s' open failed\n", aFileName);
        response.fileIoResponse(FILE_IO_OPEN_FAILED);
        ASYNC_OUT0("");
        return;
    }

    // Seek to end of file
    MSG1("Seeking to end of file '%s'\n", aFileName);
    if( fseek( theFile, 0, SEEK_END) != 0)
    {
        // seek failed
        MSG0("Seek failed");
        MSG1("Closing file '%s'\n", aFileName);
        fclose(theFile);
        response.fileIoResponse(FILE_IO_SEEK_FAILED);
        ASYNC_OUT0("");
        return;
    }

    // Retrieve file size
    *aSizePtr = (t_uint32)ftell(theFile);
    MSG2("File '%s' size %ld\n", aFileName, *aSizePtr);

    // File size sucessfully retrieved
    MSG1("Closing file '%s'\n", aFileName);
    fclose(theFile);
    response.fileIoResponse(FILE_IO_OK);
    ASYNC_OUT0("");
    return;
}

/*
 * Read method
 */
void fileio::read(const char* aFileName, void* aDestBuffer, t_uint32 aNumBytes)
{
    ASYNC_IN0("");
    FILE* theFile = NULL;
    size_t readBytesCount = 0;

    // Sanity check
    if( aFileName == NULL || aDestBuffer == NULL || aNumBytes == 0)
    {
        // bad parameter
        MSG3("Bad parameter: aFileName=%p aDestBuffer=%p aNumBytes=%ld\n", aFileName, aDestBuffer, aNumBytes);
        response.fileIoResponse(FILE_IO_BAD_PARAMETER);
        ASYNC_OUT0("");
        return;
    }

    // Open the file
    MSG1("Opening file '%s' for reading\n", aFileName);
    theFile = fopen( aFileName, "rb");
    if(theFile==NULL)
    {
        // could not open the file
        MSG1("File '%s' open failed\n", aFileName);
        response.fileIoResponse(FILE_IO_OPEN_FAILED);
        ASYNC_OUT0("");
        return;
    }

    // Read the file
    MSG2("Reading %ld bytes from file '%s'\n", aNumBytes, aFileName);
    readBytesCount = fread( aDestBuffer, 1, (size_t)aNumBytes, theFile);
    if( readBytesCount != aNumBytes )
    {
        // could not read the file
        MSG2("Read failed, expected %ld bytes, actually got %d bytes\n", aNumBytes, readBytesCount);
        MSG1("Closing file '%s'\n", aFileName);
        fclose(theFile);
        response.fileIoResponse(FILE_IO_READ_FAILED);
        ASYNC_OUT0("");
        return;
    }

    // File sucessfully read
    MSG1("Closing file '%s'\n", aFileName);
    fclose(theFile);
    response.fileIoResponse(FILE_IO_OK);
    ASYNC_OUT0("");
    return;
}

/*
 * Write method
 */
void fileio::write(const char* aFileName, const void* aSrcBuffer, t_uint32 aNumBytes)
{
    ASYNC_IN0("");
    FILE* theFile = NULL;
    size_t writtenBytesCount = 0;

    // Sanity check
    if( aFileName == NULL || aSrcBuffer == NULL || aNumBytes == 0)
    {
        // bad parameter
        MSG3("Bad parameter: aFileName=%p aSrcBuffer=%p aNumBytes=%ld\n", aFileName, aSrcBuffer, aNumBytes);
        response.fileIoResponse(FILE_IO_BAD_PARAMETER);
        ASYNC_OUT0("");
        return;
    }

    // Open the file
    MSG1("Opening file '%s' for writing\n", aFileName);
    theFile = fopen( aFileName, "wb");
    if(theFile==NULL)
    {
        // could not open the file
        MSG1("File '%s' open failed\n", aFileName);
        response.fileIoResponse(FILE_IO_OPEN_FAILED);
        ASYNC_OUT0("");
        return;
    }

    // Actual write
    MSG2("Writing %ld bytes to file '%s'\n", aNumBytes, aFileName);
    writtenBytesCount = fwrite( aSrcBuffer, 1, (size_t)aNumBytes, theFile);
    if( writtenBytesCount != aNumBytes )
    {
        // could not write into file
        MSG0("Write failed\n");
        MSG1("Closing file '%s'\n", aFileName);
        fclose(theFile);
        response.fileIoResponse(FILE_IO_WRITE_FAILED);
        ASYNC_OUT0("");
        return;
    }

    // File sucessfully written
    MSG1("Closing file '%s'\n", aFileName);
    fclose(theFile);
    response.fileIoResponse(FILE_IO_OK);
    ASYNC_OUT0("");
    return;
}

/*
 * errorCode2String method
 */
const char* fileio::errorCode2String(t_fileio_error aErr)
{
    for(unsigned int i=0; i<KFioErrorCodesListSize; i++) {
        if(KFioErrorCodesList[i].id == aErr)
            return KFioErrorCodesList[i].name;
    }
    return "*** ERROR CODE NOT FOUND ***";
}
