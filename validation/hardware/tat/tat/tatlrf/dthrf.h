/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   dthrf.h
* \brief   declare the DTH main interfaces for HATS RF
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef DTHRF_H_
#define DTHRF_H_

#include <dthsrvhelper/dthsrvhelper.h>	/* for dth_element definition */

/* DTH service standard procedures */
int dth_init_service();
void dth_uninit_service();

/* Function tide to PA INFO         */
/************************************/
int DthRf_PaInfo_get(struct dth_element *elem, void *value);
int DthRf_PaInfo_set(struct dth_element *elem, void *value);
int DthRf_PaInfo_exec(struct dth_element *elem);

/* Function used to check 9P Server */
/************************************/
int DthRf_Check9pServer_get(struct dth_element *elem, void *value);
int DthRf_Check9pServer_set(struct dth_element *elem, void *value);

/* Function tide to RF Rx 2G5        */
/*************************************/
int DthRf_StartRx2G5Parm_get(struct dth_element *elem, void *value);
int DthRf_StartRx2G5Parm_set(struct dth_element *elem, void *value);
int DthRf_StartRx2G5_exec(struct dth_element *elem);
int DthRf_StartRx2G5_set(struct dth_element *elem, void *value);

/* Function tide to RF Get Info Parameters */
/*******************************************/
int DthRf_GetRfInfoParam_get(struct dth_element *elem, void *value);
int DthRf_GetRfInfoParam_set(struct dth_element *elem, void *value);
int DthRf_GetRfInfo_exec(struct dth_element *elem);

/* Function tide to RF Tx 2G5           */
/*************************************/
int DthRf_StartTx2G5Parm_get(struct dth_element *elem, void *value);
int DthRf_StartTx2G5Parm_set(struct dth_element *elem, void *value);
int DthRf_StartTx2G5_exec(struct dth_element *elem);
int DthRf_StartTx2G5_set(struct dth_element *elem, void *value);

/* Function tide to PMM Parameters management */
/**********************************************/
int DthRf_PMM_Get(struct dth_element *elem, void *value);
int DthRf_PMM_Set(struct dth_element *elem, void *value);
int DthRf_Get_PMM_Parameter(struct dth_element *elem, void *value);
int DthRf_Get_PMM_Parameter_Q1(struct dth_element *elem, void *value);
int DthRf_Get_PMM_Parameter_Q2(struct dth_element *elem, void *value);
int DthRf_Get_PMM_Parameter_Q3(struct dth_element *elem, void *value);
int DthRf_Get_PMM_Parameter_Q6(struct dth_element *elem, void *value);
int DthRf_Get_PMM_Parameter_Q8(struct dth_element *elem, void *value);
int DthRf_Get_PMM_Parameter_Q10(struct dth_element *elem, void *value);
int DthRf_Get_PMM_Parameter_Q12(struct dth_element *elem, void *value);
int DthRf_Get_PMM_Parameter_Q14(struct dth_element *elem, void *value);
int DthRf_Get_PMM_Parameter_Q16(struct dth_element *elem, void *value);
int DthRf_Get_PMM_Parameter_Q23(struct dth_element *elem, void *value);
int DthRf_Set_PMM_Parameter(struct dth_element *elem, void *value);
int DthRf_Set_PMM_Parameter_Q1(struct dth_element *elem, void *value);
int DthRf_Set_PMM_Parameter_Q2(struct dth_element *elem, void *value);
int DthRf_Set_PMM_Parameter_Q3(struct dth_element *elem, void *value);
int DthRf_Set_PMM_Parameter_Q6(struct dth_element *elem, void *value);
int DthRf_Set_PMM_Parameter_Q8(struct dth_element *elem, void *value);
int DthRf_Set_PMM_Parameter_Q10(struct dth_element *elem, void *value);
int DthRf_Set_PMM_Parameter_Q12(struct dth_element *elem, void *value);
int DthRf_Set_PMM_Parameter_Q14(struct dth_element *elem, void *value);
int DthRf_Set_PMM_Parameter_Q16(struct dth_element *elem, void *value);
int DthRf_Set_PMM_Parameter_Q23(struct dth_element *elem, void *value);

/* Function tide to RF RX 3G          */
/*************************************/
int DthRf_StartRx3G_Parm_get(struct dth_element *elem, void *value);
int DthRf_StartRx3G_Parm_set(struct dth_element *elem, void *value);
int DthRf_StartRx3G_exec(struct dth_element *elem);
int DthRf_StartRx3G_set(struct dth_element *elem, void *value);

/* Function tide to RF TX 3G           */
/*************************************/
int DthRf_StartTx3G_Parm_get(struct dth_element *elem, void *value);
int DthRf_StartTx3G_Parm_set(struct dth_element *elem, void *value);
int DthRf_StartTx3G_exec(struct dth_element *elem);
int DthRf_StartTx3G_set(struct dth_element *elem, void *value);

/* Function tide to access to 2G5 and 3G registers */
/**************************************************/
int DthRf_Register_Get(struct dth_element *elem, void *value);
int DthRf_Register_Set(struct dth_element *elem, void *value);
int DthRf_Register_Exec(struct dth_element *elem);

/* Function tide to Start GSM Tx IQ                */
/**************************************************/
int DthRf_Gsm_Tx_Iq_Exec(struct dth_element *elem);
int DthRf_Gsm_Tx_Iq_Get(struct dth_element *elem, void *value);
int DthRf_Gsm_Tx_Iq_Set(struct dth_element *elem, void *value);

/* Function tide to Start WCDMA Tx IQ                */
/**************************************************/
int DthRf_Wcdma_Tx_Iq_Exec(struct dth_element *elem);
int DthRf_Wcdma_Tx_Iq_Get(struct dth_element *elem, void *value);
int DthRf_Wcdma_Tx_Iq_Set(struct dth_element *elem, void *value);

/* Function tide to Start Power Tx power level     */
/**************************************************/
int DthRf_Power_Tx_Get(struct dth_element *elem, void *value);
int DthRf_Power_Tx_Set(struct dth_element *elem, void *value);
int DthRf_Power_Tx_Exec(struct dth_element *elem);

/* Function tide to Start SelfTest                */
/**************************************************/
int DthRf_SelfTest_Get(struct dth_element *elem, void *value);
int DthRf_SelfTest_Set(struct dth_element *elem, void *value);
int DthRf_SelfTest_Exec(struct dth_element *elem);

/* Function tide to Start GSM PA                  */
/**************************************************/
int DthRf_Gsm_Pa_Exec(struct dth_element *elem);
int DthRf_Gsm_Pa_Get(struct dth_element *elem, void *value);
int DthRf_Gsm_Pa_Set(struct dth_element *elem, void *value);

#if !defined(NO_RF_TUNING)

/* Function tide to RF Tuning                     */
/**************************************************/
int DthRf_Tuning_Get(struct dth_element *elem, void *value);
int DthRf_Tuning_Set(struct dth_element *elem, void *value);
int DthRf_Tuning_Exec(struct dth_element *elem);

#endif /* !defined(NO_RF_TUNING) */

/* Function tide to RX/TX multi slots             */
/**************************************************/
int DthRf_RxTxMultiSlot_Get(struct dth_element *elem, void *value);
int DthRf_RxTxMultiSlot_Set(struct dth_element *elem, void *value);
int DthRf_RxTxMultiSlot_Exec(struct dth_element *elem);

/* Function tide to RX IQ sample reading */
/*********************************/
int DthRf_IQSample_exec(struct dth_element *elem);
int DthRf_IQSample_get(struct dth_element *elem, void *value);
int DthRf_IQSample_set(struct dth_element *elem, void *value);

#if !defined(NO_RF_SBER_2G)
/* Function tide to Single Ended BER 2G           */
/**************************************************/
int DthRf_SingleBer2G_get(struct dth_element *elem, void *value);
int DthRf_SingleBer2G_set(struct dth_element *elem, void *value);
int DthRf_SingleBer2G_exec(struct dth_element *elem);
#endif

#if !defined(NO_RF_SBER_3G)
/* Function tide to Single Ended BER 3G           */
/**************************************************/
int DthRf_SingleBer3G_get(struct dth_element *elem, void *value);
int DthRf_SingleBer3G_set(struct dth_element *elem, void *value);
int DthRf_SingleBer3G_exec(struct dth_element *elem);
#endif

#if !defined(NO_TX3G_POWERSWEEP)
/* Function tide to WCDMA TX Power Sweep           */
/**************************************************/
int DthRf_Tx3G_PowerSweep_Get(struct dth_element *elem, void *value);
int DthRf_Tx3G_PowerSweep_Set(struct dth_element *elem, void *value);
int DthRf_Tx3G_PowerSweep_Exec(struct dth_element *elem);
#endif

#if !defined(NO_TX3G_OUTPWRMEAS)
/* Function tide to WCDMA TX Output Power Measure */
/**************************************************/
int DthRf_OutPwrMeasure3G_get(struct dth_element *elem, void *value);
int DthRf_OutPwrMeasure3G_set(struct dth_element *elem, void *value);
int DthRf_OutPwrMeasure3G_exec(struct dth_element *elem);
#endif

#if !defined(NO_TX2G5_INTERNALPOWERMETER_MEASURE)
/* Function tide to GSM/EDGE Internal Power Measurement */
/********************************************************/
int DthRf_IntPwrMeasure2G5_get(struct dth_element *elem, void *value);
int DthRf_IntPwrMeasure2G5_set(struct dth_element *elem, void *value);
int DthRf_IntPwrMeasure2G5_exec(struct dth_element *elem);
#endif

#if !defined(NO_TX3G_INTERNALPOWERMETER_MEASURE)
/* Function tide to WCDMA Internal Power Measurement */
/*****************************************************/
int DthRf_IntPwrMeasure3G_get(struct dth_element *elem, void *value);
int DthRf_IntPwrMeasure3G_set(struct dth_element *elem, void *value);
int DthRf_IntPwrMeasure3G_exec(struct dth_element *elem);
#endif

#if !defined(NO_TX2G_POWERSWEEP)
/* Function tide to GSM TX Power Sweep           */
/**************************************************/
int DthRf_Tx2G_PowerSweep_get(struct dth_element *elem, void *value);
int DthRf_Tx2G_PowerSweep_set(struct dth_element *elem, void *value);
int DthRf_Tx2G_PowerSweep_exec(struct dth_element *elem);
#endif

#endif /* DTHRF_H_ */
