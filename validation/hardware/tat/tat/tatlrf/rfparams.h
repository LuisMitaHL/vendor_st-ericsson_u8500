/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   rfparams.h
* \brief   routines to load DTH definition of RF parameters stored in an
*          external file.
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef PMMCSV_H_
#define PMMCSV_H_


/* Load an external parameters definition file and constructs the corresponding
* array of dth_element structs
* @return the count of parameters loaded or -1 if an error occurred.
*/
int load_rf_param_file(const char *filename);

/* gets the storage format of a PMM parameter */
int get_param_store_type(int param_id, int *store_type);

/* release all resources allocated for PMM data */
void free_rf_params(void);

#endif /* PMMCSV_H_ */

