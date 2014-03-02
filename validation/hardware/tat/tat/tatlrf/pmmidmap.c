/*****************************************************************************/
 /**
 *  © ST-Ericsson, 2011 - All rights reserved
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson
 *
 * \file	   pmmidmap.c
 * \brief   routines to retrieve Permanent Modem Memory definition
 * \author  ST-Ericsson
 */
 /*****************************************************************************/
 
 #include "rfdef.h"
 #include "pmmidmap.h"
 
 #include "isimsg.h"
 
 RETURN_INFO_PARAM_STR *g_pmm_assoc_table = NULL;
 size_t g_pmm_assoc_count = 0;
 
 void tatl14_03SetNbParameters(void);
 
 int tatl14_00GetAllRfParam(void)
 {
 	size_t vl_i;
 	uint32 vl_value;
 	struct dth_element vl_elem;
 
 	vl_i = 0;
 	vl_value = 0;
 	memset(&vl_elem, 0, sizeof(vl_elem));
 
 	/* Set paramters' number. */
 	tatl14_03SetNbParameters();
 	if (g_pmm_assoc_count == 0) {
 		return -1;
 	}
 
 	/* Allocate and initialize memory for all RF parameters. */
 	g_pmm_assoc_table =
 	    malloc(sizeof(RETURN_INFO_PARAM_STR) * g_pmm_assoc_count);
 
 	SYSLOG(LOG_NOTICE, "Loading %d RF parameters\n", g_pmm_assoc_count);
 
 	if (g_pmm_assoc_table == NULL) {
 		SYSLOG(LOG_ERR, "error during memory allocation\n");
 		return -1;
 	}
 
 	for (vl_i = 0; vl_i < g_pmm_assoc_count; vl_i++) {
 		memset((char *)(g_pmm_assoc_table + vl_i)->string, '\0', 
             sizeof(g_pmm_assoc_table->string));
 	}
 
 	/* Get all parameters data. */
 	int vl_error = TAT_ERROR_OFF;
 	for (vl_i = 0; (vl_i < g_pmm_assoc_count) && TAT_OK(vl_error);
 	     vl_i++) {
 		vl_error = tatl14_02GetRfParam(vl_i);
 	}
 
 	return vl_error;
 }
 
 /**
  * Send and receive "TEST RF PARAM" ISI messages.
  * @param[in] vp_index specifies index of requested parameter.
  * @param[in/out] pp_Resp specifies location to store ISI response message.
  * @retval 0 if success.
  * @retval -1 if an error occured while processing.
  */
 int tatl14_01IsiSendAndReceiveRfParam(int vp_index,
 				      struct tatrf_isi_msg_t *pp_st)
 {
 	C_TEST_TUNING_REQ_STR *pl_req;
 	pl_req =
 	    tatrf_isi_msg_begin(pp_st, PN_DSP_COMMON_TEST, C_TEST_TUNING_REQ,
 				numberOfSubBlocks);
 	if (NULL == pl_req) {
 		return TAT_ISI_HANDLER_ERROR;
 	}
 
 	C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1_STR *pl_sb =
 	    tatrf_isi_msg_add_sb(pp_st, C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1);
 	if (NULL == pl_sb) {
 		return TAT_ISI_HANDLER_ERROR;
 	}
 
 	u8 *pl_ptrby = (u8 *) pl_sb;
 	GET_INFO_CTRL_DATA_STR *pl_get_info =
 	    (GET_INFO_CTRL_DATA_STR *) (pl_ptrby +
 					SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ1_STR);
 	pl_get_info->requested_action = C_TEST_RF_PARAM;
 	pl_get_info->fill_index = vp_index;
 	tatrf_isi_append_data(pp_st, (u16 *) pl_get_info,
 			      SIZE_GET_INFO_CTRL_DATA_STR / sizeof(u16));
 
 	/* send ISI message - blocking function */
 	int vl_Error = tatrf_isi_send(pp_st, "Get PMM map");
 	if (!TAT_OK(vl_Error)) {
 		SYSLOG(LOG_ERR, "Get PMM map response error\n");
 		return vl_Error;
 	}
 
 	/* message successfully sent. waiting for response */
 	C_TEST_TUNING_RESP_STR *pl_resp =
 	    tatrf_isi_read(pp_st, C_TEST_TUNING_RESP, numerOfSubBlocks,
 			   "Get PMM map response", &vl_Error);
 	if (NULL != pl_resp) {
 		/* this what we expected */
 		C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR *pl_sbr =
 		    tatrf_isi_find_sb(pp_st,
 				      C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1,
 				      NULL);
 		if (NULL != pl_sbr) {
 			vl_Error = TAT_ERROR_OFF;
 		} else {
 			vl_Error = TAT_ERROR_NOT_MATCHING_MSG;
 		}
 	}
 
 	return vl_Error;
 }
 
 int tatl14_01CountRfParam(void)
 {
     return g_pmm_assoc_count;
 }
 
 void tatl14_03SetNbParameters(void)
 {
 	struct tatrf_isi_msg_t vl_msg_info;
 	tatrf_isi_init(&vl_msg_info);
 
 	int vl_error = tatl14_01IsiSendAndReceiveRfParam(0, &vl_msg_info);
 	if (vl_error == TAT_ERROR_OFF) {
 		C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR *pl_sbr =
 		    tatrf_isi_find_sb(&vl_msg_info,
 				      C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1,
 				      NULL);
 
 		u8 *pl_ptrby = (u8 *) pl_sbr;
 		RETURN_INFO_PARAM_RESP_STR *pl_info_param =
 		    (RETURN_INFO_PARAM_RESP_STR *) (pl_ptrby +
 						    SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR);
 
 		g_pmm_assoc_count = pl_info_param->nb_of_param;
 	} else {
 		g_pmm_assoc_count = 0;
 	}
 }
 
 int tatl14_02GetRfParam(size_t vp_position)
 {
 	struct tatrf_isi_msg_t vl_msg_info;
 	tatrf_isi_init(&vl_msg_info);
 
 	if (vp_position >= g_pmm_assoc_count) {
 		SYSLOG(LOG_ERR, "index exceeds array size!");
 		return TAT_MISC_ERR;
 	}
 
 	int vl_error =
 	    tatl14_01IsiSendAndReceiveRfParam(vp_position, &vl_msg_info);
 	if (TAT_OK(vl_error)) {
 		C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR *pl_sbr =
 		    tatrf_isi_find_sb(&vl_msg_info,
                 C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1,
 				      NULL);
 
 		u8 *pl_ptrby = (u8 *) pl_sbr;
 		RETURN_INFO_PARAM_RESP_STR *pl_info_param =
 		    (RETURN_INFO_PARAM_RESP_STR *) (pl_ptrby +
 						    SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP1_STR);
 
 		g_pmm_assoc_table[vp_position].paramID =
 		    pl_info_param->return_info_param.paramID;
 		memcpy(g_pmm_assoc_table[vp_position].string,
 		       pl_info_param->return_info_param.string,
 		       sizeof(pl_info_param->return_info_param.string));
 	}
 
 	return vl_error;
 }
 
 int tatl14_06GetRfParamId(char *pp_param_name, size_t *pp_position)
 {
 	int vl_result = -1;
 	size_t vl_i = 0;
 	*pp_position = 0;
 
 	/* Search param id according to param name. */
 	for (vl_i = 0; vl_i < g_pmm_assoc_count; vl_i++) {
 		if (strcmp
 		    ((char *)(g_pmm_assoc_table + vl_i)->string,
 		     pp_param_name) == 0) {
 			vl_result = (g_pmm_assoc_table + vl_i)->paramID;
 		}
 
 		if (vl_result >= 0)
 			break;
 
 		*pp_position = *pp_position + 1;
 	}
 
 	return vl_result;
 }


const RETURN_INFO_PARAM_STR *tatl14_GetRfParamInfo(size_t vp_position)
 {
     const RETURN_INFO_PARAM_STR *pl_info = NULL;
     if ( vp_position < g_pmm_assoc_count )
     {
         pl_info = g_pmm_assoc_table + vp_position;
     }
 
     return pl_info;
 }
 
void tatl14_04ReleaseParamInfo(void)
/*void free_pmm_assoc_table(void)*/
 {
     if (NULL != g_pmm_assoc_table) {
 		free(g_pmm_assoc_table);
 		g_pmm_assoc_table = NULL;
 	}
 }
