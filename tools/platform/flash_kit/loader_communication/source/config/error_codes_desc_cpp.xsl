<?xml version="1.0" encoding="utf-8"?>
<!--
* Copyright (C) ST-Ericsson SA 2012
* License terms: 3-clause BSD license
-->
<stylesheet version="1.0" xmlns="http://www.w3.org/1999/XSL/Transform">

<output method="text"/>
<strip-space elements="*"/>
<param name="target"/>
<param name="errorCodesLcmXml" />
<variable name="lcm_error_codes" select="document($errorCodesLcmXml)/commandspec"/>

<template match="/commandspec">/* $Copyright ST-Ericsson 2012$ */
/* NOTE: This is an automatically generated file. DO NOT EDIT! */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include &lt;string&gt;
#include &lt;map&gt;

#include "LCDriver.h"
#include "error_codes_desc.h"
#if defined(__MINGW32__) || defined(__linux__) || defined(__APPLE__)
#include "String_s.h"
#endif // __MINGW32__ || __linux__ || __APPLE__)

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
std::map &lt;int, desc&gt; err_desc;
static uint32 func_call_once = 0;

/****************************************************************************/
/*       NAME : Init_map_err_desc                                           */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: Declares the map err_desc with short and long description   */
/*              of errors.                                                  */
/*                                                                          */
/* PARAMETERS :                                                             */
/*                                                                          */
/*    RETURN : void                                                         */
/****************************************************************************/
void Init_map_err_desc()
{
    desc desc_;

/**
 *  Description of error codes for LCDriver.
 */
<apply-templates select="status"/>
/**
 *  Description of error codes for Loader.
 */
<apply-templates select="$lcm_error_codes/status"/>
}

/****************************************************************************/
/*       NAME : GetLoaderErrorDescription_Call                              */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: For a given error code number it returns its short and      */
/*              long description.                                           */
/*                                                                          */
/* PARAMETERS :                                                             */
/*        IN : uint32 ErrorNr:Error code number.                            */
/*       OUT : uint8 ShortDescription:Buffer for short description.         */
/*       OUT : uint8 LongDescription:Buffer for long description.           */
/*        IN : uint32 ShorDescrBufLen:Max length of ShortDescription.       */
/*        IN : uint32 LongDescrBufLen:Max length of LongDescription.        */
/*                                                                          */
/*    RETURN : void                                                         */
/****************************************************************************/
void GetLoaderErrorDescription_Call(uint32 ErrorNr, uint8 *ShortDescription, uint8 *LongDescription, uint32 ShorDescrBufLen, uint32 LongDescrBufLen)
{
    std::map &lt;int, desc&gt;::iterator it;
    uint32 str_len = 0;

    // Define the map err_desc on the first call of this function
    if (!func_call_once){
        Init_map_err_desc();
        func_call_once = 1;
    }

    it=err_desc.find(ErrorNr);
    if(err_desc.end() == it){
        std::string temp = "There is no LCM/LCD error with this number";

        if((temp.length()+1) &gt; ShorDescrBufLen){
             str_len = ShorDescrBufLen-1;
        }
        else{
             str_len = temp.length();
        }
        strcpy_s((char *)ShortDescription, str_len, temp.c_str());
        ShortDescription[str_len] = '\0';

       if((temp.length()+1) &gt; LongDescrBufLen){
              str_len = LongDescrBufLen-1;
        }
        else{
              str_len = temp.length();
        }
        strcpy_s((char *)LongDescription, str_len, temp.c_str());
        LongDescription[str_len] = '\0';
    }
    else{

        if(((*it).second.short_desc.length()+1) &gt; ShorDescrBufLen){
            str_len = ShorDescrBufLen-1;
        }
        else{
            str_len = (*it).second.short_desc.length();
        }

        strcpy_s((char *)ShortDescription, str_len, (*it).second.short_desc.c_str());
        ShortDescription[str_len] = '\0';

        if(((*it).second.long_desc.length()+1) &gt; LongDescrBufLen){
            str_len = LongDescrBufLen-1;
        }
        else{
            str_len = (*it).second.long_desc.length();
        }

        strcpy_s((char *)LongDescription, str_len, (*it).second.long_desc.c_str());
        LongDescription[str_len] = '\0';
    }
}
</template>

<template match="status">
<apply-templates select="value"/>
</template>

<template match="value">
  <text>    </text><if test="position() != last()"></if>desc_.short_desc = "<value-of select="@short"/>";
    desc_.long_desc = "<value-of select="./text()"/>";
    err_desc[<value-of select="@number"/>] = desc_;
</template>

</stylesheet>
