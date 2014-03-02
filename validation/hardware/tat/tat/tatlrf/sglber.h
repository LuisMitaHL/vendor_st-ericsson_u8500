/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   sglber.h
* \brief   routines common to Single Ended Bit Error Rate test interfaces
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef SGLBER_H_
#define SGLBER_H_


#ifdef STUB_SINGLE_BER
/* variables to inspect when testing single BER */
extern u16 v_tatrf_sber_stub_dut_state;
extern int v_tatrf_sber_stub_get_status;
#endif

float tatl20_03divide(float num, float div, float if_zero);
int tatl20_01start(u8 *pp_status);
int tatl20_02stop(u8 *pp_status);
int tatl20_04clear(u8 *pp_status);
int tatl20_05refreshStatus(u8 *pp_status);
int tatl20_06pageNumber(struct dth_element *pp_elem);

#ifdef STUB_SINGLE_BER
u16 tatl20_07getDutState();
int tatl20_08getStatus();
void tatl20_09setDutState(u16 vp_state);
#endif

#endif /* SGLBER_H_ */
