/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TUNING_TYPES_H_
#define _TUNING_TYPES_H_

/** simple data types*/
#ifdef TUNING_STANDALONE_TEST /** Used for compiling the tuning object in a standalone test application*/
typedef unsigned char      t_uint8; /* 8-bit data types*/
typedef signed char        t_sint8;
typedef unsigned short     t_uint16; /* 16-bit container on 32-bits architectures */
typedef signed short       t_sint16;
typedef signed short       t_bool;
typedef unsigned long      t_uint32; /* 32-bit container on 32-bits architectures */
typedef signed long        t_sint32;
typedef unsigned char      byte;

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef GET_AND_SET_TRACE_LEVEL
#define GET_AND_SET_TRACE_LEVEL(a)
#endif

#else
#include <inc/typedef.h>
#endif

#include "tuning_iqset.h"
#include "tuning_user.h"

#if defined(TUNING_TRACE_ALL) || defined(TUNING_TRACE_PARAM) || defined(TUNING_STANDALONE_TEST)
#define TUNING_PARAM_NAMES_ENABLED
#endif

typedef enum
{
    IQSET_VT_UNKNOWN = 0,
    IQSET_VT_FLOAT,
    IQSET_VT_INT
} e_iqset_value_type;

typedef union
{
    float    f32; // 32 Bits floating point representation of the value
    t_sint32 i32; // 32 bits signed integer representation of the value
    t_uint32 r32; // 32 bits raw representation of the value
}u_iqset_value;

typedef struct s_iqset_param
{
    t_uint32           addr;
    int                rows;
    int                cols;
    e_iqset_value_type type;
    u_iqset_value*     value;
    s_iqset_param*     next;
#if defined(TUNING_PARAM_NAMES_ENABLED)
    char               name[128];
#endif
} t_iqset_param;

typedef struct
{
    t_iqset_param* pParamList;
    t_iqset_param* pLastParam;
    t_sint32       iParamCount;
} t_iqset_user;

typedef struct
{
    t_iqset_user user[IQSET_USER_COUNT];
} t_iqset;

/** \brief Version of the tuning file/firmware */
typedef struct
{
    t_uint16 major; /**< major id of the firmware for tuning file */
    t_uint16 minor; /**< minor id of the firmware for tuning file */
    t_uint16 patch; /**< patch id of the firmware for tuning file */
} t_tuning_fw_version;

/** \brief Version of the tuning file structure */
typedef struct
{
    t_uint16 structure; /**< xml structure version */
    t_uint16 entries;   /**< iq sets entries version */
} t_tuning_struct_version;

/** \brief Version of the tuning configuration */
typedef struct
{
    t_uint16 params;    /**< incremented when params are added/deleted */
    t_uint16 values;    /**< incremented when params values are changed */
} t_tuning_config_version;

/** \brief Version of the SW3A library */
typedef struct
{
    t_uint16 major; /**< major id of the sw3A library */
    t_uint16 minor; /**< minor id of the sw3A library */
    t_uint16 build; /**< build id of the sw3A library */
} t_tuning_sw3a_version;

#define SENSOR_NAME_MAX_SIZE 64

/** \brief Id of the Sensor */
typedef struct
{
    char     name[SENSOR_NAME_MAX_SIZE]; /**< name of the sensor */
    t_uint32 manuf;                      /**< sensor manufacturer id */
    t_uint32 model;                      /**< sensor model id */
    t_uint32 rev;                        /**< sensor revision number */
} t_tuning_sensor_id;

/** \brief Comments */
#define COMMENTS_MAX_COUNT  32
#define COMMENTS_MAX_LENGTH 256
typedef struct
{
    int   count;
    char* pComment[COMMENTS_MAX_COUNT];
} t_comments;

/** \brief Operative Mode */
#define OPERATIVE_MODE_MAX_SIZE 64
typedef struct
{
    char name[OPERATIVE_MODE_MAX_SIZE];
} t_operating_mode;

typedef struct
{
    t_iqset_param*  pParam;      /**< \brief Pool of free parameters */
    t_uint32        iTotal;      /**< \brief Total Number of parameters in the parameters pool */
    t_uint32        iUsed;       /**< \brief Count of used parameters in the parameters pool */
} t_tuning_params_pool;

typedef struct
{
    u_iqset_value*  pValue;     /**< \brief Pool of free values */
    t_uint32        iTotal;     /**< \brief Total Number of values in the values pool */
    t_uint32        iUsed;      /**< \brief Count of used values in the values pool */
} t_tuning_values_pool;

/** \brief structure for passing the full set of tuning data */
typedef struct
{
    t_operating_mode        iOperatingMode;         /**< \brief Name of the operating mode retrieved from the xml file */
    t_tuning_struct_version iStructVersion;         /**< \brief file structure version retrieved from the xml file */
    t_tuning_config_version iConfigVersion;         /**< \brief configuration version retrieved from the xml file */
    t_tuning_sw3a_version   iSw3aVersion;           /**< \brief sw3a version retrieved from the xml file */
    t_tuning_fw_version     iFirmwareVersion;       /**< \brief version of the FW compatible with the PE in the file */
    t_tuning_sensor_id      iSensorId;              /**< \brief Id of the sensor retrieved from the xml file */
    t_comments              iComments;              /**< \brief Comments manually added to tuning instance */
    t_iqset                 iIqSet[IQSET_COUNT];    /**< \brief Array of IQ sets entries found in the tuning file */
    t_tuning_params_pool    iParamsPool;            /**< \brief Pool of tuning parameters */
    t_tuning_values_pool    iValuesPool;            /**< \brief Pool of tuning values */
} t_tuning_data;

#endif /*_TUNING_TYPES_H_*/
