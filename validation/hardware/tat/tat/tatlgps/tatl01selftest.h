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
#ifndef TATL01SELFTEST_H_
#define TATL01SELFTEST_H_

 
 #if defined (TATL01SELFTEST_C)
 #define GLOBAL 
 #else
 #define GLOBAL extern
 #endif

/**
 * Run a GPS self test identified by elem->user_data which must
 * be one value defined in dthgps.h
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl01_00SelfTest_exec(struct dth_element *elem);

/**
 * Get a GPS self test output data.
 * Currently, there is no data returned by GPS Self test.
 * The function should not be called.
 * @param[in] elem specifies the DTH element.
 * @param[out] value adress of a element in memory, related to the DTH type,
 * to receive the current value of the elem.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl01_01SelfTest_get(struct dth_element *elem, void *value);

/**
 * Set the GPS self test arguments.
 * @param[in] elem specifies the DTH element.
 * @param[in] value the new value to apply.
 * @retval 0 success.
 * @retval errorCode if an error occured while processing.
 */
int tatl01_02SelfTest_set(struct dth_element *elem, void *value);

/* PRIVATE INTERFACE */
#define TATLGPS_SELFTEST_TRANS_ID		0x35


/* MODEM_TEST_ST_HANDLER_ID table is defined in modem_test_msg_ext.h provided
   with modem libraries but the file in /comp015/modem_lib/NWM_Include is not
   up to date. Moreover, HATS does not have dependency with another modem
   component in vob.
   use local defines when TATLGS_LOCAL_SELFTEST_DEFS is set */

#ifdef TATLGS_LOCAL_SELFTEST_DEFS

/* ----------------------------------------------------------------------- */
/* Constant Table: MODEM_TEST_ST_HANDLER_ID                                */
/* ----------------------------------------------------------------------- */
/* Modem self-test handler IDs for MODEM_TEST_GRP_SELFTEST_L1 and
   MODEM_TEST_GRP_SELFTEST_L23 groups.
*/
typedef uint16_t MODEM_TEST_ST_HANDLER_ID_CONST;

/* The self-test always returns MODEM_TEST_PASSED */
#define MODEM_TEST_ST_DUMMY                      0x0000
/* Self-test for checking accuracy of the sleep clock frequency */
#define MODEM_TEST_ST_SLEEP_CLK_FREQ             0x0001
/* Self-test for generating GPS timemark */
#define MODEM_TEST_ST_GSP_TIMEMARK_ON            0x0002
/* Self-test for clearing GPS timemark */
#define MODEM_TEST_ST_GSP_TIMEMARK_OFF           0x0003
/* Self-test for setting indication of modem TX transmission to GPS */
#define MODEM_TEST_ST_GPS_TX_ON                  0x0004
/* Self-test for clearing indication of modem TX transmission to GPS */
#define MODEM_TEST_ST_GPS_TX_OFF                 0x0005

#endif

#define TATGPS_FEATURE "GPS"
/* Structure of an ISI message for GPS self test */
typedef struct {
	t_isi_header hdr;
    uint8_t   trans_id;
    uint8_t   message_id;
    uint16_t  group_id;   /* Values from the constant table MODEM_TEST_GROUP */
    /* ID of the requested test-handler function. The licensee is responsible
       for assigning the test-handlers IDs.
    */
    uint16_t  test_id;
    /* ID of the requested test case. The licensee is responsible for
       assigning the tests case IDs.
    */
    uint16_t  case_ID;
    uint8_t   fillbyte[2];
    /* If zero, then input data length N is calculated from the PhoNet
       message length. => set to 0 for GPS Self test
    */
    uint16_t  inp_data_length;
} TATLGPS_SELF_TEST_RUN_REQ ;

typedef struct {
	t_isi_header hdr;
    uint8_t   trans_id;
    uint8_t   message_id;
    uint16_t  group_id;   /* Values from the constant table MODEM_TEST_GROUP */
    /* ID of the requested test-handler function. The value is copied from
       the request message.
    */
    uint16_t  test_id;
    /* ID of the requested test case. The value is copied from the request
       message.
    */
    uint16_t  case_ID;
    uint8_t   fillbyte;
    /* A subset of values from the constant table MODEM_TEST_STATUS */
    uint8_t   status;
    uint16_t  out_data_length;
} TATLGPS_SELF_TEST_RUN_RESP ;

#ifdef _DEBUG
void tatl01_04DumpSelfTestReq(TATLGPS_SELF_TEST_RUN_REQ *req);
void tatl01_05DumpSelfTestResp(TATLGPS_SELF_TEST_RUN_RESP *resp);
#endif /* _DEBUG */

GLOBAL int v_tatlgps_tx_state
#if defined (TATL01SELFTEST_C)
=0
#endif
;

GLOBAL int v_tatlgps_time_state
#if defined (TATL01SELFTEST_C)
=0
#endif
;

GLOBAL int v_tatlgps_tx_error
#if defined (TATL01SELFTEST_C)
=0
#endif
;

GLOBAL int v_tatlgps_time_error
#if defined (TATL01SELFTEST_C)
=0
#endif
;

#undef GLOBAL

#endif /* defined TATL01SELFTEST_H_ */
