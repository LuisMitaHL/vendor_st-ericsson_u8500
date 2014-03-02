/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2012
 * License terms: 3-clause BSD license
 ******************************************************************************/
#include "String_s.h"

char *strcpy_s(char *dst, size_t _Size, const char *src)
{
    return strncpy(dst, src, _Size);
}

int sprintf_s(char *dst, size_t _Size, const char *format, ...)
{
    va_list l;
    int ReturnValue;

    va_start(l, format);
    ReturnValue = vsnprintf(dst, _Size, format, l);
    va_end(l);

    return ReturnValue;
}

char *strncpy_s(char *dst, const char *src, size_t _Size)
{
    return strncpy(dst, src, _Size);
}

#ifndef __MINGW32__
int _stricmp(const char *s1, const char *s2)
{
    return strcasecmp(s1, s2);
}
#endif
