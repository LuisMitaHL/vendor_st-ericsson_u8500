/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMSWROUTINES_INL
#define STECAMSWROUTINES_INL

/*static*/ inline void CamSwRoutines::deinit()
{
    //unload the SWRoutines library
    if (mLibHandle != NULL)
        dlclose(mLibHandle);

    mLibHandle = NULL;

    //Set all Function pointers as NULL
    for(AIQ_U32 loop = 0; loop < EMaxType; loop++)
        mSwRoutines[static_cast<Type>(loop)] = NULL;
}

/*static*/ inline void CamSwRoutines::process(Type aType,
                                                 AIQ_U8 *aInBuffer, AIQ_U8 *aOutBuffer,
                                                 AIQ_U32 aWidth, AIQ_U32 aHeight)
{
    DBGT_ASSERT(EMaxType > aType,
               "Index: %d out of range, max index: %d", aType, EMaxType);
    DBGT_ASSERT(NULL != aInBuffer, "In buffer NULL");
    DBGT_ASSERT(0 != aWidth, "Width can not be zero");
    DBGT_ASSERT(0 != aHeight, "Height can not be zero");

    mSwRoutines[aType](aInBuffer, aOutBuffer, aWidth, aHeight);
}

#endif // STECAMSWROUTINES_INL
