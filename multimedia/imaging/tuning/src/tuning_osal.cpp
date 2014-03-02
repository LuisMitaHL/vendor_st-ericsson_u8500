/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "TUNING_OSAL"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include "tuning_osal.h"
#include "tuning_macros.h"
#include "tuning_types.h"
#include <stdio.h>

// Prevent export of symbols
#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTuningOsal);
#endif

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
CTuningOsal::CTuningOsal()
{
    iFile = NULL;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
CTuningOsal::~CTuningOsal()
{
    if(iFile)
    {
        fclose(iFile);
    }
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
bool CTuningOsal::OpenFile(const char* aName, e_mode aMode)
{
    const char* mode = aMode==eReadMode ? "rb" : "wb";
    TUNING_INFO_MSG1("Opening file %s\n",aName);
    iFile = fopen(aName,mode);
    if(iFile==NULL) {
       TUNING_ERROR_MSG1("Failed to open file %s\n", aName);
       return false;
    }
    return true;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
int CTuningOsal::GetSize()
{
    int size = 0;
    if (iFile != NULL)
    {
        long pos = ftell(iFile);
        if(pos < 0) {
            TUNING_ERROR_MSG1("Ftell failed: pos=%ld\n", pos);
            return -1;
        }
        if(fseek(iFile,0,SEEK_END) != 0) {
            TUNING_ERROR_MSG1("Seek to position pos=%d failed\n", 0);
            return -1;
        }
        size  = (int)ftell(iFile);
        if(fseek(iFile,pos,SEEK_SET)!=0) {
            TUNING_ERROR_MSG1("Seek to position pos=%ld failed\n", pos);
            return -1;
        }
    }
    return size;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
int CTuningOsal::ReadFile(char* aDest, int aSize)
{
    int nbChars = 0;
    nbChars = fread(aDest, 1, aSize, iFile);
    return nbChars;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
bool CTuningOsal::GetLine(char* line, int length)
{
    return fgets(line,length,iFile)== NULL ? false : true;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
bool CTuningOsal::WriteString(const char* str)
{
    if(iFile)
    {
        int res = fputs(str,iFile);
        return res<0 ? false : true;
    }
    return false;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
bool CTuningOsal::CloseFile()
{
    bool res = false;
    if (iFile)
    {
        fclose((FILE*)iFile);
        iFile = NULL;
        res = true;
    }
    return res;
}
