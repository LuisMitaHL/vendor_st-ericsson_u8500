/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    tatl03misc.h
* \brief   declaration of common SIM operations and tools
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TATL03MISC_H_
#define TATL03MISC_H_

#include <modem_test_msg_ext.h>

/* extract Fi factor from FD factor */
#define PPS_FI_FACTOR(fd_factor) \
    (((fd_factor) & 0xF0) >> 4)

/* extract Di factor from FiDi factor */
#define PPS_DI_FACTOR(fd_factor) \
    ((fd_factor) & 0x0F)

void tatl3_initModule(void);
void tatl3_printIsiHeader(t_isi_header *req);
void tatl3_printModemReq(t_ModemTest_RunReq *req);
void tatl3_printModemResp(t_ModemTest_RunResp *resp);
void tatl3_printHatsExchange(tSIMHALTST_HatsExchange *ex);
void tatl3_marshModemRunReq(t_ModemTest_RunReq *req, int marsh);
void tatl3_marshModemRunResp(t_ModemTest_RunResp *resp, int marsh);
void tatl3_dumpSimReq(uint8_t *req);
void tatl3_dumpSimResp(u8 *resp);

/**
 * Build and Send ISI messages needed to manage SIM
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl3_00GetSimInfo_exec(struct dth_element *elem);

/**
 * Get  SIM output values.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the location where data read must be stored.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl3_02GetSimInfoParam_get(struct dth_element *elem, void *value);

/**
 * Set SIM inputs arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value specifies the argument to set.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl3_03GetSimInfoParam_set(struct dth_element *elem, void *value);

/**
 * Stop SIM activity.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl3_04StopSimActivity(void);

#define TATLSIM_ATR_LENGTH      255
#define TATLSIM_NB_APDU_DATA    128
#define TATLSIM_NB_APDU_RESULT  255

enum {
    TATSIM_SIM_DEACTIVATED,
    TATSIM_SIM_ACTIVATED
};

#define TATLSIM_OUT_OF_ARRAY            0
#define TATLSIM_CELL                    1
#define TATLSIM_ARRAY                   2

/**
 * Tells if which part of the array \a elem is designated by \a row, \a col.
 * @param[in] elem address of a valid DTH array
 * @param[in] row   row of cell to check
 * @param[in] col   column of cell to check
 * @retval TATLSIM_OUT_OF_ARRAY : \a row, \a col is out of the array
 * @retval TATLSIM_CELL : \a row, \a col is a cell in the array
 * @retval TATLSIM_ARRAY : \a row, \a col is the entire array
 */
int talt3_07IsCellArray(struct dth_element *elem, int row, int col);

#endif /* TATL03MISC_H_ */


