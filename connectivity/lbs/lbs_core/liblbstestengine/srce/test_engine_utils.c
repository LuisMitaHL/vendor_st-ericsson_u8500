/*
 * Positioning Manager
 *
 * test_engine_utils.c.bak
 *
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#include <agpsosa.h>

#include <string.h>        // strcpy
#include <ctype.h>        // tolower()
#include <sys/types.h>

//-----------------------------------------------------------------------------
// StrToUint32
//
// Description: Converts a string to an uint32 value
//              The behaviour is undefined when the string
//              does not convert to 32 bits.
//
//              This function supports '', b, 0x and decimal
//
// Last changed: qandtin 020301 Original version
//

unsigned long TSTENG_StrToUint32(const char *const String_p)
{
    // real implementation
    unsigned long retval = 0;
    const char *str_p = String_p;

    if ((str_p != NULL) && (*str_p != 0)) {
    // Check the format of the string
    if ((*str_p == '0') && (tolower(*(str_p + 1)) == 'x')) {
        // Hexadecimal string
        str_p += 2;        // Skip '0x'
        // Convert the string
        while (*str_p != 0) {
        char ch = *str_p;

        // Make room for new nibble
        retval <<= 4;    // There is no sizeof( uint4 )

        // Check for non-decimal (0-9)
        if ((ch < '0') || (ch > '9')) {
            if (toupper(ch) >= 'A' && toupper(ch) <= 'F') {
            // isHexString and non-numerical character [A-F]
            // Add the new nibble
            retval |= ((toupper(ch) - 'A' + 10) & 0x0F);    // Only 4 bits

            }
        } else {
            // Not [A-F, a-f] must be regular number [0-9]
            // Add the new nibble
            retval |= ((ch - '0') & 0x0F);    // Only 4 bits
        }
        str_p++;
        }
    } else if (tolower(*str_p) == 'b') {
        char numBits = 0;    // For detecting bit separators '.'
        // Last character (not including termination)
        const char *endStr_p = String_p + strlen(String_p) - 1;
        // Binary string
        str_p++;        // Skip 'b'
        // Convert the string
        while (*str_p != 0) {
        // Verify the character value
        if ((*str_p != '0') && (*str_p != '1') && (*str_p != '.')) {
            return 0;    // ## RETURN ##
        }

        if (*endStr_p == '.')    // Binary separator
        {
            // Only increment if <4 bits has been written
            if (numBits % 4 != 0) {
            numBits += 4 - (numBits % 4);
            }
        } else {
            // Add new bit
            retval |= ((((*endStr_p) - '0') & 0x0001) << numBits);
            numBits++;
        }
        str_p++;
        endStr_p--;
        }
    } else if (*str_p == '\'') {
        // Character string
        str_p++;        // Skip '

        // Add char value
        retval = *str_p;    // Input string is a character
        // Can only be 1 character
    } else {
        // Decimal string
        while (*str_p != 0) {
        // Verify the character value
        // Check for non-decimal (0-9)
        if ((*str_p < '0') || (*str_p > '9')) {
            // Invalid character verified - return 0
            return 0;    // ## RETURN ##
        }
        // Build retval
        // Add decimal value (might overflow!)
        retval *= 10;    // Decimal values are 10-based
        retval += ((*str_p) - '0');    // Input string is in characters
        str_p++;
        }
    }
    }
    return retval;
}

//-----------------------------------------------------------------------------
// StrToSint32
//
// Description: Converts a string to an uint32_t value
//              The behaviour is undefined when the string
//              does not convert to 32 bits.
//
//              This function supports decimal only
//
// THIS FUNCTION IS NOT EXPORTED IN u_idbg.h
//
// Last changed: qandtin 020301 Original version
//

signed long TSTENG_StrToSint32(const char *const String_p)
{
    // real implementation
    signed long retval = 0;
    const char *str_p = String_p;

    if ((str_p != NULL) && (*str_p != 0)) {
    // Last character (not including termination)
//Removed because not used    const char *endStr_p = String_p + strlen( String_p ) - 1;
    bool isNeg = FALSE;

    // Is number negative ?
    if (*str_p == '-') {
        str_p++;
        isNeg = TRUE;
    } else if (*str_p == '+') {
        str_p++;
    }
    // Try to convert the string
    while (*str_p != 0) {
        // Verify the character value
        // Check for non-decimal (0-9)
        if (((*str_p < '0') || (*str_p > '9'))) {
        // Found an invalid character
        return 0;    // ## RETURN ##
        }
        // End of character validation

        // Add decimal value (might overflow!)
        retval *= 10;    // Decimal values are 10-based
        retval += ((*str_p) - '0');    // Input string is in characters

        str_p++;
//Removed because not used      endStr_p--;
    }
    if (isNeg) {
        retval = -retval;
    }
    }
    return retval;
}


//-----------------------------------------------------------------------------
// StrNZCpy
//
// Description: Copies Source_p to Dest_p with a limitation acc. to DestLimit
//
// Last changed: qandtin 020301 Original version
//
int TSTENG_StrNZCpy(char *const Dest_p, const char *const Source_p, const int DestLimit)
{
    // Verify input
    if ((Dest_p == NULL) || (Source_p == NULL) || (DestLimit == 0)) {
    if ((Dest_p != NULL) && (DestLimit > 0)) {
        // Source_p was NULL, DestLimit was not 0
        // then 0 terminate the Dest_p string
        *Dest_p = 0;
    }
    return FALSE;
    }
    // Copy Source_p to Dest_p
    {
    int DestLen = strlen(Source_p) + 1;

    if (DestLen > DestLimit) {
        DestLen = DestLimit;    // String did not fit
        return FALSE;    // ## RETURN ##
    }
    // Check for multiple bytes
    if (DestLen != 1) {
        memcpy(Dest_p, Source_p, DestLen - 1);    // Copy data
    }
    Dest_p[DestLen - 1] = 0;    // Always zero terminate
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
// CaseInsensitiveStrNCompare
//
// Description: Compares 2 terminated strings, returns TRUE if identical
//              The comparison is case-insensitive [A-Z], [a-z]
//
// Last changed: qandtin 020301 Original version
//
int TSTENG_CaseInsensitiveStrNCompare(const char *const String1_p, const char *const String2_p, const int MaxCharsToCompare)
{
    // real implementation
    const char *str1_p = String1_p;
    const char *str2_p = String2_p;


    if (MaxCharsToCompare == 0) {
    // No characters to compare
    return TRUE;        // ## RETURN ##
    }

    if ((str1_p == NULL) || (str2_p == NULL)) {
    if ((str1_p == NULL) && (str2_p == NULL)) {
        // Both strings are NULL (thus equal)
        return TRUE;    // ## RETURN ##
    }
    return FALSE;        // ## RETURN ##
    }

    {
    // Determine how many bytes to compare
    int nBytes = MaxCharsToCompare;
    int strLen1 = strlen(String1_p);
    int strLen2 = strlen(String2_p);

    // If nBytes is larger than strLen1 or strLen2
    // then 1 of 2 events can occur
    // (1)  nBytes = strLen1 or strLen2 (strLen1 == strLen2)
    // (2)  strings are not equaly long and thus not equal
    if ((nBytes > strLen1) || (nBytes > strLen2)) {
        // Strings equaly long ?
        if (strLen1 != strLen2) {
        // ...then they cannot be equal
        return FALSE;    // ## RETURN ##
        }
        nBytes = strLen1;    // Number of characters to compare
    }
    // Compare every character in the string
    // If the character is a letter, compare in UPPER-case
    while (nBytes--) {
        char val1 = *str1_p;
        char val2 = *str2_p;

        // Check for small character range
        // Do not convert to lower-case if non-letter
        val1 = toupper(val1);
        val2 = toupper(val2);

        if (val1 != val2) {
        // String mismatch
        return FALSE;    // ## RETURN ##
        }

        str1_p++;
        str2_p++;
    }
    }

    return TRUE;
}


//-----------------------------------------------------------------------------
// CaseInsensitiveStrCompare
//
// Description: Compares 2 terminated strings, returns TRUE if identical
//              The comparison is case-insensitive [A-Z], [a-z]
//
// Last changed: qandtin 020301 Original version
//
int TSTENG_CaseInsensitiveStrCompare(const char *const String1_p, const char *const String2_p)
{
    // real implementation
    bool retVal = TRUE;

    if ((String1_p == NULL) || (String2_p == NULL)) {
    if ((String1_p == NULL) && (String2_p == NULL)) {
        // Both strings are NULL thus equal
        return TRUE;    // ## RETURN ##
    }
    retVal = FALSE;
    }

    if (retVal) {
    int strLen1 = strlen(String1_p);    //lint !e668
    int strLen2 = strlen(String2_p);    //lint !e668

    // Strings equaly long ?
    if (strLen1 != strLen2) {
        retVal = FALSE;
    } else {
        // Compare the strings
        retVal = TSTENG_CaseInsensitiveStrNCompare(String1_p, String2_p, strLen2);
    }
    }

    return retVal;
}

