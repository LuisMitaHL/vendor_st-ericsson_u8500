/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   tx3g.h
* \brief   routines to run WCDMA transceiver
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TX3G_H_
#define TX3G_H_

int tatl7_00StartTx3G_exec(struct dth_element *elem);
int tatl7_02StartTx3G_Parm_get(struct dth_element *elem, void *value);
int tatl7_03StartTx3G_Parm_set(struct dth_element *elem, void *value);
int tatl7_04StartTx3G_set(struct dth_element *elem, void *value);

#endif /* TX3G_H_ */
