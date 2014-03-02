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

#ifndef TATL08AUDIO_H_
#define TATL08AUDIO_H_

/* ----------------------------------------------------------------------- */
/* Constants                                                               */
/* ----------------------------------------------------------------------- */
#define INIT_AUDIO		"sh /usr/bin/scripts/hats_services/initaudio.sh"
#define DEINIT_AUDIO	"sh /usr/bin/scripts/hats_services/deinitaudio.sh"
#define	START_MIC		"sh /usr/bin/scripts/hats_services/StartDMIC.sh"
#define	STOP_MIC		"sh /usr/bin/scripts/hats_services/StopDMIC.sh"
#define	START_VIB		"sh /usr/bin/scripts/hats_services/StartVIB.sh"
#define	STOP_VIB		"sh /usr/bin/scripts/hats_services/StopVIB.sh"
#define	START_MSP1		"sh /usr/bin/scripts/hats_services/StartMSP1.sh"
#define	STOP_MSP1		"sh /usr/bin/scripts/hats_services/StopMSP1.sh"
#define	START_MSP2		"sh /usr/bin/scripts/hats_services/StartMSP2.sh"
#define	STOP_MSP2		"sh /usr/bin/scripts/hats_services/StopMSP2.sh"

/* ----------------------------------------------------------------------- */
/* Typedefs                                                                */
/* ----------------------------------------------------------------------- */

/**
 * Definition of data needed to manage RF process
 */
typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process (START/STOP). */
	DTH_COEX_STATE state; /**< State of the process (ACTIVATED/DEACTIVATED). */
	char *error ; /**< Error string .*/
	char *startService ; /**< Path to the shell script to start the service.*/
	char *stopService ; /**< Path to the shell script to stop the service .*/
} DTH_AUDIO_DATA ;

/* ----------------------------------------------------------------------- */
/* Global variables                                                        */
/* ----------------------------------------------------------------------- */
DTH_AUDIO_DATA v_tatcoex_vib_data; /* Data needed for vibrators management. */
DTH_AUDIO_DATA v_tatcoex_mic_data; /* Data needed for microphones management. */
DTH_AUDIO_DATA v_tatcoex_msp1_data; /* Data needed for MSP1 bus management. */
DTH_AUDIO_DATA v_tatcoex_msp2_data; /* Data needed for MSP2 bus management. */
int v_tatcoex_init_audio_done;

/* ----------------------------------------------------------------------- */
/*  Main DTH entry points                                                  */
/* ----------------------------------------------------------------------- */

/**
 * Start/Stop traffic on DSI/YCBCR buses.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl08_00Audio_Exec(struct dth_element *elem);

/**
 * Get outputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl08_01Audio_Get(struct dth_element *elem, void *value);

/**
 * Set inputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl08_02Audio_Set(struct dth_element *elem, void *value);


/* ----------------------------------------------------------------------- */
/* Initialization                                                          */
/* ----------------------------------------------------------------------- */

/**
 *  Initialize data structure needed for Audio activity generation.
 *  @param[in] data data to be initialized.
 *  @param[in] start Path to the shell script to start the service.
 *  @param[in] stop Path to the shell script to stop the service.
 */
void tatl08_03Init_Audio_Data (DTH_AUDIO_DATA *data, char* start, char* stop);

#endif /* TATL08AUDIO_H_ */

