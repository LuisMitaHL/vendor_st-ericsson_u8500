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

#ifndef TATL12CONN_H_
#define TATL12CONN_H_

/* ----------------------------------------------------------------------- */
/* Typedefs                                                                */
/* ----------------------------------------------------------------------- */

/**
 * Definition of data needed to manage connectivity buses
 */
typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	DTH_COEX_STATE state; /**< State of the process. */
	int idx; /** Number of starts for CG2900, CW1200. */
	char *error ; /**< Error string .*/
} DTH_CONN_DATA ;

typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	DTH_COEX_STATE state; /**< State of the process. */
	u16 channel; /**< Transmission frequency */
	s8 tx_level; /**< Trasmission power level */
	s16 addr_idx; /**< LSB of device address to use */
	u8 pktt; /**< Packet Type */
	char *error ; /**< Error string .*/
} DTH_BTTX_DATA ;

typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	DTH_COEX_STATE state; /**< State of the process. */
	u32 freq; /**< Transmission frequency */
	u16 pwr; /**< Trasmission power level */
	u8 mode; /**< Mode */
	u8 src;
	u16 volume;
	u8 emp;
	u32 tonefreq;
	char *error ; /**< Error string .*/
} DTH_FMTX_DATA ;

#ifdef CONNECTIVITY_PANEL
typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	u8 state; /**< State of the process. */
    u8 mode;
	u8 frequency;
	u8 length;
	u8 payload;
	u16 rcvdpkts;
	char *error ; /**< Error string .*/
} DTH_BLEDUT_DATA ;

typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	DTH_COEX_STATE state; /**< State of the process. */
	int idx; /** Number of starts for CG2900, CW1200, FM IP.... */
	u32 interval;
	char *error ; /**< Error string .*/
} DTH_GPS_DATA ;

typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	DTH_COEX_STATE state; /**< State of the process. */
	u8 min;
	u8 maj;
	u8 rad;
	char *patchid;
	char *patchdate;
	u8 vendor;
	u8 hci;
	u8 lmp;
	char *error ; /**< Error string .*/
} DTH_CGVER_DATA ;
#endif

typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	DTH_COEX_STATE state; /**< State of the process. */
	u8 mode;
	u16 channel;
	u8 band;
	u16 rate;
	float pwridx;
	u8 ack;
	u32 ifd;
	u32 pktsent;
	u32 pktack;
	char *error ; /**< Error string .*/
} DTH_WLANTX_DATA;

typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process. */
	DTH_COEX_STATE state; /**< State of the process. */
	u8 modeband;
	u16 channel;
	u32 pktexp;
	u32 pktrx;
	u32 errors;
	u32 crcerrors;
	float per;
	char *error ; /**< Error string .*/
} DTH_WLANRX_DATA;

/* ----------------------------------------------------------------------- */
/* Global variables                                                        */
/* ----------------------------------------------------------------------- */
DTH_CONN_DATA v_tatcoex_cg2900_data; /* Data needed for CG2900 process. */
DTH_CONN_DATA v_tatcoex_btip_data; /* Data needed for BT IP process. */
DTH_CONN_DATA v_tatcoex_fmip_data; /* Data needed for FM IP process. */
DTH_CONN_DATA v_tatcoex_btdut_data; /* Data needed for BT DUT process. */

DTH_CONN_DATA v_tatcoex_cw1200_data; /* Data needed for CG2900 process. */
DTH_CONN_DATA v_tatcoex_uart0_data; /* Data needed for UART0 process. */
DTH_CONN_DATA v_tatcoex_msp0_data; /* Data needed for MSP0 process. */
DTH_CONN_DATA v_tatcoex_mc1_data; /* Data needed for MC1 process. */
DTH_CONN_DATA v_tatcoex_i2s_data; /* Data needed for I2S process. */
DTH_BTTX_DATA v_tatcoex_bttx_data; /* Data needed for BT Tx process. */
DTH_FMTX_DATA v_tatcoex_fmtx_data; /* Data needed for FM Tx process. */

#ifdef CONNECTIVITY_PANEL
DTH_BLEDUT_DATA v_tatcoex_bledut_data; /* Data needed for BLE DUT process. */

DTH_GPS_DATA v_tatcoex_gpsip_data; /* Data needed for GPS IP process. */
DTH_CGVER_DATA v_tatcoex_cgver_data; /* Data needed for CG2900 Ver process. */
#endif

DTH_WLANTX_DATA v_tatcoex_wlantx_data; /* Data needed for WLAN Tx process. */
DTH_WLANRX_DATA v_tatcoex_wlanrx_data; /* Data needed for WLAN Rx process. */

/* ----------------------------------------------------------------------- */
/*  Main DTH entry points                                                  */
/* ----------------------------------------------------------------------- */

/**
 * Start/Stop traffic on MC buses.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl12_00Conn_Exec(struct dth_element *elem);

/**
 * Get outputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl12_01Conn_Get(struct dth_element *elem, void *value);

/**
 * Set inputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl12_02Conn_Set(struct dth_element *elem, void *value);

/* ----------------------------------------------------------------------- */
/* Initialization                                                          */
/* ----------------------------------------------------------------------- */

/**
 *  Initialize data structure needed for MC buses.
 *  @param[in] data data needed to manage MC buses.
 *  @param[in] source Path to the special device file that enables access to the memory device.
 *  @param[in] target Directory where the memory device must be mounted.
 *  @param[in] file File name where perform continuous writing accesses.
 *  @param[in] fstype Width of the picture to be displayed.
 */
void tatl12_03Conn_Init_Data(DTH_CONN_DATA *data);

/**
 *  Initialize data structure needed for MC buses.
 *  @param[in] data data needed to manage MC buses.
 *  @param[in] source Path to the special device file that enables access to the memory device.
 *  @param[in] target Directory where the memory device must be mounted.
 *  @param[in] file File name where perform continuous writing accesses.
 *  @param[in] fstype Width of the picture to be displayed.
 */
void tatl12_03Conn_Init_BTTX_Data(DTH_BTTX_DATA *data);
void tatl12_03Conn_Init_FMTX_Data(DTH_FMTX_DATA *data);

void tatl12_03Conn_Init_WLANTX_Data(DTH_WLANTX_DATA *data);

void tatl12_03Conn_Init_WLANRX_Data(DTH_WLANRX_DATA *data);

#ifdef CONNECTIVITY_PANEL
void tatl12_03Conn_Init_BLEDUT_Data(DTH_BLEDUT_DATA *data);

void tatl12_03Conn_Init_GPS_Data(DTH_GPS_DATA *data);
void tatl12_03Conn_Init_CGVER_Data(DTH_CGVER_DATA *data);
#endif

int tatl12_18Conn_FMRX_Audio_Configuration();
int tatl12_18Conn_FMRX_Audio_DeConfiguration();
int tatl12_18Conn_FMTX_Audio_Configuration();
int tatl12_18Conn_FMTX_Audio_DeConfiguration();

#endif /* TATL12CONN_H_ */

