/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file
* \brief   DTH filesystem
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTH_PORTING_H_
#define DTH_PORTING_H_

void *dth_malloc(unsigned long size);
void dth_free(void *ptr);
unsigned long dth_strlen(const char *);
int dth_strncmp(const char *, const char *, unsigned long);
unsigned long dth_get_time(void);

#endif /*DTH_PORTING_H_*/
