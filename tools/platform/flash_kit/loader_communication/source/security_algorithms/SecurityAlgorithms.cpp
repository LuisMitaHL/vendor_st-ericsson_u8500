/******************************************************************************
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
******************************************************************************/
#include "SecurityAlgorithms.h"
#include "sha2.h"

int SecurityAlgorithms::SHA256(unsigned char *pData, unsigned long ulDataLen, unsigned char *pDigest)
{
    SHA256_CTX  c256;
    SHA256_Init(&c256);
    SHA256_Update(&c256, pData, ulDataLen);
    SHA256_Final(pDigest, &c256);

    return 0;
}
