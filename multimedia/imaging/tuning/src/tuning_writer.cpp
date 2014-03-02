/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "TUNING_WRITER"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include "tuning_types.h"
#include "tuning_writer.h"
#include "tuning_osal.h"
#include "tuning_macros.h"
#include <stdio.h>
#include <string.h>

/*
 * Local definitions
 */

/*
 *  \version 1: initial released version
 *  \version 2: add of 4 digit tuning file versioning
 *  \version 3: add of <SIZE> tag for vectors/matrices
 *  \version 4: add of <SW3A> library versioning
 */
#define WRITER_STRUCT_VERSION 4

// Misc defines
#define INDENT_ELEMENT "   "
#define INDENT_ELEMENT_SIZE (sizeof(INDENT_ELEMENT)-1)

/*
 * Local variables
*/
#define HASH_ENTRY(id) {id,#id}

typedef struct
{
    t_tuning_writer_error_code id;
    const char* name;
} t_writer_error_entry;

static const t_writer_error_entry KWriterErrorCodesList[] =
{
    HASH_ENTRY(TUNING_WRITER_OK),
    HASH_ENTRY(TUNING_WRITER_INVALID_ARGUMENT),
    HASH_ENTRY(TUNING_WRITER_FILE_OPEN_FAILED),
    HASH_ENTRY(TUNING_WRITER_STRING_CONSTRUCT_FAILED),
    HASH_ENTRY(TUNING_WRITER_FILE_WRITE_FAILED),
    HASH_ENTRY(TUNING_WRITER_UNKNOWN_IQSET_ID),
    HASH_ENTRY(TUNING_WRITER_UNKNOWN_IQ_PARAM_TYPE),
    HASH_ENTRY(TUNING_WRITER_BAD_PARAM_SIZE),
    HASH_ENTRY(TUNING_WRITER_BAD_VALUE_LIST),
    HASH_ENTRY(TUNING_WRITER_BUFFER_TOO_SMALL),
    HASH_ENTRY(TUNING_WRITER_INTERNAL_ERROR),
};

#define KWriterErrorCodesListSize (sizeof(KWriterErrorCodesList)/sizeof(KWriterErrorCodesList[0]))

// Prevent export of symbols
#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTuningWriter);
#endif

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
CTuningWriter::CTuningWriter()
{
    TUNING_FUNC_IN0();
    iWriterVersion.structure = WRITER_STRUCT_VERSION;
    iWriterVersion.entries = IQSET_ENTRIES_VERSION;
    iCurLineStr[0] = '\0';
    iCurLineNum = 0;
    iIndentCount = 0;
    iIndentStr[0] = '\0';
    iValStr[0] = '\0';
    iToBuffer = true;
    iDestBuffer = NULL;
    iDestBufferSize = 0;
    iDestBufferFilledSize = 0;
    TUNING_INFO_MSG2("Writer Version: Struct=%d Entries=%d\n",iWriterVersion.structure,iWriterVersion.entries);
    TUNING_FUNC_OUT0();
    return;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
CTuningWriter::~CTuningWriter()
{
    TUNING_FUNC_IN0();
    TUNING_FUNC_OUT0();
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
void CTuningWriter::IncLineNum()
{
    iCurLineNum++;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
void CTuningWriter::ResetLineNum()
{
    iCurLineNum = 0;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
void CTuningWriter::ResetIndent()
{
    iIndentCount = 0;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
void CTuningWriter::Indent()
{
    iIndentCount++;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
void CTuningWriter::UnIndent()
{
    if(iIndentCount>0)
    {
        iIndentCount--;
    }
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeString(const t_sint16 aVal)
{
    int res = snprintf(iValStr, sizeof(iValStr), "%hd", aVal);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeString(const t_uint16 aVal)
{
    int res = snprintf(iValStr, sizeof(iValStr), "%hu", aVal);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeString(const int aVal)
{
    int res = snprintf(iValStr, sizeof(iValStr), "%d", aVal);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeRowColString(const int aRows, const int aCols)
{
    int res = snprintf(iValStr, sizeof(iValStr), "%dx%d", aRows, aCols);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeString(const t_sint32 aVal)
{
    int res = snprintf(iValStr, sizeof(iValStr), "%d", (int)aVal);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeString(const t_uint32 aVal)
{
    int res = snprintf(iValStr, sizeof(iValStr), "%u", (unsigned int)aVal);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeString(const float aVal)
{
    int res = snprintf(iValStr, sizeof(iValStr), "%#f", aVal);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeHexString(const t_sint16 aVal)
{
    int res = snprintf(iValStr, sizeof(iValStr), "0x%X", aVal);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeHexString(const t_uint16 aVal)
{
    int res = snprintf(iValStr, sizeof(iValStr), "0x%X", aVal);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeHexString(const t_sint32 aVal)
{
    int res = snprintf(iValStr, sizeof(iValStr), "0x%X", (unsigned int)aVal);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeHexString(const t_uint32 aVal)
{
    int res = snprintf(iValStr, sizeof(iValStr), "0x%X", (unsigned int)aVal);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeHex16String(const t_uint32 aVal)
{
    int res = snprintf(iValStr, sizeof(iValStr), "0x%.4X", (unsigned int)aVal);
    if( res < 0 || res >= (int)sizeof(iValStr))
      return NULL;
    else
      return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeString(const t_sint32* aVal, const int aCount)
{
    // Convert all values into string
    iValStr[0] = '\0';
    for(int i=0; i<aCount; i++)
    {
        int remaining = sizeof(iValStr)- strnlen(iValStr,sizeof(iValStr));
        int filled = sizeof(iValStr) - remaining;
        int res = snprintf(&iValStr[filled], remaining-1, "%d ", (int)aVal[i]);
        if( res < 0 || res >= remaining)
        {
            // Could not write line
            TUNING_INFO_MSG1("snprintf failed: res=%d\n", res);
            return NULL;
        }
    }
    return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeString(const float* aVal, const int aCount)
{
    // Convert all values into string
    iValStr[0] = '\0';
    for(int i=0; i<aCount; i++)
    {
        int remaining = sizeof(iValStr)- strnlen(iValStr,sizeof(iValStr));
        int filled = sizeof(iValStr) - remaining;
        int res = snprintf(&iValStr[filled], remaining-1, "%#f ", aVal[i]);
        if( res < 0 || res >= remaining)
        {
            // Could not write line
            TUNING_INFO_MSG1("snprintf failed: res=%d\n", res);
            return NULL;
        }
    }
    return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeString(const u_iqset_value* aVal, const e_iqset_value_type aType, const int aCount)
{
    // Convert all values into string
    iValStr[0] = '\0';
    for(int i=0; i<aCount; i++)
    {
        int remaining = sizeof(iValStr)- strnlen(iValStr,sizeof(iValStr));
        int filled = sizeof(iValStr) - remaining;
        int res = 0;
        if( aType == IQSET_VT_INT) {
            res = snprintf(&iValStr[filled], remaining-1, "%d ", (int)aVal[i].i32);
        }
        else {
            res = snprintf(&iValStr[filled], remaining-1, "%#.5f ", aVal[i].f32);
        }
        if( res < 0 || res >= remaining)
        {
            // Could not write line
            TUNING_INFO_MSG1("snprintf failed: res=%d\n", res);
            return NULL;
        }
    }
    return (const char*)iValStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::DoWriteLine()
{
    t_tuning_writer_error_code ret = TUNING_WRITER_OK;

    if(iToBuffer)
    {
        // Writer mode is "write to buffer"
        int remainingLength = iDestBufferSize - iDestBufferFilledSize;
        int lineLength = strnlen(iCurLineStr,sizeof(iCurLineStr));

        // Write current line to buffer
        int res = snprintf(&iDestBuffer[iDestBufferFilledSize], remainingLength, "%s", iCurLineStr);
        if( res < 0 )
        {
            // Could not write line, reason unknown
            ret = TUNING_WRITER_STRING_CONSTRUCT_FAILED;
            return ret;
        }
        else if( res >= remainingLength)
        {
            // Could not write because of insufficient space
            ret = TUNING_WRITER_BUFFER_TOO_SMALL;
            return ret;
        }

        // Wrote line to buffer
        iDestBufferFilledSize += lineLength;
    }
    else
    {
        // Writer mode is "write to file"
        if( iOsal.WriteString(iCurLineStr) == false )
        {
            ret = TUNING_WRITER_FILE_WRITE_FAILED;
            return ret;
        }
    }

    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::MakeIndent()
{
    // Construct Indentation
    iIndentStr[0] = '\0';
    for(int cnt=0; cnt<iIndentCount; cnt++)
    {
        int remaining = sizeof(iIndentStr)- strnlen(iIndentStr,sizeof(iIndentStr));
        int filled = sizeof(iIndentStr) - remaining;
        int res = snprintf(&iIndentStr[filled], remaining-1, INDENT_ELEMENT);
        if( res < 0 || res >= remaining)
        {
            // Could not write line
            TUNING_INFO_MSG1("snprintf failed: res=%d\n", res);
            return NULL;
        }
    }
    return (const char*)iIndentStr;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::WriteLine(const char* aLine)
{
    return WriteLine("",aLine,"");
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::WriteLine(const char* aHead, const char* aMiddle, const char* aTail)
{
    TUNING_FUNC_IN0();
    t_tuning_writer_error_code ret = TUNING_WRITER_OK;
    int res = 0;

    // Check parameters sanity
    if( aHead==NULL || aMiddle==NULL || aTail==NULL)
    {
        // String construction failed
        TUNING_FUNC_OUTR(TUNING_WRITER_INTERNAL_ERROR);
        return TUNING_WRITER_INTERNAL_ERROR;
    }

    // New Line => inc line number
    IncLineNum();

    // Construct indentation
    const char *pIndent = MakeIndent();
    if( pIndent == 0)
    {
        // Indentation construction failed
        TUNING_FUNC_OUTR(TUNING_WRITER_STRING_CONSTRUCT_FAILED);
        return TUNING_WRITER_STRING_CONSTRUCT_FAILED;
    }

    // Make the whole line
    res = snprintf(iCurLineStr, sizeof(iCurLineStr), "%s%s%s%s\n", pIndent,aHead,aMiddle,aTail);
    if( res < 0 )
    {
        // String construction failed
        TUNING_FUNC_OUTR(TUNING_WRITER_STRING_CONSTRUCT_FAILED);
        return TUNING_WRITER_STRING_CONSTRUCT_FAILED;
    }
    else if (res >= (int)sizeof(iCurLineStr))
    {
        // Insufficient space in destination buffer
        TUNING_FUNC_OUTR(TUNING_WRITER_STRING_CONSTRUCT_FAILED);
        return TUNING_WRITER_STRING_CONSTRUCT_FAILED;
    }

    TUNING_INFO_MSG2("[Line %d] %s",iCurLineNum,iCurLineStr);

    // Do write
    ret = DoWriteLine();
    if( ret != TUNING_WRITER_OK )
    {
        // Could not write line
        TUNING_FUNC_OUTR(ret);
        return ret;
    }

    // Wrote String
    TUNING_FUNC_OUTR(TUNING_WRITER_OK);
    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::WriteXmlHeaderSection()
{
    TUNING_FUNC_IN0();
    t_tuning_writer_error_code res = TUNING_WRITER_OK;

    // Write general XML header stuff
    res = WriteLine("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    res = WriteLine("<?xml-stylesheet type=\"text/xsl\" href=\"tuning_stylesheet.xsl\"?>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    TUNING_FUNC_OUTR(TUNING_WRITER_OK);
    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::OpenFile(const char * aFileName)
{
    // Open the file
    TUNING_FUNC_IN0();
    if( iOsal.OpenFile(aFileName, eWriteMode)==false )
    {
        // Failed to open file
        TUNING_FUNC_OUTR(TUNING_WRITER_FILE_OPEN_FAILED);
        return TUNING_WRITER_FILE_OPEN_FAILED;
    }
    TUNING_FUNC_OUTR(TUNING_WRITER_OK);
    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::CloseFile()
{
    TUNING_FUNC_IN0();
    iOsal.CloseFile();
    TUNING_FUNC_OUTR(TUNING_WRITER_OK);
    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::DumpToFile(const char* aFileName, t_tuning_data* pTuningData)
{
    TUNING_FUNC_IN0();
    t_tuning_writer_error_code res = TUNING_WRITER_OK;

    // Check parameters
    if( aFileName==NULL || pTuningData == NULL)
    {
        TUNING_FUNC_OUTR(TUNING_WRITER_INVALID_ARGUMENT);
        return TUNING_WRITER_INVALID_ARGUMENT;
    }

    // Open the XML file to be populated
    res = OpenFile(aFileName);
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Populate the file
    iToBuffer = false;
    res = DoMakeXml(pTuningData);
    if(res != TUNING_WRITER_OK) {
        CloseFile();
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // File successfully populated
    CloseFile();
    TUNING_FUNC_OUTR(TUNING_WRITER_OK);
    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::DumpToBuffer( void*                 pDestBuffer,
                                                        const int             aDestBufferSize,
                                                        int*                  pBufferFilledSize,
                                                        const t_tuning_data*  pTuningData)
{
    TUNING_FUNC_IN0();
    t_tuning_writer_error_code res = TUNING_WRITER_OK;

    TUNING_INFO_MSG2("ToBuffer %p, aDestBufferSize=%d\n", pDestBuffer, aDestBufferSize);

    // Check parameters
    if( pDestBuffer==0 || aDestBufferSize == 0 || pBufferFilledSize==NULL || pTuningData == NULL)
    {
        TUNING_INFO_MSG2("Invalid argument: aDestBuffer=%p, aDestBufferSize=%d\n", pDestBuffer, aDestBufferSize);
        TUNING_INFO_MSG2("Invalid argument: aBufferFilledSize=%p, pTuningData=%p\n", pBufferFilledSize, pTuningData);
        TUNING_FUNC_OUTR(TUNING_WRITER_INVALID_ARGUMENT);
        return TUNING_WRITER_INVALID_ARGUMENT;
    }

    // Store buffer information
    iDestBuffer = (char*)pDestBuffer;
    iDestBufferSize = aDestBufferSize;
    iDestBufferFilledSize = 0;

    // Populate the buffer
    iToBuffer = true;
    res = DoMakeXml(pTuningData);
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // return the actual size of generated buffer
    TUNING_INFO_MSG2("Populated buffer %p with %lu bytes\n", pDestBuffer, iDestBufferFilledSize);
    *pBufferFilledSize = iDestBufferFilledSize;

    // Buffer successfully populated
    TUNING_FUNC_OUTR(TUNING_WRITER_OK);
    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::DoMakeXml(const t_tuning_data* pTuningData)
{
    TUNING_FUNC_IN0();
    t_tuning_writer_error_code res = TUNING_WRITER_OK;

    // Resets
    ResetIndent();
    ResetLineNum();

    // Write the XML header
    res = WriteXmlHeaderSection();
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the IQ Settings
    res = WriteIqSettingsSection(pTuningData);
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // XML successfully populated
    TUNING_FUNC_OUTR(TUNING_WRITER_OK);
    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::WriteIqSettingsSection(const t_tuning_data* pTuningData)
{
    TUNING_FUNC_IN0();
    t_tuning_writer_error_code res = TUNING_WRITER_OK;

    // Open the IQ settings section and Indent
    res = WriteLine("<IQSETTINGS>");
    Indent();
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write comments if any
    for(int i=0; i<pTuningData->iComments.count && i<COMMENTS_MAX_COUNT; i++)
    {
        res = WriteLine("<!--", pTuningData->iComments.pComment[i], "-->");
        if(res != TUNING_WRITER_OK) {
            TUNING_FUNC_OUTR(res);
            return res;
        }
    }

    // Write the file id section
    res = WriteFileIdSection(pTuningData);
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the configuration section
    res = WriteConfigurationSection(pTuningData);
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Unindent and close the IQ settings section
    UnIndent();
    res = WriteLine("</IQSETTINGS>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    TUNING_FUNC_OUTR(TUNING_WRITER_OK);
    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::WriteFileIdSection(const t_tuning_data* pTuningData)
{
    TUNING_FUNC_IN0();
    t_tuning_writer_error_code res = TUNING_WRITER_OK;

    // Open the section ID and Indent
    res = WriteLine("<ID>");
    Indent();
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the section OP_MODE
    res = WriteLine("<OP_MODE>", pTuningData->iOperatingMode.name, "</OP_MODE>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the section VERSION and Indent
    res = WriteLine("<VERSION>");
    Indent();
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the STRUCT section
    res = WriteLine("<STRUCT>", MakeString(iWriterVersion.structure), "</STRUCT>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the ENTRIES section
    res = WriteLine("<ENTRIES>", MakeString(iWriterVersion.entries), "</ENTRIES>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the PARAMS section
    res = WriteLine("<PARAMS>", MakeString(pTuningData->iConfigVersion.params), "</PARAMS>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the VALUES section
    res = WriteLine("<VALUES>", MakeString(pTuningData->iConfigVersion.values), "</VALUES>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Unindent and close the section VERSION
    UnIndent();
    res = WriteLine("</VERSION>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Open the SENSOR section and Indent
    res = WriteLine("<SENSOR>");
    Indent();
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the SENSOR NAME section
    res = WriteLine("<NAME>", pTuningData->iSensorId.name, "</NAME>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the SENSOR MANUFACTURER section
    res = WriteLine("<MANUFACTURER>", MakeHexString(pTuningData->iSensorId.manuf), "</MANUFACTURER>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the SENSOR MODEL section
    res = WriteLine("<MODEL>", MakeHexString(pTuningData->iSensorId.model), "</MODEL>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the SENSOR REVISION section
    res = WriteLine("<REVISION>", MakeString(pTuningData->iSensorId.rev), "</REVISION>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Unindent and close the section SENSOR
    UnIndent();
    res = WriteLine("</SENSOR>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Open the FIRMWARE section and Indent
    res = WriteLine("<FIRMWARE>");
    Indent();
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the FIRMWARE MAJOR section
    res = WriteLine("<MAJOR>", MakeString(pTuningData->iFirmwareVersion.major), "</MAJOR>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the FIRMWARE MINOR section
    res = WriteLine("<MINOR>", MakeString(pTuningData->iFirmwareVersion.minor), "</MINOR>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the FIRMWARE PATCH section
    res = WriteLine("<PATCH>", MakeString(pTuningData->iFirmwareVersion.patch), "</PATCH>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Unindent and close the section FIRMWARE
    UnIndent();
    res = WriteLine("</FIRMWARE>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Open the SW3A section and Indent
    res = WriteLine("<SW3A>");
    Indent();
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the SW3A MAJOR section
    res = WriteLine("<MAJOR>", MakeString(pTuningData->iSw3aVersion.major), "</MAJOR>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the SW3A MINOR section
    res = WriteLine("<MINOR>", MakeString(pTuningData->iSw3aVersion.minor), "</MINOR>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write the SW3A PATCH section
    res = WriteLine("<BUILD>", MakeString(pTuningData->iSw3aVersion.build), "</BUILD>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Unindent and close the section SW3A
    UnIndent();
    res = WriteLine("</SW3A>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Unindent and close the section ID
    UnIndent();
    res = WriteLine("</ID>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Wrote the file ID section
    TUNING_FUNC_OUTR(TUNING_WRITER_OK);
    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::WriteUserSection(const e_iquser_id aUserId, const t_iqset_user* pIqUser)
{
    TUNING_FUNC_IN0();
    t_tuning_writer_error_code res = TUNING_WRITER_OK;
    const t_iqset_param* pParam = NULL;

    // Check parameters
    if(CTuningUser::IsValidUserId(aUserId)==false || pIqUser==NULL )
    {
        TUNING_ERROR_MSG2("Bad argument: User=%d, pIqUser=%p\n", aUserId,pIqUser);
        TUNING_FUNC_OUTR(TUNING_WRITER_INTERNAL_ERROR);
        return TUNING_WRITER_INTERNAL_ERROR;
    }

    // Retrieve param list of this user
    pParam =  pIqUser->pParamList;

    // Write params of this user (note: user may have no params)
    if(pParam != NULL)
    {
        // Retrieve the name of this user
        const char* pUserName = CTuningUser::FindUserNameFromId(aUserId);
        if(pUserName==NULL)
        {
            TUNING_ERROR_MSG1("Unknown User %d\n", aUserId);
            TUNING_FUNC_OUTR(TUNING_WRITER_INTERNAL_ERROR);
            return TUNING_WRITER_INTERNAL_ERROR;
        }

        // Open the user section and Indent
        res = WriteLine("<",pUserName,">");
        Indent();
        if(res != TUNING_WRITER_OK) {
            TUNING_ERROR_MSG0("Failed to write userName\n");
            TUNING_FUNC_OUTR(res);
            return res;
        }

        // Parse the param list and write them
        while(pParam != NULL)
        {
            // Open a PARAM section and Indent
            res = WriteLine("<PARAM>");
            Indent();
            if(res != TUNING_WRITER_OK) {
                TUNING_ERROR_MSG0("Failed to write PARAM tag\n");
                TUNING_FUNC_OUTR(res);
                return res;
            }

            // Write PARAM NAME section
#if defined(TUNING_PARAM_NAMES_ENABLED)
            res = WriteLine("<NAME>", pParam->name, "</NAME>");
#else
            res = WriteLine("<NAME>", "*** COMPILED WITH PARAMETERS NAMES DISABLED ***", "</NAME>");
#endif
            if(res != TUNING_WRITER_OK) {
                TUNING_ERROR_MSG0("Failed to write NAME section tag\n");
                TUNING_FUNC_OUTR(res);
                return res;
            }

            // Write PARAM ADDR section
            res = WriteLine("<ADDR>", MakeHexString(pParam->addr), "</ADDR>");
            if(res != TUNING_WRITER_OK) {
                TUNING_FUNC_OUTR(res);
                return res;
            }

            // Check values array
            if(pParam->value == NULL)
            {
                // Array pointer invalid
                TUNING_FUNC_OUTR(TUNING_WRITER_BAD_VALUE_LIST);
                return TUNING_WRITER_BAD_VALUE_LIST;
            }

            // Writing value depends on value size
            if(pParam->rows*pParam->cols <= 0)
            {
                // Invalid size value
                TUNING_FUNC_OUTR(TUNING_WRITER_BAD_PARAM_SIZE);
                return TUNING_WRITER_BAD_PARAM_SIZE;
            }
            else if(pParam->rows*pParam->cols == 1)
            {
                // Write PARAM VALUE section
                switch(pParam->type)
                {
                    case IQSET_VT_FLOAT:
                        // Write float value
                        TUNING_INFO_MSG2("Addr 0x%lX, Val=%f\n",pParam->addr,pParam->value[0].f32);
                        res = WriteLine("<VALUE>", MakeString(pParam->value[0].f32), "</VALUE>");
                        break;
                    case IQSET_VT_INT:
                        // Write integer value
                        TUNING_INFO_MSG2("Addr 0x%lX, Val=%ld\n",pParam->addr,pParam->value[0].i32);
                        res = WriteLine("<VALUE>", MakeString(pParam->value[0].i32), "</VALUE>");
                        break;
                    default:
                        res = TUNING_WRITER_UNKNOWN_IQ_PARAM_TYPE;
                        break;
                }

                if(res != TUNING_WRITER_OK) {
                    TUNING_FUNC_OUTR(res);
                    return res;
                }
            }
            else
            {
                // Write vector/matrix dimension
                res = WriteLine("<SIZE>", MakeRowColString(pParam->rows,pParam->cols), "</SIZE>");
                if(res != TUNING_WRITER_OK) {
                    TUNING_FUNC_OUTR(res);
                    return res;
                }

                // Open the VALUE section and Indent
                res = WriteLine("<VALUE>");
                Indent();
                if(res != TUNING_WRITER_OK) {
                    TUNING_FUNC_OUTR(res);
                    return res;
                }

                // Write values
                for(int row=0; row<pParam->rows; row++)
                {
                    u_iqset_value* pValuesRow = &pParam->value[row*pParam->cols];
                    for(int col=0; col<pParam->cols; col++)
                    {
                       if(pParam->type==IQSET_VT_FLOAT) {
                           TUNING_INFO_MSG4("Addr 0x%lX, Val[%d][%d]=%f\n",pParam->addr, row, col, pValuesRow[col].f32);
                       }
                       else {
                           TUNING_INFO_MSG4("Addr 0x%lX, Val[%d][%d]=%ld\n",pParam->addr, row, col, pValuesRow[col].i32);
                       }
                    }
                    // Do write line
                    res = WriteLine(MakeString(pValuesRow, pParam->type, pParam->cols));
                    if(res != TUNING_WRITER_OK) {
                        TUNING_FUNC_OUTR(res);
                        return res;
                    }
                 }

                // Unindent and close the VALUE
                UnIndent();
                res = WriteLine("</VALUE>");
                if(res != TUNING_WRITER_OK) {
                    TUNING_FUNC_OUTR(res);
                    return res;
                }
            }

            // Unindent and close the PARAM section
            UnIndent();
            res = WriteLine("</PARAM>");
            if(res != TUNING_WRITER_OK) {
                TUNING_FUNC_OUTR(res);
                return res;
            }
            
            // Next parameter of the list
            pParam = pParam->next;
        }

        // Unindent and close the User section
        UnIndent();
        res = WriteLine("</",pUserName,">");
        if(res != TUNING_WRITER_OK) {
            TUNING_FUNC_OUTR(res);
            return res;
        }
    }

    // Done
    TUNING_FUNC_OUTR(TUNING_WRITER_OK);
    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_tuning_writer_error_code CTuningWriter::WriteConfigurationSection(const t_tuning_data* pTuningData)
{
    TUNING_FUNC_IN0();
    t_tuning_writer_error_code res = TUNING_WRITER_OK;
    t_sint32 userId = 0;
    t_sint32 paramCount = 0;

    // Check parameter
    if(pTuningData==NULL) {
        TUNING_FUNC_OUTR(TUNING_WRITER_INTERNAL_ERROR);
        return TUNING_WRITER_INTERNAL_ERROR;
    }

    // Open the CONFIGURATION section and Indent
    res = WriteLine("<CONFIGURATION>");
    Indent();
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Write IQ sets
    for(int IqSetId=0; IqSetId<(int)IQSET_COUNT; IqSetId++)
    {
        // IQSET is printed out only if it has entries
        for( userId=0, paramCount=0; userId < IQSET_USER_COUNT; userId++)
        {
            paramCount += pTuningData->iIqSet[IqSetId].user[userId].iParamCount;
        }
        if(paramCount==0)
        {
            continue;
        }

        // Open the IQSET section and Indent
        res = WriteLine("<IQSET>");
        Indent();
        if(res != TUNING_WRITER_OK) {
            TUNING_FUNC_OUTR(res);
            return res;
        }

        // Retrieve IQSET name from ID
        const char* IqSetName = CTuningIQSet::FindIQSetNameFromId((e_iqset_id)IqSetId);
        if(IqSetName==NULL) {
            TUNING_FUNC_OUTR(TUNING_WRITER_UNKNOWN_IQSET_ID);
            return TUNING_WRITER_UNKNOWN_IQSET_ID;
        }

        // Write IQSET name section
        res = WriteLine("<NAME>", IqSetName, "</NAME>");
        if(res != TUNING_WRITER_OK) {
            TUNING_FUNC_OUTR(res);
            return res;
        }

        // Write all user sections
        for(userId=0; userId < IQSET_USER_COUNT; userId++)
        {
            res = WriteUserSection( (e_iquser_id)userId, &pTuningData->iIqSet[IqSetId].user[userId]);
            if(res != TUNING_WRITER_OK) {
                TUNING_FUNC_OUTR(res);
                return res;
            }
        }

        // Unindent and close the IQSET section
        UnIndent();
        res = WriteLine("</IQSET>");
        if(res != TUNING_WRITER_OK) {
            TUNING_FUNC_OUTR(res);
            return res;
        }
    }

    // Unindent and close the CONFIGURATION section
    UnIndent();
    res = WriteLine("</CONFIGURATION>");
    if(res != TUNING_WRITER_OK) {
        TUNING_FUNC_OUTR(res);
        return res;
    }

    // Wrote the CONFIGURATION section
    TUNING_FUNC_OUTR(TUNING_WRITER_OK);
    return TUNING_WRITER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningWriter::ErrorCode2String(const t_tuning_writer_error_code aErr)
{
    for(unsigned int i=0; i<KWriterErrorCodesListSize; i++) {
        if(KWriterErrorCodesList[i].id == aErr)
            return KWriterErrorCodesList[i].name;
    }
    return "*** ERROR CODE NOT FOUND ***";
}

