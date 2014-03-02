/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*! \file ste_adm_db.h
*   \brief Exports database connection functionality.
*
*   Copyright (C) ST-Ericsson AB 2010
*
*   This file exports functions to connect and disconnect to/from the ADM
*   database.
*
*/

#ifndef STE_ADM_DB_H_
#define STE_ADM_DB_H_

#include "ste_adm_client.h" // ste_adm_res_t

#ifdef __cplusplus
extern "C" {
#endif

#include <sqlite3.h>

/* Global Settings in db */
#define ADM_DB_GLOBAL_SETTING_VC_SAMPLERATE "VoiceCallSampleRate"
#define ADM_DB_GLOBAL_SETTING_LOOPBACK_DELAY  "LoopbackDelay"

#include "ste_adm_db_remap_check.h"

// TODO: teststuff, move elsewhere later
ste_adm_res_t dbg_dumpdev(const char* dev);

    // TODO: Export properly
ste_adm_res_t adm_db_exec_sql(const char* query);


ste_adm_res_t adm_db_init();


/**
* Disconnect from the ADM database and release all resources associated with db.
*/
ste_adm_res_t adm_db_disconnect();

/**
* Return the SQLite database handle
*
*/
int adm_db_get_handle(sqlite3** db_hp);



typedef struct
{
  // Content not part of interface definition
  int magic;
  int end_reached; // 0 if sqlite_step has not yet returned SQLITE_DONE. sqlite_step may only return DONE once.
} adm_db_comp_iter_t;

typedef struct
{
  // Content not part of interface definition
  int          magic;
  int          end_reached; // 0 if sqlite_step has not yet returned SQLITE_DONE. sqlite_step may only return DONE once.
  char*        cfg_buf;
  unsigned int cfg_bufsz;
} adm_db_cfg_iter_t;


/**
* Interface to determine direction of device (input or output)
*/
ste_adm_res_t adm_db_io_info(const char* dev,
                   int* is_input);

/**
* Interface to get extarnal device name from internal or external name
*/
ste_adm_res_t adm_db_extname(char* dev);


/**
* Interface to retrieve the information needed to create an app, vc or common
* effect chain from the database.
* Prereq: The database must be open - adm_db_init() called.
*
*  dev     [IN] Name of the top-level device, e.g. 'Phy/Speaker'
*  dev_2nd [IN] Name of 2nd top-level device. This is applicable for VC effect
*               chain which is indexed by a pair of in/out devices.
*  type    [IN] Which chain to retrieve components for: app, vc or common
*  samplerate [IN] Samplerate in which the chain should be setup
*  iter   [OUT] Iterator for the specified component chain.
*
*  Returns: < 0 failure, iter is not valid
*           ADM_RES_OK   success, iter is valid. When the caller is finished
*               with the iterator, adm_db_comp_iter_destroy must be called.
*
*  Related: adm_db_comp_iter_get_next
*/
ste_adm_res_t adm_db_comp_iter_create(const char* dev, const char* dev_2nd,
                                ste_adm_effect_chain_type_t type,
                                int samplerate,
                                adm_db_comp_iter_t* iter);

/**
*  Retrieve information about the next IL component in the chain. For each component, the
*  IL name (to be used with OMX_GetHandle) and an iterator to the configuration data (to be
*  sent down with OMX_SetConfig and OMX_SetParameter) will be provided. The components will
*  be returned in order of increasing position, i.e. in execution order. It is possible for
*  a chain to be empty.
*
*  In case of an input device, the first returned IL component in the COMMON chain will
*  be the source component (one IL port only; the output). Likewise, for the COMMON chain
*  for an output device, the last returned IL component will be the sink component.
*
*  iter     [IN]  The current component iterator
*  il_name  [OUT] Zero-terminated string with the IL name, for use with OMX_GetHandle.
*                 The memory is owned by adm_db_comp_iter_get_next() and valid until
*                 the next call to adm_db_comp_iter_get_next() or until
*                 adm_db_comp_iter_destroy is called.
*  cfg_iter [OUT] An iterator for the component configuration, to be used with
*                 adm_db_cfg_iter_get_next.
*                 Allocated by adm_db_cfg_iter_destroy and ownership is
*                 transfered to caller that must call free() when no longer needed.
*
*  Returns: < 0 failure, iter is not valid
*           ADM_RES_OK         Success. il_name and cfg_iter are valid.
*            ADM_RES_NO_MORE   No more components were found, not a failure. No outparams valid.
*/
ste_adm_res_t adm_db_comp_iter_get_next(adm_db_comp_iter_t*   iter,
                                    const char**          il_name,
                                      adm_db_cfg_iter_t*    cfg_iter);

/**
* Release resources associated with the provided component iterator
*/
void adm_db_comp_iter_destroy(adm_db_comp_iter_t* iter);

/**
* Returns the next configuration entry for the specified component.
*
*  iter     [IN]     The current configuration iterator
*  is_param [OUT]    set to 1 if use with OMX_SetParameter is intended,
*                    set to 0 if use with  MX_SetConfig is intended.
*  index_name [OUT]  Zero-terminated string to pass to OMX_GetExtensionIndex
*                    to get the index of the configuration. The index is the
*                    second argument to OMX_SetParameter / OMX_SetConfig.
*                    If the string starts with #, the index number of
*                    decimal follows directly, e.g. #3232445.
*                    The memory is owned by adm_db_comp_iter_get_next() and valid until
*                    the next call to adm_db_comp_iter_get_next() or until
*                    adm_db_comp_iter_destroy is called.
*  config_data [OUT] The configuration data to pass as third argument to
*                    OMX_SetConfig / OMX_SetParameter.
*                    The memory is owned by adm_db_comp_iter_get_next() and valid until
*                    the next call to adm_db_comp_iter_get_next() or until
*                    adm_db_comp_iter_destroy is called.
*
*  Returns: < 0 failure
*           ADM_RES_OK       Success. is_param, index_name and config_data are valid.
*            ADM_RES_NO_MORE No more configurations found, not a failure. No outparams valid.
*/
ste_adm_res_t adm_db_cfg_iter_get_next(adm_db_cfg_iter_t*    iter,
                                  int*                   is_param,
                                  const char**           index_name,
                                  const void**           config_data);

/**
* Returns the config iterator for the specified speech settings.
*
*  dev_in     [IN]   Toplevel Input device to match configuration data with
*  dev_out    [IN]   Toplevel Output device to match configuration data with
*  samplerate [IN]   Speech sample rate
*  iter       [OUT]  Iterator to be used with adm_db_cfg_iter_get_next(...).

*  Returns: < 0 failure
*           ADM_RES_OK       Success. Iterator can be passed to adm_db_cfg_iter_get_next.
*/
ste_adm_res_t adm_db_speech_cfg_iter_create(const char* dev_in,
                                        const char* dev_out,
                                        int samplerate,
                                        adm_db_cfg_iter_t* iter);

/**
* Returns the config iterator for the specified speech settings.
*
*  dev_in     [IN]   Toplevel Device to match configuration data with
*  name       [OUT]  String containing component name, must be free'd by caller
*  iter       [OUT]  Iterator to be used with adm_db_cfg_iter_get_next(...).

*  Returns: < 0 failure
*           ADM_RES_OK       Success. Iterator can be passed to adm_db_cfg_iter_get_next.
*/
ste_adm_res_t adm_db_io_cfg_iter_create(const char* dev,
                                    char **name,
                                    adm_db_cfg_iter_t* iter);

/**
* Returns the pre effect component name and config iterator for the
* the specified top level device name.
*
*  dev_in     [IN]   Toplevel Device to match configuration data with
*  name       [OUT]  String containing component name, must be free'd by caller
*  iter       [OUT]  Iterator to be used with adm_db_cfg_iter_get_next(...).
*
*  Returns:
*  < 0 failure
*  ADM_RES_OK  Success. Iterator can be passed to adm_db_cfg_iter_get_next.
*/
ste_adm_res_t adm_db_pre_effect_cfg_iter_create(const char* dev,
                                            char **name,
                                            adm_db_cfg_iter_t* iter);


/**
* Release the configuration iterator returned by adm_db_comp_iter_get_next()
*/
void adm_db_cfg_iter_destroy(adm_db_cfg_iter_t* iter);

/**
* Interface to get a global setting from the db
*
*  name     [IN]    Global settings name
*  data     [OUT]   Global settings data
*
*  Returns: ADM_RES_OK       Success.
*           ADM_RES_DB_NODEV Failure, setting doesn't exist in the database.
*           ADM_RES_FATAL    All other errors.
*/
ste_adm_res_t adm_db_global_settings_get(const char* name, const char** data);

/**
* Maps a toplevel device name to an actual device
*
*  toplevel_dev  [IN]   Toplevel device
*  actual_dev    [IN]   Actual device
*
*  Returns: ADM_RES_OK       Success. Iterator can be passed to adm_db_cfg_iter_get_next.
*           ADM_RES_DB_NODEV Failure, either the toplevel_dev or the actual_dev don't exist in the database.
*           ADM_RES_FATAL    All other errors.
*/
ste_adm_res_t adm_db_toplevel_mapping_set(  const char* toplevel_dev,
                                        const char* actual_dev);

/**
* Maps a toplevel device name to an actual device
*
*  toplevel_dev  [IN]   Toplevel device
*  actual_dev    [OUT]   Actual device
*
*  Returns: ADM_RES_OK       Success. Iterator can be passed to adm_db_cfg_iter_get_next.
*           ADM_RES_DB_NODEV Failure, the toplevel_dev don't exist in the database.
*           ADM_RES_FATAL    All other errors.
*/
ste_adm_res_t adm_db_toplevel_mapping_get(const char* toplevel_dev,
                                          const char** actual_dev);

/**
* Get the toplevel device associated with actual device
*
*  actual_dev   [IN]   Actual device
*  toplevel_dev [OUT]  Toplevel device
*
*  Returns: ADM_RES_OK       Success. Iterator can be passed to adm_db_cfg_iter_get_next.
*           ADM_RES_DB_NODEV Failure, the actual_dev don't exist in the database.
*           ADM_RES_FATAL    All other errors.
*/
ste_adm_res_t adm_db_toplevel_device_get(const char* actual_dev,
                                         const char** toplevel_dev);

/**
* Interface to get the external delay of a device
*
*  toplevel_dev     [IN]    Toplevel device, i.e. Name in DeviceMap table
*  external_delay   [OUT]   External delay for toplevel_dev
*
*  Returns: ADM_RES_OK       Success.
*           ADM_RES_DB_NODEV Failure, dev doesn't exist in the database.
*           ADM_RES_FATAL    All other errors.
*/
ste_adm_res_t adm_db_external_delay_get(const char* toplevel_dev,
                                    int* external_delay);


/**
* Interface to set the external delay of a device
*
*  dev              [IN]    Name in Device table
*  external_delay   [IN]    New external delay for dev to be set in database.
*
*  Returns: ADM_RES_OK       Success.
*           ADM_RES_DB_NODEV Failure, dev doesn't exist in the database.
*           ADM_RES_FATAL    All other errors.
*/
ste_adm_res_t adm_db_external_delay_set(const char* dev, int external_delay);

/**
* Interface to execute string as sql statement
*
*  sql              [IN]    Name in Device table

*
*  Returns: ADM_RES_OK       Success.
*           ADM_RES_FATAL    Failed to execute sql.
*/
ste_adm_res_t adm_db_execute_sql(const char *sql);

/**
* Returns the apf file for the specified speech settings.
*
*  dev_in       [IN]   Toplevel Input device to match configuration data with
*  dev_out      [IN]   Toplevel Output device to match configuration data with
*  samplerate   [IN]   Speech sample rate
*  apf_file     [OUT]  Name of the apf file associtaed to the speech seting.

*  Returns: < 0 failure
*           ADM_RES_OK       Success.
*/
ste_adm_res_t adm_db_speech_apf_get(const char* dev_in,
                                        const char* dev_out,
                                        int samplerate,
                                        const char** apf_file);

/**
* Interface to set tuning mode for db
*
*  enabled              [IN]    Mode to set
*
*  Returns: void
*/
void adm_db_set_tuning_mode(int enabled);

#ifdef __cplusplus
}
#endif

#endif /* STE_ADM_DB_H_ */
