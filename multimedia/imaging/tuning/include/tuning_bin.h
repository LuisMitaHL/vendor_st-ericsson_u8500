/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TUNING_BIN_H_
#define _TUNING_BIN_H_

typedef enum
{
    TUNING_BIN_OK,
    TUNING_BIN_BAD_ARGUMENT,
    TUNING_BIN_ALREADY_CONSTRUCTED,
    TUNING_BIN_NOT_CONSTRUCTED,
    TUNING_BIN_MALLOC_FAILED,
    TUNING_BIN_MEMCPY_WOULD_OVERFLOW
} t_tuning_bin_data_error;

class CTuningBinData
{
    public:
        CTuningBinData();
        CTuningBinData(const char*);
        ~CTuningBinData();
        t_tuning_bin_data_error construct(int);
        t_tuning_bin_data_error construct(int,const void*);
        void destroy();
        void* getAddr();
        int getSize();
        void setStride(int);
        int getStride();
        const char* getName();
        t_tuning_bin_data_error fill(const void*);
        t_tuning_bin_data_error fill(const void* aSrc, const int, const int);
        static const char* errorCode2String(t_tuning_bin_data_error);

    private:
        // Members
        const char* pInstanceName;
        unsigned char* pData;
        int iSize;
        int iStride;
        // Functions
        void init();
};

#endif // _TUNING_BIN_H_
