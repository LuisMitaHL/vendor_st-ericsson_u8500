/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#define TATL12CONN_C
#include "tatlcoex.h"
#undef  TATL12CONN_C

char *ConnError = "Unable to start/stop this connectivity bus";
char *ConnIPError = "Unable to start/stop this service";
char *CW1200Error = "Unable to start/stop CW1200";
char *BTIPError = "Unable to start/stop BT IP";
char *FMIPError = "Unable to start/stop FM IP";
char *ACError = "Unable to cofigure/decofigure audio";

#ifdef CONNECTIVITY_PANEL
static int tatl12_04Conn_Management_ConnCGVER(DTH_CGVER_DATA *data)
{
	uint32_t vl_error = 0, i = 0;
	t_ConnServ_BT_ChipInformation l_ChipInformation;
	char *strcat_patch = NULL;

	vl_error = ConnServ_Chip_Identifier(&l_ChipInformation);
	if (vl_error == 0) {
		data->min = l_ChipInformation.CG2900_Chip_Minor_Version;
		data->maj = l_ChipInformation.CG2900_Chip_Major_Version;
		data->rad = l_ChipInformation.CG2900_Chip_Radio_Version;
		data->vendor = l_ChipInformation.CG2900_Manufacturer_Name;
		data->hci = l_ChipInformation.CG2900_BT_HCI_Version;
		data->lmp = l_ChipInformation.CG2900_LMP_Version;
		sprintf(data->patchid, "%d", l_ChipInformation.CG2900_Patch_ID[0]);
		for (i = 1; i < CONNSERV_CG2900_PATCH_ID_LENGTH; i++) {
			sprintf(strcat_patch, "%d", l_ChipInformation.CG2900_Patch_ID[i]);
			strcat(data->patchid, strcat_patch);
		}
		sprintf(data->patchdate, "%d", l_ChipInformation.CG2900_SW_Build_Date[0]);
		for (i = 1; i < CONNSERV_CG2900_SW_BUILD_LENGTH; i++) {
			sprintf(strcat_patch, "%d", l_ChipInformation.CG2900_SW_Build_Date[i]);
			strcat(data->patchdate, strcat_patch);
		}
	} else {
		data->error = ConnIPError;
	}
	return vl_error;
}
#endif

static int tatl12_04Conn_Management_ConnWLANTX(DTH_WLANTX_DATA *data)
{
	uint32_t vl_error = 0;

	u32 l_pktsent, l_pktack;

	if ((data->order == START) && (data->state == DEACTIVATED)) {
		printf("data->rate = %d, data->band = %d, data->mode = %d, data->channel = %d, data->ack = %d, data->ifd = %d, data->pwridx = %f", data->rate, data->band,
		       data->mode, data->channel, data->ack, data->ifd, data->pwridx);
		vl_error = ConnServ_WLAN_Tx_Start(data->rate, data->band, data->mode, data->channel, data->ack, data->ifd, data->pwridx);
		if (vl_error == 0) {
			data->state = ACTIVATED;
			data->error = NO_ERROR;
		} else {
			data->error = ConnIPError;
		}
	} else if ((data->order == STOP) && (data->state == ACTIVATED)) {
		vl_error = ConnServ_WLAN_Tx_Stop(&l_pktsent, &l_pktack);
		if (vl_error == 0) {
			data->pktsent = l_pktsent;
			data->pktack = l_pktack;
			data->state = DEACTIVATED;
			data->error = NO_ERROR;
		} else {
			data->error = ConnIPError;
		}
	}

	return vl_error;
}

int tatl12_18Conn_FMTX_Audio_Configuration()
{
	uint32_t ulRetVal = 0;
	if (0 == ulRetVal) {
		ulRetVal = SetSrc(AUDIO_DEVICE_0, CODEC_SRC_MICROPHONE_1B);
		if (0 != ulRetVal)
			printf("FM TX Start: failed to set source to Mic 1B on AB8500");
	}

	if (0 == ulRetVal) {
		ulRetVal = SetMute(AUDIO_DEVICE_0, CODEC_CAPTURE, STREAM_UNMUTED);
		if (0 != ulRetVal)
			printf("FM TX Start: failed to unmute Mic 1B on AB8500");
	}

	if (0 == ulRetVal) {
		ulRetVal = SetVolume(AUDIO_DEVICE_0, CODEC_CAPTURE, 100, 100);
		if (0 != ulRetVal)
			printf("FM TX Start: failed to set the volume of Mic 1B on AB8500");
	}

	if (0 == ulRetVal) {
		ulRetVal = StartPCMTransfer(AUDIO_DEVICE_0, CODEC_PLAYBACK, DIGITAL_LOOPBACK_MODE, NULL);
		if (0 != ulRetVal)
			printf("FM TX Start: failed to Start I2S transfer on AB8500");
	}

	if (0 == ulRetVal) {
		/* This does ? */
		system("sk-ab W 0x1014 4");
		system("sk-ab W 0x1024 4");
		system("sk-ab W 0x1034 4");

		/*
		 * This ensures that the audio are indeed routed to the CG2900
		 */
		system("sk-ab W 0x0D27 0x22");	/* Set emission from Mic 1B to CG2900 (plugged on port IF1), slots 16 & 17 */
		system("sk-ab W 0x0D1F 0xCC");	/* Prevent MIC 1B from emitting on IF0 */
		system("sk-ab W 0x0D20 0xCC");	/* Prevent MIC 1B from emitting on IF0 */
		system("sk-ab W 0x0D21 0xCC");	/* Prevent MIC 1B from emitting on IF0 */
		system("sk-ab W 0x0D22 0xCC");	/* Prevent MIC 1B from emitting on IF0 */
		system("sk-ab W 0x0D1E 0x18");
	}
	return ulRetVal;
}

int tatl12_18Conn_FMTX_Audio_DeConfiguration()
{
	uint32_t ulRetVal = 0;

	ulRetVal = StopPCMTransfer(AUDIO_DEVICE_0, CODEC_PLAYBACK);
	if (0 != ulRetVal)
		printf("FM Tx Stop: Error while stopping I2S on AB/DB 8500. Carrying on anyway");

	return ulRetVal;
}

int tatl12_18Conn_I2S_Audio_Configuration()
{
	uint32_t ulCmdStatus = 0;

	ulCmdStatus = SetSink(AUDIO_DEVICE_0, CODEC_DEST_HEADSET);
	if (0 != ulCmdStatus)
		printf("[I2S] in ConnEngine_I2S_loop_back: failed to Set Sink to Headset\n");

	if (0 == ulCmdStatus) {
		ulCmdStatus = SetMute(AUDIO_DEVICE_0, CODEC_PLAYBACK, STREAM_UNMUTED);
		if (CONNSERV_NO_ERROR != ulCmdStatus)
			printf("[I2S] in ConnEngine_I2S_loop_back: failed to unmute Headset\n");
	}

	if (0 == ulCmdStatus) {
		ulCmdStatus = SetVolume(AUDIO_DEVICE_0, CODEC_PLAYBACK, 100, 100);
		if (CONNSERV_NO_ERROR != ulCmdStatus)
			printf("[I2S] in ConnEngine_I2S_loop_back: failed to set the volume of the Headset\n");
	}

	if (0 == ulCmdStatus) {
		ulCmdStatus = SetSrc(AUDIO_DEVICE_0, CODEC_SRC_MICROPHONE_1B);
		if (CONNSERV_NO_ERROR != ulCmdStatus)
			printf("[I2S] in ConnEngine_I2S_loop_back: failed to set source to Mic 1B\n");
	}

	if (0 == ulCmdStatus) {
		ulCmdStatus = SetMute(AUDIO_DEVICE_0, CODEC_CAPTURE, STREAM_UNMUTED);
		if (CONNSERV_NO_ERROR != ulCmdStatus)
			printf("[I2S] in ConnEngine_I2S_loop_back: failed to unmute Mic 1B\n");
	}

	if (0 == ulCmdStatus) {
		ulCmdStatus = SetVolume(AUDIO_DEVICE_0, CODEC_CAPTURE, 100, 100);
		if (CONNSERV_NO_ERROR != ulCmdStatus)
			printf("[I2S] in ConnEngine_I2S_loop_back: failed to set the volume of Mic 1B\n");
	}

	if (0 == ulCmdStatus) {
		ulCmdStatus = StartPCMTransfer(AUDIO_DEVICE_0, CODEC_PLAYBACK, DIGITAL_LOOPBACK_MODE, NULL);
		if (CONNSERV_NO_ERROR != ulCmdStatus)
			printf("[I2S] in ConnEngine_I2S_loop_back: failed to Start I2S transfer\n");
	}

	if (0 == ulCmdStatus) {
		/* This does ? */
		system("sk-ab W 0x1014 4");
		system("sk-ab W 0x1024 4");
		system("sk-ab W 0x1034 4");

		/*
		 * This ensures that the audio are indeed routed to/from the CG2900
		 */
		system("sk-ab W 0x0D33 0x98");	/* Set reception from CG2900 (plugged on port IF1) to Headset, reg DA1, slot 24 */
		system("sk-ab W 0x0D34 0x19");	/* Set reception from CG2900 (plugged on port IF1) to Headset, reg DA2, slot 25 */
		system("sk-ab W 0x0D27 0x22");	/* Set emission from Mic 1B to CG2900 (plugged on port IF1), slots 16 & 17 */
		system("sk-ab W 0x0D1F 0xCC");	/* Prevent MIC 1B from emitting on IF0 */
		system("sk-ab W 0x0D20 0xCC");	/* Prevent MIC 1B from emitting on IF0 */
		system("sk-ab W 0x0D21 0xCC");	/* Prevent MIC 1B from emitting on IF0 */
		system("sk-ab W 0x0D22 0xCC");	/* Prevent MIC 1B from emitting on IF0 */
	}

	return ulCmdStatus;
}

int tatl12_18Conn_I2S_Audio_DeConfiguration()
{

	uint32_t ulRetVal = 1;

	ulRetVal = StopPCMTransfer(AUDIO_DEVICE_0, CODEC_PLAYBACK);

	if (0 != ulRetVal)
		printf("[I2S] Error while stopping PCM loop on AB/DB 8500. Carrying on anyway\n");

	return ulRetVal;
}

static int tatl12_04Conn_Management_BTTX(DTH_BTTX_DATA *data)
{
	uint32_t vl_error;

	vl_error = 0;

	if ((data->order == START) && (data->state == DEACTIVATED)) {
		printf("data->channel = %d, data->pktt = %d, data->tx_level = %d, data->addr_idx = %d\n", data->channel, data->pktt, data->tx_level, data->addr_idx);
		vl_error = ConnServ_BT_TxMode_Start(data->channel, data->pktt, data->tx_level, data->addr_idx);
		if (vl_error == 0) {
			data->state = ACTIVATED;
			data->error = NO_ERROR;
		} else {
			printf("ConnServ_BT_TxMode_Start failed with vl_error=%d\n", vl_error);
			data->error = ConnIPError;
		}
	} else if ((data->order == STOP) && (data->state == ACTIVATED)) {
		vl_error = ConnServ_BT_TxMode_Stop();
		if (vl_error == 0) {
			data->state = DEACTIVATED;
			data->error = NO_ERROR;
		} else {
			printf("ConnServ_BT_TxMode_Stop failed with vl_error=%d\n", vl_error);
			data->error = ConnIPError;
		}
	}

	return vl_error;
}

static int tatl12_04Conn_Management_FMTX(DTH_FMTX_DATA *data)
{
	uint32_t vl_error = 0;

	if ((data->order == START) && (data->state == DEACTIVATED)) {
		printf("freq = %d, pwr = %d, mode = %d, src = %d, volume = %d, emp = %d, tonefreq = %d\n", data->freq, data->pwr, data->mode, data->src, data->volume,
		       1 + (data->emp), data->tonefreq);
		vl_error = ConnServ_FM_Tx_Start(data->freq, data->pwr, data->mode, data->src, data->volume, 1 + (data->emp), data->tonefreq);
		if (vl_error == 0) {
			data->state = ACTIVATED;
			data->error = NO_ERROR;
		} else {
			data->error = ConnIPError;
		}
	} else if ((data->order == STOP) && (data->state == ACTIVATED)) {
		vl_error = ConnServ_FM_Tx_Stop();
		if (vl_error == 0) {
			data->state = DEACTIVATED;
			data->error = NO_ERROR;
		} else {
			data->error = ConnIPError;
		}
	}

	return vl_error;
}
#ifdef CONNECTIVITY_PANEL
static int tatl12_04Conn_Management_BLEDUT(DTH_BLEDUT_DATA *data)
{
	uint32_t vl_error;

	vl_error = 0;
	u16 l_rcvdpkts = 0;

	if ((data->order == START) && (data->state == 0)) {	/* state=0 -> DEACTIVATED */
		vl_error = ConnServ_BLE_DUT_mode_Start(data->mode, data->frequency, data->length, data->payload);

		if (vl_error == 0) {
			data->state = (data->mode) + 1;	/* for mode Tx(=0) state is "Activated Tx"(=1) */
			data->error = NO_ERROR;
		} else {
			data->error = ConnIPError;
		}
	} else if ((data->order == STOP) && (data->state > 0)) {
		vl_error = ConnServ_BLE_DUT_mode_Stop(&l_rcvdpkts);
		if (vl_error == 0) {
			data->state = 0;
			data->error = NO_ERROR;
			data->rcvdpkts = l_rcvdpkts;
		} else {
			data->error = ConnIPError;
		}
	}
	return vl_error;
}

static int tatl12_04Conn_Management_ConnBTRX(DTH_BTRX_DATA *data)
{
	uint32_t vl_error;

	vl_error = 0;
	u16 total_packets;
	u16 wrongtype_packets;
	u16 wronglength_packets;
	u16 biterrors;
	u16 erroneous_packets;
	u8 l_rssi = 0;
	u32 l_ber = 0;

	if ((data->order == START) && (data->state == DEACTIVATED)) {
		vl_error = ConnServ_BT_RxMode_Start(data->channel, data->pktt, data->addr_idx);

		if (vl_error == 0) {
			data->state = ACTIVATED;
			data->error = NO_ERROR;
		} else {
			data->error = ConnIPError;
		}
	} else if ((data->order == STOP) && (data->state == ACTIVATED)) {
		vl_error = ConnServ_BT_RxMode_Stop(&total_packets, &wrongtype_packets, &wronglength_packets, &biterrors, &erroneous_packets, &l_rssi, &l_ber);
		if (vl_error == 0) {
			data->state = DEACTIVATED;
			data->error = NO_ERROR;
			data->rssi = l_rssi;
			data->ber = l_ber;
			data->nump = total_packets;
			data->wrtype = wrongtype_packets;
			data->wrlen = wronglength_packets;
			data->berrors = biterrors;
			data->err = erroneous_packets;
		} else {
			data->error = ConnIPError;
		}
	}
	return vl_error;
}

static int tatl12_04Conn_Management_ConnFMRX(DTH_FMRX_DATA *data)
{
	uint32_t vl_error = 0;
	int32_t l_rssi;

	if ((data->order == START) && (data->state == DEACTIVATED)) {
		vl_error = ConnServ_FM_Rx_Start(data->channel, data->mode, 1 + (data->emp));

		if (vl_error == 0) {
			data->state = ACTIVATED;
			data->error = NO_ERROR;
		} else {
			data->error = ConnIPError;
		}
	} else if ((data->order == STOP) && (data->state == ACTIVATED)) {
		vl_error = ConnServ_FM_Rx_Stop();
		if (vl_error == 0) {
			data->state = DEACTIVATED;
			data->error = NO_ERROR;
		} else {
			data->error = ConnIPError;
		}
	} else if ((data->state == ACTIVATED) && (data->order == 2)) {	/* Get RSSI */
		vl_error = ConnServ_FM_Signal_Strength(&l_rssi);
		if (vl_error != 0)
			printf("ConnServ_FM_Signal_Strength failed with vl_error=%d\n", vl_error);
		else
			data->rssi = l_rssi;
	}
	return vl_error;
}
#endif
/**
 *  Manage START/STOP of all connectivity buses.
 *  @param[in] data data needed to manage connectivity buses.
 *  @param[in] start function to be called to start the service.
 *  @param[in] stop function to be called to stop the service.
 * @retval 0 success.
 * @retval -1 if an error occured while processing.
 */
static int tatl12_04Conn_Management(DTH_CONN_DATA *data, uint32_t(*start) (void), uint32_t(*stop) (void))
{
	uint32_t vl_error;

	vl_error = 0;
	printf("DTH_CONN_DATA idx = %d\n", data->idx);
	/* Idx is ++ when START is called and -- when STOP is called. STANDBY has no influence in idx */
	if ((data->order == START) && (data->state == DEACTIVATED) && (data->idx == 0)) {	/* first start */
		vl_error = start();
		if (vl_error == 0) {
			if (data->state == DEACTIVATED) {
				data->idx = (data->idx) + 1;
			}
			data->state = ACTIVATED;
			data->error = NO_ERROR;
		} else {
			data->error = ConnError;
		}
	} else if ((data->order == START) && (data->state == ACTIVATED)) {	/* not the first start */
		data->idx = (data->idx) + 1;
	} else if ((data->order == STOP) && (data->state == ACTIVATED) && (data->idx == 1)) {	/* only one process left */
		vl_error = stop();
		if (vl_error == 0) {
			data->state = DEACTIVATED;
			data->error = NO_ERROR;
			data->idx = (data->idx) - 1;
		} else {
			data->error = ConnError;
		}
	} else if ((data->order == STOP) && (data->state == ACTIVATED) && (((data->idx) - 1) > 0)) {	/* there are many processes */
		data->idx = (data->idx) - 1;
	}
	return vl_error;
}

int tatl12_00Conn_Exec(struct dth_element *elem)
{
	int vl_error, vl_error2;

	vl_error = 0;
	vl_error2 = 0;

	switch (elem->user_data) {
	case ACT_CG2900:
		vl_error = tatl12_04Conn_Management(&v_tatcoex_cg2900_data, &ConnServ_CG2900_Start, &ConnServ_CG2900_Stop);
		break;

	case ACT_CW1200:
		{
			vl_error = tatl12_04Conn_Management(&v_tatcoex_cw1200_data, &ConnServ_CW1200_Start, &ConnServ_CW1200_Stop);
		}
		break;

	case ACT_BTIP:
		{
			vl_error = tatl12_04Conn_Management(&v_tatcoex_btip_data, &ConnServ_BT_Start, &ConnServ_BT_Stop);
		}
		break;

	case ACT_FMIP:
		{
			vl_error = tatl12_04Conn_Management(&v_tatcoex_fmip_data, &ConnServ_FM_Start, &ConnServ_FM_Stop);
		}
		break;

#ifdef CONNECTIVITY_PANEL
	case ACT_GPSIP:
		{
			vl_error = tatl12_04Conn_Management_GPS(&v_tatcoex_gpsip_data);
		}
		break;

#endif
	case ACT_BTTX:
		{
			if ((v_tatcoex_bttx_data.order == START) && (v_tatcoex_bttx_data.state == DEACTIVATED)) {
				vl_error = tatl12_04Conn_Management(&v_tatcoex_btip_data, &ConnServ_BT_Start, &ConnServ_BT_Stop);
				if (vl_error != 0) {
					printf("ConnServ_BT_Start failed with vl_error = %d\n", vl_error);
					strncpy(BTIPError, v_tatcoex_bttx_data.error, MAX_STRNG_SIZE);
				} else {
					vl_error = tatl12_04Conn_Management_BTTX(&v_tatcoex_bttx_data);
				}
			} else if ((v_tatcoex_bttx_data.order == STOP) && (v_tatcoex_bttx_data.state == ACTIVATED)) {
				vl_error = tatl12_04Conn_Management_BTTX(&v_tatcoex_bttx_data);
				if (vl_error != 0) {
					printf("ConnServ_BT_Stop failed with vl_error = %d\n", vl_error);
					strncpy(BTIPError, v_tatcoex_bttx_data.error, MAX_STRNG_SIZE);
				} else
					vl_error2 = tatl12_04Conn_Management(&v_tatcoex_btip_data, &ConnServ_BT_Start, &ConnServ_BT_Stop);
			}
		}
		break;

	case ACT_FMTX:
		{
			if ((v_tatcoex_fmtx_data.order == START) && (v_tatcoex_fmtx_data.state == DEACTIVATED)) {
				vl_error = tatl12_04Conn_Management(&v_tatcoex_fmip_data, &ConnServ_FM_Start, &ConnServ_FM_Stop);
				if (vl_error != 0) {
					printf("ConnServ_FM_Start failed with vl_error = %d\n", vl_error);
					strncpy(FMIPError, v_tatcoex_fmtx_data.error, MAX_STRNG_SIZE);
				} else {
					vl_error = tatl12_18Conn_FMTX_Audio_Configuration();
					if (vl_error != 0) {
						printf("Audio Configuration failed with vl_error = %d\n", vl_error);
						strncpy(ACError, v_tatcoex_fmtx_data.error, MAX_STRNG_SIZE);
					} else {
						vl_error = tatl12_04Conn_Management_FMTX(&v_tatcoex_fmtx_data);
					}
				}
			} else if ((v_tatcoex_fmtx_data.order == STOP) && (v_tatcoex_fmtx_data.state == ACTIVATED)) {
				vl_error = tatl12_04Conn_Management_FMTX(&v_tatcoex_fmtx_data);
				if (vl_error != 0) {
					printf("ConnServ_FM_Stop failed with vl_error = %d\n", vl_error);
					strncpy(FMIPError, v_tatcoex_fmtx_data.error, MAX_STRNG_SIZE);
				} else {
					vl_error = tatl12_18Conn_FMTX_Audio_DeConfiguration();
					if (vl_error != 0) {
						printf("Audio DeConfiguration failed with vl_error = %d\n", vl_error);
						strncpy(ACError, v_tatcoex_fmtx_data.error, MAX_STRNG_SIZE);
					}
				}
				vl_error2 = tatl12_04Conn_Management(&v_tatcoex_fmip_data, &ConnServ_FM_Start, &ConnServ_FM_Stop);
			}
		}
		break;

	case ACT_UART0:
		{
			vl_error = tatl12_04Conn_Management(&v_tatcoex_uart0_data, &ConnServ_UART_activity_Start, &ConnServ_UART_activity_Stop);
		}
		break;

	case ACT_I2S:
		{
			if ((v_tatcoex_i2s_data.order == START) && (v_tatcoex_i2s_data.state == DEACTIVATED)) {
				vl_error = tatl12_18Conn_I2S_Audio_Configuration();
				if (vl_error != 0) {
					printf("Audio configuration for I2S failed\n");
					strncpy(ACError, v_tatcoex_i2s_data.error, MAX_STRNG_SIZE);
				} else {
					vl_error = tatl12_04Conn_Management(&v_tatcoex_i2s_data, &ConnServ_I2S_activity_Start, &ConnServ_I2S_activity_Stop);
				}
			} else if ((v_tatcoex_i2s_data.order == STOP) && (v_tatcoex_i2s_data.state == ACTIVATED)) {
					vl_error = tatl12_04Conn_Management(&v_tatcoex_i2s_data, &ConnServ_I2S_activity_Start, &ConnServ_I2S_activity_Stop);
					if (vl_error != 0) {
						printf("ConnServ_I2S_Stop failed with vl_error = %d\n", vl_error);
						strncpy(ConnError, v_tatcoex_i2s_data.error, MAX_STRNG_SIZE);
					}
			}
		}
		break;

	case ACT_MSP0:
		{
			vl_error = tatl12_04Conn_Management(&v_tatcoex_msp0_data, &ConnServ_PCM_activity_Start, &ConnServ_PCM_activity_Stop);
		}
		break;

	case ACT_MC1:
		{
			if ((v_tatcoex_mc1_data.order == START) && (v_tatcoex_mc1_data.state == DEACTIVATED)) {
				if (vl_error != 0) {
					printf("ConnServ_CW1200_Start/Stop failed with vl_error = %d\n", vl_error);
					strncpy(CW1200Error, v_tatcoex_mc1_data.error, MAX_STRNG_SIZE);
				} else {
					vl_error = tatl12_04Conn_Management(&v_tatcoex_mc1_data, &ConnServ_SDIO_activity_Start, &ConnServ_SDIO_activity_Stop);
				}
			} else if ((v_tatcoex_mc1_data.order == STOP) && (v_tatcoex_mc1_data.state == ACTIVATED)) {
				vl_error = tatl12_04Conn_Management(&v_tatcoex_mc1_data, &ConnServ_SDIO_activity_Start, &ConnServ_SDIO_activity_Stop);
				if (vl_error != 0) {
					printf("ConnServ_CW1200_Stop failed with vl_error = %d\n", vl_error);
					strncpy(CW1200Error, v_tatcoex_mc1_data.error, MAX_STRNG_SIZE);
				}
			}
		}
		break;

	case ACT_WLANTX:
		{
			if ((v_tatcoex_wlantx_data.order == START) && (v_tatcoex_wlantx_data.state == DEACTIVATED)) {
				if (vl_error != 0) {
					printf("ConnServ_CW1200_Start failed with vl_error = %d\n", vl_error);
					strncpy(CW1200Error, v_tatcoex_wlantx_data.error, MAX_STRNG_SIZE);
				} else {
					vl_error = tatl12_04Conn_Management_ConnWLANTX(&v_tatcoex_wlantx_data);
				}
			} else if ((v_tatcoex_wlantx_data.order == STOP) && (v_tatcoex_wlantx_data.state == ACTIVATED)) {
				vl_error = tatl12_04Conn_Management_ConnWLANTX(&v_tatcoex_wlantx_data);
				if (vl_error != 0) {
					printf("ConnServ_WLAN_Tx_Stop failed with vl_error = %d\n", vl_error);
					strncpy(CW1200Error, v_tatcoex_wlantx_data.error, MAX_STRNG_SIZE);
				}
			}
		}
		break;


	case ACT_CONN_BTDUT:
		{
			if ((v_tatcoex_btdut_data.order == START) && (v_tatcoex_btdut_data.state == DEACTIVATED)) {
				vl_error = tatl12_04Conn_Management(&v_tatcoex_btip_data, &ConnServ_BT_Start, &ConnServ_BT_Stop);
				if (vl_error != 0) {
					printf("ConnServ_BT_Start failed with vl_error = %d\n", vl_error);
					strncpy(BTIPError, v_tatcoex_btdut_data.error, MAX_STRNG_SIZE);
				} else {
					vl_error = tatl12_04Conn_Management(&v_tatcoex_btdut_data, &ConnServ_BT_DUT_mode_Start, &ConnServ_BT_DUT_mode_Stop);
				}
			} else if ((v_tatcoex_btdut_data.order == STOP) && (v_tatcoex_btdut_data.state == ACTIVATED)) {
				vl_error = tatl12_04Conn_Management(&v_tatcoex_btdut_data, &ConnServ_BT_DUT_mode_Start, &ConnServ_BT_DUT_mode_Stop);
				if (vl_error != 0) {
					printf("ConnServ_BT_DUT_Mode_Stop failed with vl_error = %d\n", vl_error);
					strncpy(BTIPError, v_tatcoex_btdut_data.error, MAX_STRNG_SIZE);
				} else
					vl_error2 = tatl12_04Conn_Management(&v_tatcoex_btip_data, &ConnServ_BT_Start, &ConnServ_BT_Stop);
			}
		}
		break;

		/* Panel Connectivity */
#ifdef CONNECTIVITY_PANEL
	case ACT_CONN_BLEDUT:
		{

			if ((v_tatcoex_bledut_data.order == START) && (v_tatcoex_bledut_data.state == DEACTIVATED)) {
				vl_error = tatl12_04Conn_Management(&v_tatcoex_btip_data, &ConnServ_BT_Start, &ConnServ_BT_Stop);
				if (vl_error != 0) {
					printf("ConnServ_BT_Start failed with vl_error = %d\n", vl_error);
					strncpy(BTIPError, v_tatcoex_bledut_data.error, MAX_STRNG_SIZE);
				} else {
					vl_error = tatl12_04Conn_Management_BLEDUT(&v_tatcoex_bledut_data);
				}
			} else if ((v_tatcoex_bledut_data.order == STOP) && (v_tatcoex_bledut_data.state == ACTIVATED)) {
				vl_error = tatl12_04Conn_Management_BLEDUT(&v_tatcoex_bledut_data);
				if (vl_error != 0) {
					printf("ConnServ_BT_DUT_Mode_Stop failed with vl_error = %d\n", vl_error);
					strncpy(BTIPError, v_tatcoex_bledut_data.error, MAX_STRNG_SIZE);
				} else
					vl_error2 = tatl12_04Conn_Management(&v_tatcoex_btip_data, &ConnServ_BT_Start, &ConnServ_BT_Stop);
			}
		}
		break;

	case ACT_CONN_BTRX:
		{
			vl_error = tatl12_04Conn_Management_ConnBTRX(&v_tatcoex_btrxone_data);
		}
		break;

	case ACT_CONN_FMRX:
		{
			vl_error = tatl12_04Conn_Management_ConnFMRX(&v_tatcoex_fmrxone_data);
		}
		break;

	case ACT_CONN_GPS:
		{
			vl_error = tatl12_04Conn_Management_ConnGPS(&v_tatcoex_gpsv_data);
		}
		break;

	case ACT_CG2900VERSION:
		{
			vl_error = tatl12_04Conn_Management_ConnCGVER(&v_tatcoex_cgver_data);
		}
		break;
#endif

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;
}

int tatl12_01Conn_Get(struct dth_element *elem, void *value)
{
	int vl_error;

	vl_error = 0;

	switch (elem->user_data) {
	case ACT_CG2900_STATE:
		{
			*((u8 *) value) = v_tatcoex_cg2900_data.state;
			SYSLOG(LOG_DEBUG, "Get CG2900 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CG2900_ERROR:
		{
			strncpy((char *)value, v_tatcoex_cg2900_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get CG2900 error string: %s\n", (char *)value);
		}
		break;

	case ACT_CW1200_STATE:
		{
			*((u8 *) value) = v_tatcoex_cw1200_data.state;
			SYSLOG(LOG_DEBUG, "Get CW1200 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CW1200_ERROR:
		{
			strncpy((char *)value, v_tatcoex_cw1200_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get CW1200 error string: %s\n", (char *)value);
		}
		break;

	case ACT_BTIP_STATE:
		{
			*((u8 *) value) = v_tatcoex_btip_data.state;
			SYSLOG(LOG_DEBUG, "Get BT IP state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_BTIP_ERROR:
		{
			strncpy((char *)value, v_tatcoex_btip_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get BT IP error string: %s\n", (char *)value);
		}
		break;

	case ACT_FMIP_STATE:
		{
			*((u8 *) value) = v_tatcoex_fmip_data.state;
			SYSLOG(LOG_DEBUG, "Get FM IP state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_FMIP_ERROR:
		{
			strncpy((char *)value, v_tatcoex_fmip_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get FM IP error string: %s\n", (char *)value);
		}
		break;

#ifdef CONNECTIVITY_PANEL
	case ACT_GPSIP_STATE:
		{
			*((u8 *) value) = v_tatcoex_gpsip_data.state;
			SYSLOG(LOG_DEBUG, "Get GPS IP state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_GPSIP_ERROR:
		{
			strncpy((char *)value, v_tatcoex_gpsip_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get GPS IP error string: %s\n", (char *)value);
		}
		break;
#endif

	case ACT_BTTX_STATE:
		{
			*((u8 *) value) = v_tatcoex_bttx_data.state;
			SYSLOG(LOG_DEBUG, "Get BT Tx state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_BTTX_ERROR:
		{
			strncpy((char *)value, v_tatcoex_bttx_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get BT Tx error string: %s\n", (char *)value);
		}
		break;

	case ACT_FMTX_STATE:
		{
			*((u8 *) value) = v_tatcoex_fmtx_data.state;
			SYSLOG(LOG_DEBUG, "Get FM Tx state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_FMTX_ERROR:
		{
			strncpy((char *)value, v_tatcoex_fmtx_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get FM Tx error string: %s\n", (char *)value);
		}
		break;

	case ACT_WLANTX_STATE:
		{
			*((u8 *) value) = v_tatcoex_wlantx_data.state;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_WLANTX_ERROR:
		{
			strncpy((char *)value, v_tatcoex_wlantx_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get WLAN Tx error string: %s\n", (char *)value);
		}
		break;

	case ACT_UART0_STATE:
		{
			*((u8 *) value) = v_tatcoex_uart0_data.state;
			SYSLOG(LOG_DEBUG, "Get UART0 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_UART0_ERROR:
		{
			strncpy((char *)value, v_tatcoex_uart0_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get UART0 error string: %s\n", (char *)value);
		}
		break;

	case ACT_I2S_STATE:
		{
			*((u8 *) value) = v_tatcoex_i2s_data.state;
			SYSLOG(LOG_DEBUG, "Get I2S state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_I2S_ERROR:
		{
			strncpy((char *)value, v_tatcoex_i2s_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get I2S error string: %s\n", (char *)value);
		}
		break;

	case ACT_MSP0_STATE:
		{
			*((u8 *) value) = v_tatcoex_msp0_data.state;
			SYSLOG(LOG_DEBUG, "Get MSP0 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_MSP0_ERROR:
		{
			strncpy((char *)value, v_tatcoex_msp0_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get MSP0 error string: %s\n", (char *)value);
		}
		break;

	case ACT_MC1_STATE:
		{
			*((u8 *) value) = v_tatcoex_mc1_data.state;
			SYSLOG(LOG_DEBUG, "Get MC1 state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_MC1_ERROR:
		{
			strncpy((char *)value, v_tatcoex_mc1_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get MC1 error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_BTTX_CHAN:
		{
			*((u16 *) value) = v_tatcoex_bttx_data.channel;
			SYSLOG(LOG_DEBUG, "Get BT Tx channel : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_BTTX_PKTT:
		{
			*((u8 *) value) = v_tatcoex_bttx_data.pktt;
			SYSLOG(LOG_DEBUG, "Get BT Tx pktt : %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_FMTX_FREQ:
		{
			*((u32 *) value) = v_tatcoex_fmtx_data.freq;
			SYSLOG(LOG_DEBUG, "Get FM Tx channel : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_FMTX_TONEFREQ:
		{
			*((u32 *) value) = v_tatcoex_fmtx_data.tonefreq;
			SYSLOG(LOG_DEBUG, "Get FM Tx tone freq : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_FMTX_VOL:
		{
			*((u16 *) value) = v_tatcoex_fmtx_data.volume;
			SYSLOG(LOG_DEBUG, "Get FM Tx volume : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_FMTX_PWR:
		{
			*((u32 *) value) = v_tatcoex_fmtx_data.pwr;
			SYSLOG(LOG_DEBUG, "Get FM Tx power : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_WLANTX_MODE:
		{
			*((u8 *) value) = v_tatcoex_wlantx_data.mode;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx mode : %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_WLANTX_CHAN:
		{
			*((u16 *) value) = v_tatcoex_wlantx_data.channel;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx channel : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_WLANTX_RATE:
		{
			*((u16 *) value) = v_tatcoex_wlantx_data.rate;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx rate : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_WLANTX_PWRIDX:
		{
			*((float *)value) = v_tatcoex_wlantx_data.pwridx;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx pwr idx : %f\n", *((float *)value));
		}
		break;

	case ACT_COEX_WLANTX_PKTSNT:
		{
			*((u32 *) value) = v_tatcoex_wlantx_data.pktsent;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx pkt sent : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_WLANTX_PKTACK:
		{
			*((u32 *) value) = v_tatcoex_wlantx_data.pktack;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx pkt ack : %i\n", *((u32 *) value));
		}
		break;

		/* Panel Connectivity */
#ifdef CONNECTIVITY_PANEL
	case ACT_CG2900VERSION_ERROR:
		{
			strncpy((char *)value, v_tatcoex_cgver_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get CG Ver error string: %s\n", (char *)value);
		}
		break;

	case ACT_CG2900VERSION_MIN:
		{
			*((u8 *) value) = v_tatcoex_cgver_data.min;
			SYSLOG(LOG_DEBUG, "Get CG Ver min frequency ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CG2900VERSION_MAJ:
		{
			*((u8 *) value) = v_tatcoex_cgver_data.maj;
			SYSLOG(LOG_DEBUG, "Get CG Ver maj ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CG2900VERSION_RAD:
		{
			*((u8 *) value) = v_tatcoex_cgver_data.rad;
			SYSLOG(LOG_DEBUG, "Get CG Ver radio ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CG2900VERSION_PATCHID:
		{
			strncpy((char *)value, v_tatcoex_cgver_data.patchid, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get CG Ver patch id string: %s\n", (char *)value);
		}
		break;

	case ACT_CG2900VERSION_PATCHDATE:
		{
			strncpy((char *)value, v_tatcoex_cgver_data.patchdate, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get CG Ver patch id string: %s\n", (char *)value);
		}
		break;

	case ACT_CG2900VERSION_VENDOR:
		{
			*((u8 *) value) = v_tatcoex_cgver_data.vendor;
			SYSLOG(LOG_DEBUG, "Get CG Ver min frequency ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CG2900VERSION_HCI:
		{
			*((u8 *) value) = v_tatcoex_cgver_data.hci;
			SYSLOG(LOG_DEBUG, "Get CG Ver maj ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CG2900VERSION_LMP:
		{
			*((u8 *) value) = v_tatcoex_cgver_data.lmp;
			SYSLOG(LOG_DEBUG, "Get CG Ver radio ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;



	case ACT_CONN_BLEDUT_STATE:
		{
			*((u8 *) value) = v_tatcoex_bledut_data.state;
			SYSLOG(LOG_DEBUG, "Get BLE DUT state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CONN_BLEDUT_ERROR:
		{
			strncpy((char *)value, v_tatcoex_bledut_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get BLE DUT error string: %s\n", (char *)value);
		}
		break;

	case ACT_CONN_BLEDUT_FREQ:
		{
			*((u8 *) value) = v_tatcoex_bledut_data.frequency;
			SYSLOG(LOG_DEBUG, "Get BLE DUT frequency ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CONN_BLEDUT_LEN:
		{
			*((u8 *) value) = v_tatcoex_bledut_data.length;
			SYSLOG(LOG_DEBUG, "Get BLE DUT length ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CONN_BLEDUT_RCVDPKTS:
		{
			*((u16 *) value) = v_tatcoex_bledut_data.rcvdpkts;
			SYSLOG(LOG_DEBUG, "Get BLE DUT length ACTIVATED(1) DEACTIVATED(0): %i\n", *((u16 *) value));
		}
		break;

	case ACT_CONN_BTRX_STATE:
		{
			*((u8 *) value) = v_tatcoex_btrxone_data.state;
			SYSLOG(LOG_DEBUG, "Get BT Rx state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CONN_BTRX_ERROR:
		{
			strncpy((char *)value, v_tatcoex_btrxone_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get BT Rx error string: %s\n", (char *)value);
		}
		break;

	case ACT_CONN_BTRX_CHAN:
		{
			*((u16 *) value) = v_tatcoex_btrxone_data.channel;
			SYSLOG(LOG_DEBUG, "Get BT Rx channel : %i\n", *((u16 *) value));
		}
		break;

	case ACT_CONN_BTRX_ADDRIDX:
		{
			*((s16 *) value) = v_tatcoex_btrxone_data.addr_idx;
			SYSLOG(LOG_DEBUG, "Get BT Rx addr idx : %i\n", *((s16 *) value));
		}
		break;

	case ACT_CONN_BTRX_PKTT:
		{
			*((u8 *) value) = v_tatcoex_btrxone_data.pktt;
			SYSLOG(LOG_DEBUG, "Get BT Rx pktt : %i\n", *((u8 *) value));
		}
		break;

	case ACT_CONN_BTRX_NUMP:
		{
			*((u16 *) value) = v_tatcoex_btrxone_data.nump;
			SYSLOG(LOG_DEBUG, "Get BT Rx num pkts : %i\n", *((u16 *) value));
		}
		break;

	case ACT_CONN_BTRX_WRTYPE:
		{
			*((u16 *) value) = v_tatcoex_btrxone_data.wrtype;
			SYSLOG(LOG_DEBUG, "Get BT Rx wrong type : %i\n", *((u16 *) value));
		}
		break;

	case ACT_CONN_BTRX_WRLEN:
		{
			*((u16 *) value) = v_tatcoex_btrxone_data.wrlen;
			SYSLOG(LOG_DEBUG, "Get BT Rx wrong length : %i\n", *((u16 *) value));
		}
		break;

	case ACT_CONN_BTRX_ERR:
		{
			*((u16 *) value) = v_tatcoex_btrxone_data.err;
			SYSLOG(LOG_DEBUG, "Get BT Rx erroneous : %i\n", *((u16 *) value));
		}
		break;

	case ACT_CONN_BTRX_RSSI:
		{
			*((s16 *) value) = v_tatcoex_btrxone_data.rssi;
			SYSLOG(LOG_DEBUG, "Get BT Rx RSSI : %i\n", *((s16 *) value));
		}
		break;

	case ACT_CONN_BTRX_BER:
		{
			*((float *)value) = v_tatcoex_btrxone_data.ber;
			SYSLOG(LOG_DEBUG, "Get BT Rx BER : %f\n", *((float *)value));
		}
		break;

	case ACT_CONN_BTRX_BERRORS:
		{
			*((float *)value) = v_tatcoex_btrxone_data.berrors;
			SYSLOG(LOG_DEBUG, "Get BT Rx bit errors : %f\n", *((float *)value));
		}
		break;

	case ACT_CONN_FMRX_STATE:
		{
			*((u8 *) value) = v_tatcoex_fmrxone_data.state;
			SYSLOG(LOG_DEBUG, "Get FM Rx state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CONN_FMRX_ERROR:
		{
			strncpy((char *)value, v_tatcoex_fmrxone_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get FM Rx error string: %s\n", (char *)value);
		}
		break;

	case ACT_CONN_FMRX_CHAN:
		{
			*((u32 *) value) = v_tatcoex_fmrxone_data.channel;
			SYSLOG(LOG_DEBUG, "Get FM Rx channel : %i\n", *((u32 *) value));
		}
		break;

	case ACT_CONN_FMRX_RSSI:
		{
			*((u16 *) value) = v_tatcoex_fmrxone_data.rssi;
			SYSLOG(LOG_DEBUG, "Get FM Rx RSSI : %i\n", *((u16 *) value));
		}
		break;

	case ACT_GPSIP_INT:
		{
			*((u32 *) value) = v_tatcoex_gpsip_data.interval;
			SYSLOG(LOG_DEBUG, "Get GPS IP interval : %i\n", *((u32 *) value));
		}
		break;

	case ACT_CONN_GPS_TTFF:
		{
			*((u32 *) value) = v_tatcoex_gpsv_data.ttff;
			SYSLOG(LOG_DEBUG, "Get GPS TTFF : %i\n", *((u32 *) value));
		}
		break;

	case ACT_CONN_GPS_LONG:
		{
			*((float *)value) = v_tatcoex_gpsv_data.longitude;
			SYSLOG(LOG_DEBUG, "Get GPS interval ACTIVATED(1) DEACTIVATED(0): %f\n", *((float *)value));
		}
		break;

	case ACT_CONN_GPS_LAT:
		{
			*((float *)value) = v_tatcoex_gpsv_data.latitude;
			SYSLOG(LOG_DEBUG, "Get GPS interval ACTIVATED(1) DEACTIVATED(0): %f\n", *((float *)value));
		}
		break;

	case ACT_CONN_GPS_ALT:
		{
			*((u16 *) value) = v_tatcoex_gpsv_data.altitude;
			SYSLOG(LOG_DEBUG, "Get GPS interval ACTIVATED(1) DEACTIVATED(0): %i\n", *((u16 *) value));
		}
		break;

	case ACT_CONN_GPS_SNUM:
		{
			*((u8 *) value) = v_tatcoex_gpsv_data.satnum;
			SYSLOG(LOG_DEBUG, "Get GPS interval ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CONN_GPS_SNRDATA:
		{
			struct dth_array *ValTab = NULL;
			u8 *pl_DataOut = NULL;
			int i;
			ValTab = (struct dth_array *)value;
			pl_DataOut = (u8 *) ValTab->array;
			for (i = 0; i < CONNSERV_NUMBER_OF_SATELLITES; i++) {
				*pl_DataOut = v_tatcoex_gpsv_data.snrdata[i];
				pl_DataOut++;
			}
		}
		break;
#endif

	case ACT_CONN_BTDUT_STATE:
		{
			*((u8 *) value) = v_tatcoex_btdut_data.state;
			SYSLOG(LOG_DEBUG, "Get BT DUT state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_CONN_BTDUT_ERROR:
		{
			strncpy((char *)value, v_tatcoex_btdut_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get BT DUT error string: %s\n", (char *)value);
		}
		break;
	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

int tatl12_02Conn_Set(struct dth_element *elem, void *value)
{
	int vl_error;

	vl_error = 0;

	switch (elem->user_data) {
	case ACT_CG2900:
		{
			v_tatcoex_cg2900_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set CG2900 order START(0) STOP(1): %i\n", v_tatcoex_cg2900_data.order);
		}
		break;

	case ACT_CW1200:
		{
			v_tatcoex_cw1200_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set CG2900 order START(0) STOP(1): %i\n", v_tatcoex_cw1200_data.order);
		}
		break;

	case ACT_BTIP:
		{
			v_tatcoex_btip_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BT IP order START(0) STOP(1): %i\n", v_tatcoex_btip_data.order);
		}
		break;

	case ACT_FMIP:
		{
			v_tatcoex_fmip_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM IP order START(0) STOP(1): %i\n", v_tatcoex_fmip_data.order);
		}
		break;

#ifdef CONNECTIVITY_PANEL
	case ACT_GPSIP:
		{
			v_tatcoex_gpsip_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set GPSIP order START(0) STOP(1): %i\n", v_tatcoex_gpsip_data.order);
		}
		break;
#endif

	case ACT_UART0:
		{
			v_tatcoex_uart0_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set UART0 order START(0) STOP(1): %i\n", v_tatcoex_uart0_data.order);
		}
		break;

	case ACT_I2S:
		{
			v_tatcoex_i2s_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set I2S order START(0) STOP(1): %i\n", v_tatcoex_i2s_data.order);
		}
		break;

	case ACT_MSP0:
		{
			v_tatcoex_msp0_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set MSP0 order START(0) STOP(1): %i\n", v_tatcoex_msp0_data.order);
		}
		break;

	case ACT_MC1:
		{
			v_tatcoex_mc1_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set MC1 order START(0) STOP(1): %i\n", v_tatcoex_mc1_data.order);
		}
		break;

	case ACT_BTTX:
		{
			v_tatcoex_bttx_data.order = *((u8 *) value);
			v_tatcoex_btip_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Tx order START(0) STOP(1): %i\n", v_tatcoex_bttx_data.order);
		}
		break;

	case ACT_COEX_BTTX_CHAN:
		{
			v_tatcoex_bttx_data.channel = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Tx channel : %d\n", v_tatcoex_bttx_data.channel);
		}
		break;

	case ACT_COEX_BTTX_TXLEV:
		{
			v_tatcoex_bttx_data.tx_level = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Tx tx level : %d\n", v_tatcoex_bttx_data.tx_level);
		}
		break;

	case ACT_COEX_BTTX_ADDRIDX:
		{
			v_tatcoex_bttx_data.addr_idx = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Tx addr idx : %d\n", v_tatcoex_bttx_data.addr_idx);
		}
		break;

	case ACT_COEX_BTTX_PKTT:
		{
			v_tatcoex_bttx_data.pktt = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Tx pkt type : %d\n", v_tatcoex_bttx_data.pktt);
		}
		break;

	case ACT_FMTX:
		{
			v_tatcoex_fmtx_data.order = *((u8 *) value);
			v_tatcoex_fmip_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx order START(0) STOP(1): %i\n", v_tatcoex_fmtx_data.order);
		}
		break;

	case ACT_COEX_FMTX_FREQ:
		{
			v_tatcoex_fmtx_data.freq = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx channel : %d\n", v_tatcoex_fmtx_data.freq);
		}
		break;

	case ACT_COEX_FMTX_TONEFREQ:
		{
			v_tatcoex_fmtx_data.tonefreq = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx tone freq : %d\n", v_tatcoex_fmtx_data.tonefreq);
		}
		break;

	case ACT_COEX_FMTX_PWR:
		{
			v_tatcoex_fmtx_data.pwr = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx level : %d\n", v_tatcoex_fmtx_data.pwr);
		}
		break;

	case ACT_COEX_FMTX_MODE:
		{
			v_tatcoex_fmtx_data.mode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx mode : %d\n", v_tatcoex_fmtx_data.mode);
		}
		break;

	case ACT_COEX_FMTX_SRC:
		{
			v_tatcoex_fmtx_data.src = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx source : %d\n", v_tatcoex_fmtx_data.src);
		}
		break;

	case ACT_COEX_FMTX_VOL:
		{
			v_tatcoex_fmtx_data.volume = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx volume : %d\n", v_tatcoex_fmtx_data.volume);
		}
		break;

	case ACT_COEX_FMTX_EMP:
		{
			v_tatcoex_fmtx_data.emp = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx emphasis : %d\n", v_tatcoex_fmtx_data.emp);
		}
		break;

	case ACT_WLANTX:
		{
			v_tatcoex_wlantx_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx order START(0) STOP(1): %i\n", v_tatcoex_wlantx_data.order);
		}
		break;

	case ACT_COEX_WLANTX_MODE:
		{
			v_tatcoex_wlantx_data.mode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx mode : %d\n", v_tatcoex_wlantx_data.mode);
		}
		break;

	case ACT_COEX_WLANTX_CHAN:
		{
			v_tatcoex_wlantx_data.channel = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx channel : %d\n", v_tatcoex_wlantx_data.channel);
		}
		break;

	case ACT_COEX_WLANTX_BAND:
		{
			v_tatcoex_wlantx_data.band = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx band : %d\n", v_tatcoex_wlantx_data.band);
		}
		break;

	case ACT_COEX_WLANTX_RATE:
		{
			v_tatcoex_wlantx_data.rate = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx rate : %d\n", v_tatcoex_wlantx_data.rate);
		}
		break;

	case ACT_COEX_WLANTX_PWRIDX:
		{
			v_tatcoex_wlantx_data.pwridx = *((float *)value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx pwr idx : %f\n", v_tatcoex_wlantx_data.pwridx);
		}
		break;

	case ACT_COEX_WLANTX_ACK:
		{
			v_tatcoex_wlantx_data.ack = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx ack : %d\n", v_tatcoex_wlantx_data.ack);
		}
		break;

	case ACT_COEX_WLANTX_IFD:
		{
			v_tatcoex_wlantx_data.ifd = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx ifd : %d\n", v_tatcoex_wlantx_data.ifd);
		}
		break;

	case ACT_COEX_WLANTX_PKTSNT:
		{
			v_tatcoex_wlantx_data.pktsent = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx pkt snt : %d\n", v_tatcoex_wlantx_data.pktsent);
		}
		break;

	case ACT_COEX_WLANTX_PKTACK:
		{
			v_tatcoex_wlantx_data.pktack = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN pkt ack mode : %d\n", v_tatcoex_wlantx_data.pktack);
		}
		break;


	case ACT_CONN_BTDUT:
		{
			v_tatcoex_btdut_data.order = *((u8 *) value);
			v_tatcoex_btip_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BT IP order START(0) STOP(1): %i\n", v_tatcoex_btdut_data.order);
		}
		break;

		/* Panel Connectivity */
#ifdef CONNECTIVITY_PANEL
	case ACT_CONN_BLEDUT:
		{
			v_tatcoex_bledut_data.order = *((u8 *) value);
			v_tatcoex_btip_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BLEDUT IP order START(0) STOP(1): %i\n", v_tatcoex_bledut_data.order);
		}
		break;

	case ACT_CONN_BLEDUT_MODE:
		{
			v_tatcoex_bledut_data.mode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BLE DUT mode : %d\n", v_tatcoex_bledut_data.mode);
		}
		break;

	case ACT_CONN_BLEDUT_FREQ:
		{
			v_tatcoex_bledut_data.frequency = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BLE DUT frequency : %d\n", v_tatcoex_bledut_data.frequency);
		}
		break;

	case ACT_CONN_BLEDUT_LEN:
		{
			v_tatcoex_bledut_data.length = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BLE DUT mode : %d\n", v_tatcoex_bledut_data.length);
		}
		break;

	case ACT_CONN_BLEDUT_PLD:
		{
			v_tatcoex_bledut_data.payload = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BLE DUT mode : %d\n", v_tatcoex_bledut_data.payload);
		}
		break;

	case ACT_CONN_BTRX:
		{
			v_tatcoex_btrxone_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BT IP order START(0) STOP(1): %i\n", v_tatcoex_btrxone_data.order);
		}
		break;

	case ACT_CONN_BTRX_CHAN:
		{
			v_tatcoex_btrxone_data.channel = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Rx channel : %d\n", v_tatcoex_btrxone_data.channel);
		}
		break;

	case ACT_CONN_BTRX_ADDRIDX:
		{
			v_tatcoex_btrxone_data.addr_idx = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Rx addridx : %d\n", v_tatcoex_btrxone_data.addr_idx);
		}
		break;

	case ACT_CONN_BTRX_PKTT:
		{
			v_tatcoex_btrxone_data.pktt = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Rx pktt : %d\n", v_tatcoex_btrxone_data.pktt);
		}
		break;

	case ACT_CONN_BTRX_RSSI:
		{
			v_tatcoex_btrxone_data.rssi = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Rx rssi : %d\n", v_tatcoex_btrxone_data.rssi);

		}

	case ACT_CONN_BTRX_BER:
		{
			v_tatcoex_btrxone_data.ber = *((float *)value);
			SYSLOG(LOG_DEBUG, "Set BT Rx ber : %f\n", v_tatcoex_btrxone_data.ber);

		}

	case ACT_CONN_FMRX:
		{
			v_tatcoex_fmrxone_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BT IP order START(0) STOP(1): %i\n", v_tatcoex_fmrxone_data.order);
		}
		break;

	case ACT_CONN_FMRX_CHAN:
		{
			v_tatcoex_fmrxone_data.channel = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Rx channel : %d\n", v_tatcoex_fmrxone_data.channel);
		}
		break;

	case ACT_CONN_FMRX_MODE:
		{
			v_tatcoex_fmrxone_data.mode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Rx mode : %d\n", v_tatcoex_fmrxone_data.mode);
		}
		break;

	case ACT_CONN_FMRX_EMP:
		{
			v_tatcoex_fmrxone_data.emp = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Rx emphasis : %d\n", v_tatcoex_fmrxone_data.emp);
		}
		break;

	case ACT_CONN_FMRX_RSSI:
		{
			v_tatcoex_fmrxone_data.rssi = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Rx rssi : %d\n", v_tatcoex_fmrxone_data.rssi);

		}
		break;

	case ACT_GPSIP_INT:
		{
			v_tatcoex_gpsip_data.interval = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set GPS IP interval : %d\n", v_tatcoex_gpsip_data.interval);

		}
		break;

	case ACT_COEX_GPS:
		{
			v_tatcoex_gpsv_data.order = *((u8 *) value);;
			SYSLOG(LOG_DEBUG, "Set GPS order START(0) STOP(1): %i\n", v_tatcoex_gpsv_data.order);
		}
		break;
#endif

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

void tatl12_03Conn_Init_Data(DTH_CONN_DATA *data)
{
	data->order = STOP;
	data->state = DEACTIVATED;
	data->error = NO_ERROR;
	data->idx = 0;
}

void tatl12_03Conn_Init_BTTX_Data(DTH_BTTX_DATA *data)
{
	data->order = STOP;
	data->state = DEACTIVATED;
	data->channel = 39;
	data->tx_level = 0;
	data->addr_idx = 0;
	data->pktt = 2;
	data->error = NO_ERROR;
}

void tatl12_03Conn_Init_FMTX_Data(DTH_FMTX_DATA *data)
{
	data->order = STOP;
	data->state = DEACTIVATED;
	data->freq = 95000;
	data->pwr = 100;
	data->mode = 0;
	data->src = 0;
	data->volume = 1;
	data->emp = 0;
	data->tonefreq = 1000;
	data->error = NO_ERROR;
}

#ifdef CONNECTIVITY_PANEL
void tatl12_03Conn_Init_BLEDUT_Data(DTH_BLEDUT_DATA *data)
{
	data->order = STOP;
	data->state = DEACTIVATED;
	data->mode = 0;
	data->frequency = 20;
	data->length = 37;
	data->payload = 0;
	data->rcvdpkts = 0;
	data->error = NO_ERROR;
}

void tatl12_03Conn_Init_GPS_Data(DTH_GPS_DATA *data)
{
	data->order = STOP;
	data->state = DEACTIVATED;
	data->error = NO_ERROR;
	data->idx = 0;
	data->interval = 500;
}

void tatl12_03Conn_Init_CGVER_Data(DTH_CGVER_DATA *data)
{
	data->order = STOP;
	data->error = NO_ERROR;
	data->min = 0;
	data->maj = 0;
	data->rad = 0;
	data->patchid = calloc(strlen("XXXXX") + 1, sizeof(char));
	strncpy(data->patchid, "XXXXX", strlen("XXXXX"));
	data->patchdate = calloc(strlen("XXXXXXXXXXXXXXXXX") + 1, sizeof(char));
	strncpy(data->patchdate, "XXXXXXXXXXXXXXXXX", strlen("XXXXXXXXXXXXXXXXX"));
	data->vendor = 0;
	data->hci = 0;
	data->lmp = 0;
}
#endif

void tatl12_03Conn_Init_WLANTX_Data(DTH_WLANTX_DATA *data)
{
	data->order = STOP;
	data->error = NO_ERROR;
	data->mode = 2;
	data->channel = 6;
	data->band = 0;
	data->rate = 11;
	data->pwridx = 18.0;
	data->ack = 0;
	data->ifd = 0;
	data->pktsent = 0;
	data->pktack = 0;
}

void tatl12_03Conn_Init_WLANRX_Data(DTH_WLANRX_DATA *data)
{
	data->order = STOP;
	data->error = NO_ERROR;
	data->modeband = 2;
	data->channel = 6;
	data->pktrx = 0;
	data->errors = 0;
	data->crcerrors = 0;
}

/**
 *  Manage START/STOP/STANDBY of all connectivity buses.
 *  From state=DEACTIVATED we can pass ACTIVATED (order=START)
 *  From state=ACTIVATED we can pass DEACTIVATED (order=STOP)
 *  or ON_STANDBY (order=STANDBY)
 *  From state=ON_STANDBY we can pass ACTIVATED (order=START)
 *  or DEACTIVATED (order=STOP)
 */

#ifdef CONNECTIVITY_PANEL
/**
 *  Manage START/STOP/STANDBY of GPS.
 *  @param[in] data data needed to manage connectivity buses.
 * @retval 0 success.
 * @retval -1 if an error occured while processing.
 */
static int tatl12_04Conn_Management_GPS(DTH_GPS_DATA *data)
{
	uint32_t vl_error;

	vl_error = 0;
	printf("DTH_GPS_DATA idx = %d\n", data->idx);
	/* Idx is ++ when START is called and -- when STOP is called. STANDBY has no influence in idx */
	if ((data->order == START) && ((data->state == DEACTIVATED) || (data->state == ON_STANDBY)) && (data->idx == 0)) {	/* first start */
		vl_error = ConnServ_GPS_Start(data->interval);
		if (vl_error == 0) {
			if (data->state == DEACTIVATED) {
				data->idx = (data->idx) + 1;
			}
			data->state = ACTIVATED;
			data->error = NO_ERROR;
		} else {
			printf("ConnServ_GPS_Start failed with vl_error=%d\n", vl_error);
			data->error = ConnError;
		}
	} else if ((data->order == START) && ((data->state == DEACTIVATED) || (data->state == ON_STANDBY))) {	/* not the first start */
		data->idx = (data->idx) + 1;
	} else if ((data->order == STOP) && ((data->state == ACTIVATED) || (data->state == ON_STANDBY)) && (data->idx == 1)) {	/* only one process left */

		vl_error = ConnServ_GPS_Stop();
		if (vl_error == 0) {
			data->state = DEACTIVATED;
			data->error = NO_ERROR;
			data->idx = (data->idx) - 1;
		} else {
			printf("ConnServ_GPS_Stop failed with vl_error=%d\n", vl_error);
			data->error = ConnError;
		}
	} else if ((data->order == STOP) && ((data->state == ACTIVATED) || (data->state == ON_STANDBY)) && (((data->idx) - 1) > 0)) {	/* there are many processes */
		data->idx = (data->idx) - 1;
	} else if ((data->order == STANDBY) && (data->state == ACTIVATED)) {
		vl_error = ConnServ_GPS_Standby();
		if (vl_error == 0) {
			data->state = ON_STANDBY;
			data->error = NO_ERROR;
		} else {
			printf("ConnServ_GPS_Standby failed with vl_error=%d\n", vl_error);
			data->error = ConnError;
		}
	}
	return vl_error;
}

static int tatl12_04Conn_Management_ConnGPS(DTH_GPSV_DATA *data)
{
	uint32_t vl_error = 0;
	int8_t l_TimeOfLock;
	t_ConnServGPS_Position l_Position;
	t_ConnServGPS_Satellites l_Satellites;

	vl_error = ConnServ_GPS_GetLocation(&l_TimeOfLock, &l_Position, &l_Satellites);
	if (vl_error == 0) {
		data->ttff = l_TimeOfLock;
		data->longitude = l_Position.longitude;
		data->latitude = l_Position.latitude;
		data->altitude = l_Position.altitude;
		data->satnum = l_Satellites.SatsInView;
	} else {
		printf("ConnServ_GPS_GetLocation failed with vl_error=%d\n", vl_error);
	}

	return vl_error;
}
#endif

int tatl12_18Conn_FMRX_Audio_Configuration()
{
	uint32_t ulCmdStatus = 0;
	/* Select FMRX as source */
	if (CONNSERV_NO_ERROR == ulCmdStatus) {
		ulCmdStatus = SetSrc(AUDIO_DEVICE_0, CODEC_SRC_FM_RX);
		if (CONNSERV_NO_ERROR != ulCmdStatus) {
			printf("FM RX: Failed to set source to FM RX on AB8500\n");
		}
	}

	/* Unmute FMRX */
	if (CONNSERV_NO_ERROR == ulCmdStatus) {
		ulCmdStatus = SetMute(AUDIO_DEVICE_0, CODEC_CAPTURE, STREAM_UNMUTED);
		if (CONNSERV_NO_ERROR != ulCmdStatus) {
			printf("FM RX: Failed to unmute FM RX source on AB8500\n");
		}
	}

	/* Set Volume of FMRX to maximum */
	if (CONNSERV_NO_ERROR == ulCmdStatus) {
		ulCmdStatus = SetVolume(AUDIO_DEVICE_0, CODEC_CAPTURE, 100, 100);
		if (CONNSERV_NO_ERROR != ulCmdStatus) {
			printf("FM RX: Failed to set volume of FM RX source on AB8500\n");
		}
	}

	/* Select the Headset as sink */
	if (CONNSERV_NO_ERROR == ulCmdStatus) {
		ulCmdStatus = SetSink(AUDIO_DEVICE_0, CODEC_DEST_HEADSET);
		if (CONNSERV_NO_ERROR != ulCmdStatus) {
			printf("FM RX: failed to Set Sink to Headset on AB8500\n");
		}
	}

	/* Unmute Headset */
	if (CONNSERV_NO_ERROR == ulCmdStatus) {
		ulCmdStatus = SetMute(AUDIO_DEVICE_0, CODEC_PLAYBACK, STREAM_UNMUTED);
		if (CONNSERV_NO_ERROR != ulCmdStatus) {
			printf("FM RX: failed to unmute Headset on AB8500\n");
		}
	}

	/* Set Volume of Headset to maximum */
	if (CONNSERV_NO_ERROR == ulCmdStatus) {
		ulCmdStatus = SetVolume(AUDIO_DEVICE_0, CODEC_PLAYBACK, 100, 100);
		if (CONNSERV_NO_ERROR != ulCmdStatus) {
			printf("FM RX: failed to set the volume of the Headset on AB8500\n");
		}
	}

	/* Start the Stream */
	if (CONNSERV_NO_ERROR == ulCmdStatus) {
		ulCmdStatus = StartPCMTransfer (AUDIO_DEVICE_0, CODEC_PLAYBACK, DIGITAL_LOOPBACK_MODE, NULL);
		if (CONNSERV_NO_ERROR != ulCmdStatus) {
			printf("FM RX: failed to Start FMRX to Headset stream on AB8500\n");
		}
	}

	/* Configure AB8500 I2S port to sample Audio on the rising edge (default is falling edge) */
	if (CONNSERV_NO_ERROR == ulCmdStatus) {
		system("sk-ab w 0xD1E 0x18");
	}

	return ulCmdStatus;
}

int tatl12_18Conn_FMRX_Audio_DeConfiguration()
{
	uint32_t ulCmdStatus = 0;
	ulCmdStatus = StopPCMTransfer(AUDIO_DEVICE_0, CODEC_PLAYBACK);

	if (0 != ulCmdStatus)
		printf("[FM Rx Stop I2S] Error while stopping I2S on AB/DB 8500. Carrying on anyway");

	return ulCmdStatus;
}

#ifdef MSP_config_audio
static int tatl12_18Conn_MSP0_Audio_Configuration()
{
	volatile uint32_t data[HCI_BUF_MAX_DATA];
	uint32_t pattern1 = 0x12345678;
	volatile uint32_t tmp_reg;
	int loop;
	char found;

	printf("[_DB8500_MSP0] LOAD REGISTER\n");

	/* Config to start DB8500 MSP0 GPIO12 - GPIO15 */
	ConnEngine_GPIO(GPIO_MSP0_TX, GPIO_ALTA, 1);
	ConnEngine_GPIO(GPIO_MSP0_FS, GPIO_ALTA, 1);
	ConnEngine_GPIO(GPIO_MSP0_CLK, GPIO_ALTA, 1);
	ConnEngine_GPIO(GPIO_MSP0_RX, GPIO_ALTA, 1);

	/* Peripheral bus clock for peripheral 1 (MSP0) activation */
	ConnEngine_8500RegWR(0x8012F000, 0x00007FFF);
	/* kernel clock for peripheral 1 (MSP0) activation */
	ConnEngine_8500RegWR(0x8012F008, 0x00007FFF);

	sleep(1);

	/* Configuration of PRCMU register to 15.36 MHz ( (PLL @ 384 MHz) / 25), MSP0 clock enabled */
	ConnEngine_8500RegWR(0x8015701C, 0x00000159);
	/* Configuration of register SRG (15.36 MHz / (29+1)) */
	ConnEngine_8500RegWR(0x80123010, 0x001F3C1D);

	/* GCR register configuration */
	ConnEngine_8500RegWR(0x80123004, 0x0035D951);

	/* by default the 8 slots are enables for transmitting */
	/* TCF register */
	ConnEngine_8500RegWR(((MSP_0_REG_BASE_ADDR) | (TCF_REGISTER)), (uint32_t) (0x00000007));

	/* by default the 8 slots are enables for receiving */
	/* RCF register */
	ConnEngine_8500RegWR(((MSP_0_REG_BASE_ADDR) | (RCF_REGISTER)), (uint32_t) (0x00000007));

	sleep(1);

	/* Send a pattern to PCM bus */
	ConnEngine_8500RegWR((MSP_0_REG_BASE_ADDR) | (DR_REGISTER), pattern1);

	/* Verify data we received */
	loop = 0;
	found = FALSE;

	while ((loop < HCI_BUF_MAX_DATA)) {
		tmp_reg = ConnEngine_8500RegRD((MSP_0_REG_BASE_ADDR) | (FLR_REGISTER));

		/* read Rx FIFO */
		if ((tmp_reg & RFE_BIT) == 0) {
			uint32_t Data;

			Data = ConnEngine_8500RegRD((MSP_0_REG_BASE_ADDR) | (DR_REGISTER));

			data[loop++] = Data;

			/* verify if pattern is found */
			if (Data != 0x00000000) {
				loop = HCI_BUF_MAX_DATA;
				found = TRUE;
			}
		}
	}

	if (found == TRUE) {
		printf("[MSP0] Loop back PCM DB8500 / CG2900 success\n");

		for (loop = 0; loop < (HCI_BUF_MAX_DATA / 10); loop++) {
			if (data[loop])
				printf("\n [MSP0] data %02d value %08x\n", loop, data[loop]);

		}
	} else {
		printf("[MSP0] !!! Loop back PCM DB8500 / CG2900 failure\n");
#ifdef MSP_DEBUG
		/* Display received data */
		for (loop = 0; loop < HCI_BUF_MAX_DATA; loop++)
			printf("[MSP0] data %02d value %08x\n", loop, data[loop]);
#endif
	}

	printf("[_DB8500_MSP0] END\n");

	return 0;
}

static int tatl12_18Conn_MSP0_Audio_DeConfiguration()
{
	printf("[MSP0] ConnEngine_DB8500_MSP0_OFF:  entering\n");

	ConnEngine_8500RegWR(((MSP_0_REG_BASE_ADDR) | (TCF_REGISTER)),
			     /* 8 slots are disable for transmiting */
			     (uint32_t) (0x00000000));

	/* RCE0 register */
	ConnEngine_8500RegWR(((MSP_0_REG_BASE_ADDR) | (RCF_REGISTER)),
			     /* 8 slots are disable for receiving */
			     (uint32_t) (0x00000000));

	ConnEngine_8500RegWR(((MSP_0_REG_BASE_ADDR) | (TCE0_REGISTER)),
			     /* 8 slots are disable for transmiting */
			     (uint32_t) (0x00000000));

	/* RCE0 register */
	ConnEngine_8500RegWR(((MSP_0_REG_BASE_ADDR) | (RCE0_REGISTER)),
			     /* 8 slots are disable */
			     (uint32_t) (0x00000000));

	/* MCR register */
	ConnEngine_8500RegWR(((MSP_0_REG_BASE_ADDR) | (MCR_REGISTER)), 0);

	/* GCR register configuration */
	ConnEngine_8500RegWR(0x80123004, 0x00000000);

	printf("[MSP0] ConnEngine_DB8500_MSP0_OFF: exiting\n");

	return 0;
}
#endif
