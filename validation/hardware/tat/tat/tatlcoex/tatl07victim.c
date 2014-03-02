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

#include "tatlcoex.h"

char *RfSetError = "Unable to set RF parameter";
char *RfStartRxError = "Unable to start RX";
char *RfStartGetRfInfoError = "Unable to start RX";
char *RfWriteFileError = "Unable to write rssi measurement in log file";
char *RfRssiError = "Unable to get rssi measurement";
char *VictimSystemError = "Undefined victim system";
char *RfChannelLimit = "First/Last channel error";

char *BTIPStartVError = "Unable to start BT IP";
char *BTIPStopVError = "Unable to stop BT IP";
char *BTRxStartError = "Unable start BT Rx";
char *BTRxStopError = "Unable stop BT Rx";
char *BTTxStartError = "Unable start BT Tx";
char *BTTxStopError = "Unable stop BT Tx";
char *FMIPStartVError = "Unable to start FM IP";
char *FMIPStopVError = "Unable to stop FM IP";
char *FMRxStartError = "Unable start FM Rx";
char *FMRxStopError = "Unable stop FM Rx";
char *FMALLResultIs = "RSSI results are available";
char *FMTxStartError = "Unable start FM Tx";
char *FMTxStopError = "Unable stop FM Tx";
char *GPSIPStartVError = "Unable to start GPS IP";
char *GPSIPStopVError = "Unable to stop GPS IP";
char *GPSGetLocError = "Unable to get location";
char *CW1200StartVError = "Unable to start CW1200";
char *CW1200StopVError = "Unable to stop CW1200";
char *WLANRxStartError = "Unable to start WLAN Rx";
char *WLANRxStopError = "Unable to stop WLAN Rx";
char *WLANTxStartError = "Unable to start WLAN Tx";
char *WLANTxStopError = "Unable to stop WLAN Tx";

char *ACVErrror = "Unable to configure/deconfigure audio";


/**
 *  Initialize data structure needed for RF activity generation.
 */
static void tatl07_04Init_Rf_Data()
{
	v_tatcoex_2grfone_data.order = STOP;
	v_tatcoex_2grfone_data.state = DEACTIVATED;
	v_tatcoex_2grfone_data.error = NO_ERROR;
	v_tatcoex_2grfone_data.type = RF2G;
	v_tatcoex_2grfone_data.firstArfcn = 0;
	v_tatcoex_2grfone_data.lastArfcn = 0;
	v_tatcoex_2grfone_data.band = 0;
	v_tatcoex_2grfone_data.step = 0;
	v_tatcoex_2grfone_data.channel = 0;
	v_tatcoex_2grfone_data.monitor = 0;
	v_tatcoex_2grfone_data.operationMode = 0;
	v_tatcoex_2grfone_data.afcTable = 0;
	v_tatcoex_2grfone_data.afcValue = 0;
	v_tatcoex_2grfone_data.expectedPower = 0;

	v_tatcoex_3grfone_data.order = STOP;
	v_tatcoex_3grfone_data.state = DEACTIVATED;
	v_tatcoex_3grfone_data.error = NO_ERROR;
	v_tatcoex_3grfone_data.type = RF3G;
	v_tatcoex_3grfone_data.firstArfcn = 0;
	v_tatcoex_3grfone_data.lastArfcn = 0;
	v_tatcoex_3grfone_data.band = 0;
	v_tatcoex_3grfone_data.step = 0;
	v_tatcoex_3grfone_data.channel = 0;
	v_tatcoex_3grfone_data.operationMode = 0;
	v_tatcoex_3grfone_data.afcTable = 0;
	v_tatcoex_3grfone_data.afcValue = 0;
	v_tatcoex_3grfone_data.expectedPower = 0;

	v_tatcoex_2grfall_data.order = STOP;
	v_tatcoex_2grfall_data.state = DEACTIVATED;
	v_tatcoex_2grfall_data.error = NO_ERROR;
	v_tatcoex_2grfall_data.type = RF2G;
	v_tatcoex_2grfall_data.firstArfcn = 0;
	v_tatcoex_2grfall_data.lastArfcn = 0;
	v_tatcoex_2grfall_data.band = 0;
	v_tatcoex_2grfall_data.step = 0;
	v_tatcoex_2grfall_data.channel = 0;
	v_tatcoex_2grfall_data.operationMode = 0;
	v_tatcoex_2grfall_data.afcTable = 0;
	v_tatcoex_2grfall_data.afcValue = 0;
	v_tatcoex_2grfall_data.expectedPower = 0;

	v_tatcoex_3grfall_data.order = STOP;
	v_tatcoex_3grfall_data.state = DEACTIVATED;
	v_tatcoex_3grfall_data.error = NO_ERROR;
	v_tatcoex_3grfall_data.type = RF3G;
	v_tatcoex_3grfall_data.firstArfcn = 0;
	v_tatcoex_3grfall_data.lastArfcn = 0;
	v_tatcoex_3grfall_data.band = 0;
	v_tatcoex_3grfall_data.step = 0;
	v_tatcoex_3grfall_data.channel = 0;
	v_tatcoex_3grfall_data.operationMode = 0;
	v_tatcoex_3grfall_data.afcTable = 0;
	v_tatcoex_3grfall_data.afcValue = 0;
	v_tatcoex_3grfall_data.expectedPower = 0;
}

/**
 *  Initialize data structure needed for FM activity generation.
 */
static void tatl07_10Init_FM_Data()
{
	v_tatcoex_fmrxone_data.order = STOP;
	v_tatcoex_fmrxone_data.state = DEACTIVATED;
	v_tatcoex_fmrxone_data.channel = 95000;
	v_tatcoex_fmrxone_data.mode = 0;
	v_tatcoex_fmrxone_data.rssi = 0;
	v_tatcoex_fmrxone_data.emp = 0;
	v_tatcoex_fmrxone_data.error = NO_ERROR;

	v_tatcoex_fmrxall_data.order = STOP;
	v_tatcoex_fmrxall_data.state = DEACTIVATED;
	v_tatcoex_fmrxall_data.fchannel = 70000;
	v_tatcoex_fmrxall_data.lchannel = 108000;
	v_tatcoex_fmrxall_data.step = 100;
	v_tatcoex_fmrxall_data.hold = 50;
	v_tatcoex_fmrxall_data.mode = 0;
	v_tatcoex_fmrxall_data.emp = 0;
	v_tatcoex_fmrxall_data.rssi = 0;
	v_tatcoex_fmrxall_data.error = NO_ERROR;
	v_tatcoex_fmrxall_data.rssifile = calloc(strlen(FM_RESULTS_PATH) + 1, sizeof(char));
	strncpy(v_tatcoex_fmrxall_data.rssifile, FM_RESULTS_PATH,
	strlen(FM_RESULTS_PATH));
}

/**
 *  Initialize data structure needed for BT activity generation.
 */
static void tatl07_13Init_BT_Data()
{
	v_tatcoex_btrxone_data.order = STOP;
	v_tatcoex_btrxone_data.state = DEACTIVATED;
	v_tatcoex_btrxone_data.channel = 39;
	v_tatcoex_btrxone_data.addr_idx = 0;
	v_tatcoex_btrxone_data.pktt = 2;
	v_tatcoex_btrxone_data.error = NO_ERROR;
	v_tatcoex_btrxone_data.nump = 0;
	v_tatcoex_btrxone_data.wrtype = 0;
	v_tatcoex_btrxone_data.wrlen = 0;
	v_tatcoex_btrxone_data.connectionhandle = 1;
	v_tatcoex_btrxone_data.err = 0;
	v_tatcoex_btrxone_data.rssi = 0;
	v_tatcoex_btrxone_data.ber = 0;

	v_tatcoex_btrxall_data.order = STOP;
	v_tatcoex_btrxall_data.state = DEACTIVATED;
	v_tatcoex_btrxall_data.channel = 39;
	v_tatcoex_btrxall_data.addr_idx = 0;
	v_tatcoex_btrxall_data.pktt = 2;
	v_tatcoex_btrxall_data.error = NO_ERROR;
	v_tatcoex_btrxall_data.nump = 0;
	v_tatcoex_btrxall_data.wrtype = 0;
	v_tatcoex_btrxall_data.wrlen = 0;
	v_tatcoex_btrxall_data.err = 0;
	v_tatcoex_btrxall_data.rssi = 0;
	v_tatcoex_btrxall_data.ber = 0;
}

/**
 *  Initialize data structure needed for GPS activity generation.
 */
static void tatl07_16Init_GPSV_Data()
{
	int i = 0;

	v_tatcoex_gpsv_data.order = STOP;
	v_tatcoex_gpsv_data.state = DEACTIVATED;
	v_tatcoex_gpsv_data.error = NO_ERROR;

	v_tatcoex_gpsv_data.interval = 500;
	v_tatcoex_gpsv_data.ttff = 0;
	v_tatcoex_gpsv_data.longitude = 0;
	v_tatcoex_gpsv_data.latitude = 0;
	v_tatcoex_gpsv_data.altitude = 0;
	v_tatcoex_gpsv_data.satnum = 0;

	for (i = 0; i < CONNSERV_NUMBER_OF_SATELLITES; i++)
		v_tatcoex_gpsv_data.snrdata[i] = 0;
}

/**
 *  Set 2G input parameters thanks to tatlrf library.
 *  @param[in] rfData set of data to be set.
 *  @retval 0 success.
 *  @retval errorCode if an error occures while processing.
 */
static int tatl07_05Set_All_2G_Rf_Parameter(DTH_RF_SYSTEM_DATA *rfData)
{
	int vl_error = 0;
	u32 value = 0;
	struct dth_element elem;

	elem.type = DTH_TYPE_U32;
	elem.user_data = ACT_RX2G5_FREQBAND;
	vl_error = DthRf_StartRx2G5Parm_set(&elem, (void *)&rfData->band);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U16;
	elem.user_data = ACT_RX2G5_CHANNEL;
	vl_error = DthRf_StartRx2G5Parm_set(&elem, (void *)&rfData->channel);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U16;
	elem.user_data = ACT_RX2G5_MONITOR;
	vl_error = DthRf_StartRx2G5Parm_set(&elem, (void *)&rfData->monitor);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U8;
	elem.user_data = ACT_RX2G5_AFCTAB;
	vl_error = DthRf_StartRx2G5Parm_set(&elem, (void *)&rfData->afcTable);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U32;
	elem.user_data = ACT_RX2G5_AFCVAL;
	vl_error = DthRf_StartRx2G5Parm_set(&elem, (void *)&rfData->afcValue);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U8;
	elem.user_data = ACT_RX2G5_AGCMODE;
	vl_error = DthRf_StartRx2G5Parm_set(&elem, (void *)&rfData->agcMode);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_S16;
	elem.user_data = ACT_RX2G5_EXPECTPW;
	vl_error = DthRf_StartRx2G5Parm_set(&elem, (void *)&rfData->expectedPower);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U8;
	elem.user_data = ACT_RX2G5_OPMODE;
	vl_error = DthRf_StartRx2G5Parm_set(&elem, (void *)&rfData->operationMode);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U32;
	elem.user_data = ACT_RX2G5;
	vl_error = DthRf_StartRx2G5_set(&elem, (void *)&value);
	if (vl_error != 0)
		return vl_error;

	return vl_error;
}

/**
 *  Set 3G input parameters thanks to tatlrf library.
 *  @param[in] rfData set of data to be set.
 *  @retval 0 success.
 *  @retval errorCode if an error occures while processing.
 */
static int tatl07_06Set_All_3G_Rf_Parameter(DTH_RF_SYSTEM_DATA *rfData)
{
	int vl_error = 0;
	u32 value = 0;
	struct dth_element elem;

	elem.type = DTH_TYPE_U32;
	elem.user_data = ACT_RX3G_FREQBAND;
	vl_error = DthRf_StartRx3G_Parm_set(&elem, (void *)&rfData->band);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U16;
	elem.user_data = ACT_RX3G_UARFCN;
	vl_error = DthRf_StartRx3G_Parm_set(&elem, (void *)&rfData->channel);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U8;
	elem.user_data = ACT_RX3G_AFC_TABLE;
	vl_error = DthRf_StartRx3G_Parm_set(&elem, (void *)&rfData->afcTable);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U32;
	elem.user_data = ACT_RX3G_MANUAL_AFC;
	vl_error = DthRf_StartRx3G_Parm_set(&elem, (void *)&rfData->afcValue);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U8;
	elem.user_data = ACT_RX3G_AGCMODE;
	vl_error = DthRf_StartRx3G_Parm_set(&elem, (void *)&rfData->agcMode);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_S16;
	elem.user_data = ACT_RX3G_RX_LEVEL;
	vl_error = DthRf_StartRx3G_Parm_set(&elem, (void *)&rfData->expectedPower);
	if (vl_error != 0)
		return vl_error;

	elem.type = DTH_TYPE_U32;
	elem.user_data = ACT_RX3G;
	vl_error = DthRf_StartRx3G_set(&elem, (void *)&value);
	if (vl_error != 0)
		return vl_error;

	return vl_error;
}

/**
 *  Write RSSI measurement in a log file.
 *  @param[in] pathName log file path.
 *  @param[in] channel value of the selected channel.
 *  @param[in] rssi last rssi measurement.
 *  @retval 0 success.
 *  @retval errorCode if an error occures while processing.
 */
static int tatl07_07Write_Rssi_In_Log_File(char *pathName, int channel, float rssi)
{
	int fd = 0;
	int vl_error = 0;
	char string[MAX_STRNG_SIZE];

	memset(string, '\0', MAX_STRNG_SIZE);

	/* File creation. */
	fd = open(pathName, O_CREAT | O_RDWR, 0666);
	if (fd < 0) {
		SYSLOG(LOG_ERR, "Error during opening %s file: %s\n", pathName, strerror(errno));
		return vl_error;
	}

	/* Set offset at the end of the file. */
	vl_error = lseek(fd, 0, SEEK_END);
	if (vl_error < 0) {
		SYSLOG(LOG_ERR, "Error during lseek operation in %s file: %s\n", pathName, strerror(errno));
		return vl_error;
	}

	/* Prepare the string to be written. */
	vl_error = snprintf(string, MAX_STRNG_SIZE, "RSSI measured for channel %i: %lf\n", channel, rssi);
	if (vl_error < 0) {
		SYSLOG(LOG_ERR, "fprintf error: %s\n", strerror(errno));
		close(fd);
		return vl_error;
	}

	/* Write the string in the file. */
	vl_error = write(fd, string, strlen(string));
	if (vl_error < 0) {
		SYSLOG(LOG_ERR, "Error during writing into %s file: %s\n", pathName, strerror(errno));
		close(fd);
		return vl_error;
	}

	close(fd);
	return vl_error;
}

/**
 *  Start RX 2G5/3G for one channel and perform continous RSSI measuremrnt.
 * 	@param[in] threadData data needed for processing.
 */
static void tatl07_08Rf_One_Channel_Process(void *threadData)
{
	DTH_RF_SYSTEM_DATA *rfData;
	struct dth_element elem;

	rfData = (DTH_RF_SYSTEM_DATA *) threadData;

	rfData->state = ACTIVATED;
	if (rfData->type == RF2G) {
		/* Set all RX2G5 input parameters. */
		if (tatl07_05Set_All_2G_Rf_Parameter(rfData) != 0) {
			rfData->state = DEACTIVATED;
			rfData->order = STOP;
			rfData->error = RfSetError;
			pthread_exit(NULL);
		}

		/* Start RX 2G5 for a single channel. */
		elem.type = DTH_TYPE_U32;
		elem.user_data = ACT_RX2G5;
		if (DthRf_StartRx2G5_exec(&elem) != 0) {
			rfData->state = DEACTIVATED;
			rfData->order = STOP;
			rfData->error = RfStartRxError;
			pthread_exit(NULL);
		}
	} else if (rfData->type == RF3G) {
		/* Set all RX3G input parameters. */
		if (tatl07_06Set_All_3G_Rf_Parameter(rfData) != 0) {
			rfData->state = DEACTIVATED;
			rfData->order = STOP;
			rfData->error = RfSetError;
			pthread_exit(NULL);
		}

		/* Start RX 3G */
		elem.type = DTH_TYPE_U32;
		elem.user_data = ACT_RX3G;
		if (DthRf_StartRx3G_exec(&elem) != 0) {
			rfData->state = DEACTIVATED;
			rfData->order = STOP;
			rfData->error = RfStartRxError;
			pthread_exit(NULL);
		}
	} else {
		rfData->state = DEACTIVATED;
		rfData->order = STOP;
		rfData->error = VictimSystemError;
		pthread_exit(NULL);
	}

	/* Perform continuous RSSI measurement for this channel. */
	while (rfData->order == START) {
		elem.type = DTH_TYPE_U32;
		elem.user_data = ACT_GETINFO;
		if (DthRf_GetRfInfo_exec(&elem) != 0) {
			rfData->state = DEACTIVATED;
			rfData->order = STOP;
			rfData->error = RfSetError;
			pthread_exit(NULL);
		}
		rfData->error = NO_ERROR;
		usleep(500);
	}

	rfData->state = DEACTIVATED;
	rfData->order = STOP;
	rfData->error = NO_ERROR;
	pthread_exit(NULL);
}

/**
 *  Start RX 2G5/3G and measure RSSI for all channel.
 *	@param[in] threadData data needed for processing.
 */
static void tatl07_09Rf_All_Channel_Process(void *threadData)
{
	DTH_RF_SYSTEM_DATA *rfData;
	struct dth_element elem;
	u32 value;

	rfData = (DTH_RF_SYSTEM_DATA *) threadData;
	value = 0;

	rfData->state = ACTIVATED;
	if (rfData->type == RF2G) {
		/* Set all RX2G5 input parameters. */
		if (tatl07_05Set_All_2G_Rf_Parameter(rfData) != 0) {
			rfData->state = DEACTIVATED;
			rfData->order = STOP;
			rfData->error = RfSetError;
			pthread_exit(NULL);
		}

		if ((rfData->firstArfcn > 0) && (rfData->lastArfcn > 0)) {
			/* Perform START RX 2G5 and RSSI measurement for all channel. */
			while (rfData->order == START) {
				int i;
				float rssi = 0.00;

				/* StartRX and Start RSSI for all channels. */
				for (i = rfData->firstArfcn; i <= rfData->lastArfcn; i += rfData->step) {
					/*Set channel */
					elem.type = DTH_TYPE_U32;
					elem.user_data = ACT_RX2G5_CHANNEL;
					if (DthRf_StartRx2G5Parm_set(&elem, ((void *)&i)) != 0) {
						rfData->state = DEACTIVATED;
						rfData->order = STOP;
						rfData->error = RfSetError;
						pthread_exit(NULL);
					}

					/* Start RX 2G5 */
					elem.type = DTH_TYPE_U32;
					elem.user_data = ACT_RX2G5;
					if (DthRf_StartRx2G5_exec(&elem) != 0) {
						rfData->state = DEACTIVATED;
						rfData->order = STOP;
						rfData->error = RfStartRxError;
						pthread_exit(NULL);
					}

					/* Start RSSI measurement */
					elem.type = DTH_TYPE_U32;
					elem.user_data = ACT_GETINFO;
					if (DthRf_GetRfInfo_exec(&elem) != 0) {
						rfData->state = DEACTIVATED;
						rfData->order = STOP;
						rfData->error = RfStartGetRfInfoError;
						pthread_exit(NULL);
					}

					/* Write RSSI measurement in a log file. */
					struct dth_element elem;
					elem.type = DTH_TYPE_FLOAT;
					elem.user_data = ACT_GET_INFO_RSSI;

					if (DthRf_GetRfInfoParam_get(&elem, (void *)&rssi) != 0) {
						rfData->state = DEACTIVATED;
						rfData->order = STOP;
						rfData->error = RfRssiError;
						pthread_exit(NULL);
					}

					if (tatl07_07Write_Rssi_In_Log_File(RF_2G_LOG_FILE, i, rssi) < 0) {
						rfData->state = DEACTIVATED;
						rfData->order = STOP;
						rfData->error = RfWriteFileError;
						pthread_exit(NULL);
					}
					rfData->error = NO_ERROR;
					/* pause */
					usleep(500);
				} /* end for */
				sleep(1);
			}	/* end while */
		} else {
			rfData->state = DEACTIVATED;
			rfData->order = STOP;
			rfData->error = RfChannelLimit;
			pthread_exit(NULL);
		}
	} else if (rfData->type == RF3G) {
		/* Set all RX3G input parameters. */
		if (tatl07_06Set_All_3G_Rf_Parameter(rfData) != 0) {
			rfData->state = DEACTIVATED;
			rfData->order = STOP;
			rfData->error = RfSetError;
			pthread_exit(NULL);
		}

		if ((rfData->firstArfcn > 0) && (rfData->lastArfcn > 0)) {
			/* Perform START RX 3G and RSSI measurement for all channel. */
			while (rfData->order == START) {
				int i;
				float rssi = 0;

				/* StartRX and Start RSSI for all channels. */
				for (i = rfData->firstArfcn; i < rfData->lastArfcn; i += rfData->step) {
					/*Set channel */
					elem.type = DTH_TYPE_U32;
					elem.user_data = ACT_RX3G_UARFCN;
					if (DthRf_StartRx3G_Parm_set(&elem, ((void *)&i)) != 0) {
						rfData->state = DEACTIVATED;
						rfData->order = STOP;
						rfData->error = RfSetError;
						pthread_exit(NULL);
					}

					/* Start RX 3G */
					elem.type = DTH_TYPE_U32;
					elem.user_data = ACT_RX3G;
					if (DthRf_StartRx3G_exec(&elem) != 0) {
						rfData->state = DEACTIVATED;
						rfData->order = STOP;
						rfData->error = RfStartRxError;
						pthread_exit(NULL);
					}

					/* Start RSSI measurement */
					elem.type = DTH_TYPE_U32;
					elem.user_data = ACT_GETINFO;
					if (DthRf_GetRfInfo_exec(&elem) != 0) {
						rfData->state = DEACTIVATED;
						rfData->order = STOP;
						rfData->error = RfStartGetRfInfoError;
						pthread_exit(NULL);
					}

					/* Write RSSI measurement in a log file. */
					struct dth_element elem;
					elem.type = DTH_TYPE_FLOAT;
					elem.user_data = ACT_GET_INFO_RSSI;

					if (DthRf_GetRfInfoParam_get(&elem, (void *)&rssi) != 0) {
						rfData->state = DEACTIVATED;
						rfData->order = STOP;
						rfData->error = RfRssiError;
						pthread_exit(NULL);
					}

					if (tatl07_07Write_Rssi_In_Log_File(RF_3G_LOG_FILE, i, rssi) < 0) {
						rfData->state = DEACTIVATED;
						rfData->order = STOP;
						rfData->error = RfWriteFileError;
						pthread_exit(NULL);
					}
					rfData->error = NO_ERROR;
					usleep(500);
				}	/* end for */
				sleep(1);
			}	/* end while */
		} else {
			rfData->state = DEACTIVATED;
			rfData->order = STOP;
			rfData->error = RfChannelLimit;
			pthread_exit(NULL);
		}
	} else {
		rfData->state = DEACTIVATED;
		rfData->order = STOP;
		rfData->error = VictimSystemError;
		pthread_exit(NULL);
	}

	rfData->state = DEACTIVATED;
	rfData->order = STOP;
	rfData->error = NO_ERROR;
	pthread_exit(NULL);
}

static void tatl07_11FM_One_Channel_Process(void *threadData)
{
	DTH_FMRX_DATA *data;
	uint32_t vl_error = 0;
	int32_t l_rssi;

	data = (DTH_FMRX_DATA *) threadData;

	data->state = ACTIVATED;

	vl_error = tatl12_18Conn_FMRX_Audio_Configuration();
	if (vl_error != 0) {
		printf("Fm Rx AB Audio configuration failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = ACVErrror;
		pthread_exit(NULL);
	}

	vl_error = ConnServ_FM_Start();
	if (vl_error != 0) {
		printf("ConnServ_FM_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = FMIPStartVError;
		pthread_exit(NULL);
	}
	v_tatcoex_fmip_data.state = ACTIVATED;
	v_tatcoex_fmip_data.error = NO_ERROR;

	printf("data->channel = %d, data->mode = %d, 1+(data->emp) = %d\n", data->channel, data->mode, 1 + (data->emp));
	vl_error = ConnServ_FM_Rx_Start(data->channel, data->mode, 1 + (data->emp));

	if (vl_error != 0) {
		printf("ConnServ_FM_Rx_Start failed with vl_error=%d\n", vl_error);
		vl_error = ConnServ_FM_Rx_Stop();
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = FMRxStartError;
		pthread_exit(NULL);
	}
	while (data->order == START) {
		usleep(500);
	}

	data->state = DEACTIVATED;
	vl_error = ConnServ_FM_Signal_Strength(&l_rssi);
	if (vl_error != 0) {
		printf("ConnServ_FM_Signal_Strength failed with vl_error=%d\n", vl_error);
	} else {
		data->rssi = l_rssi;
		printf("RSSI = %d\n", data->rssi);
	}

	vl_error = ConnServ_FM_Rx_Stop();
	if (vl_error != 0) {
		printf("ConnServ_FM_Rx_Stop failed with vl_error=%d\n", vl_error);
		data->error = FMRxStopError;
		pthread_exit(NULL);
	}

	vl_error = ConnServ_FM_Stop();
	if (vl_error != 0) {
		printf("ConnServ_FM_Stop failed with vl_error=%d\n", vl_error);
		data->error = FMIPStopVError;
		pthread_exit(NULL);
	}
	v_tatcoex_fmip_data.state = DEACTIVATED;
	v_tatcoex_fmip_data.error = NO_ERROR;

	vl_error = tatl12_18Conn_FMRX_Audio_DeConfiguration();
	if (vl_error != 0) {
		printf("Fm Rx AB Audio deconfiguration failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = ACVErrror;
		pthread_exit(NULL);
	}

	data->order = STOP;
	data->error = NO_ERROR;
	pthread_exit(NULL);
}

static void tatl07_12BT_One_Channel_Process(void *threadData)
{
	DTH_BTRX_DATA *data;
	uint32_t vl_error;
	u16 total_packets;
	u16 wrongtype_packets;
	u16 wronglength_packets;
	u16 biterrors;
	u16 erroneous_packets;
	u8 l_rssi = 0;
	u32 l_ber = 0;

	data = (DTH_BTRX_DATA *) threadData;
	vl_error = 0;

	data->state = ACTIVATED;

	vl_error = ConnServ_BT_Start();
	if (vl_error != 0) {
		printf("ConnServ_BT_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = BTIPStartVError;
		pthread_exit(NULL);
	}
	v_tatcoex_btip_data.state = ACTIVATED;
	v_tatcoex_btip_data.error = NO_ERROR;

	printf("data->channel = %d, data->pktt = %d, data->addr_idx = %d\n", data->channel, data->pktt, data->addr_idx);
	vl_error = ConnServ_BT_RxMode_Start(data->channel, data->pktt, data->addr_idx);
	if (vl_error != 0) {
		printf("ConnServ_BT_RxMode_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = BTRxStartError;
		pthread_exit(NULL);
	}

	while (data->order == START) {
		usleep(500);
	}

	data->state = DEACTIVATED;

	vl_error = ConnServ_BT_RxMode_Stop(&total_packets, &wrongtype_packets, &wronglength_packets, &biterrors, &erroneous_packets, &l_rssi, &l_ber);
	if (vl_error != 0) {
		printf("ConnServ_BT_RxMode_Stop failed with vl_error=%d\n", vl_error);
		data->error = BTRxStopError;
		pthread_exit(NULL);
	}
	data->rssi = l_rssi;
	printf("RSSI = %d\n", data->rssi);
	data->ber = l_ber;
	printf("BER = %f\n", data->ber);

	vl_error = ConnServ_BT_Stop();
	if (vl_error != 0) {
		printf("ConnServ_BT_Stop failed with vl_error=%d\n", vl_error);
		data->error = BTIPStopVError;
		pthread_exit(NULL);
	}
	v_tatcoex_btip_data.state = DEACTIVATED;
	v_tatcoex_btip_data.error = NO_ERROR;

	data->state = DEACTIVATED;
	data->order = STOP;
	data->error = NO_ERROR;
	pthread_exit(NULL);
}

static void tatl07_14FM_All_Channel_Process(void *threadData)
{
	DTH_FMRX_DATA *data;
	uint32_t vl_error;
	int isResult = 0;

	data = (DTH_FMRX_DATA *) threadData;
	vl_error = 0;

	data->state = ACTIVATED;

	vl_error = ConnServ_FM_Start();
	if (vl_error != 0) {
		printf("ConnServ_FM_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = FMIPStartVError;
		pthread_exit(NULL);
	}
	v_tatcoex_fmip_data.state = ACTIVATED;
	v_tatcoex_fmip_data.error = NO_ERROR;

	vl_error = ConnServ_FM_Rx_RSSI_Scan_Start(data->fchannel, data->lchannel, data->step, data->hold);
	if (vl_error != 0) {
		printf("ConnServ_FM_Rx_RSSI_Scan_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = FMRxStartError;
		pthread_exit(NULL);
	}

	while (data->order != STOP) {
		if (data->order == 2) {	/* GET RESULT */
			isResult = ConnServ_FM_Get_Result();
			if (isResult) {
				data->error = FMALLResultIs;
				data->order = STOP;
				sleep(1);
			} else {
				data->order = START;
			}
		}
		usleep(500);
	}
	data->state = DEACTIVATED;
	if (isResult == 0) {	/* Thus we know that we get here NOT by GET RESULT but by STOP button so we must stop RSSI scan */
		vl_error = ConnServ_FM_Rx_RSSI_Scan_Stop();
		if (vl_error != 0) {
			printf("ConnServ_FM_Rx_RSSI_Scan_Stop failed with vl_error=%d\n", vl_error);
			data->error = FMRxStopError;
			pthread_exit(NULL);
		}
	}

	vl_error = ConnServ_FM_Stop();	/* In any case we stop FM IP */
	if (vl_error != 0) {
		printf("ConnServ_FM_Stop failed with vl_error=%d\n", vl_error);
		data->error = FMIPStopVError;
		pthread_exit(NULL);
	}
	v_tatcoex_fmip_data.state = DEACTIVATED;
	v_tatcoex_fmip_data.error = NO_ERROR;

	data->state = DEACTIVATED;
	data->order = STOP;
	data->error = NO_ERROR;
	pthread_exit(NULL);
}

static void tatl07_17GPS_Process(void *threadData)
{
	DTH_GPSV_DATA *data;
	uint32_t vl_error;
	int i;
	int8_t l_TimeOfLock;
	t_ConnServGPS_Position l_Position;
	t_ConnServGPS_Satellites l_Satellites;

	data = (DTH_GPSV_DATA *) threadData;
	data->state = ACTIVATED;
	vl_error = 0;

	vl_error = ConnServ_GPS_Start(data->interval);
	if (vl_error != 0) {
		printf("ConnServ_GPS_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = GPSIPStartVError;
		pthread_exit(NULL);
	}

	while (data->order == START) {
		usleep(500);
	}
	data->state = DEACTIVATED;
	vl_error = ConnServ_GPS_GetLocation(&l_TimeOfLock, &l_Position, &l_Satellites);

	if (vl_error != 0) {
		printf("ConnServ_GPS_GetLocation failed with vl_error=%d\n", vl_error);
		data->error = GPSGetLocError;
		pthread_exit(NULL);
	}
	data->longitude = l_Position.longitude;
	data->latitude = l_Position.latitude;
	data->altitude = l_Position.altitude;
	data->satnum = l_Satellites.SatsInView;

	for (i = 0; i < CONNSERV_NUMBER_OF_SATELLITES; i++) {
		data->snrdata[i] = l_Satellites.SatsInViewSNR[i];
	}

	vl_error = ConnServ_GPS_Stop();

	if (vl_error != 0) {
		printf("ConnServ_GPS_Stop failed with vl_error=%d\n", vl_error);
		data->error = GPSIPStopVError;
		pthread_exit(NULL);
	}

	data->state = DEACTIVATED;
	data->order = STOP;
	data->error = NO_ERROR;
	pthread_exit(NULL);
}

static void tatl07_18WLAN_Process(void *threadData)
{
	DTH_WLANRX_DATA *data;
	uint32_t vl_error;
	u32 l_pktrx, l_errors, l_crcerrors;
	float l_per;

	data = (DTH_WLANRX_DATA *) threadData;
	vl_error = 0;

	data->state = ACTIVATED;

	if (vl_error != 0) {
		printf("ConnServ_CW1200_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = CW1200StartVError;
		pthread_exit(NULL);
	}

	printf("data->modeband = %d, data->channel = %d\n", data->modeband, data->channel);
	vl_error = ConnServ_WLAN_Rx_Start(data->modeband, data->channel);

	if (vl_error != 0) {
		printf("ConnServ_WLAN_Rx_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = WLANRxStartError;
		pthread_exit(NULL);
	}

	while (data->order == START)
		usleep(500);

	data->state = DEACTIVATED;
	printf("data->pktexp = %d", data->pktexp);
	vl_error = ConnServ_WLAN_Rx_Stop(data->pktexp, &l_pktrx, &l_errors, &l_crcerrors, &l_per);
	if (vl_error != 0) {
		printf("ConnServ_WLAN_Rx_Stop failed with vl_error=%d\n", vl_error);
		data->error = WLANRxStopError;
		pthread_exit(NULL);
	}
	data->pktrx = l_pktrx;
	data->errors = l_errors;
	data->crcerrors = l_crcerrors;
	data->per = l_per;
	printf("data->pktrx = %d, data->errors = %d, data->crcerrors = %d, data->per = %f \n", data->pktrx, data->errors, data->crcerrors, data->per);

	if (vl_error != 0) {
		printf("ConnServ_CW1200_Stop failed with vl_error=%d\n", vl_error);
		data->error = CW1200StopVError;
		pthread_exit(NULL);
	}

	data->state = DEACTIVATED;
	data->order = STOP;
	data->error = NO_ERROR;
	pthread_exit(NULL);
}

static void tatl07_18WLANTX_Process(void *threadData)
{
	DTH_WLANTX_DATA *data;
	uint32_t vl_error = 0;
	u32 l_pktsent, l_pktack;

	data = (DTH_WLANTX_DATA *) threadData;

	data->state = ACTIVATED;

	if (vl_error != 0) {
		printf("ConnServ_CW1200_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = CW1200StartVError;
		pthread_exit(NULL);
	}

	printf("data->rate = %d, data->band = %d, data->mode = %d, data->channel = %d, data->ack = %d, data->ifd = %d, data->pwridx = %f",
	       data->rate, data->band, data->mode, data->channel, data->ack, data->ifd, data->pwridx);
	vl_error = ConnServ_WLAN_Tx_Start(data->rate, data->band, data->mode, data->channel, data->ack, data->ifd, data->pwridx);

	if (vl_error != 0) {
		printf("ConnServ_WLAN_Tx_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = WLANTxStartError;
		pthread_exit(NULL);
	}

	while (data->order == START)
		usleep(500);

	data->state = DEACTIVATED;
	vl_error = ConnServ_WLAN_Tx_Stop(&l_pktsent, &l_pktack);
	if (vl_error != 0) {
		printf("ConnServ_WLAN_Tx_Stop failed with vl_error=%d\n", vl_error);
		data->error = WLANTxStopError;
		pthread_exit(NULL);
	}
	data->pktsent = l_pktsent;
	data->pktack = l_pktack;

	if (vl_error != 0) {
		printf("ConnServ_CW1200_Stop failed with vl_error=%d\n", vl_error);
		data->error = CW1200StopVError;
		pthread_exit(NULL);
	}

	data->state = DEACTIVATED;
	data->order = STOP;
	data->error = NO_ERROR;
	pthread_exit(NULL);
}

static void tatl07_12BTTX_Process(void *threadData)
{
	DTH_BTTX_DATA *data;
	uint32_t vl_error;

	data = (DTH_BTTX_DATA *) threadData;
	vl_error = 0;

	data->state = ACTIVATED;

	vl_error = ConnServ_BT_Start();
	if (vl_error != 0) {
		printf("ConnServ_BT_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = BTIPStartVError;
		pthread_exit(NULL);
	}
	v_tatcoex_btip_data.state = ACTIVATED;
	v_tatcoex_btip_data.error = NO_ERROR;

	printf("data->channel = %d, data->pktt = %d, data->tx_level = %d, data->addr_idx = %d\n", data->channel, data->pktt, data->tx_level, data->addr_idx);
	vl_error = ConnServ_BT_TxMode_Start(data->channel, data->pktt, data->tx_level, data->addr_idx);
	if (vl_error != 0) {
		printf("ConnServ_BT_RxMode_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = BTTxStartError;
		pthread_exit(NULL);
	}

	while (data->order == START)
		usleep(500);

	data->state = DEACTIVATED;
	vl_error = ConnServ_BT_TxMode_Stop();
	if (vl_error != 0) {
		printf("ConnServ_BT_RxMode_Stop failed with vl_error=%d\n", vl_error);
		data->error = BTTxStopError;
		pthread_exit(NULL);
	}

	vl_error = ConnServ_BT_Stop();
	if (vl_error != 0) {
		printf("ConnServ_BT_Stop failed with vl_error=%d\n", vl_error);
		data->error = BTIPStopVError;
		pthread_exit(NULL);
	}
	v_tatcoex_btip_data.state = DEACTIVATED;
	v_tatcoex_btip_data.error = NO_ERROR;

	data->state = DEACTIVATED;
	data->order = STOP;
	data->error = NO_ERROR;
	pthread_exit(NULL);
}

static void tatl07_11FMTX_Process(void *threadData)
{
	DTH_FMTX_DATA *data;
	uint32_t vl_error = 0;

	data = (DTH_FMTX_DATA *) threadData;

	data->state = ACTIVATED;

	vl_error = tatl12_18Conn_FMTX_Audio_Configuration();
	if (vl_error != 0) {
		printf("Fm Tx AB Audio configuration failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = ACVErrror;
		pthread_exit(NULL);
	}

	vl_error = ConnServ_FM_Start();
	if (vl_error != 0) {
		printf("ConnServ_FM_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = FMIPStartVError;
		pthread_exit(NULL);
	}
	v_tatcoex_fmip_data.state = ACTIVATED;
	v_tatcoex_fmip_data.error = NO_ERROR;

	printf("freq = %d, pwr = %d, mode = %d, src = %d, volume = %d, emp = %d, tonefreq = %d\n", data->freq, data->pwr, data->mode, data->src, data->volume, 1 + (data->emp),
	       data->tonefreq);
	vl_error = ConnServ_FM_Tx_Start(data->freq, data->pwr, data->mode, data->src, data->volume, 1 + (data->emp), data->tonefreq);

	if (vl_error != 0) {
		printf("ConnServ_FM_Rx_Start failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = FMTxStartError;
		pthread_exit(NULL);
	}
	while (data->order == START)
		usleep(500);

	data->state = DEACTIVATED;

	vl_error = ConnServ_FM_Tx_Stop();
	if (vl_error != 0) {
		printf("ConnServ_FM_Rx_Stop failed with vl_error=%d\n", vl_error);
		data->error = FMTxStopError;
		pthread_exit(NULL);
	}

	vl_error = ConnServ_FM_Stop();
	if (vl_error != 0) {
		printf("ConnServ_FM_Stop failed with vl_error=%d\n", vl_error);
		data->error = FMIPStopVError;
		pthread_exit(NULL);
	}
	v_tatcoex_fmip_data.state = DEACTIVATED;
	v_tatcoex_fmip_data.error = NO_ERROR;

	vl_error = tatl12_18Conn_FMTX_Audio_DeConfiguration();
	if (vl_error != 0) {
		printf("Fm Tx AB Audio deconfiguration failed with vl_error=%d\n", vl_error);
		data->state = DEACTIVATED;
		data->order = STOP;
		data->error = ACVErrror;
		pthread_exit(NULL);
	}

	data->order = STOP;
	data->error = NO_ERROR;
	pthread_exit(NULL);
}

int tatl07_00Victim_Exec(struct dth_element *elem)
{
	int vl_error = 0;

	switch (elem->user_data) {
	case ACT_COEX_RX2G5_ONE:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_2grfone_data,
						     (void *)tatl07_08Rf_One_Channel_Process, v_tatcoex_2grfone_data.order, v_tatcoex_2grfone_data.state);
		break;

	case ACT_COEX_RX3G_ONE:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_3grfone_data,
						     (void *)tatl07_08Rf_One_Channel_Process, v_tatcoex_3grfone_data.order, v_tatcoex_3grfone_data.state);
		break;

	case ACT_COEX_BTRX_ONE:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_btrxone_data,
						     (void *)tatl07_12BT_One_Channel_Process, v_tatcoex_btrxone_data.order, v_tatcoex_btrxone_data.state);
		sleep(3);
		break;

	case ACT_COEX_FMRX_ONE:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_fmrxone_data,
						     (void *)tatl07_11FM_One_Channel_Process, v_tatcoex_fmrxone_data.order, v_tatcoex_fmrxone_data.state);
		break;

	case ACT_COEX_RX2G5_ALL:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_2grfall_data,
						     (void *)tatl07_09Rf_All_Channel_Process, v_tatcoex_2grfall_data.order, v_tatcoex_2grfall_data.state);
		break;

	case ACT_COEX_RX3G_ALL:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_3grfall_data,
						     (void *)tatl07_09Rf_All_Channel_Process, v_tatcoex_3grfall_data.order, v_tatcoex_3grfall_data.state);
		break;

	case ACT_COEX_FMRX_ALL:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_fmrxall_data,
						     (void *)tatl07_14FM_All_Channel_Process, v_tatcoex_fmrxall_data.order, v_tatcoex_fmrxall_data.state);
		break;

	case ACT_COEX_GPS:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_gpsv_data,
						     (void *)tatl07_17GPS_Process, v_tatcoex_gpsv_data.order, v_tatcoex_gpsv_data.state);
		break;

	case ACT_WLANRX:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_wlanrx_data,
						     (void *)tatl07_18WLAN_Process, v_tatcoex_wlanrx_data.order, v_tatcoex_wlanrx_data.state);
		sleep(3);
		break;

	case ACT_VIC_WLANTX:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_wlantx_data,
						     (void *)tatl07_18WLANTX_Process, v_tatcoex_wlantx_data.order, v_tatcoex_wlantx_data.state);
		break;

	case ACT_VIC_BTTX:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_bttx_data,
						     (void *)tatl07_12BTTX_Process, v_tatcoex_bttx_data.order, v_tatcoex_bttx_data.state);
		break;

	case ACT_VIC_FMTX:
		vl_error = tatl01_00Manage_Processes(&v_tatcoex_victim_thread, (void *)&v_tatcoex_fmtx_data,
						     (void *)tatl07_11FMTX_Process, v_tatcoex_fmtx_data.order, v_tatcoex_fmtx_data.state);
		break;

	case ACT_COEX_RADIO_RSSI:
		break;

	case ACT_COEX_STOP:
		{
			struct dth_element elem_inter;
			u32 value = 1;
			elem_inter.type = DTH_TYPE_U32;
			elem_inter.user_data = ACT_RX2G5;

			/* Stop victim process. */
			v_tatcoex_3grfall_data.order = STOP;
			v_tatcoex_3grfone_data.order = STOP;
			v_tatcoex_2grfall_data.order = STOP;
			v_tatcoex_2grfone_data.order = STOP;

			/* Stop RF activity. */
			vl_error = DthRf_StartRx2G5_set(&elem_inter, (void *)&value);
			vl_error = DthRf_StartRx2G5_exec(&elem_inter);

			/* Re-initialize victim process. */
			tatl07_03Init_Victim_Data();
		}
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;
}

int tatl07_01Victim_Get(struct dth_element *elem, void *value)
{
	int vl_error = 0, i;

	switch (elem->user_data) {
	case ACT_COEX_RX2G5_ONE_STATE:
		{
			*((u8 *) value) = v_tatcoex_2grfone_data.state;
			SYSLOG(LOG_DEBUG, "Get RF state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_RX3G_ONE_STATE:
		{
			*((u8 *) value) = v_tatcoex_3grfone_data.state;
			SYSLOG(LOG_DEBUG, "Get RF state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_RX2G5_ALL_STATE:
		{
			*((u8 *) value) = v_tatcoex_2grfall_data.state;
			SYSLOG(LOG_DEBUG, "Get RF state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_RX3G_ALL_STATE:
		{
			*((u8 *) value) = v_tatcoex_3grfall_data.state;
			SYSLOG(LOG_DEBUG, "Get RF state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_RX2G5_ONE_ERROR:
		{
			strncpy((char *)value, v_tatcoex_2grfone_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get RF error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_RX3G_ONE_ERROR:
		{
			strncpy((char *)value, v_tatcoex_3grfone_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get RF error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_RX2G5_ALL_ERROR:
		{
			strncpy((char *)value, v_tatcoex_2grfall_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get RF error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_RX3G_ALL_ERROR:
		{
			strncpy((char *)value, v_tatcoex_3grfall_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get RF error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_RADIO_RSSI_VALUE:
		{
			struct dth_element elem;
			float *rssi;

			elem.type = DTH_TYPE_FLOAT;
			elem.user_data = ACT_GET_INFO_RSSI;
			rssi = (float *)value;

			vl_error = DthRf_GetRfInfoParam_get(&elem, (void *)rssi);
			SYSLOG(LOG_DEBUG, "Get RSSI measure: %lf\n", *rssi);
		}
		break;

	case ACT_COEX_BTRX_ONE_STATE:
		{
			*((u8 *) value) = v_tatcoex_btrxone_data.state;
			SYSLOG(LOG_DEBUG, "Get BT Rx state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_BTRX_ONE_ERROR:
		{
			strncpy((char *)value, v_tatcoex_btrxone_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get BT Rx error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_VIC_BTTX_STATE:
		{
			*((u8 *) value) = v_tatcoex_bttx_data.state;
			SYSLOG(LOG_DEBUG, "Get BT Tx state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_VIC_BTTX_ERROR:
		{
			strncpy((char *)value, v_tatcoex_bttx_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get BT Tx error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_FMRX_ONE_STATE:
		{
			*((u8 *) value) = v_tatcoex_fmrxone_data.state;
			SYSLOG(LOG_DEBUG, "Get FM state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_FMRX_ONE_ERROR:
		{
			strncpy((char *)value, v_tatcoex_fmrxone_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get FM Rx error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_FMRX_ALL_STATE:
		{
			*((u8 *) value) = v_tatcoex_fmrxall_data.state;
			SYSLOG(LOG_DEBUG, "Get FM All Rx state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_FMRX_ALL_ERROR:
		{
			strncpy((char *)value, v_tatcoex_fmrxall_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get FM All Rx error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_VIC_FMTX_STATE:
		{
			*((u8 *) value) = v_tatcoex_fmtx_data.state;
			SYSLOG(LOG_DEBUG, "Get FM Tx state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_VIC_FMTX_ERROR:
		{
			strncpy((char *)value, v_tatcoex_fmtx_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get FM Tx error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_GPS_STATE:
		{
			*((u8 *) value) = v_tatcoex_gpsv_data.state;
			SYSLOG(LOG_DEBUG, "Get GPS state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_GPS_ERROR:
		{
			strncpy((char *)value, v_tatcoex_gpsv_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get GPS error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_WLANRX_STATE:
		{
			*((u8 *) value) = v_tatcoex_wlanrx_data.state;
			SYSLOG(LOG_DEBUG, "Get WLAN Rx state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_WLANRX_ERROR:
		{
			strncpy((char *)value, v_tatcoex_wlanrx_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get WLAN Rx error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_VIC_WLANTX_STATE:
		{
			*((u8 *) value) = v_tatcoex_wlantx_data.state;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx state ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_VIC_WLANTX_ERROR:
		{
			strncpy((char *)value, v_tatcoex_wlantx_data.error, MAX_STRNG_SIZE);
			SYSLOG(LOG_DEBUG, "Get WLAN Tx error string: %s\n", (char *)value);
		}
		break;

	case ACT_COEX_BTRX_ONE_CHAN:
		{
			*((u16 *) value) = v_tatcoex_btrxone_data.channel;
			SYSLOG(LOG_DEBUG, "Get BT Rx channel : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_BTRX_ONE_ADDRIDX:
		{
			*((s16 *) value) = v_tatcoex_btrxone_data.addr_idx;
			SYSLOG(LOG_DEBUG, "Get BT Rx addr idx : %i\n", *((s16 *) value));
		}
		break;

	case ACT_COEX_BTRX_ONE_PKTT:
		{
			*((u8 *) value) = v_tatcoex_btrxone_data.pktt;
			SYSLOG(LOG_DEBUG, "Get BT Rx pktt : %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_BTRX_ONE_RSSI:
		{
			*((s16 *) value) = v_tatcoex_btrxone_data.rssi;
			SYSLOG(LOG_DEBUG, "Get BT Rx RSSI : %i\n", *((s16 *) value));
		}
		break;

	case ACT_COEX_BTRX_ONE_BER:
		{
			*((float *)value) = v_tatcoex_btrxone_data.ber;
			SYSLOG(LOG_DEBUG, "Get BT Rx BER : %f\n", *((float *)value));
		}
		break;

	case ACT_COEX_VIC_BTTX_CHAN:
		{
			*((u16 *) value) = v_tatcoex_bttx_data.channel;
			SYSLOG(LOG_DEBUG, "Get BT Tx channel : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_VIC_BTTX_PKTT:
		{
			*((u8 *) value) = v_tatcoex_bttx_data.pktt;
			SYSLOG(LOG_DEBUG, "Get BT Tx pktt : %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_FMRX_ONE_CHAN:
		{
			*((u32 *) value) = v_tatcoex_fmrxone_data.channel;
			SYSLOG(LOG_DEBUG, "Get FM Rx channel : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_FMRX_ONE_RSSI:
		{
			*((u16 *) value) = v_tatcoex_fmrxone_data.rssi;
			SYSLOG(LOG_DEBUG, "Get FM Rx RSSI : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_FMRX_ALL_FCHAN:
		{
			*((u32 *) value) = v_tatcoex_fmrxall_data.fchannel;
			SYSLOG(LOG_DEBUG, "Get FM All Rx first channel : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_FMRX_ALL_LCHAN:
		{
			*((u32 *) value) = v_tatcoex_fmrxall_data.lchannel;
			SYSLOG(LOG_DEBUG, "Get FM All Rx last channel : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_FMRX_ALL_STEP:
		{
			*((u16 *) value) = v_tatcoex_fmrxall_data.step;
			SYSLOG(LOG_DEBUG, "Get FM All Rx step : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_FMRX_ALL_HOLD:
		{
			*((u32 *) value) = v_tatcoex_fmrxall_data.hold;
			SYSLOG(LOG_DEBUG, "Get FM All Rx hold : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_VIC_FMTX_FREQ:
		{
			*((u32 *) value) = v_tatcoex_fmtx_data.freq;
			SYSLOG(LOG_DEBUG, "Get FM Tx channel : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_VIC_FMTX_TONEFREQ:
		{
			*((u32 *) value) = v_tatcoex_fmtx_data.tonefreq;
			SYSLOG(LOG_DEBUG, "Get FM Tx tone freq : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_VIC_FMTX_VOL:
		{
			*((u16 *) value) = v_tatcoex_fmtx_data.volume;
			SYSLOG(LOG_DEBUG, "Get FM Tx volume : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_VIC_FMTX_PWR:
		{
			*((u32 *) value) = v_tatcoex_fmtx_data.pwr;
			SYSLOG(LOG_DEBUG, "Get FM Tx power : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_GPS_INT:
		{
			*((u32 *) value) = v_tatcoex_gpsv_data.interval;
			SYSLOG(LOG_DEBUG, "Get GPS interval ACTIVATED(1) DEACTIVATED(0): %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_GPS_LONG:
		{
			*((float *)value) = v_tatcoex_gpsv_data.longitude;
			SYSLOG(LOG_DEBUG, "Get GPS interval ACTIVATED(1) DEACTIVATED(0): %f\n", *((float *)value));
		}
		break;

	case ACT_COEX_GPS_LAT:
		{
			*((float *)value) = v_tatcoex_gpsv_data.latitude;
			SYSLOG(LOG_DEBUG, "Get GPS interval ACTIVATED(1) DEACTIVATED(0): %f\n", *((float *)value));
		}
		break;

	case ACT_COEX_GPS_ALT:
		{
			*((u16 *) value) = v_tatcoex_gpsv_data.altitude;
			SYSLOG(LOG_DEBUG, "Get GPS interval ACTIVATED(1) DEACTIVATED(0): %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_GPS_SNUM:
		{
			*((u8 *) value) = v_tatcoex_gpsv_data.satnum;
			SYSLOG(LOG_DEBUG, "Get GPS interval ACTIVATED(1) DEACTIVATED(0): %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_GPS_SNRDATA:
		{
			struct dth_array *ValTab = NULL;
			u8 *pl_DataOut = NULL;
			ValTab = (struct dth_array *)value;
			pl_DataOut = (u8 *) ValTab->array;
			for (i = 0; i < CONNSERV_NUMBER_OF_SATELLITES; i++) {
				*pl_DataOut = v_tatcoex_gpsv_data.snrdata[i];
				pl_DataOut++;
			}
		}
		break;

	case ACT_COEX_WLANRX_MODEBAND:
		{
			*((u8 *) value) = v_tatcoex_wlanrx_data.modeband;
			SYSLOG(LOG_DEBUG, "Get WLAN Rx modeband : %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_WLANRX_CHAN:
		{
			*((u16 *) value) = v_tatcoex_wlanrx_data.channel;
			SYSLOG(LOG_DEBUG, "Get WLAN Rx channel : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_WLANRX_PKTEXP:
		{
			*((u32 *) value) = v_tatcoex_wlanrx_data.pktexp;
			SYSLOG(LOG_DEBUG, "Get WLAN Rx pkt exp : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_WLANRX_PKTRX:
		{
			*((u32 *) value) = v_tatcoex_wlanrx_data.pktrx;
			SYSLOG(LOG_DEBUG, "Get WLAN Rx pkt rx : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_WLANRX_ERRORS:
		{
			*((u32 *) value) = v_tatcoex_wlanrx_data.errors;
			SYSLOG(LOG_DEBUG, "Get WLAN Rx errors : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_WLANRX_CRCERRORS:
		{
			*((u32 *) value) = v_tatcoex_wlanrx_data.crcerrors;
			SYSLOG(LOG_DEBUG, "Get WLAN Rx crc errors : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_WLANRX_PER:
		{
			*((float *)value) = v_tatcoex_wlanrx_data.per;
			SYSLOG(LOG_DEBUG, "Get WLAN PER: %f\n", *((float *)value));
		}
		break;

	case ACT_COEX_VIC_WLANTX_MODE:
		{
			*((u8 *) value) = v_tatcoex_wlantx_data.mode;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx mode : %i\n", *((u8 *) value));
		}
		break;

	case ACT_COEX_VIC_WLANTX_CHAN:
		{
			*((u16 *) value) = v_tatcoex_wlantx_data.channel;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx channel : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_VIC_WLANTX_RATE:
		{
			*((u16 *) value) = v_tatcoex_wlantx_data.rate;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx rate : %i\n", *((u16 *) value));
		}
		break;

	case ACT_COEX_VIC_WLANTX_PWRIDX:
		{
			*((float *)value) = v_tatcoex_wlantx_data.pwridx;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx pwr idx : %f\n", *((float *)value));
		}
		break;

	case ACT_COEX_VIC_WLANTX_PKTSNT:
		{
			*((u32 *) value) = v_tatcoex_wlantx_data.pktsent;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx pkt sent : %i\n", *((u32 *) value));
		}
		break;

	case ACT_COEX_VIC_WLANTX_PKTACK:
		{
			*((u32 *) value) = v_tatcoex_wlantx_data.pktack;
			SYSLOG(LOG_DEBUG, "Get WLAN Tx pkt ack : %i\n", *((u32 *) value));
		}
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

int tatl07_02Victim_Set(struct dth_element *elem, void *value)
{
	int vl_error = 0;

	switch (elem->user_data) {
	case ACT_COEX_RX2G5_ONE:
		{
			u8 order = *((u8 *) value);
			/* Only one victim system could be activated. */
			if (order == START) {
				if ((v_tatcoex_3grfall_data.order == STOP) && (v_tatcoex_3grfone_data.order == STOP)
				    && (v_tatcoex_2grfall_data.order == STOP) && (v_tatcoex_2grfone_data.order == STOP)) {
					v_tatcoex_2grfone_data.order = order;
				}
			} else {
				v_tatcoex_2grfone_data.order = order;
			}
			SYSLOG(LOG_DEBUG, "Set RF 2G RX ONE order START(0) STOP(1): %i\n", v_tatcoex_2grfone_data.order);
		}
		break;

	case ACT_COEX_RX2G5_ALL:
		{
			u8 order = *((u8 *) value);
			/* Only one victim system could be activated. */
			if (order == START) {
				if ((v_tatcoex_3grfall_data.order == STOP) && (v_tatcoex_3grfone_data.order == STOP)
				    && (v_tatcoex_2grfall_data.order == STOP) && (v_tatcoex_2grfone_data.order == STOP)) {
					v_tatcoex_2grfall_data.order = order;
				}
			} else {
				v_tatcoex_2grfall_data.order = order;
			}
			SYSLOG(LOG_DEBUG, "Set RF 2G RX ALL order START(0) STOP(1): %i\n", v_tatcoex_2grfall_data.order);
		}
		break;

	case ACT_COEX_RX3G_ONE:
		{
			u8 order = *((u8 *) value);
			/* Only one victim system could be activated. */
			if (order == START) {
				if ((v_tatcoex_3grfall_data.order == STOP) && (v_tatcoex_3grfone_data.order == STOP)
				    && (v_tatcoex_2grfall_data.order == STOP) && (v_tatcoex_2grfone_data.order == STOP)) {
					v_tatcoex_3grfone_data.order = order;
				}
			} else {
				v_tatcoex_3grfone_data.order = order;
			}
			SYSLOG(LOG_DEBUG, "Set RF 3G RX ONE order START(0) STOP(1): %i\n", v_tatcoex_3grfone_data.order);
		}
		break;

	case ACT_COEX_RX3G_ALL:
		{
			u8 order = *((u8 *) value);
			/* Only one victim system could be activated. */
			if (order == START) {
				if ((v_tatcoex_3grfall_data.order == STOP) && (v_tatcoex_3grfone_data.order == STOP)
				    && (v_tatcoex_2grfall_data.order == STOP) && (v_tatcoex_2grfone_data.order == STOP)) {
					v_tatcoex_3grfall_data.order = order;
				}
			} else {
				v_tatcoex_3grfall_data.order = order;
			}
			SYSLOG(LOG_DEBUG, "Set RF 3G RX ALL order START(0) STOP(1): %i\n", v_tatcoex_3grfall_data.order);
		}
		break;

	case ACT_COEX_BTRX_ONE:
		{
			u8 order = *((u8 *) value);
			/* Only one victim system could be activated. */
			if (order == START) {
				if ((v_tatcoex_btrxall_data.order == STOP)) {
					v_tatcoex_btrxone_data.order = order;
					v_tatcoex_btip_data.order = order;
				}
			} else {
				v_tatcoex_btrxone_data.order = order;
				v_tatcoex_btip_data.order = order;
			}
			SYSLOG(LOG_DEBUG, "Set BT RX ONE order START(0) STOP(1): %i\n", v_tatcoex_btrxone_data.order);
		}
		break;

	case ACT_COEX_FMRX_ONE:
		{
			u8 order = *((u8 *) value);
			/* Only one victim system could be activated. */
			if (order == START) {
				if ((v_tatcoex_fmrxall_data.order == STOP)) {
					v_tatcoex_fmrxone_data.order = order;
					v_tatcoex_fmip_data.order = order;
				}
			} else {
				v_tatcoex_fmrxone_data.order = order;
				v_tatcoex_fmip_data.order = order;
			}
			SYSLOG(LOG_DEBUG, "Set FM RX ONE order START(0) STOP(1): %i\n", v_tatcoex_fmrxone_data.order);
		}
		break;

	case ACT_COEX_FMRX_ALL:
		{
			u8 order = *((u8 *) value);
			/* Only one victim system could be activated. */
			if (order == START) {
				if ((v_tatcoex_fmrxone_data.order == STOP)) {
					v_tatcoex_fmrxall_data.order = order;
					v_tatcoex_fmip_data.order = order;
				}
			} else {
				v_tatcoex_fmrxall_data.order = order;
				v_tatcoex_fmip_data.order = order;
			}
			SYSLOG(LOG_DEBUG, "Set FM RX All order START(0) STOP(1): %i\n", v_tatcoex_fmrxall_data.order);
		}
		break;

	case ACT_WLANRX:
		{
			u8 order = *((u8 *) value);
			/* Only one victim system could be activated. */
			if (order == START) {
				if ((v_tatcoex_wlanrx_data.order == STOP))
					v_tatcoex_wlanrx_data.order = order;

			} else {
				v_tatcoex_wlanrx_data.order = order;
			}
			SYSLOG(LOG_DEBUG, "Set WLAN RX order START(0) STOP(1): %i\n", v_tatcoex_wlanrx_data.order);
		}
		break;

	case ACT_VIC_WLANTX:
		{
			v_tatcoex_wlantx_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx order START(0) STOP(1): %i\n", v_tatcoex_wlantx_data.order);
		}
		break;

	case ACT_VIC_BTTX:
		{
			v_tatcoex_bttx_data.order = *((u8 *) value);
			v_tatcoex_btip_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Tx order START(0) STOP(1): %i\n", v_tatcoex_bttx_data.order);
		}
		break;

	case ACT_VIC_FMTX:
		{
			v_tatcoex_fmtx_data.order = *((u8 *) value);
			v_tatcoex_fmip_data.order = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Tx order START(0) STOP(1): %i\n", v_tatcoex_fmtx_data.order);
		}
		break;

	case ACT_COEX_GPS:
		{
			u8 order = *((u8 *) value);
			/* Only one victim system could be activated. */
			if (order == START) {
				v_tatcoex_gpsv_data.order = order;
			} else {
				v_tatcoex_gpsv_data.order = order;
			}
			SYSLOG(LOG_DEBUG, "Set GPS order START(0) STOP(1): %i\n", v_tatcoex_gpsv_data.order);
		}
		break;

	case ACT_COEX_RX2G5_ALL_FIRST_ARFCN:
		{
			v_tatcoex_2grfall_data.firstArfcn = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set first ARFCN value: %i\n", v_tatcoex_2grfall_data.firstArfcn);
		}
		break;

	case ACT_COEX_RX3G_ALL_FIRST_ARFCN:
		{
			v_tatcoex_3grfall_data.firstArfcn = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set first ARFCN value: %i\n", v_tatcoex_3grfall_data.firstArfcn);
		}
		break;

	case ACT_COEX_RX2G5_ALL_LAST_ARFCN:
		{
			v_tatcoex_2grfall_data.lastArfcn = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set last ARFCN value: %i\n", v_tatcoex_2grfall_data.lastArfcn);
		}
		break;

	case ACT_COEX_RX3G_ALL_LAST_ARFCN:
		{
			v_tatcoex_3grfall_data.lastArfcn = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set last ARFCN value: %i\n", v_tatcoex_3grfall_data.lastArfcn);
		}
		break;

	case ACT_COEX_RX2G5_ALL_STEP:
		{
			v_tatcoex_2grfall_data.step = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set step value: %i\n", v_tatcoex_2grfall_data.step);
		}
		break;

	case ACT_COEX_RX3G_ALL_STEP:
		{
			v_tatcoex_3grfall_data.step = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set step value: %i\n", v_tatcoex_3grfall_data.step);
		}
		break;

	case ACT_COEX_RX2G5_ONE_FREQBAND:
		{
			v_tatcoex_2grfone_data.band = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set band value: %i\n", v_tatcoex_2grfone_data.band);
		}
		break;

	case ACT_COEX_RX3G_ONE_FREQBAND:
		{
			v_tatcoex_3grfone_data.band = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set band value: %i\n", v_tatcoex_3grfone_data.band);
		}
		break;

	case ACT_COEX_RX2G5_ALL_FREQBAND:
		{
			v_tatcoex_2grfall_data.band = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set band value: %i\n", v_tatcoex_2grfall_data.band);
		}
		break;

	case ACT_COEX_RX3G_ALL_FREQBAND:
		{
			v_tatcoex_3grfall_data.band = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set band value: %i\n", v_tatcoex_3grfall_data.band);
		}
		break;

	case ACT_COEX_RX2G5_ONE_CHANNEL:
		{
			v_tatcoex_2grfone_data.channel = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set channel value: %i\n", v_tatcoex_2grfone_data.channel);
		}
		break;

	case ACT_COEX_RX2G5_ONE_MONITOR:
		{
			v_tatcoex_2grfone_data.monitor = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set monitor value: %i\n", v_tatcoex_2grfone_data.monitor);
		}
		break;

	case ACT_COEX_RX3G_ONE_UARFCN:
		{
			v_tatcoex_3grfone_data.channel = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set channel value: %i\n", v_tatcoex_3grfone_data.channel);
		}
		break;

	case ACT_COEX_RX2G5_ONE_AFCTAB:
		{
			v_tatcoex_2grfone_data.afcTable = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set afc table value: %i\n", v_tatcoex_2grfone_data.afcTable);
		}
		break;

	case ACT_COEX_RX3G_ONE_AFC_TABLE:
		{
			v_tatcoex_3grfone_data.afcTable = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set afc table value: %i\n", v_tatcoex_3grfone_data.afcTable);
		}
		break;

	case ACT_COEX_RX2G5_ALL_AFCTAB:
		{
			v_tatcoex_2grfall_data.afcTable = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set afc table value: %i\n", v_tatcoex_2grfall_data.afcTable);
		}
		break;

	case ACT_COEX_RX3G_ALL_AFC_TABLE:
		{
			v_tatcoex_3grfall_data.afcTable = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set afc table value: %i\n", v_tatcoex_3grfall_data.afcTable);
		}
		break;

	case ACT_COEX_RX2G5_ONE_AFCVAL:
		{
			v_tatcoex_2grfone_data.afcValue = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set afc value: %i\n", v_tatcoex_2grfone_data.afcValue);
		}
		break;

	case ACT_COEX_RX3G_ONE_MANUAL_AFC:
		{
			v_tatcoex_3grfone_data.afcValue = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set afc value: %i\n", v_tatcoex_3grfone_data.afcValue);
		}
		break;

	case ACT_COEX_RX2G5_ALL_AFCVAL:
		{
			v_tatcoex_2grfall_data.afcValue = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set afc value: %i\n", v_tatcoex_2grfall_data.afcValue);
		}
		break;

	case ACT_COEX_RX3G_ALL_MANUAL_AFC:
		{
			v_tatcoex_3grfall_data.afcValue = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set afc value: %i\n", v_tatcoex_3grfall_data.afcValue);
		}
		break;

	case ACT_COEX_RX2G5_ONE_AGCMODE:
		{
			v_tatcoex_2grfone_data.agcMode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set agc mode value: %i\n", v_tatcoex_2grfone_data.agcMode);
		}
		break;

	case ACT_COEX_RX3G_ONE_AGCMODE:
		{
			v_tatcoex_3grfone_data.agcMode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set agc mode value: %i\n", v_tatcoex_3grfone_data.agcMode);
		}
		break;

	case ACT_COEX_RX2G5_ALL_AGCMODE:
		{
			v_tatcoex_2grfall_data.agcMode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set agc mode value: %i\n", v_tatcoex_2grfall_data.agcMode);
		}
		break;

	case ACT_COEX_RX3G_ALL_AGCMODE:
		{
			v_tatcoex_3grfall_data.agcMode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set agc mode value: %i\n", v_tatcoex_3grfall_data.agcMode);
		}
		break;

	case ACT_COEX_RX2G5_ONE_EXPECTPW:
		{
			v_tatcoex_2grfone_data.expectedPower = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set expected power value: %i\n", v_tatcoex_2grfone_data.expectedPower);
		}
		break;
	case ACT_COEX_RX3G_ONE_RX_LEVEL:
		{
			v_tatcoex_3grfone_data.expectedPower = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set expected power value: %i\n", v_tatcoex_3grfone_data.expectedPower);
		}
		break;
	case ACT_COEX_RX2G5_ALL_EXPECTPW:
		{
			v_tatcoex_2grfall_data.expectedPower = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set expected power value: %i\n", v_tatcoex_2grfall_data.expectedPower);
		}
		break;
	case ACT_COEX_RX3G_ALL_RX_LEVEL:
		{
			v_tatcoex_3grfall_data.expectedPower = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set expected power value: %i\n", v_tatcoex_3grfall_data.expectedPower);
		}
		break;

	case ACT_COEX_RX2G5_ONE_OPMODE:
		{
			v_tatcoex_2grfone_data.operationMode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set operation mode value: %i\n", v_tatcoex_2grfone_data.operationMode);
		}
		break;

	case ACT_COEX_RX2G5_ALL_OPMODE:
		{
			v_tatcoex_2grfall_data.operationMode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set operation mode value: %i\n", v_tatcoex_2grfall_data.operationMode);
		}
		break;

	case ACT_COEX_BTRX_ONE_CHAN:
		{
			v_tatcoex_btrxone_data.channel = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Rx channel : %d\n", v_tatcoex_btrxone_data.channel);
		}
		break;

	case ACT_COEX_BTRX_ONE_ADDRIDX:
		{
			v_tatcoex_btrxone_data.addr_idx = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Rx addridx : %d\n", v_tatcoex_btrxone_data.addr_idx);
		}
		break;

	case ACT_COEX_BTRX_ONE_PKTT:
		{
			v_tatcoex_btrxone_data.pktt = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Rx pktt : %d\n", v_tatcoex_btrxone_data.pktt);
		}
		break;

	case ACT_COEX_BTRX_ONE_RSSI:
		{
			v_tatcoex_btrxone_data.rssi = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Rx rssi : %d\n", v_tatcoex_btrxone_data.rssi);

		}

	case ACT_COEX_BTRX_ONE_BER:
		{
			v_tatcoex_btrxone_data.ber = *((float *)value);
			SYSLOG(LOG_DEBUG, "Set BT Rx ber : %f\n", v_tatcoex_btrxone_data.ber);

		}

	case ACT_COEX_VIC_BTTX_CHAN:
		{
			v_tatcoex_bttx_data.channel = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Tx channel : %d\n", v_tatcoex_bttx_data.channel);
		}
		break;

	case ACT_COEX_VIC_BTTX_TXLEV:
		{
			v_tatcoex_bttx_data.tx_level = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Tx tx level : %d\n", v_tatcoex_bttx_data.tx_level);
		}
		break;

	case ACT_COEX_VIC_BTTX_ADDRIDX:
		{
			v_tatcoex_bttx_data.addr_idx = *((s16 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Tx addr idx : %d\n", v_tatcoex_bttx_data.addr_idx);
		}
		break;

	case ACT_COEX_VIC_BTTX_PKTT:
		{
			v_tatcoex_bttx_data.pktt = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set BT Tx pkt type : %d\n", v_tatcoex_bttx_data.pktt);
		}
		break;

	case ACT_COEX_FMRX_ONE_CHAN:
		{
			v_tatcoex_fmrxone_data.channel = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Rx channel : %d\n", v_tatcoex_fmrxone_data.channel);
		}
		break;

	case ACT_COEX_FMRX_ONE_MODE:
		{
			v_tatcoex_fmrxone_data.mode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Rx mode : %d\n", v_tatcoex_fmrxone_data.mode);
		}
		break;

	case ACT_COEX_FMRX_ONE_EMP:
		{
			v_tatcoex_fmrxone_data.emp = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Rx emphasis : %d\n", v_tatcoex_fmrxone_data.emp);
		}
		break;

	case ACT_COEX_FMRX_ONE_RSSI:
		{
			v_tatcoex_fmrxone_data.rssi = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Rx rssi : %d\n", v_tatcoex_fmrxone_data.rssi);

		}

	case ACT_COEX_FMRX_ALL_MODE:
		{
			v_tatcoex_fmrxall_data.mode = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set FM All Rx mode : %d\n", v_tatcoex_fmrxall_data.mode);
		}
		break;

	case ACT_COEX_FMRX_ALL_FCHAN:
		{
			v_tatcoex_fmrxall_data.fchannel = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set FM All Rx first channel : %d\n", v_tatcoex_fmrxall_data.fchannel);
		}
		break;

	case ACT_COEX_FMRX_ALL_LCHAN:
		{
			v_tatcoex_fmrxall_data.lchannel = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set FM All Rx last channel : %d\n", v_tatcoex_fmrxall_data.lchannel);
		}
		break;

	case ACT_COEX_FMRX_ALL_STEP:
		{
			v_tatcoex_fmrxall_data.step = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set FM All Rx step : %d\n", v_tatcoex_fmrxall_data.step);
		}
		break;

	case ACT_COEX_FMRX_ALL_HOLD:
		{
			v_tatcoex_fmrxall_data.hold = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set FM All Rx hold : %d\n", v_tatcoex_fmrxall_data.hold);
		}
		break;

	case ACT_COEX_VIC_FMTX_FREQ:
		{
			v_tatcoex_fmtx_data.freq = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx channel : %d\n", v_tatcoex_fmtx_data.freq);
		}
		break;

	case ACT_COEX_VIC_FMTX_TONEFREQ:
		{
			v_tatcoex_fmtx_data.tonefreq = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx tone freq : %d\n", v_tatcoex_fmtx_data.tonefreq);
		}
		break;

	case ACT_COEX_VIC_FMTX_PWR:
		{
			v_tatcoex_fmtx_data.pwr = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx level : %d\n", v_tatcoex_fmtx_data.pwr);
		}
		break;

	case ACT_COEX_VIC_FMTX_MODE:
		{
			v_tatcoex_fmtx_data.mode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx mode : %d\n", v_tatcoex_fmtx_data.mode);
		}
		break;

	case ACT_COEX_VIC_FMTX_SRC:
		{
			v_tatcoex_fmtx_data.src = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx emphasis : %d\n", v_tatcoex_fmtx_data.src);
		}
		break;

	case ACT_COEX_VIC_FMTX_VOL:
		{
			v_tatcoex_fmtx_data.volume = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx volume : %d\n", v_tatcoex_fmtx_data.volume);
		}
		break;

	case ACT_COEX_VIC_FMTX_EMP:
		{
			v_tatcoex_fmtx_data.emp = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set FM Tx mode : %d\n", v_tatcoex_fmtx_data.emp);
		}
		break;

	case ACT_COEX_GPS_INT:
		{
			v_tatcoex_gpsv_data.interval = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set GPS interval : %d\n", v_tatcoex_gpsv_data.interval);
		}
		break;

	case ACT_COEX_GPS_LONG:
		{
			v_tatcoex_gpsv_data.longitude = *((float *)value);
			SYSLOG(LOG_DEBUG, "Set GPS longitude : %f\n", v_tatcoex_gpsv_data.longitude);
		}
		break;

	case ACT_COEX_GPS_LAT:
		{
			v_tatcoex_gpsv_data.latitude = *((float *)value);
			SYSLOG(LOG_DEBUG, "Set GPS longitude : %f\n", v_tatcoex_gpsv_data.latitude);
		}
		break;

	case ACT_COEX_GPS_ALT:
		{
			v_tatcoex_gpsv_data.altitude = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set GPS longitude : %d\n", v_tatcoex_gpsv_data.altitude);
		}
		break;

	case ACT_COEX_GPS_SNUM:
		{
			v_tatcoex_gpsv_data.satnum = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set GPS satnum : %d\n", v_tatcoex_gpsv_data.satnum);
		}
		break;

	case ACT_COEX_WLANRX_MODEBAND:
		{
			v_tatcoex_wlanrx_data.modeband = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN modeband : %d\n", v_tatcoex_wlanrx_data.modeband);
		}
		break;

	case ACT_COEX_WLANRX_CHAN:
		{
			v_tatcoex_wlanrx_data.channel = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN channel : %d\n", v_tatcoex_wlanrx_data.channel);
		}
		break;

	case ACT_COEX_WLANRX_PKTRX:
		{
			v_tatcoex_wlanrx_data.pktrx = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN pkt rx : %d\n", v_tatcoex_wlanrx_data.pktrx);
		}
		break;

	case ACT_COEX_WLANRX_PKTEXP:
		{
			v_tatcoex_wlanrx_data.pktexp = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN pkt exp : %d\n", v_tatcoex_wlanrx_data.pktexp);
		}
		break;

	case ACT_COEX_WLANRX_ERRORS:
		{
			v_tatcoex_wlanrx_data.errors = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN errors : %d\n", v_tatcoex_wlanrx_data.errors);
		}
		break;

	case ACT_COEX_WLANRX_CRCERRORS:
		{
			v_tatcoex_wlanrx_data.crcerrors = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN crc errors : %d\n", v_tatcoex_wlanrx_data.crcerrors);
		}
		break;

	case ACT_COEX_WLANRX_PER:
		{
			v_tatcoex_wlanrx_data.per = *((float *)value);
			SYSLOG(LOG_DEBUG, "Set WLAN per : %f\n", v_tatcoex_wlanrx_data.per);
		}
		break;

	case ACT_COEX_VIC_WLANTX_MODE:
		{
			v_tatcoex_wlantx_data.mode = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx mode : %d\n", v_tatcoex_wlantx_data.mode);
		}
		break;

	case ACT_COEX_VIC_WLANTX_CHAN:
		{
			v_tatcoex_wlantx_data.channel = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx channel : %d\n", v_tatcoex_wlantx_data.channel);
		}
		break;

	case ACT_COEX_VIC_WLANTX_BAND:
		{
			v_tatcoex_wlantx_data.band = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx band : %d\n", v_tatcoex_wlantx_data.band);
		}
		break;

	case ACT_COEX_VIC_WLANTX_RATE:
		{
			v_tatcoex_wlantx_data.rate = *((u16 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx rate : %d\n", v_tatcoex_wlantx_data.rate);
		}
		break;

	case ACT_COEX_VIC_WLANTX_PWRIDX:
		{
			v_tatcoex_wlantx_data.pwridx = *((float *)value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx pwr idx : %f\n", v_tatcoex_wlantx_data.pwridx);
		}
		break;

	case ACT_COEX_VIC_WLANTX_ACK:
		{
			v_tatcoex_wlantx_data.ack = *((u8 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx ack : %d\n", v_tatcoex_wlantx_data.ack);
		}
		break;

	case ACT_COEX_VIC_WLANTX_IFD:
		{
			v_tatcoex_wlantx_data.ifd = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx ifd : %d\n", v_tatcoex_wlantx_data.ifd);
		}
		break;

	case ACT_COEX_VIC_WLANTX_PKTSNT:
		{
			v_tatcoex_wlantx_data.pktsent = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx pkt snt : %d\n", v_tatcoex_wlantx_data.pktsent);
		}
		break;

	case ACT_COEX_VIC_WLANTX_PKTACK:
		{
			v_tatcoex_wlantx_data.pktack = *((u32 *) value);
			SYSLOG(LOG_DEBUG, "Set WLAN Tx pkt ack : %d\n", v_tatcoex_wlantx_data.pktack);
		}
		break;

	default:
		vl_error = EBADRQC;
		break;
	}

	return vl_error;

}

void tatl07_03Init_Victim_Data()
{
	tatl07_04Init_Rf_Data();

	tatl07_10Init_FM_Data();

	tatl07_13Init_BT_Data();

	tatl07_16Init_GPSV_Data();
}

