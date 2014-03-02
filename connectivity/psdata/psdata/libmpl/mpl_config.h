/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  Message (m) Parameter (p) List (l) library
 */

#ifndef MPL_CONFIG_H
#define MPL_CONFIG_H


/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/

#include "mpl_list.h"
#include "mpl_param.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

typedef mpl_list_t *mpl_config_t;

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/
/**
 * mpl_config init - Initiate library
 *
 * @user_p          User pointer
 * @log_fp          Logging function
 * @config_p        pointer to configuration
 * @param_set_id    id of parameter set used in config file
 *
 * Returns:  0 on success, -1 on failure
 *
 **/
int mpl_config_init(void* user_p, mpl_log_fp log_fp, mpl_config_t *config_p, int param_set_id);


/**
 * mpl_config_read_config - read config parameter from given config file
 *
 * @config_path         path to the config file.
 * @config_p            pointer to configuration
 * @param_set_id        parameter set id
 *
 * Returns:             ( 0) on success
 *                      (-1) on open file error
 *                      (-2) on error when read parameter value in the file
 *
 **/
int mpl_config_read_config(char* config_path,
                           mpl_config_t *config_p,
                           int param_set_id);



/**
 * mpl_config_get_para - get a parameter element for the given parameter id
 *
 * Description:         For parameter that is not found in the list, a default value
 *                      will be used
 *                      For parameter id "connid", "result" and "cause", NULL will be
 *                      returned.
 *
 * @param_id            parameter id to find
 * @config_p            pointer to configuration
 *
 * Return Values :      pointer to parameter element or NULL when not find
 *
 **/
mpl_param_element_t* mpl_config_get_para(mpl_param_element_id_t param_id,
                                         mpl_config_t *config_p);


/**
 * mpl_config_tuple_key_get_para - get a parameter element for the given parameter id using
 *                                 a given key or wildcard (fallback/default)
 *
 * Description:         For parameter that is not found in the list, a default value
 *                      will be used
 *                      For parameter id "connid", "result" and "cause", NULL will be
 *                      returned.
 *
 * @param_id            parameter id to find
 * @key_p               key string to search for
 * @wildcard_p          wildcard string to search for if key not found
 * @config_p            pointer to configuration
 *
 * Return Values :      pointer to parameter element or NULL when not find
 *
 **/
mpl_param_element_t* mpl_config_tuple_key_get_para(mpl_param_element_id_t param_id,
                                                   char *key_p, char *wildcard_p, mpl_config_t *config_p);


/**
 * mpl_config_reset - destory the parameter list
 *
 * @config_p            pointer to configuration
 *
 **/
void mpl_config_reset(mpl_config_t *config_p);
#endif
