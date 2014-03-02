/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "c_type.h"

/**************************************
 * Returns true if the character is a
 * digit and false otherwise.
 **************************************/
int is_digit(char c)
{
    return c >= '0' && c <= '9';
}

/**************************************
 * A lower case letter is changed into
 * an upper case.
 **************************************/
int to_upper(char c)
{
    int diff = 'A' - 'a';

    if (c >= 'a' && c <= 'z') {
        c += diff;
    }

    return c;
}

