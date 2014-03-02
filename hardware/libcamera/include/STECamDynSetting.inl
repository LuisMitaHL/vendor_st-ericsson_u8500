/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef STECAMDYNSETTING_INL
#define STECAMDYNSETTING_INL

/* static */ inline int DynSetting::get(Type type)
{
    char value[PROPERTY_VALUE_MAX];

    //get value
    get(type, value);

    //if valid value, update
    if(strlen(value))
        strncpy(mValue[type], value, PROPERTY_VALUE_MAX);

    return atoi(mValue[type]);
}

/* static */ inline void DynSetting::get(Type type, char *value)
{
    //ignore return value
    property_get(DynSetting::mKey[type], value, mValue[type]);
}

/* static */ inline void DynSetting::set(Type type, int value)
{
    snprintf(mValue[type], PROPERTY_VALUE_MAX, "%d", value);
}

/* static */ inline void DynSetting::set(Type type, const char *value)
{
    strncpy(mValue[type], value, PROPERTY_VALUE_MAX);
}

#endif // STECAMDYNSETTING_INL
