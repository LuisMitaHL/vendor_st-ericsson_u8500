/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  Generic parameters API
 */

#ifndef MPL_PARAM_H
#define MPL_PARAM_H

/**************************************************************************
 * Includes
 *************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "mpl_list.h"

#ifdef  __cplusplus
extern "C" {
#endif

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/
#define max_limit_string  256
#define max_limit_enum    0
#define max_limit_int     INT_MAX
#define max_limit_uint8   UINT8_MAX
#define max_limit_uint32  UINT32_MAX
#define max_limit_bool    1
#define max_limit_uint8_array   UINT8_MAX
#define max_limit_string_tuple   UINT8_MAX

#define MPL_MAX_ARGS 100
#define MPL_PARAMID_PREFIX_MAXLEN 20
#define MPL_PARAM_SET_ID_TO_PARAMID_BASE(param_set_id) ((param_set_id) << 16)
#define MPL_PARAMID_TO_PARAM_SET_ID(paramid) (((paramid) & 0x7fff0000) >> 16)

/**
 * This macro can be used to define the structure 'paramsetname_param_descr_set'
 *
 * Note that both lowercase name and uppercase name for the set are required.
 *
 */
#define MPL_DEFINE_PARAM_DESCR_SET(paramsetname, PARAMSETNAME) \
static mpl_param_descr_set_t paramsetname ##_param_descr_set =  \
{                                                                \
    paramsetname ##_param_descr,                                  \
    PARAMSETNAME ##_PARAMID_PREFIX,                               \
    PARAMSETNAME ##_PARAM_SET_ID,                                 \
    paramsetname ##_end_of_paramids,                              \
    {                                                              \
      NULL                                                         \
    }                                                              \
}


  
/**
 * mpl_log_fp
 * 
 * Description: Log function
 *
 * @user_p   User pointer
 * @level:   Log (level (see syslog.h)
 * @format:  Log format
 *
 * Return -
 *
 */
typedef void (*mpl_log_fp)(void* user_p, int level, const char* file, int line, const char *format, ...);

/**
 * mpl_param_element_id_t
 *
 * Parameter id
 *
 */
typedef int mpl_param_element_id_t;


/**
 * mpl_param_element_t
 *
 * Listable parameter element
 *
 */
typedef struct
{
  mpl_param_element_id_t id;
  void*                  value_p;
  mpl_list_t             list_entry;
} mpl_param_element_t;


/**
 * mpl_uint8_array_t
 *
 * Parameter type for array of uint8
 *
 */
typedef struct
{
  int len;
  uint8_t *arr_p;
} mpl_uint8_array_t;


/**
 * mpl_string_tuple_t
 *
 * Parameter type for tuple of key/value strings
 *
 */
typedef struct
{
  char *key_p;
  char *value_p;
} mpl_string_tuple_t;


/**
 * mpl_arg_t - array of key/value pairs
 *
 * @key_p     pointing to key string
 * @value_p   pointing to value string
 *
 **/
typedef mpl_string_tuple_t mpl_arg_t;


/**
 * mpl_pack_param_fp
 * 
 * Description: Pack function (method) for a specific parameter
 *
 * Parameters:
 *     param_value_p:     Pointer to the parameter value
 *     buf:               Buffer to pack into
 *     buflen:            Length of buffer
 *     stringarr:         Array of strings for value naming
 *     stringarr_size:    Length of array
 *
 * Return Values : Number of bytes written to buf (or the number
 *                 that would have been written if buflen is too
 *                 small). Returns negative value on error.
 *
 */
typedef int (*mpl_pack_param_fp)(void* param_value_p, char *buf, size_t buflen, const char **stringarr, int stringarr_size);

/**
 * mpl_unpack_param_fp
 * 
 * Description: Unpack function (method) for a specific parameter
 *
 * Parameters:
 *     value_str:         String containing the value (to unpack from)
 *     value_pp:          Pointing to the variable where the value is unpacked into
 *     stringarr:         Array of strings for value naming
 *     stringarr_size:    Length of array
 *     max_p              Pointer to max value or stringlen (NULL if no max)
 *
 * Return Values : 0 on success, -1 on error.
 *
 */
typedef int (*mpl_unpack_param_fp)(const char* value_str, void **value_pp,
                                       const char **stringarr, int stringarr_size,
                                       const uint64_t *max_p);

/**
 * mpl_clone_param_fp
 * 
 * Description: Clone function (method) for a specific parameter
 *
 * Parameters:
 *     new_value_pp:      The parameter-copy is returned here
 *     old_value_p:       Pointer to the parameter to be cloned
 *
 * Return Values : 0 on success, -1 on error.
 *
 */
typedef int (*mpl_clone_param_fp)(void **new_value_pp, const void* old_value_p);


/**
 * mpl_compare_param_fp
 * 
 * Description: Compare function (method) for a specific parameter
 *
 * Parameters:
 *     value1_p:      Parameter 1
 *     value2_p:      Parameter 2
 *
 * Return Values : 0 on success (equal), -1 on error.
 *
 */
typedef int (*mpl_compare_param_fp)(const void *value1_p, const void* value2_p);


/**
 * mpl_free_param_fp
 * 
 * Description: Free function (method) for a specific parameter
 *
 * Parameters:
 *     value_p:      Parameter to be freed
 *
 * Return Values : -
 *
 */
typedef void (*mpl_free_param_fp)(void *value_p);


/**
 * mpl_param_descr_t - decription of a specific parameter
 *
 * @name              name of parameter
 * @allow_set         Is it allowed to set this parameter
 * @allow_get         Is it allowed to get this parameter
 * @allow_config      Is it allowed to get this parameter from config file
 * @default_value_p   Pointer to default value (NULL if no default)
 * @max_p             Pointer to max value or stringlen (NULL if no max)
 * @pack_func         pack method
 * @unpack_func       unpack method 
 * @clone_func        clone method
 * @compare_func      compare method
 * @stringarr         array of valid strings-names of parameter
 * @stringarr_size    size of array
 *
 **/
typedef struct
{
  char *name;
  bool allow_set;
  bool allow_get;
  bool allow_config;
  const void *default_value_p;
  const uint64_t *max_p;
  mpl_pack_param_fp pack_func;
  mpl_unpack_param_fp unpack_func;
  mpl_clone_param_fp clone_func;
  mpl_compare_param_fp compare_func;
  mpl_free_param_fp free_func;
  const char **stringarr;
  int stringarr_size;
} mpl_param_descr_t;

/**
 * mpl_param_descr_set_t - a set of parameter decriptors
 *
 * @array              array of parameter decriptors
 * @size               size of array
 *
 **/
typedef struct 
{
  const mpl_param_descr_t *array;
  char paramid_prefix[MPL_PARAMID_PREFIX_MAXLEN + 1];
  int param_set_id;
  int paramid_end;
  mpl_list_t list_entry;
} mpl_param_descr_set_t;


/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/
/**
 * mpl_param_init - Initiate library
 *
 * @user_p               User pointer
 * @log_fp               Logging function
 * @param_descr_set_p    pointer to parameter descriptor set struct
 *
 * Returns:  0 on success, -1 on failure
 *
 **/
  int mpl_param_init(void* user_p, mpl_log_fp log_fp, mpl_param_descr_set_t *param_descr_set_p);

/**
 * mpl_param_pack - pack a parameter to be sent to psccd (PS Connection
 *                          Control Daemon)
 *
 * @element_p The parameter element (ID + value) to pack
 * @buf     buffer to store packed message
 * @buflen  length of the buffer
 *          The function does not write more than buflen bytes to the buffer
 *          (including '\0').
 *          If more would have been written, it is truncated.
 * Returns:  Bytes written to buf (excluding '\0') on success (-1) on error
 *           If the output was truncated due 'buflen' then the return value is 
 *           the number of characters which would have been written to the final 
 *           string if enough space had been available
 *
 **/
#define mpl_param_pack(element_p, buf, buflen) mpl_param_pack_internal((element_p),(buf),(buflen),false)

#define mpl_param_pack_no_prefix(element_p, buf, buflen) mpl_param_pack_internal((element_p),(buf),(buflen),true)

int
mpl_param_pack_internal(const mpl_param_element_t* element_p,
                        char *buf, 
                        size_t buflen,
                        bool no_prefix);

/**
 * mpl_param_unpack - unpack received parameter strings
 *
 * @key_str        Parameter set and name (null terminated string)
 * @value_str      Parameter value (null terminated string)
 *                 NULL means no value (i.e. element value
 *                 is set to NULL)
 * @element_pp     Returned unpacked parameter element (ID+value)
 *                 This is allocated by the function.
 *
 * Returns: (0) on success, (-1) on error
 *
 * NOTE: '*element_pp' needs to be freed with
 *       mpl_param_element_destroy()
 *
 **/
#define mpl_param_unpack(id_str, value_str, element_pp) \
  mpl_param_unpack_internal((id_str), (value_str), (element_pp), -1)
  
/**
 * mpl_param_unpack_param_set - unpack received parameter strings
 *                              falling back to default parameter set
 *                              if not indicated by key string
 *
 * @key_str        Parameter set and name (null terminated string)
 * @value_str      Parameter value (null terminated string)
 *                 NULL means no value (i.e. element value
 *                 is set to NULL)
 * @element_pp     Returned unpacked parameter element (ID+value)
 *                 This is allocated by the function.
 * @param_set_id   default parameter set id (fallback)
 *
 * Returns: (0) on success, (-1) on error
 *
 * NOTE: '*element_pp' needs to be freed with
 *       mpl_param_element_destroy()
 *
 **/
#define mpl_param_unpack_param_set(id_str, value_str, element_pp, param_set_id) \
  mpl_param_unpack_internal((id_str), (value_str), (element_pp), (param_set_id))
  
int
mpl_param_unpack_internal(const char* id_str, 
			      const char* value_str,
			      mpl_param_element_t** element_pp,
			      int param_set_id);

/**
 * mpl_param_id_get_string
 * 
 * Description: Get string corresponding to the parameter ID
 *
 * Parameters:
 *     param_id:       Param ID
 *
 * Return Values : String (statically allocated, overwritten by each call)
 *
 */
const char *
mpl_param_id_get_string(mpl_param_element_id_t param_id);

/**
 * mpl_param_value_get_string
 * 
 * Description: Get string corresponding to the parameter value
 *
 * Parameters:
 *     param_id:       Param ID
 *     value_p:        Param value
 *
 * Return Values : String (statically allocated, overwritten by each call)
 *
 */
const char *
mpl_param_value_get_string(mpl_param_element_id_t param_id,
                               void* value_p);


/**
 * mpl_param_allow_get
 * 
 * Description: Check if the parameter is allowed to be fetched
 *              with the 'get' command
 *
 * Parameters:
 *     param_id:     Param ID
 *
 * Return Values : true/false
 *
 */
bool
mpl_param_allow_get(mpl_param_element_id_t param_id);

/**
 * mpl_param_allow_set
 * 
 * Description: Check if the parameter is allowed to be fetched
 *              with the 'set' command
 *
 * Parameters:
 *     param_id:     Param ID
 *
 * Return Values : true/false
 *
 */
bool
mpl_param_allow_set(mpl_param_element_id_t param_id);

/**
 * mpl_param_allow_config
 * 
 * Description: Check if it is allowed to get this parameter from config file
 *
 * Parameters:
 *     param_id:     Param ID
 *
 * Return Values : true/false
 *
 */
bool
mpl_param_allow_config(mpl_param_element_id_t param_id);

/**
 * mpl_param_element_create_stringn
 * 
 * Description: Allocate and fill in a parameter element with
 *              string type value
 *
 * Parameters:
 *     param_id:     Param ID
 *     value:        Value
 *     len           string length
 *
 * Return Values : Parameter element or NULL
 *
 * NOTE: The returned parameter element needs to be freed with
 *       mpl_param_element_destroy()
 *
 */
mpl_param_element_t*
mpl_param_element_create_stringn(mpl_param_element_id_t param_id,
                                     const char* value, size_t len);

/**
 * mpl_param_element_create_empty
 * 
 * Description: Allocate and fill in a parameter element
 *              without any value.
 *
 * Parameters:
 *     param_id:     Param ID
 *
 * Return Values : Parameter element or NULL
 *
 * NOTE: The returned parameter element needs to be freed with
 *       mpl_param_element_destroy()
 *
 */
mpl_param_element_t*
mpl_param_element_create_empty(mpl_param_element_id_t param_id);

/**
 * mpl_param_element_create
 * 
 * Description: Allocate and fill in a parameter element
 *              including the value
 *
 * Parameters:
 *     param_id:     Param ID
 *     value_p:      Param value
 *
 * Return Values : Parameter element or NULL
 *
 * NOTE: The returned parameter element needs to be freed with
 *       mpl_param_element_destroy()
 *
 */
mpl_param_element_t*
mpl_param_element_create(mpl_param_element_id_t param_id,
                             const void* value_p);

/**
 * mpl_param_element_clone
 * 
 * Description: Creates (and allocates) a clone of the param element
 *
 * Parameters:
 *     @element_p:     Paramameter element to clone
 *
 * Return Values : Parameter element or NULL
 *
 * NOTE: The returned parameter element needs to be freed with
 *       mpl_param_element_destroy()
 *
 */
mpl_param_element_t*
mpl_param_element_clone(const mpl_param_element_t* element_p);
  
/**
 * mpl_param_element_compare
 * 
 * Description: Compares two param elements
 *
 * Parameters:
 *     @element1_p:     Paramameter element 1
 *     @element2_p:     Paramameter element 2
 *
 * Return Values : 0 if equal, -1 if not
 *
 *
 */
int
mpl_param_element_compare(const mpl_param_element_t* element1_p,
                          const mpl_param_element_t* element2_p);
  
/**
 * mpl_param_element_get_default
 * 
 * Description: Get param element with default value
 *
 * Parameters:
 *     param_id:     Param ID
 *
 * Return Values : Parameter element or NULL
 *
 * NOTE: The returned parameter element needs to be freed with
 *       mpl_param_element_destroy()
 *
 */
mpl_param_element_t*
mpl_param_element_get_default(mpl_param_element_id_t param_id);

/**
 * mpl_param_element_destroy
 * 
 * Description: Free parameter element (including value)
 *
 * Parameters:
 *     element_p:     Paramameter element to destroy
 *
 * Return Values : -
 *
 */
void
mpl_param_element_destroy(mpl_param_element_t* element_p);
  
/**
 * mpl_param_list_destroy
 * 
 * Description: Destroy all parameters elements in list
 *
 * Parameters:
 *     param_list_p: List of parameters
 *
 * Return Values : -
 *
 */
void
mpl_param_list_destroy( mpl_list_t **param_list_pp );

/**
 * mpl_param_list_find
 * 
 * Description: Find a parameter element identified by param_id in list
 *
 * Parameters:
 *     param_id:     Param ID to search for
 *     param_list_p: List of parameters
 *
 * Return Values : Found matching parameter element or NULL
 *
 */
mpl_param_element_t*
mpl_param_list_find( mpl_param_element_id_t param_id, mpl_list_t *param_list_p );

/**
 * mpl_param_list_find_next
 * 
 * Description: Find the next parameter element identified by param_id in list
 *
 * Parameters:
 *     param_id:     Param ID to search for
 *     elem:         Current element
 *
 * Return Values : Found matching parameter element or NULL
 *
 */
#define mpl_param_list_find_next(param_id,elem) mpl_param_list_find((param_id),((elem)==NULL?NULL:(elem)->list_entry.next_p))


/**
 * mpl_param_list_tuple_key_find
 * 
 * Description: Find a parameter element identified by param_id and tuple key in list
 *              Note that the parameter must be of type 'string_tuple'
 *
 * Parameters:
 *     param_id:     Param ID to search for
 *     key_p:        Key to search for
 *     param_list_p: List of parameters
 *
 * Return Values : Found matching parameter element or NULL
 *
 */
mpl_param_element_t*
  mpl_param_list_tuple_key_find( mpl_param_element_id_t param_id, char *key_p, mpl_list_t *param_list_p );
  
  
/**
 * mpl_param_list_tuple_key_find_wildcard
 * 
 * Description: Find a parameter element identified by param_id and tuple key in list, but with wildcard fallback.
 *              Note that the parameter must be of type 'string_tuple'
 *
 * Parameters:
 *     param_id:     Param ID to search for
 *     key_p:        Key to search for
 *     wildcard_p:   Wildcard to search for
 *     param_list_p: List of parameters
 *
 * Return Values : Found matching parameter element or NULL
 *
 */
mpl_param_element_t*
  mpl_param_list_tuple_key_find_wildcard( mpl_param_element_id_t param_id, char *key_p, char *wildcard_p, mpl_list_t *param_list_p );
  
  
/**
 * mpl_pack_param_value_*
 * 
 * Description: Encode (pack) a value of a specific type from local format to "on the wire" format
 *
 * See mpl_pack_param_fp for more details.
 *
 */
int mpl_pack_param_value_string(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size);
int mpl_pack_param_value_int(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size);
int mpl_pack_param_value_uint8(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size);
int mpl_pack_param_value_uint32(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size);
int mpl_pack_param_value_enum(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size);
int mpl_pack_param_value_bool(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size);
int mpl_pack_param_value_uint8_array(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size);
int mpl_pack_param_value_string_tuple(void* param_value_p, char *buf, size_t buflen, const char** stringarr, int stringarr_size);

/**
 * mpl_unpack_param_value_*
 * 
 * Description: Decode (unpack) a value of a specific type from "on the wire" format to local format
 *
 * See mpl_unpack_param_fp for more details.
 *
 */
int mpl_unpack_param_value_string(const char* value_str, void **value_pp,
                                      const char** stringarr, int stringarr_size,
                                      const uint64_t *max_p);
int mpl_unpack_param_value_int(const char* value_str, void **value_pp,
                                   const char** stringarr, int stringarr_size,
                                   const uint64_t *max_p);
int mpl_unpack_param_value_uint8(const char* value_str, void **value_pp,
                                     const char** stringarr, int stringarr_size,
                                     const uint64_t *max_p);
int mpl_unpack_param_value_uint32(const char* value_str, void **value_pp,
                                      const char** stringarr, int stringarr_size,
                                      const uint64_t *max_p);
int mpl_unpack_param_value_enum(const char* value_str, void **value_pp,
                                    const char** stringarr, int stringarr_size,
                                    const uint64_t *max_p);
int mpl_unpack_param_value_bool(const char* value_str, void **value_pp,
                                    const char** stringarr, int stringarr_size,
                                    const uint64_t *max_p);
int mpl_unpack_param_value_uint8_array(const char* value_str, void **value_pp,
                                       const char** stringarr, int stringarr_size,
                                       const uint64_t *max_p);
int mpl_unpack_param_value_string_tuple(const char* value_str, void **value_pp,
                                        const char** stringarr, int stringarr_size,
                                        const uint64_t *max_p);

/**
 * mpl_clone_param_value_*
 * 
 * Description: Clone a parameter of a specific type
 *
 * See mpl_clone_param_fp for more details.
 *
 */
int mpl_clone_param_value_string(void **new_value_pp, const void* old_value_p);
int mpl_clone_param_value_int(void **new_value_pp, const void* old_value_p);
int mpl_clone_param_value_uint8(void **new_value_pp, const void* old_value_p);
int mpl_clone_param_value_uint32(void **new_value_pp, const void* old_value_p);
#define mpl_clone_param_value_enum mpl_clone_param_value_int
int mpl_clone_param_value_bool(void **new_value_pp, const void* old_value_p);
int mpl_clone_param_value_uint8_array(void **new_value_pp, const void* old_value_p);
int mpl_clone_param_value_string_tuple(void **new_value_pp, const void* old_value_p);

/**
 * mpl_compare_param_value_*
 * 
 * Description: Compare parameters of a specific type
 *
 * See mpl_compare_param_fp for more details.
 *
 */
int mpl_compare_param_value_string(const void *value1_p, const void* value2_p);
int mpl_compare_param_value_int(const void *value1_p, const void* value2_p);
int mpl_compare_param_value_uint8(const void *value1_p, const void* value2_p);
int mpl_compare_param_value_uint32(const void *value1_p, const void* value2_p);
#define mpl_compare_param_value_enum mpl_compare_param_value_int
int mpl_compare_param_value_bool(const void *value1_p, const void* value2_p);
int mpl_compare_param_value_uint8_array(const void *value1_p, const void* value2_p);
int mpl_compare_param_value_string_tuple(const void *value1_p, const void* value2_p);

/**
 * mpl_free_param_value_*
 * 
 * Description: Free parameters of a specific type
 *
 * See mpl_free_param_fp for more details.
 *
 */
#define mpl_free_param_value_string free
#define mpl_free_param_value_int free
#define mpl_free_param_value_uint8 free
#define mpl_free_param_value_uint32 free
#define mpl_free_param_value_enum free
#define mpl_free_param_value_bool free
void mpl_free_param_value_uint8_array(void *value_p);
void mpl_free_param_value_string_tuple(void *value_p);

/**
 * mpl_get_args
 * 
 * Description: Split argument-buffer into array of key/value pairs
 *
 * Parameters:
 *     args:         Array of key/value pairs
 *     args_len:     Length of array
 *     buf:          Null-terminated buffer containing the original arguments
 *                   Note that the buffer is changed.
 *     equal:        Character used as "equal sign" between key and value (argument format could be key=value)
 *     delimiter:    Character used as delimiter between arguments
 *     escape:       Character used as escape
 *
 * Return Values : Number of key/value pairs found or -1 on error.
 *
 */
int mpl_get_args(mpl_arg_t *args, int args_len, char *buf, char equal, char delimiter, char escape);


/**
 * mpl_add_param_to_list - add parameter to param list
 *
 */
int mpl_add_param_to_list(mpl_list_t **param_list_pp,
			      mpl_param_element_id_t param_id,
			      const void *value_p);


#define MPL_PARAM_PRESENT_IN_LIST(paramid,param_list_p) (NULL != mpl_param_list_find((paramid), (param_list_p)))
#define MPL_GET_VALUE_FROM_PARAM_ELEMENT(type,param_element_p) *((type*)(param_element_p)->value_p)
#define MPL_GET_VALUE_REF_FROM_PARAM_ELEMENT(reftype,param_element_p) (reftype)(param_element_p)->value_p
#define MPL_GET_PARAM_VALUE_FROM_LIST(type,paramid,param_list_p) \
  MPL_GET_VALUE_FROM_PARAM_ELEMENT(type, mpl_param_list_find((paramid), (param_list_p)))
#define MPL_GET_PARAM_VALUE_REF_FROM_LIST(reftype,paramid,param_list_p) \
  MPL_GET_VALUE_REF_FROM_PARAM_ELEMENT(reftype, mpl_param_list_find((paramid), (param_list_p)))

/**
 * mpl_trimstring
 *
 * Description: Takes away whitespace in front and rear, unless last whitespace is
 * escaped.
 *
 * Returns pointer to where non-whitespace string begins.
 */
char *mpl_trimstring(char *str, char escape);

/**
 * ARRAY_SIZE
 *
 * Description: Calculate the number of elements in an array
 *
 * Parameters:
 *     arr:      The array
 *
 * Return Values : Number of elements
 *
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))


/**
 * MPL_IDENTIFIER_NOT_USED
 *
 * Description: Report to compiler that an input parameter is not used by function
 *
 * Parameters:
 *     P: The parameter
 *
 * Return Values : void
 *
 */
#define MPL_IDENTIFIER_NOT_USED(P) (void)(P);


#ifdef  __cplusplus
}
#endif

#endif /* MPL_PARAM_H */
