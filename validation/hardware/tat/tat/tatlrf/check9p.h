/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   check9p.h
* \brief   This module provides API to manage ISI link and modem
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef CHECK9P_H_
#define CHECK9P_H_

int tatl4_03Check9pServer_exec(struct dth_element *elem);
int tatl4_04Check9pServer_get(struct dth_element *elem, void *Value);
int tatl4_05Check9pServer_Set(struct dth_element *elem, void *Value);

#endif /* CHECK9P_H_ */
