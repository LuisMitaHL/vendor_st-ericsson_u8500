/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file	   tx2gpwrswp.c
* \brief   routines to manage GSM TX power sweep
* \author  ST-Ericsson
*/
/*****************************************************************************/

#include "rfdef.h"
#include "tx2gpwrswp.h"

#include "misc.h"
#include "isimsg.h"

/* CONSTS */
// Every operation including Band, Channel, Offtime, PCL count and 4 pairs PCL value(PCL value, Edge ON/OFF)
#define C_RF_GSM_TX_POWERSWEEP_OPER_ELEM_NBR 12
// Support 100 operations in max
#define C_RF_GSM_TX_POWERSWEEP_OPER_NBR 100
// For a uint8 PCL value, MSB = 1 stands for edge ON, which means PCL greater then 0b10000000(0d128) is edge ON
const uint8 C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON = 128;

/* GLOBALS */
C_HAL_SB_GSM_TX_SWEEP_TEST_CONTROL_STR *p_tatrf_sb_gsm_powersweep_control;

GSM_TX_SWEEP_TEST_OPER_STR *p_tatrf_seq_gsm_powersweep_oper;

uint16 v_tatrf_gsm_powersweep_tx_setup = 0;
uint16 v_tatrf_gsm_powersweep_oper_nbr = 1;
uint32 v_tatrf_gsm_powersweep_oper_band[C_RF_GSM_TX_POWERSWEEP_OPER_NBR] = {0};
uint16 v_tatrf_gsm_powersweep_oper_channel[C_RF_GSM_TX_POWERSWEEP_OPER_NBR] = {0};
uint16 v_tatrf_gsm_powersweep_oper_offtime[C_RF_GSM_TX_POWERSWEEP_OPER_NBR] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
uint16 v_tatrf_gsm_powersweep_oper_pcl_nbr[C_RF_GSM_TX_POWERSWEEP_OPER_NBR] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
uint16 v_tatrf_gsm_powersweep_oper_pcl[2 * C_RF_GSM_TX_POWERSWEEP_OPER_NBR] = {0};

/* Function tide to WCDMA TX power sweep          */
/**************************************************/
int DthRf_Tx2G_PowerSweep_get(struct dth_element *elem, void *value)
{
	return tatl21_05Tx2G_PowerSweep_Get(elem, value);
}

int DthRf_Tx2G_PowerSweep_set(struct dth_element *elem, void *value)
{
	return tatl21_06Tx2G_PowerSweep_Set(elem, value);
}

int DthRf_Tx2G_PowerSweep_exec(struct dth_element *elem)
{
	return tatl21_07Tx2G_PowerSweep_Exec(elem);
}

int tatl21_08Tx2G_PowerSweep_Action()
{
	int vl_Error = TAT_ERROR_OFF;

	// for tracking the struct array elements
	int i;
	GSM_TX_SWEEP_TEST_OPER_STR *pl_seq;

	int sizeof_sub = SIZE_C_HAL_SB_GSM_TX_SWEEP_TEST_CONTROL_STR
				+ (SIZE_GSM_TX_SWEEP_TEST_OPER_STR * v_tatrf_gsm_powersweep_oper_nbr);
	int sizeof_seq = SIZE_GSM_TX_SWEEP_TEST_OPER_STR * v_tatrf_gsm_powersweep_oper_nbr;

	p_tatrf_seq_gsm_powersweep_oper = malloc(sizeof_seq);
	p_tatrf_sb_gsm_powersweep_control = malloc(sizeof_sub);
	if (NULL == p_tatrf_seq_gsm_powersweep_oper || NULL == p_tatrf_sb_gsm_powersweep_control)
	{
		SYSLOG(LOG_ERR, "malloc failed for the subblock");
		return TAT_ISI_HANDLER_ERROR;
	}

	/* Init power sweep data */
	memset(p_tatrf_seq_gsm_powersweep_oper,
		0,
		sizeof_seq);
	memset(p_tatrf_sb_gsm_powersweep_control,
		0,
		sizeof_sub);

	struct tatrf_isi_msg_t vl_msg_info;
	tatrf_isi_init(&vl_msg_info);

	C_HAL_RF_TEST_CONTROL_REQ_STR *vl_req;
	vl_req = tatrf_isi_msg_begin(&vl_msg_info, PN_DSP_COMMON_TEST,
					C_HAL_RF_TEST_CONTROL_REQ, num_sb_id);
	if (NULL == vl_req)
	{
		return TAT_ISI_HANDLER_ERROR;
	}

	p_tatrf_sb_gsm_powersweep_control->tx_setup = v_tatrf_gsm_powersweep_tx_setup;
	p_tatrf_sb_gsm_powersweep_control->number_of_tx_opers = v_tatrf_gsm_powersweep_oper_nbr;

	pl_seq = p_tatrf_seq_gsm_powersweep_oper;
	for (i = 0; i < v_tatrf_gsm_powersweep_oper_nbr; i++)
	{
		pl_seq->band_info = v_tatrf_gsm_powersweep_oper_band[i];
		pl_seq->channel_nbr = v_tatrf_gsm_powersweep_oper_channel[i];
		pl_seq->off_time = v_tatrf_gsm_powersweep_oper_offtime[i];
		pl_seq->number_of_pcl = v_tatrf_gsm_powersweep_oper_pcl_nbr[i];
		pl_seq->power_level[0] = v_tatrf_gsm_powersweep_oper_pcl[i*2];
		pl_seq->power_level[1] = v_tatrf_gsm_powersweep_oper_pcl[i*2 + 1];
		pl_seq->fill1 = 0;
		pl_seq += 1;
	}

	C_HAL_SB_GSM_TX_SWEEP_TEST_CONTROL_STR *pl_sb =
	tatrf_isi_msg_add_sb_copy(&vl_msg_info,
				C_HAL_SB_GSM_TX_SWEEP_TEST_CONTROL,
				p_tatrf_sb_gsm_powersweep_control,
				sizeof_sub);
	if (NULL == pl_sb)
	{
		return TAT_ISI_HANDLER_ERROR;
	}

	vl_Error = tatrf_isi_append_data(&vl_msg_info, (u16 *)p_tatrf_seq_gsm_powersweep_oper,
					sizeof_seq / sizeof(u16));
	if (!TAT_OK(vl_Error))
	{
		return vl_Error;
	}

	/* send ISI message - blocking function */
	vl_Error = tatrf_isi_send(&vl_msg_info, "Start GSM TX Power Sweep Request");
	if (!TAT_OK(vl_Error))
	{
		return vl_Error;
	}

	/* message successfully sent. waiting for response */
	tatl17_06read(&vl_msg_info, C_HAL_RF_TEST_CONTROL_RESP, NULL,
		"Start GSM TX Power Sweep Response", &vl_Error);

	free(p_tatrf_seq_gsm_powersweep_oper);
	p_tatrf_seq_gsm_powersweep_oper = NULL;
	free(p_tatrf_sb_gsm_powersweep_control);
	p_tatrf_sb_gsm_powersweep_control = NULL;
	pl_seq = NULL;

	return vl_Error;
}

int tatl21_05Tx2G_PowerSweep_Get(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;	/* errno return code */
	uint8 tmp;
	uint8 *p_pcl;
	int oper, fctr;

	switch(elem->user_data)
	{
		case ACT_TX2G_POWERSWEEP:
			break;
		case ACT_TX2G_POWERSWEEP_SETUP:
			*((uint16*)Value) = v_tatrf_gsm_powersweep_tx_setup;
			break;
		case ACT_TX2G_POWERSWEEP_NUMBER:
			*((uint16*)Value) = v_tatrf_gsm_powersweep_oper_nbr;
			break;
		default:
			if(elem->user_data >=ACT_TX2G_POWERSWEEP_BAND_001 &&
			elem->user_data <= ACT_TX2G_POWERSWEEP_PCL4_EDGE_100)
			{
				oper = (elem->user_data - ACT_TX2G_POWERSWEEP_BAND_001) / C_RF_GSM_TX_POWERSWEEP_OPER_ELEM_NBR;
				fctr = (elem->user_data - ACT_TX2G_POWERSWEEP_BAND_001) % C_RF_GSM_TX_POWERSWEEP_OPER_ELEM_NBR;

				switch(fctr)
				{
					case 0:
						*((uint32*)Value) = v_tatrf_gsm_powersweep_oper_band[oper];
						break;
					case 1:
						*((uint16*)Value) = v_tatrf_gsm_powersweep_oper_channel[oper];
						break;
					case 2:
						*((uint16*)Value) = v_tatrf_gsm_powersweep_oper_offtime[oper];
						break;
					case 3:
						*((uint16*)Value) = v_tatrf_gsm_powersweep_oper_pcl_nbr[oper];
						break;
					case 4:
						p_pcl = &(v_tatrf_gsm_powersweep_oper_pcl[2 * oper]);
						if( (*p_pcl) >= C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON )
						{ *((uint8*)Value) = (*p_pcl) - C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON; }
						else
						{ *((uint8*)Value) = *p_pcl; }
						break;
					case 5:
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper];
						tmp = (*p_pcl) & C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON;
						if ( tmp )
						{ *((uint8*)Value) = 1; }
						else
						{ *((uint8*)Value) = 0; }
						break;
					case 6:
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper];
						p_pcl += 1;
						if(*p_pcl >= C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON)
						{ *((uint8*)Value) = (*p_pcl) - C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON; }
						else
						{ *((uint8*)Value) = *p_pcl; }
						break;
					case 7:
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper];
						p_pcl += 1;
						tmp = (*p_pcl) & C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON;
						if ( tmp )
						{ *((uint8*)Value) = 1; }
						else
						{ *((uint8*)Value) = 0; }
						break;
					case 8:
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper + 1];
						if(*p_pcl >= C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON)
						{ *((uint8*)Value) = (*p_pcl) - C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON; }
						else
						{ *((uint8*)Value) = *p_pcl; }
						break;
					case 9:
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper + 1];
						tmp = (*p_pcl) & C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON;
						if ( tmp )
						{ *((uint8*)Value) = 1; }
						else
						{ *((uint8*)Value) = 0; }
						break;
					case 10:
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper + 1];
						p_pcl += 1;
						if(*p_pcl >= C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON)
						{ *((uint8*)Value) = (*p_pcl) - C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON; }
						else
						{ *((uint8*)Value) = *p_pcl; }
						break;
					case 11:
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper + 1];
						p_pcl += 1;
						tmp = (*p_pcl) & C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON;
						if ( tmp )
						{ *((uint8*)Value) = 1; }
						else
						{ *((uint8*)Value) = 0; }
						break;
					default:
						vl_Error = TAT_BAD_REQ;
						break;
				}
			}
			break;
	}

	return vl_Error;
}

int tatl21_06Tx2G_PowerSweep_Set(struct dth_element *elem, void *Value)
{
	int vl_Error = TAT_ERROR_OFF;	/* errno return code */
	uint8 tmp;
	uint8 *p_pcl;
	int oper, fctr;

	switch(elem->user_data)
	{
		case ACT_TX2G_POWERSWEEP:
			break;
		case ACT_TX2G_POWERSWEEP_SETUP:
			v_tatrf_gsm_powersweep_tx_setup = *((uint16*)Value);
			break;
		case ACT_TX2G_POWERSWEEP_NUMBER:
			v_tatrf_gsm_powersweep_oper_nbr = *((uint16*)Value);
			break;
		default:
			// 100 Operations
			if(elem->user_data >=ACT_TX2G_POWERSWEEP_BAND_001 &&
			elem->user_data <= ACT_TX2G_POWERSWEEP_PCL4_EDGE_100)
			{
				// TX operation index
				oper = (elem->user_data - ACT_TX2G_POWERSWEEP_BAND_001) / C_RF_GSM_TX_POWERSWEEP_OPER_ELEM_NBR;
				// Operation info index
				fctr = (elem->user_data - ACT_TX2G_POWERSWEEP_BAND_001) % C_RF_GSM_TX_POWERSWEEP_OPER_ELEM_NBR;

				switch(fctr)
				{
					case 0: // Band
						v_tatrf_gsm_powersweep_oper_band[oper] = *((uint32*)Value);
						break;
					case 1: // Channel
						v_tatrf_gsm_powersweep_oper_channel[oper] = *((uint16*)Value);
						break;
					case 2: // Offtime
						v_tatrf_gsm_powersweep_oper_offtime[oper] = *((uint16*)Value);
						break;
					case 3: // PCL count
						v_tatrf_gsm_powersweep_oper_pcl_nbr[oper] = *((uint16*)Value);
						break;
					/* Power levels in terms of PCL values
					MSb '1' indicating EDGE ON
					power_level[0] xxxxxxxx-------- PCL[0]
					power_level[0] --------xxxxxxxx PCL[1]
					power_level[1] xxxxxxxx-------- PCL[2]
					power_level[1] --------xxxxxxxx PCL[3]
					*/
					case 4: // PCL1 value
						// uint8 in MSB 1~8 of uint16[0]
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper];
						*p_pcl = *((uint8*)Value);
						break;
					case 5: // PCL1 edge
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper];
						tmp = *((uint8*)Value);
						if( tmp > 0)
						{ (*p_pcl) += C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON; }
						break;
					case 6: // PCL2 value
						// uint8 in MSB 9~16 of uint16[0]
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper];
						p_pcl += 1;
						*p_pcl = *((uint8*)Value);
						break;
					case 7: // PCL2 edge
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper];
						p_pcl += 1;
						tmp = *((uint8*)Value);
						if( tmp > 0)
						{ (*p_pcl) += C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON; }
						break;
					case 8: // PCL3 value
						// uint8 in MSB 1~8 of uint16[1]
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper + 1];
						*p_pcl = *((uint8*)Value);
						break;
					case 9: // PCL edge
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper + 1];
						tmp = *((uint8*)Value);
						if( tmp > 0)
						{ (*p_pcl) += C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON; }
						break;
					case 10:// PCL4 value
						// uint8 in MSB 9~16 of uint16[1]
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper + 1];
						p_pcl += 1;;
						*p_pcl = *((uint8*)Value);
						break;
					case 11: // PCL edge
						p_pcl = &v_tatrf_gsm_powersweep_oper_pcl[2 * oper + 1];
						p_pcl += 1;;
						tmp = *((uint8*)Value);
						if( tmp > 0)
						{ (*p_pcl) += C_RF_GSM_TX_POWERSWEEP_OPER_PCL_EDGE_ON; }
						break;
					default:
						vl_Error = TAT_BAD_REQ;
						break;
				}
			}
			break;
	}

	return vl_Error;
}

int tatl21_07Tx2G_PowerSweep_Exec(struct dth_element *elem)
{
	int vl_Error = TAT_ERROR_OFF;

	switch (elem->user_data)
	{
		case ACT_TX2G_POWERSWEEP:
		{
			vl_Error = tatl21_08Tx2G_PowerSweep_Action();
			break;
		}
		default:
		{
			SYSLOG(LOG_ERR, "invalid action code: %d", elem->user_data);
			vl_Error = TAT_BAD_REQ;
			break;
		}
	}

	return vl_Error;
}
