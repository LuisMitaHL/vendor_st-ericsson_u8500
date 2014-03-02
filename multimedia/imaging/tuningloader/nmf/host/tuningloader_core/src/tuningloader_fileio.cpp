/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "TUNING_LOADER_FILEIO"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include "tuningloader_fileio.h"
#include <stdio.h>
#include "osi_trace.h"
#include "ImgConfig.h"

// Caution: the order _does_ matter, do not modify
static const char* gCalibrationPaths[] =
{
#if defined(FETCH_FILES_FROM_SDCARD)
   "/sdcard/camera/tuning",
#endif

#if defined (__ARM_ANDROID)

#if (IMG_CONFIG_NVM_PATH == 1)
 "/data/tuning" 
 #else 
 "/system/usr/share/camera/nvm_cal"
 #endif /* IMG_CONFIG_NVM_PATH */
 
#else
   "/usr/share/camera/tuning"
#endif	/*__ARM_ANDROID*/
};

static const unsigned int gNumCalibrationPaths = sizeof(gCalibrationPaths)/sizeof(gCalibrationPaths[0]);

// Caution: the order _does_ matter, do not modify
static const char* gTuningPaths[] =
{
#if defined(FETCH_FILES_FROM_SDCARD)
   "/sdcard/camera/tuning",
#endif

#if defined (__ARM_ANDROID)
   "/system/usr/share/camera/tuning"
#else
   "/usr/share/camera/tuning"
#endif
};

static const unsigned int gNumTuningPaths = sizeof(gTuningPaths)/sizeof(gTuningPaths[0]);

/*
 * tuningloader_fileio constructor
 */
tuningloader_fileio::tuningloader_fileio()
{
    pFile = NULL;
    iFullyQualifiedFileName[0] = '\0';
    iFileSize = 0;
}

/*
 * tuningloader_fileio destructor
 */
tuningloader_fileio::~tuningloader_fileio()
{
    if(pFile != NULL) {
        fclose(pFile);
    }
}

/*
 * Search and open a file for reading
 */
t_fileio_error tuningloader_fileio::search(const char** pPaths,
                                           const int    aNumPaths,
                                           const char*  pFileName)
{
    IN0("\n");

    // Close file in case previous one is still open
    if(pFile!=NULL) {
        MSG0("Attenmting to open a new file without having closed previous one\n");
        OUTR(" ",FILEIO_FILE_NOT_CLOSED);
        return FILEIO_FILE_NOT_CLOSED;
    }

    // Loop on all root paths
    for(int i=0; i<aNumPaths; i++)
    {
        // Contruct file name
        snprintf(iFullyQualifiedFileName, sizeof(iFullyQualifiedFileName), "%s/%s", pPaths[i], pFileName);

        // Open file
        MSG1("Opening file (read only) '%s'\n", iFullyQualifiedFileName);
        pFile = fopen( iFullyQualifiedFileName, "rb");
        if(pFile!=NULL)
        {
           // File sucessfully open, retrieve file size
           if( fseek( pFile, 0, SEEK_END) != 0)
           {
               // Seek failed
               MSG0("Failed to seek to end of file, aborting\n");
               close();
               OUTR(" ", FILEIO_SEEK_FAILED);
               return FILEIO_SEEK_FAILED;
           }
           iFileSize = (int)ftell(pFile);
           MSG1("File found, size=%d\n", iFileSize);
           (void)fseek(pFile, 0, SEEK_SET);
           OUTR(" ", FILEIO_OK);
           return FILEIO_OK;
        }
        MSG0("File not found, try next path\n");
    }
    MSG0("All paths have been visited, file not found\n");
    OUTR(" ", FILEIO_OPEN_FAILED);
    return FILEIO_OPEN_FAILED;
}

/*
 * Open a file
 */
t_fileio_error tuningloader_fileio::open(const char*   pFileName,
                                         t_fileio_mode aFileIoMode)
{
    IN0("\n");

    const char* pOpenMode = aFileIoMode==FILEIO_MODE_READ_ONLY ? "rb" : "wb";

    // Close file in case previous one is still open
    if(pFile!=NULL) {
        MSG0("Attempting to open a new file without having closed previous one\n");
        OUTR(" ",FILEIO_FILE_NOT_CLOSED);
        return FILEIO_FILE_NOT_CLOSED;
    }

    // Open file
    MSG2("Opening file (%s only) '%s'\n", aFileIoMode==FILEIO_MODE_READ_ONLY?"read":"write", pFileName);
    pFile = fopen( pFileName, pOpenMode);
    if(pFile==NULL)
    {
       MSG0("Failed to open file\n");
       OUTR(" ",FILEIO_OPEN_FAILED);
       return FILEIO_OPEN_FAILED;
    }

    // File sucessfully open
    snprintf(iFullyQualifiedFileName, sizeof(iFullyQualifiedFileName), "%s", pFileName);
    if(aFileIoMode==FILEIO_MODE_READ_ONLY)
    {
       // File open in "read only" mode: retrieve file size
       if( fseek( pFile, 0, SEEK_END) != 0)
       {
          // Seek failed
          MSG0("Failed to seek to end of file, aborting\n");
          close();
          OUTR(" ", FILEIO_SEEK_FAILED);
          return FILEIO_SEEK_FAILED;
      }
      iFileSize = (int)ftell(pFile);
      (void)fseek(pFile, 0, SEEK_SET);
   }
   MSG1("File sucessfully open, size=%d\n", iFileSize);
   OUTR(" ", FILEIO_OK);
   return FILEIO_OK;
}

/*
 * tuningloader_fileio close method
 */
void tuningloader_fileio::close()
{
    IN0("\n");
    if(pFile != NULL) {
       MSG1("Closing file %s\n", iFullyQualifiedFileName);
       fclose(pFile);
       pFile = NULL;
       iFullyQualifiedFileName[0] = '\0';
       iFileSize = 0;
    }
    OUT0("\n");
}

/*
 * tuningloader_fileio isOpen method
 */
bool tuningloader_fileio::isOpen()
{
    return pFile!=NULL?true:false;
}

/*
 * tuningloader_fileio size method
 */
int tuningloader_fileio::size()
{
    return iFileSize;
}

/*
 * tuningloader_fileio fullyQualifiedFileName method
 */
const char* tuningloader_fileio::fullyQualifiedFileName()
{
    return iFullyQualifiedFileName;
}

/*
 * tuningloader_fileio read method
 */
t_fileio_error tuningloader_fileio::read(void* pDestBuffer, int aNumBytes)
{
    IN0("\n");
    size_t iNumBytesRead = 0;

    // Sanity check
    if( pDestBuffer == NULL || aNumBytes == 0)
    {
        // Bad parameter
        MSG2("Bad parameter: pDestBuffer=%p aNumBytes=%d\n", pDestBuffer, aNumBytes);
        OUTR(" ", FILEIO_BAD_ARGUMENT);
        return FILEIO_BAD_ARGUMENT;
    }
    if( pFile == NULL)
    {
        // File is not open
        MSG0("File not open\n");
        OUTR(" ", FILEIO_FILE_NOT_OPEN);
        return FILEIO_FILE_NOT_OPEN;
    }

    // Read the file
    MSG1("Reading %d bytes from file\n", aNumBytes);
    iNumBytesRead = fread( pDestBuffer, 1, (size_t)aNumBytes, pFile);
    if( iNumBytesRead != (size_t)aNumBytes )
    {
        // Could not read the file
        MSG2("Read failed, expected %d bytes, got %d bytes\n", aNumBytes, iNumBytesRead);
        OUTR(" ", FILEIO_READ_FAILED);
        return FILEIO_READ_FAILED;
    }

    // File sucessfully read
    OUTR(" ",FILEIO_OK);
    return FILEIO_OK;
}

/*
 * tuningloader_fileio write method
 */
t_fileio_error tuningloader_fileio::write(const void* pSrcBuffer, int aNumBytes)
{
    IN0("\n");
    size_t iNumBytesWritten = 0;

    // Sanity check
    if( pSrcBuffer == NULL || aNumBytes == 0)
    {
        // Bad parameter
        MSG2("Bad parameter: pSrcBuffer=%p aNumBytes=%d\n", pSrcBuffer, aNumBytes);
        OUTR(" ", FILEIO_BAD_ARGUMENT);
        return FILEIO_BAD_ARGUMENT;
    }
    if( pFile == NULL)
    {
        // File is not open
        MSG0("File not open\n");
        OUTR(" ", FILEIO_FILE_NOT_OPEN);
        return FILEIO_FILE_NOT_OPEN;
    }

    // Write to file
    iNumBytesWritten = fwrite( pSrcBuffer, 1, (size_t)aNumBytes, pFile);
    if( iNumBytesWritten != (size_t)aNumBytes )
    {
        // Could not write into file
        MSG0("Write failed, closing file\n");
        OUTR(" ", FILEIO_WRITE_FAILED);
        return FILEIO_WRITE_FAILED;
    }

    // Sucessfully wrote to file
    OUTR(" ",FILEIO_OK);
    return FILEIO_OK;
}

/*
 * Function that reads a string from file
 */
char* tuningloader_fileio::gets(char* pString, int aSize)
{
    return fgets(pString, aSize, pFile);
}

/*
 * NVM file class
 */
t_fileio_error tuningloader_nvmfile::open(const char* pNvmFileName)
{
    return tuningloader_fileio::search( gCalibrationPaths, gNumCalibrationPaths, pNvmFileName);
}

t_fileio_error tuningloader_nvmfile::open(const char* pNvmShortName,t_fileio_mode aMode)
{
	char  FullyFileName[128];
	//Assuming file to write at 0th location
	snprintf(FullyFileName, sizeof(FullyFileName), "%s/%s", gCalibrationPaths[0],pNvmShortName);
	return tuningloader_fileio::open( FullyFileName, aMode);
}

/*
 * Tuning file class
 */
t_fileio_error tuningloader_tuningfile::open(const char* pFileName)
{
    return tuningloader_fileio::search( gTuningPaths, gNumTuningPaths, pFileName);
}

/*
 * Calibration file class
 */
t_fileio_error tuningloader_calfile::open(const char* pCalFileName)
{
    return tuningloader_fileio::search( gCalibrationPaths, gNumCalibrationPaths, pCalFileName);
}

t_fileio_error tuningloader_calfile::process(t_cal_file_info* pCalFileInfo)
{
    bool  bCalStatusFound = false;
    bool  bFuseIdFound = false;
    char  cFileLine[128] = "";
    char* pString = NULL;

    // Sanity check
    if( pCalFileInfo == NULL )
    {
        // Bad parameter
        MSG3("Bad parameter: pCalFileInfo=%p \n", pCalFileInfo);
        OUTR(" ", FILEIO_BAD_ARGUMENT);
        return FILEIO_BAD_ARGUMENT;
    }

    pCalFileInfo->iCalStatus[0] = '\0';
    pCalFileInfo->Fuse_Id4 =0;
    while(gets(cFileLine,sizeof(cFileLine))!= NULL)
    {
        if(bCalStatusFound == false)
        {
            if(strstr(cFileLine,"Success") != NULL)
            {
                bCalStatusFound = true;
                snprintf(pCalFileInfo->iCalStatus, sizeof(pCalFileInfo->iCalStatus)-1, "Success");
            }
            else if(strstr(cFileLine,"Fail") != NULL)
            {
                bCalStatusFound = true;
                snprintf(pCalFileInfo->iCalStatus, sizeof(pCalFileInfo->iCalStatus)-1, "Fail");
            }
        }
        if(bFuseIdFound == false)
        {
            if((pString = strstr(cFileLine,"Fuse_ID[4]")) != NULL)
            {
                bFuseIdFound = true;
                pString +=(sizeof("Fuse_ID[4]=0x") - 0x01);
                sscanf(pString,"%lx",&(pCalFileInfo->Fuse_Id4));
            }
        }
        if(bFuseIdFound == true && bCalStatusFound == true)
        {
            // Got both Status and fuse id, so no need for further read
            MSG0("Found Both status and FuseId4 in Calibration file\n");
            break;
        }
    }
    OUTR(" ", FILEIO_OK);
    return FILEIO_OK;
}
