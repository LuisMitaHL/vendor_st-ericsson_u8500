/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _ERROR_H_
#define _ERROR_H_

#define VERIFY(Condition, ErrorCode)\
    do\
    {\
        if(!(Condition))\
        {\
            ReturnValue = (ErrorCode);\
            goto ErrorExit;\
        }\
    } while(0)

#define VERIFY_SUCCESS(ErrorCode)\
    do\
    {\
        ReturnValue = (ErrorCode);\
        VERIFY(E_SUCCESS == ReturnValue, ReturnValue);\
    } while (0)

#endif // _ERROR_H_
