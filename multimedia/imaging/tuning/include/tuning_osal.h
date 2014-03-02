/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file
* \brief   header file for tuning_osal.cpp
* \author  ST-Ericsson
*/

#ifndef _TUNING_OSAL_H_
#define _TUNING_OSAL_H_

#include <stdio.h>

typedef enum
{
   eReadMode,
   eWriteMode
} e_mode;

class CTuningOsal
{
    public:
        CTuningOsal();
        ~CTuningOsal();
        /* file access functions*/
        bool OpenFile(const char* aName, e_mode);
        bool CloseFile();
        int  GetSize();
        int  ReadFile(char* aDest, int aSize);
        bool GetLine(char* line, int lenght);
        bool WriteString(const char* string);

    private:
        FILE* iFile;
};

#endif /*_TUNING_OSAL_H_*/
