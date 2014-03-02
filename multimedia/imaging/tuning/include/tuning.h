/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \file    
* \brief   header file for tuning.cpp: api for tuning class
* \author  ST-Ericsson
*/

#ifndef _TUNING_H_
#define _TUNING_H_

#include "tuning_types.h"
#include "tuning_parser.h"
#include "tuning_writer.h"
#include "tuning_user.h"
#include <stdint.h>

/** \brief General tuning param definition */
typedef struct
{
    t_uint32 addr;
    t_uint32 value;
    float f32dbg;
} t_tuning_param;

/** \brief General tuning param info definition */
typedef struct
{
    t_uint32 addr;
    int size;
} t_tuning_param_info;

/** \brief General merge option */
typedef enum {
    TUNING_HANDLE_DUPLICATES_AS_ERRORS,
    TUNING_IGNORE_DUPLICATE_ENTRIES
} e_merge_option;

/** \brief error code types used by CTuning method */
typedef enum
{
    TUNING_OK = 0,                /**< last method called performed as expected*/
    TUNING_ALREADY_CONSTRUCTED,   /**< The tuning instance has already been constructed */
    TUNING_INVALID_ARGUMENT,      /**< Invalid argument passed to API function */
    TUNING_MALLOC_ERROR,          /**< A memory allocation failed*/
    TUNING_NOT_CONSTRUCTED,       /**< Tuning instance has not been constructed by caller */
    TUNING_FILE_NOT_FOUND,        /**< tuning file to parse could not be opened */
    TUNING_FILE_READ_ERROR,       /**< could not read the file */
    TUNING_FOUND_NO_ENTRIES,      /**< nothing found in the tuning config */
    TUNING_TOO_MANY_ENTRIES,      /**< found too many configuration entries */
    TUNING_PARAM_SIZE_MISMATCH,   /**< param size does not match what was expected */
    TUNING_VERSION_MISMATCH,      /**< tuning config cannot be parsed as structure not known (version mismatch)*/
    TUNING_XML_ERROR,             /**< tuning config has a wrong structure or unknown keywords */
    TUNING_DUPLICATE_PARAM,       /**< parser/user attempt to add duplicate param to configuration */
    TUNING_PARAM_NOT_FOUND,       /**< The requested parameter not been found in the list */
    TUNING_PARAM_TYPE_MISMATCH,   /**< The type retrieved from file does not match the type expected bu caller */
    TUNING_ARRAY_TOO_SMALL,       /**< Destination array is too small => copy would overflow */
    TUNING_INVALID_IQSET_ID,      /**< The IQSet ID passed to function is unknown */
    TUNING_INVALID_USER_ID,       /**< The User ID passed to function is unknown */
    TUNING_PARAMS_ARRAY_FULL,     /**< No more space in Param array => cannot add a new entry */
    TUNING_COMMENTS_ARRAY_FULL,   /**< No more space in Comments array => cannot add a new entry */
    TUNING_FILE_CREATION_FAILED,  /**< Could not create the file requested */
    TUNING_FILE_WRITE_ERROR,      /**< Error while writing into the file */
    TUNING_BUFFER_TOO_SMALL,      /**< Buffer too small => configuration cannot be dumped*/
    TUNING_SENSOR_ID_MISMATCH,    /**< Sensor IDs mismatch */
    TUNING_INTERNAL_ERROR         /**< Internal error */

} t_tuning_error_code;

/** \brief class implementing the storage of PE configuration for tuning purpose */
class CTuning
{
    public:
        // Construction/destruction
        CTuning();
        CTuning(const char*);
        ~CTuning();
        t_tuning_error_code Construct(unsigned int);
        void Destroy(void);

        // Parsing/Writing entry points
        t_tuning_error_code FetchTuningConfiguration(const char* aFileName);
        t_tuning_error_code FetchTuningConfiguration(char* aBuffer, const int);
        t_tuning_error_code DumpTuningConfiguration(const char*);
        t_tuning_error_code DumpTuningConfiguration(void*, const int, int*);

        // Getters
        const char*         GetInstanceName();
        t_tuning_error_code GetFileStructureVersion(t_tuning_struct_version* pVersion);
        t_tuning_error_code GetConfigurationVersion(t_tuning_config_version* pVersion);
        t_tuning_error_code GetFirmwareVersion(t_tuning_fw_version* pVersion);
        t_tuning_error_code GetSw3aVersion(t_tuning_sw3a_version* pVersion);
        t_tuning_error_code GetSensorId(t_tuning_sensor_id* pSensorId);
        const char*         GetSensorName();
        const char*         GetOperativeModeName();

        t_tuning_error_code GetParam(const e_iqset_id, const e_iquser_id, const t_uint32, t_sint32*);
        t_tuning_error_code GetParam(const e_iqset_id, const e_iquser_id, const t_uint32, int*);
        t_tuning_error_code GetParam(const e_iqset_id, const e_iquser_id, const t_uint32, float*);

        t_sint32            GetIntParam(const e_iqset_id aIqset, const e_iquser_id, t_uint32 aAddr);
        float               GetFloatParam(const e_iqset_id aIqset, const e_iquser_id, t_uint32 aAddr);

        t_tuning_error_code GetParamList(const e_iqset_id, const e_iquser_id, t_tuning_param*,t_sint32*);
        t_sint32            GetParamListA(const e_iqset_id, const e_iquser_id, t_tuning_param*, int);
        t_tuning_error_code GetParamInfoList(const e_iqset_id, const e_iquser_id, t_tuning_param_info*,t_sint32*);

        t_tuning_error_code GetMatrix(const e_iqset_id, const e_iquser_id, t_uint32, t_sint32*, int*, int*);
        t_tuning_error_code GetMatrix(const e_iqset_id, const e_iquser_id, t_uint32, float*, int*, int*);
        t_tuning_error_code GetMatrixAsFloat(const e_iqset_id, const e_iquser_id, t_uint32, float*, int*, int*);
        t_tuning_error_code GetMatrixAsInt(const e_iqset_id, const e_iquser_id, t_uint32, t_sint32*, int*, int*);

        t_tuning_error_code GetVector(const e_iqset_id, const e_iquser_id, t_uint32, t_sint32*, int*);
        t_tuning_error_code GetVector(const e_iqset_id, const e_iquser_id, t_uint32, float*, int*);
        t_tuning_error_code GetVectorAsInt(const e_iqset_id, const e_iquser_id, t_uint32, t_sint32*, int*);
        t_tuning_error_code GetVectorAsFloat(const e_iqset_id, const e_iquser_id, t_uint32, float*, int*);

        t_sint32            GetParamCount();
        t_sint32            GetParamCount(const e_iqset_id, const e_iquser_id);

        // Setters
        t_tuning_error_code SetConfigurationVersion(const t_tuning_config_version* pVersion);
        t_tuning_error_code SetOperativeModeName(const char*);
        t_tuning_error_code SetFirmwareVersion(const t_tuning_fw_version* pVersion);
        t_tuning_error_code SetSensorId(const t_tuning_sensor_id* pSensorId);
        t_tuning_error_code SetSensorName(const char*);

        t_tuning_error_code AddParam(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const t_sint32);
        t_tuning_error_code AddParam(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const t_uint32);
        t_tuning_error_code AddParam(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const uint32_t);
        t_tuning_error_code AddParam(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const int);
        t_tuning_error_code AddParam(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const float);

        t_tuning_error_code AddMatrix(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const t_sint32*, const int, const int);
        t_tuning_error_code AddMatrix(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const t_uint32*, const int, const int);
        t_tuning_error_code AddMatrix(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const uint32_t*, const int, const int);
        t_tuning_error_code AddMatrix(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const int*, const int, const int);
        t_tuning_error_code AddMatrix(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const float*, const int, const int);

        t_tuning_error_code AddMatrix(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const t_sint32*, const int, const int, const int);
        t_tuning_error_code AddMatrix(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const t_uint32*, const int, const int, const int);
        t_tuning_error_code AddMatrix(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const uint32_t*, const int, const int, const int);
        t_tuning_error_code AddMatrix(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const int*, const int, const int, const int);
        t_tuning_error_code AddMatrix(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const float*, const int, const int, const int);

        t_tuning_error_code AddVector(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const t_sint32*, const int);
        t_tuning_error_code AddVector(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const t_uint32*, const int);
        t_tuning_error_code AddVector(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const uint32_t*, const int);
        t_tuning_error_code AddVector(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const int*, const int);
        t_tuning_error_code AddVector(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const float*, const int);

        t_tuning_error_code AddComment(const char*);
        t_tuning_error_code MergeWith(const CTuning*, const e_merge_option);

        // Debug stringyfication helpers
        static const char*  ErrorCode2String(t_tuning_error_code);
        static const char*  GetIqSetName(const e_iqset_id);
        static const char*  GetUserName(const e_iquser_id);

    private:
        // Private types
        typedef enum {
            TUNING_STRICT_TYPE_MATCH,
            TUNING_CONVERT_IF_TYPE_MISMATCH
        } e_type_check;

        // Private data
        bool                iConstructed;
        const char*         iInstanceName;
        t_tuning_data       iTuningData;

        // Private methods
        void                Reset();
        inline bool         IsInstanceConstructed() {return iConstructed;};
        t_iqset_param*      FindIqParam(t_iqset_param* aParamList, const t_uint32 aAddr);
        t_tuning_error_code DoGetParam( const e_iqset_id, const e_iquser_id, const t_uint32, const e_iqset_value_type, const e_type_check, void*,int*,int*);
        t_tuning_error_code DoAddParam(const e_iqset_id, const e_iquser_id, const t_uint32, const char*, const void*, const e_iqset_value_type, const int, const int, const int);
        t_tuning_error_code ConvertErrorCode(t_tuning_parser_error_code);
        t_tuning_error_code ConvertErrorCode(t_tuning_writer_error_code);
} ;

#endif /*_TUNING_H_*/

