/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage audio via alsa framework
* \author  ST-Ericsson
*/
/*****************************************************************************/


#ifndef DTHDB_H_
#define DTHDB_H_

#include <dthsrvhelper/dthsrvhelper.h> /* for dth_element definition */

int dth_init_service();
int DthDB_exec(struct dth_element *elem);
int DthDB_set(struct dth_element *elem, void *value);
int DthDB_get(struct dth_element *elem, void *value);
int get_version(struct dth_element *elem, void *value);
struct dth_element * dth_element_alloc2(int nb_element);
int count_elements2(char * filename);

#endif /* DTHDB_H_ */
