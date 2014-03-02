/* This header is created temporarly until following informations are available
 in official header files from Modem or NWM*/
/* Inside this header, I will put all missing inforamtions available in Common
 DSP Test Server Message Specification v000.135.html but not provided in 
common_dsp_test_isi.h from RFHAL */

/* Information missing to compile srce/tatl14param.c */

#define C_TEST_RF_PARAM         0x0001	/* ---------------1 */

/* ----------------------------------------------------------------------- */
/* Sequence: GET_INFO_CTRL_DATA    */
/* ----------------------------------------------------------------------- */

typedef struct {
	uint16 requested_action;	/* requested action */
	uint16 fill_index;	/* If ACTION & C_TEST_RF_PARAM, index of requested parameter else filler */
} GET_INFO_CTRL_DATA_STR;

#define SIZE_GET_INFO_CTRL_DATA_STR   \
    sizeof(GET_INFO_CTRL_DATA_STR)

typedef struct {
	uint16 paramID;
	uint16 string[40];	/* char  array[68] */
} RETURN_INFO_PARAM_STR;

#define SIZE_RETURN_INFO_PARAM_STR\
    sizeof(RETURN_INFO_PARAM_STR)

/* ----------------------------------------------------------------------- */
/* Sequence: RETURN_INFO_PARAM_RESP   */
/* ----------------------------------------------------------------------- */

typedef struct {
	uint16 requested_action;	/* requested action */
	uint16 action_status;
	uint16 nb_of_param;
	uint16 index;
	RETURN_INFO_PARAM_STR return_info_param;
} RETURN_INFO_PARAM_RESP_STR;

#define SIZE_RETURN_INFO_PARAM_RESP_STR   sizeof(RETURN_INFO_PARAM_RESP_STR)

/* ----------------------------------------------------------------------- */
/* Sequence: RETURN_INFO_RESP   */
/* ----------------------------------------------------------------------- */

typedef struct {
    uint16  requested_action;   /* requested action */
    uint16  action_status;
} RETURN_INFO_RESP_STR;

/* Information missing to compile tx3goutpwrmeas.c */

/* ----------------------------------------------------------------------- */
/* Sequence: C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_SEQ */
/* ----------------------------------------------------------------------- */

typedef struct {
	uint32 band;
	uint16 channel;
	uint16 control;
	int16 power;

	uint16 filler01;
	uint16 filler02;
	uint16 filler03;
	uint16 filler04;
	uint16 filler05;
	uint16 filler06;
	uint16 filler07;
	uint16 filler08;
	uint16 filler09;
	uint16 filler10;
	uint16 filler11;
	uint16 filler12;
	uint16 filler13;
	uint16 filler14;
	uint16 filler15;
	uint16 filler16;
	uint16 filler17;
	uint16 filler18;
	uint16 filler19;
	uint16 filler20;
} C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_SEQ;

#define SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_SEQ \
	sizeof(C_TEST_SB_RESERVED_FOR_LICENSEE_REQ4_SEQ)

/* ----------------------------------------------------------------------- */
/* Sequence: C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ */
/* ----------------------------------------------------------------------- */

typedef struct {
	uint32 band;
	uint16 channel;
	uint16 status;
	int16 power_in_test;
	int16 power_last_request;
	int16 power_last_report;
	int16 power_delta;
	uint16 filler1;
	uint16 filler2;
} C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ;

#define SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ \
	sizeof(C_TEST_SB_RESERVED_FOR_LICENSEE_RESP4_SEQ)

#define SIZE_RETURN_INFO_RESP_STR   sizeof(RETURN_INFO_RESP_STR)


/* Information missing to compile tx2g5/3gintpwrmeas.c */

/* ----------------------------------------------------------------------- */
/* Sequence: C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_SEQ */
/* ----------------------------------------------------------------------- */

typedef struct
{
	uint16 system;
	uint16 band_LSB; /* using two u16 to insteadof one u32 */
	uint16 band_MSB; /* to avoid alignment issue           */
	int16  powerlevel;
	uint16 mode_filler;
	uint16 channel;
}C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_SEQ;

#define SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_SEQ \
	sizeof(C_TEST_SB_RESERVED_FOR_LICENSEE_REQ2_SEQ)

/* ----------------------------------------------------------------------- */
/* Sequence: C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ */
/* ----------------------------------------------------------------------- */

typedef struct
{
	uint16 filler_status;
	int16  transpower;
	uint16 transpower_inraw;
	int16  recvpower;
	uint16 recvpower_inraw;
	uint16 filler;
}C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ;

#define SIZE_C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ \
	sizeof(C_TEST_SB_RESERVED_FOR_LICENSEE_RESP2_SEQ)
