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

#ifndef TATL03DISPLAY_H_
#define TATL03DISPLAY_H_

/* ----------------------------------------------------------------------- */
/* Constants                                                               */
/* ----------------------------------------------------------------------- */
#define	DSI0_FILE			    "/usr/share/multimediaservices/videoservices/chess_864x480_rgb888.rgb"
#define	DSI0_WIDTH			    864
#define	DSI0_HEIGHT			    480
#define	DSI0_PICTURE_FORMAT	    1 /* RGB888 */
#define	DSI0_LOOP_MODE		    1 /* Fixed rate. */
#define	DSI0_LOOP_RATE		    100
#define	DSI0_FREQUENCY		    0
#define	DSI0_SCAN_MODE		    0

#define	DSI2_FILE			    "/usr/share/multimediaservices/videoservices/image_1920_1080.png"
#define	DSI2_WIDTH			    1920
#define	DSI2_HEIGHT			    1080
#define	DSI2_PICTURE_FORMAT	    1 /* PNG. */
#define	DSI2_LOOP_MODE		    1 /* Fixed rate. */
#define	DSI2_LOOP_RATE		    100
#define	DSI2_FREQUENCY		    30
#define	DSI2_SCAN_MODE		    1 /* Progressive. */

#define POWER_MODE_ON           1
#define POWER_MODE_OFF          0


/* ----------------------------------------------------------------------- */
/* Typedefs                                                                */
/* ----------------------------------------------------------------------- */

/**
 * Definition of data needed to manage display
 */
typedef struct{
	DTH_COEX_ORDER order; /**< Task to be done by the process (START/STOP). */
	DTH_COEX_STATE state; /**< State of the process (ACTIVATED/DEACTIVATED). */
	char *error ; /**< Error string .*/
	u32 displayId ; /**< Id of the display. */
	u32 powerMode ; /**< Power mode of the display selected: Off(0) On(1) Lowpower(2). */
	char *filePath ; /**< Path to the picture to be displayed. */
	u32 width ; /**< Width of the picture to be displayed. */
	u32 height ; /**< Height of the picture to be displayed. */
	u32 pictureFormat ; /**< Format of the picture: RGB888(0) PNG(1) JPEG(2) RAW (3). */
	u32 loopMode ; /**< Loop mode : no_loop(0) loop_at_fixed_rate(1) loop_at_max_speed(2). */
	u32 loopRate ; /**< Loop rate. */
	u32 frequency ; /**< Display frequency (only for TV-OUT and HDMI). */
	u32 scanMode ; /**< Scan Mode (only for TV-OUT and HDMI). */
} DTH_DISPLAY_DATA ;


/* ----------------------------------------------------------------------- */
/* Global variables                                                        */
/* ----------------------------------------------------------------------- */
DTH_DISPLAY_DATA v_tatcoex_dsi0_data; /* Data needed for DSI0 (main display). */
DTH_DISPLAY_DATA v_tatcoex_dsi2_data; /* Data needed for DSI2 (HDMI display). */


/* ----------------------------------------------------------------------- */
/*  Main DTH entry points                                                  */
/* ----------------------------------------------------------------------- */

/**
 * Start/Stop traffic on DSI/YCBCR buses.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl03_00Display_Exec(struct dth_element *elem);

/**
 * Get outputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl03_01Display_Get(struct dth_element *elem, void *value);

/**
 * Set inputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl03_02Display_Set(struct dth_element *elem, void *value);


/* ----------------------------------------------------------------------- */
/* Initialization                                                          */
/* ----------------------------------------------------------------------- */

/**
 *  Initialize data structure needed for DSI and YCBCR buses.
 *  @param[in] data data needed to manage DSI and YCBCR buses.
 *  @param[in] displayId Id of the display.
 *  @param[in] powerMode Power mode of the display selected.
 *  @param[in] filePath Path to the picture to be displayed.
 *  @param[in] width Width of the picture to be displayed.
 *  @param[in] height Height of the picture to be displayed.
 *  @param[in] pictureFormat  Format of the picture.
 *  @param[in] loopMode Loop mode.
 *  @param[in] loopRate Loop rate.
 */
void tatl03_03Init_Dsi_Ycbcr_Data(DTH_DISPLAY_DATA *data, u32 displayId,
        u32 powerMode, char *filePath, u32 width, u32 height, u32 pictureFormat, 
        u32 loopMode, u32 loopRate, u32 frequency, u32 scanMode);


#endif /* TATL03DISPLAY_H_ */


