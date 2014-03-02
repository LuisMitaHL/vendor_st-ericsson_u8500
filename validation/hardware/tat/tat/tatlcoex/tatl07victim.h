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

#ifndef TATL07VICTIM_H_
#define TATL07VICTIM_H_

/* ----------------------------------------------------------------------- */
/* Constants                                                               */
/* ----------------------------------------------------------------------- */
#define	RF_2G_LOG_FILE		"/usr/local/bin/tat/tatlcoex/logFile2G"
#define	RF_3G_LOG_FILE		"/usr/local/bin/tat/tatlcoex/logFile3G"

/* ----------------------------------------------------------------------- */
/* Typedefs                                                                */
/* ----------------------------------------------------------------------- */

/**
 * List of available orders supported by a process.
 */
typedef enum{
	NO_SYSTEM = 0,
	RF2G,	/**< 2G cellular. */
	RF3G,   /**< 3G cellular. */
	UNKNOWN_SYSTEM
} DTH_CELLULAR_TYPE;

/**
 * Definition of data needed to manage RF process
 */
typedef struct{
    DTH_COEX_ORDER order; /**< Task to be done by the process (START/STOP). */
	DTH_COEX_STATE state; /**< State of the process (ACTIVATED/DEACTIVATED). */
	char *error ; /**< Error string .*/
	DTH_CELLULAR_TYPE type ; /**< Cellular type. */
	u16 firstArfcn ; /**< 1st channel of the group. */
	u16 lastArfcn ; /**< last channel of the group. */
	u32 band ; /**< RF band selected. */
	u8 step ; /**< step between each RF RX process when a group of channel is chosen. */
	u16 channel ;	/**< channed selected to perform one RF RX. */
    u16 monitor ;	/**< monitored channel selected to perform one RF RX. */
	u8 operationMode ;
	u8 afcTable ;
	u32 afcValue ;
	u8 agcMode ;
	s16 expectedPower ;
} DTH_RF_SYSTEM_DATA ;

typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	DTH_COEX_STATE state; /**< State of the process. */
	u16 channel; /**< Transmission frequency */
	s16 addr_idx; /**< LSB of device address to use */
	u8 pktt; /**< Packet Type */
	s16 rssi; /**< RSSI */
	u16 nump;
	u16 wrtype;
	u16 wrlen;
	u16 err;
	u16 connectionhandle;
	float ber; /**< Bit Error Rate */
	char *error ; /**< Error string .*/
	float berrors; /**< Bit Error Rate */
} DTH_BTRX_DATA ;

typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	DTH_COEX_STATE state; /**< State of the process. */
	u32 channel; /**< Transmission frequency */
	u32 fchannel;
	u32 lchannel;
	u32 step;
	u32 hold;
	u8 mode; /**< Mode */
	u16 rssi; /**< RSSI */
	u8 emp;
	char *error ; /**< Error string .*/
	char *rssifile ; /**< RSSI file .*/
} DTH_FMRX_DATA ;

typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	DTH_COEX_STATE state; /**< State of the process. */
	u32 interval;
	u32 ttff;
	float longitude;
	float latitude;
	u16 altitude;
	u8 satnum;
	char *error ; /**< Error string .*/
	u8 snrdata[16];
} DTH_GPSV_DATA ;

/* ----------------------------------------------------------------------- */
/* Global variables                                                        */
/* ----------------------------------------------------------------------- */
DTH_RF_SYSTEM_DATA v_tatcoex_2grfone_data; /* Data needed for 2G RF One. */
DTH_RF_SYSTEM_DATA v_tatcoex_3grfone_data; /* Data needed for 3G RF One. */
DTH_RF_SYSTEM_DATA v_tatcoex_2grfall_data; /* Data needed for 2G RF All. */
DTH_RF_SYSTEM_DATA v_tatcoex_3grfall_data; /* Data needed for 3G RF All. */
DTH_BTRX_DATA v_tatcoex_btrxone_data; /* Data needed for BT Rx One process. */
DTH_BTRX_DATA v_tatcoex_btrxall_data; /* Data needed for BT Rx All process. */
DTH_FMRX_DATA v_tatcoex_fmrxone_data; /* Data needed for FM Rx One process. */
DTH_FMRX_DATA v_tatcoex_fmrxall_data; /* Data needed for FM Rx All process. */
DTH_GPSV_DATA v_tatcoex_gpsv_data; /* Data needed for GPS Victim process. */
pthread_t v_tatcoex_victim_thread; /* Thread to start a victim system activity. */

/* ----------------------------------------------------------------------- */
/*  Main DTH entry points                                                  */
/* ----------------------------------------------------------------------- */

/**
 * Start/Stop traffic on DSI/YCBCR buses.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl07_00Victim_Exec(struct dth_element *elem);

/**
 * Get outputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl07_01Victim_Get(struct dth_element *elem, void *value);

/**
 * Set inputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl07_02Victim_Set(struct dth_element *elem, void *value);

/* ----------------------------------------------------------------------- */
/* Initialization                                                          */
/* ----------------------------------------------------------------------- */

/**
 *  Initialize data structure needed for Victim system
 */
void tatl07_03Init_Victim_Data();

/* ----------------------------------------------------------------------- */
/* Module private functions                                                */
/* ----------------------------------------------------------------------- */

/* GLOBAL void tatl07_15BT_All_Channel_Process (void *threadData); */

#endif /* TATL07VICTIM_H_ */


