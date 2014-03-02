/*
 * Copyright (C) ST-Ericsson SA 2010-2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TUNINGLOADER_FILEIO_H_
#define _TUNINGLOADER_FILEIO_H_

#include <stdio.h>
#include <inc/typedef.h>

typedef enum
{
   FILEIO_OK,
   FILEIO_BAD_ARGUMENT,
   FILEIO_OPEN_FAILED,
   FILEIO_READ_FAILED,
   FILEIO_WRITE_FAILED,
   FILEIO_SEEK_FAILED,
   FILEIO_FILE_NOT_OPEN,
   FILEIO_FILE_NOT_CLOSED
} t_fileio_error;

typedef enum
{
   FILEIO_MODE_READ_ONLY,
   FILEIO_MODE_WRITE_ONLY
} t_fileio_mode;

class tuningloader_fileio
{
   public:
      tuningloader_fileio();
      ~tuningloader_fileio();
      t_fileio_error open(const char* pFileName, t_fileio_mode aMode);
      t_fileio_error read(void* pDestBuffer, int aNumBytes);
      t_fileio_error write(const void* pSrcBuffer, int aNumBytes);
      int            size();
      const char*    fullyQualifiedFileName();
      void           close();
      bool           isOpen();

   protected:
      t_fileio_error search(const char** pPaths, const int aNumPaths, const char* pFileName);
      char*          gets(char* pString, int aSize);

   private:
      FILE* pFile;
      char  iFullyQualifiedFileName[128];
      int   iFileSize;
};

class tuningloader_nvmfile: public tuningloader_fileio
{
   public:
      t_fileio_error open(const char* pNvmShortName);
      t_fileio_error open(const char* pNvmShortName,t_fileio_mode aMode);
};

class tuningloader_tuningfile: public tuningloader_fileio
{
   public:
      t_fileio_error open(const char* pFileShortName);
};

typedef struct {
   char      iCalStatus[10];
   t_uint32  Fuse_Id4;
} t_cal_file_info;

class tuningloader_calfile: public tuningloader_fileio
{
   public:
      t_fileio_error open(const char* pCalFileShortName);
      t_fileio_error process(t_cal_file_info* pCalFileInfo);
};

#endif /*_TUNINGLOADER_FILEIO_H_*/
