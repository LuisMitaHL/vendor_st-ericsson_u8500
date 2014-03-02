/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   pmmacces.h
* \brief   routines to access Permanent Modem Memory
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef PMMACCES_H_
#define PMMACCES_H_

/**
* Launch tatl5_00Pmm_exec function.
* @param[in] elem specifies the DTH element.
* @param[in] value not used.
* @retval errorCode if an error occured while processing.
*/
int tatl5_01Pmm_GetAndResult(struct dth_element *elem, void *Value);

/**
* Set the type of memory access : reading or writing.
* @param[in] elem specifies the DTH element.
* @param[in] value specifies the the type of memory access : Read or Write.
* @retval 0 success.
* @retval errorCode if an error occured while processing.
*/
int tatl5_02Pmm_Set(struct dth_element *elem, void *Value);

/**
* Read a memory location.
* @param[in] elem specifies the DTH element.
* @param[in] value specifies the location where the data read must be stored.
* @retval 0 success.
* @retval errorCode if an error occured while processing.
*/
int tatl5_03Pmm_Read(struct dth_element *elem, void *Value, int q);

/**
* Write a memory location.
* @param[in] elem specifies the DTH element.
* @param[in] value specifies data to write in the memory location.
* @retval 0 success.
* @retval errorCode if an error occured while processing.
*/
int tatl5_04Pmm_Write(struct dth_element *elem, void *Value, int q);

#endif /* PMMACCES_H_ */

