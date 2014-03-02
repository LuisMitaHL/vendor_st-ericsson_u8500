/* This header is created temporarly based on html licensee_wcdma_dsp_test_isi_m.html
Resource Id: PN_WCDMA_DSP_TEST.
until following informations are available in official header files from Modem or NWM*/

/* Message Id */
#define W_TEST_AGC_LNA_ALIGN_RESP                0x18
#define W_TEST_AGC_LNA_ALIGN_REQ                 0x03

/* Test handler ID */
#define PN_DSP_WCDMA_TEST                        0xEE

/* ----------------------------------------------------------------------- */
/* Message: W_TEST_AGC_LNA_ALIGN_REQ                                       */
/*-------------------------------------------------------------------------*/

/* temporary definition until the final one is published */
typedef uint16 W_TEST_AGC_DAC_SEQ;

typedef struct {
	/* xxxxxxxx--------  Transaction ID
	   --------xxxxxxxx  Message ID
	 */
	u16 id;
	/* xxxxxxxx--------  Filler
	   --------xxxxxxxx Length of one AGC step
	 */
	u16 filler1_agc_step_length;
	u16 mid_chan;
	u16 afc_dac_value;	/* not used */
	W_TEST_AGC_DAC_SEQ rx_chain_gain;	/* not used */
	W_TEST_AGC_DAC_SEQ mid_gain1;	/* not used */
	W_TEST_AGC_DAC_SEQ mid_gain2;	/* not used */
	W_TEST_AGC_DAC_SEQ low_gain1;	/* not used */
	W_TEST_AGC_DAC_SEQ low_gain2;	/* not used */
	W_TEST_AGC_DAC_SEQ agc_slope_align1;	/* not used */
	W_TEST_AGC_DAC_SEQ agc_slope_align2;	/* not used */
	/* xxxxxxxx--------  Filler
	   --------xxxxxxxx Options for RX alignment control
	 */
	u16 filler2_rx_align_ctrl;
	u16 low_chan;
	u16 high_chan;
} W_TEST_AGC_LNA_ALIGN_REQ_STR;

#define SIZE_W_TEST_AGC_LNA_ALIGN_REQ_STR  \
        sizeof(W_TEST_AGC_LNA_ALIGN_REQ_STR)

/* ----------------------------------------------------------------------- */
/* Message: W_TEST_AGC_LNA_ALIGN_RESP                                      */
/*-------------------------------------------------------------------------*/
typedef struct {
	/* xxxxxxxx--------  Transaction ID
	   --------xxxxxxxx  Message ID
	 */
	u16 id;
	/* xxxxxxxx--------  Filler
	   --------xxxxxxxx Alignment status (W_TEST_STATUS)
	 */
	u16 filler1_align_status;
	s16 rx_chain_gain;	/* Q6 */
	s16 mid_gain_error;	/* Q6 */
	s16 low_gain_error;	/* Q6 */
	u16 agc_gain_error_slope;	/* Q13 */
	u16 bias_error;		/* Q6 */
	u16 low_freq_comp;	/* Q6 */
	u16 high_freq_comp;	/* Q6 */
} W_TEST_AGC_LNA_ALIGN_RESP_STR;

#define SIZE_W_TEST_AGC_LNA_ALIGN_RESP_STR  \
        sizeof(W_TEST_AGC_LNA_ALIGN_RESP_STR)
