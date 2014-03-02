/******************************************************************************
*
* Copyright (C) ST-Ericsson SA 2011
* License terms: 3-clause BSD license
*
******************************************************************************/

#ifndef _SECURITYALGORITHMS_H_
#define _SECURITYALGORITHMS_H_

class SecurityAlgorithms
{
public:
    static int SHA256(unsigned char *pData, unsigned long ulDataLen, unsigned char *pDigest);
private:
    SecurityAlgorithms();
    ~SecurityAlgorithms();
};

#endif // _SECURITYALGORITHMS_H_
