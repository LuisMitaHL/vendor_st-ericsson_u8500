/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * Trace identifiers
 */
#define OMXCOMPONENT "TUNING_PARSER"
#define OMX_TRACE_UID 0x10

/*
 * Includes
 */
#include "tuning_types.h"
#include "tuning_macros.h"
#include "tuning_parser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Local definition
*/

/*
 *  \version 1: initial released version
 *  \version 2: add of 4 digit tuning file versioning
 *  \version 3: add of <SIZE> tag for vectors/matrices
 *  \version 4: add of <SW3A> library versioning
 */
#define PARSER_STRUCT_VERSION 4

/*
 * Local variables
*/
#define HASH_ENTRY(id) {id,#id}

typedef struct
{
    t_tuning_parser_error_code id;
    const char* name;
} t_parser_error_entry;

static const t_parser_error_entry KParserErrorCodesList[] =
{
    HASH_ENTRY(TUNING_PARSER_OK),
    HASH_ENTRY(TUNING_PARSER_INVALID_ARGUMENT),
    HASH_ENTRY(TUNING_PARSER_MALLOC_ERROR),
    HASH_ENTRY(TUNING_PARSER_FILE_OPEN_FAILED),
    HASH_ENTRY(TUNING_PARSER_FILE_READ_ERROR),
    HASH_ENTRY(TUNING_PARSER_FOUND_NO_DATA),
    HASH_ENTRY(TUNING_PARSER_TOO_MANY_ENTRIES),
    HASH_ENTRY(TUNING_PARSER_PARAM_SIZE_MISMATCH),
    HASH_ENTRY(TUNING_PARSER_PARAM_TYPE_MISMATCH),
    HASH_ENTRY(TUNING_PARSER_XML_ERROR),
    HASH_ENTRY(TUNING_PARSER_DUPLICATE_PARAM),
    HASH_ENTRY(TUNING_PARSER_VERSION_STRUCT_MISMATCH),
    HASH_ENTRY(TUNING_PARSER_VERSION_ENTRIES_MISMATCH),
    HASH_ENTRY(TUNING_PARSER_INTERNAL_ERROR)
};

#define KParserErrorCodesListSize (sizeof(KParserErrorCodesList)/sizeof(KParserErrorCodesList[0]))

// Prevent export of symbols
#ifdef __SYMBIAN32__
NONSHARABLE_CLASS(CTuningParser);
#endif

/****************************************************************************/
/**
 * \brief   constructor of CTuningParser class
 * \author  vollejm
 *
 **/
/****************************************************************************/
CTuningParser::CTuningParser()
{
    TUNING_FUNC_IN0();
    iParserVersion.structure = PARSER_STRUCT_VERSION;
    iParserVersion.entries = IQSET_ENTRIES_VERSION;
    iFromBuffer = true;
    pBuffer = NULL;
    iBufferSize = 0;
    iBufferPos = 0;
    iNotParsed = true;
    iLine[0] = '\0';
    iLineCount = 0;
    iTagValue[0] = '\0';
    iTagName[0] = '\0';
    iTagDepth = 0;
    iTagPath[0][0] = '\0';
    iLineType = XML_EMPTY_LINE;
#if defined(TUNING_PARAM_NAMES_ENABLED)
    iParamName[0] = '\0';
#endif
    TUNING_INFO_MSG2("Parser Version: Struct=%d Entries=%d\n",iParserVersion.structure,iParserVersion.entries);
    TUNING_FUNC_OUT0();
    return;
}

/****************************************************************************/
/**
 * \brief   destructor of CTuningParser class
 * \author  vollejm
 *
 **/
/****************************************************************************/
CTuningParser::~CTuningParser()
{
    TUNING_FUNC_IN0();
    if(pBuffer) {
        delete [] pBuffer;
        pBuffer = NULL;
    }
    TUNING_FUNC_OUT0();
    return;
}

/****************************************************************************/
/**
 * \brief   parse the xml tuning file open via #OpenFile
 * \author  vollejm
 * \out     aIqSetList: pointer on an array to store the Iqsets descriptions
 * \out     aIqSetCount: number of IQsets retrieved
 * \out     aPageElementList: Pointer on a array to store the page elements retrieved
 * \return  parsing error code: see #t_tuning_parser_error_code
 *
 *    The parsing is done on a line by line reading of the file (#readLine) and interpretation of the line content type.
 *
 **/
/****************************************************************************/
t_tuning_parser_error_code CTuningParser::ParseFileContent(const char* aFileName, t_tuning_data* pParsedData)
{
    TUNING_FUNC_IN0();
    t_tuning_parser_error_code err = TUNING_PARSER_OK;

    // Sanity check
    if(aFileName==NULL || pParsedData==NULL)
    {
        TUNING_ERROR_MSG2("Invalid argument: aFileName=%p, pParsedData=%p\n", aFileName, pParsedData);
        TUNING_FUNC_OUTR(TUNING_PARSER_INVALID_ARGUMENT);
        return TUNING_PARSER_INVALID_ARGUMENT;
    }

    TUNING_INFO_MSG1("File to be parsed: aFileName=%s\n", aFileName);

    // Open the file
    if (iOsal.OpenFile(aFileName,eReadMode)==false)
    {
        // Open failed
        TUNING_FUNC_OUTR(TUNING_PARSER_FILE_OPEN_FAILED);
        return TUNING_PARSER_FILE_OPEN_FAILED;
    }

    // Configuration will parsed from file
    iFromBuffer = false;
    
    // Get file size
    iBufferSize = iOsal.GetSize();
    if(iBufferSize <= 0) {
        iOsal.CloseFile();
        TUNING_FUNC_OUTR(TUNING_PARSER_FOUND_NO_DATA);
        return TUNING_PARSER_FOUND_NO_DATA;
    }
    // Allocate buffer 
    pBuffer = new char[iBufferSize];
    if(pBuffer==NULL) {
        iBufferSize = 0;
        iOsal.CloseFile();
        TUNING_FUNC_OUTR(TUNING_PARSER_MALLOC_ERROR);
        return TUNING_PARSER_MALLOC_ERROR;
    }
    // Configuration will parsed from file
    iFromBuffer = true;
    iBufferPos = 0;
    // Load file into buffer
    if( iOsal.ReadFile(pBuffer,iBufferSize) != iBufferSize) {
        delete [] pBuffer;
        iBufferSize = 0;
        iOsal.CloseFile();
        TUNING_FUNC_OUTR(TUNING_PARSER_FILE_READ_ERROR);
        return TUNING_PARSER_FILE_READ_ERROR;
    }

    // Do the parsing
    err = doParse(pParsedData);

    // Release file buffer
    delete [] pBuffer;
    pBuffer = NULL;
    iBufferSize = 0;

    // Close the file
    iOsal.CloseFile();
    TUNING_FUNC_OUTR(err);
    return err;
}

/****************************************************************************/
/**
 * \brief   parse a xml tuning buffer
 * \author  vollejm
 * \out     aIqSetList: pointer on an array to store the Iqsets descriptions
 * \out     aIqSetCount: number of IQsets retrieved
 * \out     aPageElementList: Pointer on a array to store the page elements retrieved
 * \return  parsing error code: see #t_tuning_parser_error_code
 *
 *    The parsing is done on a line by line reading of the file (#readLine) and interpretation of the line content type.
 *
 **/
/****************************************************************************/
t_tuning_parser_error_code CTuningParser::ParseBufferContent( char*          aBuffer,
                                                              const t_sint32 aBufferSize,
                                                              t_tuning_data* pParsedData)
{
    TUNING_FUNC_IN0();
    t_tuning_parser_error_code err = TUNING_PARSER_OK;

    TUNING_INFO_MSG2("Buffer to be parsed: aBuffer=%p, aBufferSize=%ld\n", aBuffer, aBufferSize);

    // Sanity check
    if(aBuffer==NULL || aBufferSize==0 || pParsedData==NULL)
    {
        TUNING_ERROR_MSG3("Invalid argument: aBuffer=%p, aBufferSize=%ld, pParsedData=%p\n", aBuffer, aBufferSize, pParsedData);
        TUNING_FUNC_OUTR(TUNING_PARSER_INVALID_ARGUMENT);
        return TUNING_PARSER_INVALID_ARGUMENT;
    }

    // Configuration will parsed from buffer
    pBuffer = aBuffer;
    iBufferSize = aBufferSize;
    iBufferPos = 0;
    iFromBuffer = true;

    // Do the parsing
    err = doParse(pParsedData);

    // Done
    pBuffer = NULL;
    iBufferSize = 0;

    TUNING_FUNC_OUTR(err);
    return err;
}

/****************************************************************************/
/**
 * \brief   parse the xml tuning file open via #OpenFile
 * \author  vollejm
 * \out     aIqSetList: pointer on an array to store the Iqsets descriptions
 * \out     aIqSetCount: number of IQsets retrieved
 * \out     aPageElementList: Pointer on a array to store the page elements retrieved
 * \return  parsing error code: see #t_tuning_parser_error_code
 *
 *    The parsing is done on a line by line reading of the file (#readLine) and interpretation of the line content type.
 *
 **/
/****************************************************************************/
t_tuning_parser_error_code CTuningParser::doParse(t_tuning_data* pParsedData)
{
    TUNING_FUNC_IN0();
    t_xml_line_type    eLineType = XML_FILE_END;
    e_iqset_id         eIqSetId = IQSET_UNKNOWN;
    e_iquser_id        eUserIdTrial = IQSET_USER_UNKNOW;
    e_iquser_id        eUserId = IQSET_USER_UNKNOW;
    t_iqset*           pIqSet = NULL;
    t_iqset_param      sDummyParam = {0, 0, 0, IQSET_VT_UNKNOWN, NULL, NULL};
    t_iqset_param*     pParam = &sDummyParam;
    int                iRow = 0;
    int                iCol = 0;
    e_iqset_value_type eValType = IQSET_VT_UNKNOWN;

    // Sanity check
    if(pParsedData==NULL)
    {
        TUNING_ERROR_MSG1("Invalid argument: pParsedData=%p\n", pParsedData);
        TUNING_ASSERT(0);
        TUNING_FUNC_OUTR(TUNING_PARSER_INTERNAL_ERROR);
        return TUNING_PARSER_INTERNAL_ERROR;
    }

    iNotParsed = true;
    iLineCount = 0;
    iTagName[0] = '\0';
    iTagValue[0] ='\0';
    iTagDepth = 0;
    strncpy(iTagPath[0],"ROOT",sizeof(iTagPath[0])-1);

    /*get the very first line of the file */
    eLineType = readLine(eLineType);

    /* Loop on all lines */
    while (eLineType != XML_FILE_END)
    {
        // Next next line
        eLineType = readLine(eLineType);
        switch (eLineType)
        {
        /*------------------------------------------------------------*/
        /*           Comment/Empty Tags                               */
        /*------------------------------------------------------------*/
        case XML_COMMENT_BODY:
        case XML_COMMENT_END:
        case XML_COMMENT_LINE:
        case XML_COMMENT_START:
        case XML_EMPTY_LINE:
        case XML_PREPROC_LINE:
        case XML_FILE_END:
            break;
        /*------------------------------------------------------------*/
        /*                  Start of section Tag                      */
        /*------------------------------------------------------------*/
        case XML_TAG_START:
            /*------------------------------------------------------------*/
            /*                  IQSET Start Tag                           */
            /*------------------------------------------------------------*/
            if (!strncmp(iTagName,"IQSET",TAG_NAME_MAX_SIZE))
            {
                TUNING_INFO_MSG2("[Line %ld] IQSET: %s\n", iLineCount, iTagName);
            }
            /*------------------------------------------------------------*/
            /*                  PARAM Start Tag                           */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"PARAM",TAG_NAME_MAX_SIZE))
            {
                /* New IQ parameter: chain the entry to the relevant USER entry */
                iRow = 0;
                iCol = 0;
                if(eUserId != IQSET_USER_UNKNOW)
                {
                    TUNING_INFO_MSG2("[Line %ld] User %s: new PARAM\n", iLineCount, CTuningUser::FindUserNameFromId(eUserId));

                    // Check whether some param are available in the pool
                    if(pParsedData->iParamsPool.iUsed>=pParsedData->iParamsPool.iTotal)
                    {
                        TUNING_ERROR_MSG2("[Line %ld] too many param entries (max=%ld)\n", iLineCount, pParsedData->iParamsPool.iTotal);
                        TUNING_ASSERT(0);
                        TUNING_FUNC_OUTR(TUNING_PARSER_TOO_MANY_ENTRIES);
                        return TUNING_PARSER_TOO_MANY_ENTRIES;
                    }

                    // Get a free parameter from the pool
                    pParam = &pParsedData->iParamsPool.pParam[pParsedData->iParamsPool.iUsed];
                    pParsedData->iParamsPool.iUsed++;
                    // Link it to the current user list
                    if(pIqSet->user[eUserId].pParamList==NULL) {
                       // No parameters yet
                       pIqSet->user[eUserId].pParamList = pParam;
                    }
                    else {
                       pIqSet->user[eUserId].pLastParam->next = pParam;
                    }
                    pIqSet->user[eUserId].pLastParam = pParam;
                    pParam->next = NULL;
                    pIqSet->user[eUserId].iParamCount++;

                    // By default, we attach 1 value to parameter
                    // Check if enough values entries are available in the pool
                    if( pParsedData->iValuesPool.iUsed + 1 > pParsedData->iValuesPool.iTotal )
                    {
                        TUNING_ERROR_MSG2("[Line %ld] too many value entries (max:%ld)\n", iLineCount, pParsedData->iValuesPool.iTotal);
                        TUNING_ASSERT(0);
                        TUNING_FUNC_OUTR(TUNING_PARSER_TOO_MANY_ENTRIES);
                        return TUNING_PARSER_TOO_MANY_ENTRIES;
                    }

                    // Attach value slot to parameter
                    pParam->value = &pParsedData->iValuesPool.pValue[pParsedData->iValuesPool.iUsed];
                    pParam->rows = 1;
                    pParam->cols = 1;
                    pParsedData->iValuesPool.iUsed++;
                    TUNING_INFO_MSG5("[Line %ld] PARAM 0x%lX, values=%p, size=%dx%d\n", iLineCount,pParam->addr, pParam->value, pParam->rows, pParam->cols);
                }
                else
                {
                    /* unrecognized user retrieved from file */
                    TUNING_ERROR_MSG3("[Line %ld] TAG '%s': invalid parent '%s'", iLineCount, iTagName, getTagParent());
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
            }
            /*------------------------------------------------------------*/
            /*             CONFIGURATION Start Tag                        */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"CONFIGURATION",TAG_NAME_MAX_SIZE))
            {
                /* We have found the start of the configuration */
                TUNING_INFO_MSG1("[Line %ld] Start of section CONFIGURATION\n", iLineCount);
            }
            /*------------------------------------------------------------*/
            /*                   Is a USER Tag ?                          */
            /*------------------------------------------------------------*/
            else if ((eUserIdTrial=CTuningUser::FindUserIdFromName(iTagName))!=IQSET_USER_UNKNOW)
            {
                // User entry
                eUserId = eUserIdTrial;
                TUNING_PARAM_MSG0("[W] ----------------------------------------------\n");
                TUNING_PARAM_MSG2("[W] [%s] [%s]\n", CTuningIQSet::FindIQSetNameFromId(eIqSetId), CTuningUser::FindUserNameFromId(eUserId));
                TUNING_PARAM_MSG0("[W] ----------------------------------------------\n");
            }
            /*------------------------------------------------------------*/
            /*               Other Tags for keyword validity check        */
            /*------------------------------------------------------------*/
            else if
              (   !strncmp(iTagName,"EXTENSION",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"VALUE",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"IQSETTINGS",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"ID",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"OP_MODE",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"VERSION",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"SENSOR",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"MANUFACTURER",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"MODEL",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"REVISION",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"FIRMWARE",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"SW3A",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"HISTORY",TAG_NAME_MAX_SIZE)
               || !strncmp(iTagName,"DESC",TAG_NAME_MAX_SIZE)
              )
            {
                // Nothing to do (just for checking keyword validity)
                TUNING_INFO_MSG2("[Line %ld] Start of section %s\n", iLineCount, iTagName);
            }
            else
            {
                // Tag was not recognized
                TUNING_ERROR_MSG2("[Line %ld] Unrecognized tag: %s\n", iLineCount,iTagName);
                TUNING_ASSERT(0);
                TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                return TUNING_PARSER_XML_ERROR;
            }
            break;
        case XML_TAG_VALUE:
        case XML_TAG_LINE:
            /*------------------------------------------------------------*/
            /*                  NAME Tag                                  */
            /*------------------------------------------------------------*/
            if (!strncmp(iTagName,"NAME",TAG_NAME_MAX_SIZE))
            {
                /* We are on a <NAME> tag let's find whose name it is*/
                if (!strncmp(getTagParent(),"PARAM",TAG_NAME_MAX_SIZE))
                {
                    /* We don't retrieve the PE names, only needed for debugging*/
                    TUNING_INFO_MSG2("[Line %ld] Retrieving PARAM: %s\n", iLineCount,iTagValue);
#if defined(TUNING_PARAM_NAMES_ENABLED)
                    snprintf(iParamName, sizeof(iParamName), "%s", iTagValue);
#endif
                }
                else if (!strncmp(getTagParent(),"SENSOR",TAG_NAME_MAX_SIZE))
                {
                    TUNING_INFO_MSG2("[Line %ld] Retrieving Sensor Name: %s\n", iLineCount,iTagValue);
                    strncpy(pParsedData->iSensorId.name,iTagValue, SENSOR_NAME_MAX_SIZE-1);
                }
                else if (!strncmp(getTagParent(),"IQSET",TAG_NAME_MAX_SIZE))
                {
                    TUNING_INFO_MSG2("[Line %ld] Retrieving IQSET: %s\n", iLineCount,iTagValue);
                    eIqSetId = CTuningIQSet::FindIQSetIdFromName(iTagValue);
                    // Check IQSet validity
                    if(eIqSetId==IQSET_UNKNOWN)
                    {
                        // IQSET unknown
                        TUNING_ERROR_MSG2("[Line %ld] Unrecognized IQSET: %s\n", iLineCount,iTagValue);
                        TUNING_ASSERT(0);
                        TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                        return TUNING_PARSER_XML_ERROR;
                    }
                    pIqSet = &pParsedData->iIqSet[eIqSetId];
                }
                else
                {
                    TUNING_ERROR_MSG1("[Line %ld] UNEXPECTED NAME TAG", iLineCount);
                    TUNING_ERROR_MSG3("[Line %ld] parent->tag: %s->%s\n", iLineCount,getTagParent(),iTagName);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
            }
            /*------------------------------------------------------------*/
            /*                  ADDR Tag                                  */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"ADDR",TAG_NAME_MAX_SIZE))
            {
                t_uint32 addr = string2int32(iTagValue);
                // Sanity check
                if(pIqSet==NULL)
                {
                    TUNING_ERROR_MSG1("[Line %ld] Error: pIqSet==NULL (seems that IQSET tag was not parsed)\n", iLineCount);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
                // Check whether the current user has already this param entry
                if(isParamAlreadyStored(addr, &pIqSet->user[eUserId])==true)
                {
                    TUNING_ERROR_MSG3("[Line %ld] IQSET %s, User %s\n", iLineCount, CTuningUser::FindUserNameFromId(eUserId), CTuningUser::FindUserNameFromId(eUserId));
                    TUNING_ERROR_MSG2("[Line %ld] Duplicate param: 0x%lX\n", iLineCount, addr);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_DUPLICATE_PARAM);
                    return TUNING_PARSER_DUPLICATE_PARAM;
                }
                // Sanity check
                if(pParam==NULL)
                {
                    TUNING_ERROR_MSG1("[Line %ld] Error: pParam==NULL (seems that ADDR tag was not parsed)\n", iLineCount);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
                // Store param address
                pParam->addr = addr;
                TUNING_INFO_MSG2("[Line %ld] ADDR is: 0x%lX\n", iLineCount,pParam->addr);
#if defined(TUNING_PARAM_NAMES_ENABLED)
                snprintf(pParam->name, sizeof(pParam->name), "%s", iParamName);
#endif
            }
            /*------------------------------------------------------------*/
            /*                  SIZE Tag                                  */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"SIZE",TAG_NAME_MAX_SIZE))
            {
                // Sanity check
                if(pParam==NULL)
                {
                    TUNING_ERROR_MSG1("[Line %ld] Error: pParam==NULL (seems that ADDR tag was not parsed)\n", iLineCount);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
                // Check whether SIZE tag has already been parsed
                if(pParam->rows != 1 && pParam->cols != 1)
                {
                    // Seems that size has already been set
                    TUNING_ERROR_MSG3("[Line %ld] Size of PARAM was already fetched (was %dx%d)\n", iLineCount, pParam->rows, pParam->cols);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
                // Read the value size
                int rows = 0;
                int cols = 0;
                t_tuning_parser_error_code err = string2RowCol(iTagValue, &rows, &cols);
                if(err != TUNING_PARSER_OK)
                {
                    TUNING_ERROR_MSG1("[Line %ld] Failed to decode parameter dimension\n", iLineCount);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
                if(rows<=0 || cols<=0)
                {
                    TUNING_ERROR_MSG3("[Line %ld] invalid dimension: %dx%d\n", iLineCount, rows, cols);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }

                TUNING_INFO_MSG3("[Line %ld] IQSET %s, User %s\n", iLineCount, CTuningUser::FindUserNameFromId(eUserId), CTuningUser::FindUserNameFromId(eUserId));
                TUNING_INFO_MSG4("[Line %ld] PARAM 0x%lX: size=%dx%d\n", iLineCount, pParam->addr, rows, cols);

                // Check if enough values entries are available in the pool (already 1 values was reserved by default)
                if( pParsedData->iValuesPool.iUsed + (rows*cols) - 1 > pParsedData->iValuesPool.iTotal )
                {
                    TUNING_ERROR_MSG2("[Line %ld] too many value entries (max:%ld)\n", iLineCount, pParsedData->iValuesPool.iTotal);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_TOO_MANY_ENTRIES);
                    return TUNING_PARSER_TOO_MANY_ENTRIES;
                }

                // Value already attached, just update actual parameter dimension
                pParam->rows = rows;
                pParam->cols = cols;
                pParam->type = IQSET_VT_UNKNOWN;
                TUNING_INFO_MSG5("[Line %ld] PARAM 0x%lX, values=%p, size=%dx%d\n", iLineCount,pParam->addr, pParam->value, pParam->rows, pParam->cols);
                pParsedData->iValuesPool.iUsed += rows*cols - 1;

                TUNING_PARAM_MSG0("[W] ----------------------------------------------\n");
                TUNING_PARAM_MSG6("[W] [%s][%s][Addr=0x%X][Name=%s][Size=%dx%d]\n", CTuningIQSet::FindIQSetNameFromId(eIqSetId), CTuningUser::FindUserNameFromId(eUserId), (int)pParam->addr, pParam->name, pParam->rows, pParam->cols);
            }
            /*------------------------------------------------------------*/
            /*                  VALUE Tag                                  */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"VALUE",TAG_NAME_MAX_SIZE))
            {
                // Sanity check
                if(pParam==NULL)
                {
                    TUNING_ERROR_MSG1("[Line %ld] Error: pParam==NULL (seems that ADDR tag was not parsed)\n", iLineCount);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }

                // Check if we don't exceed the expected number of rows
                if( iRow >= pParam->rows )
                {
                    TUNING_ERROR_MSG3("[Line %ld] Param 0x%lX: too many rows (expected %d)\n", iLineCount, pParam->addr, pParam->rows);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_PARAM_SIZE_MISMATCH);
                    return TUNING_PARSER_PARAM_SIZE_MISMATCH;
                }

                // Loop on all values on this line
                const char* pTagValue = firstValue(iTagValue);
                if(pTagValue==NULL)
                {
                    // Could not fetch the first value (at least one value is mandatory)
                    TUNING_ERROR_MSG2("[Line %ld] Failed to fetch value from string: '%s' (empty string ?)\n", iLineCount, iTagValue);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }

                while(pTagValue!=NULL)
                {
                    TUNING_INFO_MSG2("[Line %ld] VALUE substring: %s\n", iLineCount,pTagValue);

                    // Check if we don't exceed the expected number of columns
                    if( iCol >= pParam->cols)
                    {
                        TUNING_ERROR_MSG3("[Line %ld] Param 0x%lX: too many columns (expected %d)\n", iLineCount, pParam->addr, pParam->cols);
                        TUNING_ASSERT(0);
                        TUNING_FUNC_OUTR(TUNING_PARSER_PARAM_SIZE_MISMATCH);
                        return TUNING_PARSER_PARAM_SIZE_MISMATCH;
                    }

                    // Read the value
                    float f32Value = 0;
                    t_sint32 i32Value = 0;
                    if(strstr(pTagValue,"."))
                    {
                        // floating point entry.
                        f32Value = string2f32(pTagValue);
                        eValType = IQSET_VT_FLOAT;
                        TUNING_INFO_MSG2("[Line %ld] VALUE decoded: %f\n", iLineCount,f32Value);
                    }
                    else
                    {
                        // We assume it is an integer:
                        i32Value = string2int32(pTagValue);
                        eValType = IQSET_VT_INT;
                        TUNING_INFO_MSG3("[Line %ld] VALUE decoded: %ld (0x%lX)\n", iLineCount,i32Value, i32Value);
                    }

                    // Check/assign value type
                    if(iRow==0 && iCol==0)
                    {
                        // First value for this param
                        pParam->type = eValType;
                    }
                    // Check that this value has same type as the very first one
                    else if(pParam->type != eValType)
                    {
                        TUNING_ERROR_MSG2("[Line %ld] Param 0x%lX, bad value type\n", iLineCount, pParam->addr);
                        TUNING_ERROR_MSG3("[Line %ld] Row=%d, Col=%d\n", iLineCount, iRow, iCol);
                        TUNING_ERROR_MSG3("[Line %ld] Expected type '%s', found '%s'\n", iLineCount,pParam->type==IQSET_VT_INT?"integer":"float",eValType==IQSET_VT_INT?"integer":"float");
                        TUNING_ASSERT(0);
                        TUNING_FUNC_OUTR(TUNING_PARSER_PARAM_TYPE_MISMATCH);
                        return TUNING_PARSER_PARAM_TYPE_MISMATCH;
                    }

                    // Assign new value
                    int index = (pParam->cols*iRow)+iCol;
                    if (eValType==IQSET_VT_FLOAT)
                    {
                        // floating point entry
                        pParam->value[index].f32 = f32Value;
                        TUNING_PARAM_MSG8("[W] [%s][%s][Addr=0x%X][Name=%s][%d][%d] [Val=%f] (Val=0x%lX)\n",
                                          CTuningIQSet::FindIQSetNameFromId(eIqSetId),
                                          CTuningUser::FindUserNameFromId(eUserId),
                                          (int)pParam->addr, pParam->name, iRow, iCol,
                                          pParam->value[index].f32,
                                          pParam->value[index].r32);
                    }
                    else
                    {
                        // integer entry
                        pParam->value[index].i32 = i32Value;
                        TUNING_PARAM_MSG8("[W] [%s][%s][Addr=0x%X][Name=%s][%d][%d] [Val=%d] (Val=0x%X)\n",
                                          CTuningIQSet::FindIQSetNameFromId(eIqSetId),
                                          CTuningUser::FindUserNameFromId(eUserId),
                                          (int)pParam->addr, pParam->name, iRow, iCol,
                                          (int)pParam->value[index].i32,
                                          (int)pParam->value[index].i32);
                    }

                    // Fetch next value if any of current line if any
                    pTagValue = nextValue(pTagValue);
                    iCol++;
                }

                // Check if we had the expected number of columns
                if( iCol < pParam->cols )
                {
                    TUNING_ERROR_MSG3("[Line %ld] Param 0x%lX: too few columns (%d columns expected)\n", iLineCount, pParam->addr, pParam->cols);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_PARAM_SIZE_MISMATCH);
                    return TUNING_PARSER_PARAM_SIZE_MISMATCH;
                }

                // Parse next row if any
                iCol = 0;
                iRow += 1;
            }
            /*------------------------------------------------------------*/
            /*                  MAJOR Tag                                 */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"MAJOR",TAG_NAME_MAX_SIZE))
            {
                if (!strncmp(getTagParent(),"FIRMWARE",TAG_NAME_MAX_SIZE))
                {
                    pParsedData->iFirmwareVersion.major = string2int32(iTagValue);
                    TUNING_INFO_MSG2("[Line %ld] Retrieving FW MAJOR:%d\n", iLineCount,pParsedData->iFirmwareVersion.major);
                }
                else if (!strncmp(getTagParent(),"SW3A",TAG_NAME_MAX_SIZE))
                {
                    pParsedData->iSw3aVersion.major = string2int32(iTagValue);
                    TUNING_INFO_MSG2("[Line %ld] Retrieving SW3A MAJOR:%d\n", iLineCount,pParsedData->iSw3aVersion.major);
                }
                else
                {
                    TUNING_ERROR_MSG2("[Line %ld] Tag MAJOR: unexpected parent %s\n", iLineCount, getTagParent());
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
            }
            /*------------------------------------------------------------*/
            /*                  MINOR Tag                                 */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"MINOR",TAG_NAME_MAX_SIZE))
            {
                if (!strncmp(getTagParent(),"FIRMWARE",TAG_NAME_MAX_SIZE))
                {
                    pParsedData->iFirmwareVersion.minor = string2int32(iTagValue);
                    TUNING_INFO_MSG2("[Line %ld] Retrieving FW MINOR: %d\n", iLineCount,pParsedData->iFirmwareVersion.minor);
                }
                else if (!strncmp(getTagParent(),"SW3A",TAG_NAME_MAX_SIZE))
                {
                    pParsedData->iSw3aVersion.minor = string2int32(iTagValue);
                    TUNING_INFO_MSG2("[Line %ld] Retrieving SW3A MINOR: %d\n", iLineCount,pParsedData->iFirmwareVersion.minor);
                }
                else
                {
                    TUNING_ERROR_MSG2("[Line %ld] Tag MINOR: unexpected parent %s\n", iLineCount, getTagParent());
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
            }
            /*------------------------------------------------------------*/
            /*                  PATCH Tag                                 */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"PATCH",TAG_NAME_MAX_SIZE))
            {
                if (!strncmp(getTagParent(),"FIRMWARE",TAG_NAME_MAX_SIZE))
                {
                    pParsedData->iFirmwareVersion.patch = string2int32(iTagValue);
                    TUNING_INFO_MSG3("[Line %ld] Retrieving FW PATCH:0x%x (%d)\n", iLineCount,pParsedData->iFirmwareVersion.patch,pParsedData->iFirmwareVersion.patch);
                }
                else
                {
                    TUNING_ERROR_MSG2("[Line %ld] Tag PATCH: unexpected parent %s\n", iLineCount, getTagParent());
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
            }
            /*------------------------------------------------------------*/
            /*                  BUILD Tag                                 */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"BUILD",TAG_NAME_MAX_SIZE))
            {
                if (!strncmp(getTagParent(),"SW3A",TAG_NAME_MAX_SIZE))
                {
                    pParsedData->iSw3aVersion.build = string2int32(iTagValue);
                    TUNING_INFO_MSG2("[Line %ld] Retrieving SW3A BUILD: %d\n", iLineCount,pParsedData->iSw3aVersion.build);
                }
                else
                {
                    TUNING_ERROR_MSG2("[Line %ld] Tag PATCH: unexpected parent %s\n", iLineCount, getTagParent());
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
            }
            /*------------------------------------------------------------*/
            /*                  STRUCT Tag                                */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"STRUCT",TAG_NAME_MAX_SIZE))
            {
                pParsedData->iStructVersion.structure = string2int32(iTagValue);
                TUNING_INFO_MSG2("[Line %ld] Retrieving file version struct: 0x%X\n", iLineCount,pParsedData->iStructVersion.structure);
                if (pParsedData->iStructVersion.structure != iParserVersion.structure)
                {
                    // File structure mismatch, will most probably lead to parsing failure => Abort now
                    TUNING_ERROR_MSG3("[Line %ld] Unsupported file structure (Parser struct version: %d, file struct version: %d)\n",iLineCount,iParserVersion.structure,pParsedData->iStructVersion.structure);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_VERSION_STRUCT_MISMATCH);
                    return TUNING_PARSER_VERSION_STRUCT_MISMATCH;
                }
            }
            /*------------------------------------------------------------*/
            /*                  ENTRIES Tag                               */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"ENTRIES",TAG_NAME_MAX_SIZE))
            {
                pParsedData->iStructVersion.entries = string2int32(iTagValue);
                TUNING_INFO_MSG2("[Line %ld] Retrieving file version entries: 0x%X\n", iLineCount,pParsedData->iStructVersion.entries);
                if (pParsedData->iStructVersion.entries > iParserVersion.entries)
                {
                    // IQSet entries version mismatch, will most probably lead to parsing failure => Abort now
                    TUNING_ERROR_MSG3("[Line %ld] Unsupported entries version (Parser entries version: %d, file entries version: %d)\n",iLineCount,iParserVersion.entries,pParsedData->iStructVersion.entries);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_VERSION_ENTRIES_MISMATCH);
                    return TUNING_PARSER_VERSION_ENTRIES_MISMATCH;
                }
            }
            /*------------------------------------------------------------*/
            /*                  PARAMS Tag                                */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"PARAMS",TAG_NAME_MAX_SIZE))
            {
                pParsedData->iConfigVersion.params= string2int32(iTagValue);
                TUNING_INFO_MSG2("[Line %ld] Retrieving file version params: 0x%X\n", iLineCount,pParsedData->iConfigVersion.params);
            }
            /*------------------------------------------------------------*/
            /*                  VALUES Tag                                */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"VALUES",TAG_NAME_MAX_SIZE))
            {
                pParsedData->iConfigVersion.values= string2int32(iTagValue);
                TUNING_INFO_MSG2("[Line %ld] Retrieving file version values: 0x%X\n", iLineCount,pParsedData->iConfigVersion.values);
            }
            /*------------------------------------------------------------*/
            /*                  MANUFACTURER Tag                          */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"MANUFACTURER",TAG_NAME_MAX_SIZE))
            {
                if (!strncmp(getTagParent(),"SENSOR",TAG_NAME_MAX_SIZE))
                {
                    pParsedData->iSensorId.manuf = string2int32(iTagValue);
                    TUNING_INFO_MSG2("[Line %ld] Retrieving Sensor MANUFACTURER: 0x%.lX\n", iLineCount,pParsedData->iSensorId.manuf);
                }
                else
                {
                    TUNING_ERROR_MSG1("[Line %ld] UNEXPECTED MANUFACTURER TAG", iLineCount);
                    TUNING_ERROR_MSG3("[Line %ld] parent->tag: %s->%s\n", iLineCount,getTagParent(),iTagName);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
            }
            /*------------------------------------------------------------*/
            /*                  MODEL Tag                                 */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"MODEL",TAG_NAME_MAX_SIZE))
            {
                if (!strncmp(getTagParent(),"SENSOR",TAG_NAME_MAX_SIZE))
                {
                    pParsedData->iSensorId.model = string2int32(iTagValue);
                    TUNING_INFO_MSG2("[Line %ld] Retrieving Sensor MODEL: 0x%.lX\n", iLineCount,pParsedData->iSensorId.model);
                }
                else
                {
                    TUNING_ERROR_MSG1("[Line %ld] UNEXPECTED MODEL TAG", iLineCount);
                    TUNING_ERROR_MSG3("[Line %ld] parent->tag: %s->%s\n", iLineCount,getTagParent(),iTagName);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
            }
            /*------------------------------------------------------------*/
            /*                  REVISION Tag                              */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"REVISION",TAG_NAME_MAX_SIZE))
            {
                if (!strncmp(getTagParent(),"SENSOR",TAG_NAME_MAX_SIZE))
                {
                    pParsedData->iSensorId.rev = string2int32(iTagValue);
                    TUNING_INFO_MSG2("[Line %ld] Retrieving Sensor REVISION: 0x%.lX\n", iLineCount,pParsedData->iSensorId.rev);
                }
                else
                {
                    TUNING_ERROR_MSG1("[Line %ld] UNEXPECTED REVISION TAG", iLineCount);
                    TUNING_ERROR_MSG3("[Line %ld] parent->tag: %s->%s\n", iLineCount,getTagParent(),iTagName);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
            }
            /*------------------------------------------------------------*/
            /*                  OP_MODE Tag                               */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"OP_MODE",TAG_NAME_MAX_SIZE))
            {
                strncpy(pParsedData->iOperatingMode.name,iTagValue,OPERATIVE_MODE_MAX_SIZE-1);
                TUNING_INFO_MSG2("[Line %ld] Retrieving operative mode name: %s\n", iLineCount,pParsedData->iOperatingMode.name);
            }
            /*------------------------------------------------------------*/
            /*                  DESC Tag                                  */
            /*------------------------------------------------------------*/
            else if (!strncmp(iTagName,"DESC",TAG_NAME_MAX_SIZE))
            {
                TUNING_INFO_MSG2("[Line %ld] Retrieving description: %s\n", iLineCount,iTagValue);
            }
            /*------------------------------------------------------------*/
            /*                Unrecognized Tag                            */
            /*------------------------------------------------------------*/
            else
            {
                TUNING_ERROR_MSG2("[Line %ld] Unrecognized tag: %s\n", iLineCount,iTagName);
                TUNING_ASSERT(0);
                TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                return TUNING_PARSER_XML_ERROR;
            }
            break;
        /*------------------------------------------------------------*/
        /*                  End of section Tag                        */
        /*------------------------------------------------------------*/
        case XML_TAG_END:
            if (!strncmp(iTagName,"PARAM",TAG_NAME_MAX_SIZE))
            {
                // Sanity check
                if(pParam==NULL)
                {
                    TUNING_ERROR_MSG1("[Line %ld] Error: pParam==NULL (seems that ADDR tag was not parsed)\n", iLineCount);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
                    return TUNING_PARSER_XML_ERROR;
                }
                TUNING_INFO_MSG2("[Line %ld] End of PARAM 0x%lX section\n", iLineCount, pParam->addr);
                // Reset the values information
                eValType = IQSET_VT_UNKNOWN;
                iRow = 0;
                iCol = 0;
            }
            else if (!strncmp(iTagName,"VALUE",TAG_NAME_MAX_SIZE))
            {
                // Check the number of fetched rows
                if( iRow != pParam->rows )
                {
                    TUNING_ERROR_MSG3("[Line %ld] Insufficient number of rows (%d rows expected, got %d)\n", iLineCount, pParam->rows, iRow);
                    TUNING_ASSERT(0);
                    TUNING_FUNC_OUTR(TUNING_PARSER_PARAM_SIZE_MISMATCH);
                    return TUNING_PARSER_PARAM_SIZE_MISMATCH;
                }
                // Reset the values information
                eValType = IQSET_VT_UNKNOWN;
                iRow = 0;
                iCol = 0;
            }
            else if (!strncmp(iTagName,"CONFIGURATION",TAG_NAME_MAX_SIZE))
            {
                // End of configuration, then just print debug info
                TUNING_INFO_MSG1("[Line %ld] End of CONFIGURATION\n", iLineCount);
            }
            break;
        /*------------------------------------------------------------*/
        /*                  Errors                                    */
        /*------------------------------------------------------------*/
        case XML_ERROR:
            TUNING_ERROR_MSG1("[Line %ld] Bad XML structure\n", iLineCount);
            TUNING_ASSERT(0);
            TUNING_FUNC_OUTR(TUNING_PARSER_XML_ERROR);
            return TUNING_PARSER_XML_ERROR;
        default:
            TUNING_ERROR_MSG2("[Line %ld] Unexpected XML tag: %d\n", iLineCount, eLineType);
            TUNING_ASSERT(0);
            TUNING_FUNC_OUTR(TUNING_PARSER_INTERNAL_ERROR);
            return TUNING_PARSER_INTERNAL_ERROR;
        } // end of switch on line types
    } // end of loop on lines in the xml file

    TUNING_INFO_MSG0("Parsing done\n");

    iNotParsed = false;

    if (iLineCount == 0)
    {
        iNotParsed = true;
        TUNING_ERROR_MSG0("Found nothing in file\n");
        TUNING_ASSERT(0);
        TUNING_FUNC_OUTR(TUNING_PARSER_FOUND_NO_DATA);
        return TUNING_PARSER_FOUND_NO_DATA;
    }

    // Parsing done
    TUNING_FUNC_OUTR(TUNING_PARSER_OK);
    return TUNING_PARSER_OK;
}

/****************************************************************************/
/**
 * \brief   read a line from the xml file and report the type of data on the line
 * \author  vollejm
 * \in      aPreviousLineType: line type found during the last call.
 * \return  return the type of the line found: #t_xml_line_type
 *
 **/
/****************************************************************************/
t_xml_line_type CTuningParser::readLine(t_xml_line_type aPreviousLineType)
{
    t_xml_line_type eLineType = XML_ERROR;
    char*           pStart = NULL;
    char*           pEnd = NULL;
    const char*     pValueStart = NULL;
    char*           pValueEnd = NULL;
    bool            bRes = false;

    // Read next line
    if(iFromBuffer == true)
        bRes = getLineFromBuffer(iLine,512);
    else
        bRes = iOsal.GetLine(iLine,512);

    if (bRes==false) {
        TUNING_INFO_MSG0("Got nothing => end of file\n");
        return XML_FILE_END;
    }

    iLineCount++; // must stay here as we return as soon as line type is found;
    TUNING_INFO_MSG2("[Line %ld] Raw line: [%s]\n",iLineCount,iLine);

    /** parsing is done by expecting sthg on the current line according to what has been seen on previous line*/
    switch(aPreviousLineType)
    {
        /** a several line comment has been started */
        case XML_COMMENT_BODY:
        {
            /**< no break */
        }
        case XML_COMMENT_START:
        {
            if (strstr(iLine,"-->"))
            {
                // this is the end of a comment started some lines ago.
                TUNING_INFO_MSG1("[Line %ld] COMMENT\n",iLineCount);
                eLineType = XML_COMMENT_END;
            }
            else
            {
                // comment is still running on this line
                TUNING_INFO_MSG1("[Line %ld] COMMENT BODY\n",iLineCount);
                eLineType = XML_COMMENT_BODY;
            }
            break;
        }
        default:
        {
            /** Does the line start a new comment?*/
            if (strstr(iLine,"<!--"))
            {
                TUNING_INFO_MSG1("[Line %ld] COMMENT START\n",iLineCount);
                eLineType = XML_COMMENT_START;
                // Is the comment on one line?
                if (strstr(iLine,"-->"))
                {
                    TUNING_INFO_MSG1("[Line %ld] COMMENT LINE\n",iLineCount);
                    eLineType = XML_COMMENT_LINE;
                }
            } // end of line with a starting comment

            /** test for a preprocessing line */
            else if (strstr(iLine,"<?"))
            {
                TUNING_INFO_MSG1("[Line %ld] PREPROC LINE\n",iLineCount);
                eLineType = XML_PREPROC_LINE;
            }
            /** test for a starting tag*/
            else
            {
                // Do we have a tag?
                pStart = strstr(iLine,"<");
                pEnd = strstr(iLine,">");
                if (pStart && pEnd)
                {
                    if (*(pStart+1) == '/')
                    {
                        TUNING_INFO_MSG1("[Line %ld] TAG END\n",iLineCount);
                        eLineType = XML_TAG_END;
                        iTagDepth--;
                        *pEnd =  '\0';
                        strncpy(iTagName,pStart+2,TAG_NAME_MAX_SIZE-1);
                    }
                    else
                    {
                        /** There is at least a starting tag, let's check if there is the ending one*/
                        TUNING_INFO_MSG1("[Line %ld] TAG START\n",iLineCount);
                        eLineType = XML_TAG_START;
                        iTagDepth++;
                        *pEnd = '\0';
                        strncpy(iTagName,pStart+1,TAG_NAME_MAX_SIZE-1);
                        if(iTagDepth>=TAG_DEPTH_MAX) {
                            TUNING_ERROR_MSG1("Max XML depth reached (max=%d)\n",TAG_DEPTH_MAX);
                            TUNING_ASSERT(0);
                            return XML_ERROR;
                        }
                        strncpy(iTagPath[iTagDepth],iTagName,TAG_NAME_MAX_SIZE-1);

                        /* We keep current position in case we have a value for the current tag*/
                        pValueStart = pEnd+1;

                        /* We start looking for an ending tag from the last position reached in the line*/
                        pStart = strstr(pEnd+1,"<");
                        pEnd = strstr(pEnd+1,">");
                        if (pStart && pEnd)
                        {
                            if (*(pStart+1) == '/')
                            {
                                TUNING_INFO_MSG1("[Line %ld] TAG LINE\n",iLineCount);
                                eLineType = XML_TAG_LINE;
                                iTagDepth--;
                                *pEnd ='\0';
                                /* We have an opening and closing tag */
                                pValueEnd = pStart;
                                *pValueEnd = '\0';
                                strncpy(iTagValue,pValueStart,sizeof(iTagValue)-1);

                            } // end of detection of a closing tag
                            else
                            {
                                // 2 opening tags on the same line
                                TUNING_INFO_MSG1("[Line %ld] 2 Opening tags on the same line\n",iLineCount);
                            }
                        } // end of looking for an ending tag on a line with a starting tag
                    } // end of looking for a line with starting end ending tag
                } // end of looking on a line with at least one tag
                else
                {
                    // The line has no tag could it have only values
                    pValueStart = firstValue(iLine);
                    if(pValueStart!=NULL)
                    {
                        // We could reach the line "payload"
                        // It can be a single/multiple line value
                        if (aPreviousLineType == XML_TAG_START || aPreviousLineType==XML_TAG_VALUE)
                        {
                            strcpy(iTagValue,pValueStart);
                            TUNING_INFO_MSG1("[Line %ld] TAG VALUE\n",iLineCount);
                            eLineType = XML_TAG_VALUE;
                        }
                        else
                        {
                            // We have found something unexpected
                            TUNING_INFO_MSG2("[Line %ld] Ooops don't understand this [%s]\n",iLineCount,pValueStart);
                            TUNING_ASSERT(0);
                            return XML_ERROR;
                        }
                    }
                    else
                    {
                        // The line is blank
                        TUNING_INFO_MSG1("[Line %ld] EMPTY LINE\n",iLineCount);
                        eLineType = XML_EMPTY_LINE;
                    }
                }
            }
        }
    }
    iLineType = eLineType;
    return eLineType;
}

/****************************************************************************/
/**
 * \brief   retrieve the name of the tag parent to the current tag
 * \author  vollejm
 * \note    Depth of the current Tag is stored in #iTagDepth
 * \note    Complete path to the current tag is stored in #iTagPath
 * \note    Depth is incremented each time a new opening tag is found and decremented each time a closing tag is found
 * For a TAG line, Depth is incremented and decremented is the same go in that case the parent of the tag is store at depth instead of depth -1
 **/
/****************************************************************************/
char* CTuningParser::getTagParent()
{
    if ( iLineType == XML_TAG_LINE )
    {
        return iTagPath[iTagDepth];
    }
    else
    {
        return iTagPath[iTagDepth-1];
    }
}

/****************************************************************************/
/**
 * \brief    read a line from the buffer
 * \author   vollejm
 * \return
 **/
/****************************************************************************/
bool CTuningParser::getLineFromBuffer(char* aLine, int aLength)
{
    int nPos = 0;
    const char* pCurChar = NULL;
    int nCnt = 0;
    int nNbCharsToRead = 0;

    nPos = iBufferPos;
    pCurChar = pBuffer+ iBufferPos;

    // did we already reach the end of the buffer?
    TUNING_ASSERT(iBufferPos<=iBufferSize);
    if (iBufferPos == iBufferSize)
        return false;

    // read at max what is remaining in the buffer
    nNbCharsToRead = aLength-1 < (iBufferSize - iBufferPos)? aLength-1:(iBufferSize - iBufferPos);

    // read until we read nbCharsToRead or we find a line feed
    for(nCnt=0; nCnt<nNbCharsToRead; nCnt++)
    {
        aLine[nCnt] = *pCurChar;
        if ((*pCurChar == 0xd) || (*pCurChar == 0xa)) break; // break on new line
        pCurChar++; nPos++;
    }

    // close the string (remove the lf already copied)
    aLine[nCnt] = '\0';
    if (nCnt == nNbCharsToRead)
    {
        // we reached the end of file without breaking on a new line
    }
    else
    {
        // skip current 0xd or 0xa in the buffer
        // pos might be out of the buffer now
        nPos++;

        // skip trailing LF (symbian/dos) in the buffer
        if( (nPos<iBufferSize) && (pBuffer[nPos]==0xa))
        {
            nPos++;
        }
    }

    iBufferPos = nPos;
    return true;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
t_sint32 CTuningParser::string2int32(const char* aStr)
{
    /* Sanity check */
    TUNING_ASSERT(!strstr(aStr,"."));
    if (strstr(aStr,"0x") || strstr(aStr,"0X") )
    {
        // hexadecimal entry
        return strtol(aStr,0,16);
    }
    else
    {
        // We assume it is decimal integer:
        return strtol(aStr,0,10);
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
t_tuning_parser_error_code CTuningParser::string2RowCol(const char* aStr, int* pRows, int* pCols)
{
    /* Sanity check */
    if(strstr(aStr,"x")==NULL) {
       return TUNING_PARSER_XML_ERROR;
    }
    *pRows = strtol(aStr,0,10);
    *pCols = strtol(strstr(aStr,"x")+1,0,10);
    return TUNING_PARSER_OK;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
float CTuningParser::string2f32(const char* aStr)
{
    /* Sanity check */
    TUNING_ASSERT(strstr(aStr,"."));
    return (float)atof(aStr);
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningParser::firstValue(const char* pString)
{
    const char* pVal = pString;
    // Sanity check
    TUNING_ASSERT(pString!=NULL);
    if(pString==NULL) {
        return NULL;
    }
    // Skip white spaces and tabulations to reach first value
    while( ((*pVal == ' ') || (*pVal == '\t')) && (*pVal != '\0')) {
       pVal++;
    }
    return *pVal=='\0' ? NULL : pVal;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningParser::nextValue(const char* pString)
{
    const char* pVal = pString;
    // Sanity check
    TUNING_ASSERT(pString!=NULL);
    if(pString==NULL) {
        return NULL;
    }
    // Skip 1st value and reach separator (white space or tabulation)
    while( (*pVal != ' ') && (*pVal != '\t') && (*pVal != '\0')) {
       pVal++;
    }
    // Reach 1st character of value (skip white spaces and tabulations)
    while( ((*pVal == ' ') || (*pVal == '\t')) && (*pVal != '\0')) {
       pVal++;
    }
    return *pVal=='\0' ? NULL : pVal;
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
const char* CTuningParser::ErrorCode2String(t_tuning_parser_error_code aErr)
{
    for(unsigned int i=0; i<KParserErrorCodesListSize; i++) {
        if(KParserErrorCodesList[i].id == aErr)
            return KParserErrorCodesList[i].name;
    }
    return "*** ERROR CODE NOT FOUND ***";
}

/****************************************************************************/
/**
 * \brief
 * \author
 * \in
 * \return
 **/
/****************************************************************************/
bool CTuningParser::isParamAlreadyStored(const t_uint32 aAddr, const t_iqset_user* pUser)
{
    t_iqset_param* pParam = pUser->pParamList;
    while( pParam != NULL) {
        if(pParam->addr == aAddr)
            return true;
        pParam = pParam->next;
    }
    return false;
}

