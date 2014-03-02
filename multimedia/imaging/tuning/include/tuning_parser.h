/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file    
* \brief   header file for tuning_parser.cpp
* \author  ST-Ericsson
*/

#ifndef _TUNING_PARSER_H_
#define _TUNING_PARSER_H_

#include "tuning_types.h"
#include "tuning_iqset.h"
#include "tuning_osal.h"

/** \brief maximum level of depth supported in the xml file*/
#define TAG_DEPTH_MAX 8
#define TAG_NAME_MAX_SIZE 64

/** parser error code*/
typedef enum
{
    TUNING_PARSER_OK = 0,                   /**< last method called performed as expected */
    TUNING_PARSER_INVALID_ARGUMENT,         /**< caller passed an invalid argument */
    TUNING_PARSER_MALLOC_ERROR,             /**< memory allocation failure */
    TUNING_PARSER_FILE_OPEN_FAILED,         /**< could not open the file */
    TUNING_PARSER_FILE_READ_ERROR,          /**< Error while reading the file */
    TUNING_PARSER_FOUND_NO_DATA,            /**< No entry found in the tuning file/buffer */
    TUNING_PARSER_TOO_MANY_ENTRIES,         /**< Found too many entries, internal tables would overflow */
    TUNING_PARSER_PARAM_SIZE_MISMATCH,      /**< Size of parameter does not match what was expected */
    TUNING_PARSER_PARAM_TYPE_MISMATCH,      /**< Type of parameter does not match what was expected */
    TUNING_PARSER_XML_ERROR,                /**< An error was detected during parsing */
    TUNING_PARSER_DUPLICATE_PARAM,          /**< Param found twice in configuration */
    TUNING_PARSER_VERSION_STRUCT_MISMATCH,  /**< The parser and the file structure version are not aligned. Proper parsing cannot be ensured */
    TUNING_PARSER_VERSION_ENTRIES_MISMATCH, /**< File/Parser IQSets entries version mismatch */
    TUNING_PARSER_INTERNAL_ERROR            /**< Parser internal error */

} t_tuning_parser_error_code;

/** type of content of a line in the xml file */
typedef enum
{
	XML_ERROR = 0,      /**< line content could not be understood */
    XML_EMPTY_LINE,     /**< current line is empty */
    XML_FILE_END,       /**< end of line has been reached when trying to read next line */
    XML_COMMENT_LINE,   /**< current line is a one line comment */
    XML_COMMENT_START,  /**< current line is the starting of a several lines comment */
    XML_COMMENT_END,    /**< current line is the end of a several lines comment */
    XML_COMMENT_BODY,   /**< current line is part of a several lines comment */
    XML_PREPROC_LINE,   /**< current line is a one line pre-processing directive */
    XML_TAG_LINE,       /**< current line contains a complete TAG */
    XML_TAG_START,      /**< current line contains a opening tag */
    XML_TAG_END,        /**< current line contains a ending tag */
    XML_TAG_VALUE       /**< current line contains a tag value */
} t_xml_line_type;

/** \brief class implementing the retrieval of PE configuration from xml tuning files */
class CTuningParser
{
    public:
        CTuningParser ();
        ~CTuningParser();
        t_tuning_parser_error_code ParseFileContent(const char* aFileName, t_tuning_data* pParsedData);
        t_tuning_parser_error_code ParseBufferContent(char* aBuffer, const t_sint32 aBufferSize, t_tuning_data* pParsedData);
        static const char*         ErrorCode2String(t_tuning_parser_error_code);

    private:
        /** private variables */
        t_tuning_struct_version    iParserVersion;              /**< \brief Current version of the parser to be compared to the one of the xml file (#iFileVersion) */
        CTuningOsal                iOsal;                       /**< \brief Osal function for file access and strings manipulations */
        bool                       iFromBuffer;                 /**< \brief True if pasing is done from a buffer, false if done from a file */
        char*                      pBuffer;                     /**< \brief Buffer where file is copied before processing */
        int                        iBufferSize;                 /**< \brief Size of buffer to process */
        int                        iBufferPos;                  /**< \brief Current reading position in buffer being read */
        bool                       iNotParsed;                  /**< \brief True if parsing was ok */
        char                       iLine[512];                  /**< \brief Current line in xml file*/
        t_sint32                   iLineCount;                  /**< \brief Current lines count in the xml file */
        char                       iTagValue[128];              /**< \brief Value of the last read Tag */
        char                       iTagName[TAG_NAME_MAX_SIZE]; /**< \brief Name of the last read Tag */
        t_sint32                   iTagDepth;                   /**< \brief Depth of the current Tag in the xml file */
        char                       iTagPath[TAG_DEPTH_MAX][TAG_NAME_MAX_SIZE];
                                                                /**< \brief Path to the current Tag, path include current tag name as long as the tag is open*/
        t_xml_line_type            iLineType;                   /**< \brief Xml line type content for the latest read line in the file*/
#if defined(TUNING_PARAM_NAMES_ENABLED)
        char                       iParamName[128];             /**< \brief Name of the parameter (for debug purpose) */
#endif

        /** private methods */
        t_tuning_parser_error_code doParse(t_tuning_data* pParsedData);
        bool                       isParamAlreadyStored(const t_uint32 aAddr, const t_iqset_user* pUser);
        t_xml_line_type            readLine(t_xml_line_type aPreviousLineType);
        bool                       getLineFromBuffer(char* aLine, int aLenght);
        char*                      getTagParent();
        t_sint32                   string2int32(const char* aStr);
        t_tuning_parser_error_code string2RowCol(const char* aStr, int*, int*);
        float                      string2f32(const char* aStr);
        const char*                firstValue(const char* pString);
        const char*                nextValue(const char* pString);
};

#endif /*_TUNING_PARSER_H_*/
