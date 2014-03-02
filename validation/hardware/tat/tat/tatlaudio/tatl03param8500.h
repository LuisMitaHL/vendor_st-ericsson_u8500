/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage ab dth functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#if !defined(TATL03PARAM_H)
#define TATL03PARAM_H

/* comments are not to be placed on the same line than the enum definitions
 * else the perl script will not parse correctly the file */
typedef enum {
    /* TAG for first element - DO NOT REMOVE OR MODIFY!!! */
    AUDIO_PARAM_BEGIN_LIST,

    /* digital/analog input selection is not really an ab8500 input so it have
     * to be out of the main section but still declared in this file. */
    AUDIO_BLOCK_IN_CFG,
    AUDIO_PARAM_ANALOG_DIGITAL_SEL,

    /* Analog inputs */
    AUDIO_BLOCK_ANALOG_IN,
    AUDIO_PARAM_MIC1,
    AUDIO_PARAM_MIC2,
    AUDIO_PARAM_LINL,
    AUDIO_PARAM_LINR,
    AUDIO_PARAM_MICSEL,
    AUDIO_PARAM_LINR_MIC2_SEL,

    /* Analog gains */
    AUDIO_BLOCK_ANALOG_IN_GAINS,
    AUDIO_PARAM_MIC1_CONFIG,
    AUDIO_PARAM_AMIC1_GAIN,
    AUDIO_PARAM_MIC2_CONFIG,
    AUDIO_PARAM_AMIC2_GAIN,
    AUDIO_PARAM_LINL_GAIN,
    AUDIO_PARAM_LINR_GAIN,

    /* Digital inputs */
    AUDIO_BLOCK_DIGITAL_IN,
    AUDIO_PARAM_DMIC1,
    AUDIO_PARAM_DMIC2,
    AUDIO_PARAM_DMIC3,
    AUDIO_PARAM_DMIC4,
    AUDIO_PARAM_DMIC5,
    AUDIO_PARAM_DMIC6,

    /* AD gains */
    AUDIO_BLOCK_AD_GAINS,
    AUDIO_PARAM_AD1_GAIN,
    AUDIO_PARAM_AD2_GAIN,
    AUDIO_PARAM_AD3_GAIN,
    AUDIO_PARAM_AD4_GAIN,
    AUDIO_PARAM_AD5_GAIN,
    AUDIO_PARAM_AD6_GAIN,

    /* AD mux */
    AUDIO_BLOCK_AD_MUX,
    AUDIO_PARAM_SLOT_AD0,
    AUDIO_PARAM_SLOT_AD1,
    AUDIO_PARAM_SLOT_AD2,
    AUDIO_PARAM_SLOT_AD3,
    AUDIO_PARAM_SLOT_AD4,
    AUDIO_PARAM_SLOT_AD5,
    AUDIO_PARAM_SLOT_AD6,
    AUDIO_PARAM_SLOT_AD7,

    /* Analog loop gain */
    AUDIO_BLOCK_ANALOG_LOOP_GAINS,
    AUDIO_PARAM_ANALOG_LOOP_GAIN_L,
    AUDIO_PARAM_ANALOG_LOOP_GAIN_R,

     /* DA mux */
    AUDIO_BLOCK_DA_MUX,
    AUDIO_PARAM_SLOT_DA1,
    AUDIO_PARAM_SLOT_DA2,
    AUDIO_PARAM_SLOT_DA3,
    AUDIO_PARAM_SLOT_DA4,
    AUDIO_PARAM_SLOT_DA5,
    AUDIO_PARAM_SLOT_DA6,

   /* DA gains */
    AUDIO_BLOCK_DA_GAINS,
    AUDIO_PARAM_DA1_GAIN,
    AUDIO_PARAM_DA2_GAIN,
    AUDIO_PARAM_DA3_GAIN,
    AUDIO_PARAM_DA4_GAIN,
    AUDIO_PARAM_DA5_GAIN,
    AUDIO_PARAM_DA6_GAIN,

    /* Outputs */
    AUDIO_BLOCK_OUT,
    AUDIO_PARAM_EAR,
    AUDIO_PARAM_HSL,
    AUDIO_PARAM_HSR,
    AUDIO_PARAM_HFL,
    AUDIO_PARAM_HFR,
    AUDIO_PARAM_VIB1,
    AUDIO_PARAM_VIB2,

    /* Output configuration */
    AUDIO_BLOCK_OUT_CFG,
    AUDIO_PARAM_CLASSD1_CONF,
    AUDIO_PARAM_VIB1N_PWM,
    AUDIO_PARAM_VIB1P_PWM,
    AUDIO_PARAM_CLASSD2_CONF,
    AUDIO_PARAM_VIB2N_PWM,
    AUDIO_PARAM_VIB2P_PWM,

    AUDIO_BLOCK_OUT_GAINS,
    /* Analog and digital gains (analog) */
    AUDIO_PARAM_HSL_ANALOG_GAIN,
    AUDIO_PARAM_HSR_ANALOG_GAIN,

    /* Analog and digital gains (digital) */
    AUDIO_PARAM_HSL_EAR_GAIN,
    AUDIO_PARAM_HSR_GAIN,

    /* Side tone */
    AUDIO_BLOCK_SIDETONE,
    AUDIO_PARAM_SIDETONE_GAIN_1,
    AUDIO_PARAM_SIDETONE_GAIN_2,
    AUDIO_PARAM_SIDETONE_SRC_1,
    AUDIO_PARAM_SIDETONE_SRC_2,

    /* TAG for last element - DO NOT REMOVE OR MODIFY!!! */
    AUDIO_PARAM_END_LIST,

    /* Here starts the internals parameters - DO NOT REMOVE OR MODIFY!!! */
    AUDIO_PARAM_INTERNALS,

} e_tatlaudio_param;

typedef enum {
	TATAUDIO_DMIC_12,
	TATAUDIO_DMIC_34,
	TATAUDIO_DMIC_56,
	TATAUDIO_DMIC_NONE
} e_tatdaudio_digital_micro;

#define LG_MAX 256
#define NB_MAX 200

typedef char Dthaudio_path[LG_MAX];

#if defined(TATL03PARAM_C)

/* Set of macros to reduce code duplicate and to enhance audio parameter access
 * readibility.
 */

#define BEGIN_AUDIO_PARAM_MAP(param)                                           \
    switch ((param)) {

#define END_AUDIO_PARAM_MAP(param)                                             \
    default:                                                                   \
        SYSLOG(LOG_ERR, "unregistered audio parameter: %d", param);              \
        TAT_SET_LAST_ERR(TAT_BAD_REQ);                                         \
    }

#define AUDIO_PARAM(id, addr, mask, type, value, set)                          \
    case id:                                                                   \
    {                                                                          \
        if ( set ) {                                                           \
            DthAudio_WriteInReg((uint16_t)addr, (uint8_t)mask,(uint8_t *)(value));                 \
            SYSLOG(LOG_INFO, "set audio parameter "#id " (@%4X, Msk%2X)to %u", addr, mask, *(value));    \
        } else {                                                               \
            DthAudio_ReadInReg((uint16_t)addr, (uint8_t)mask,(uint8_t *)(value));                 \
            SYSLOG(LOG_DEBUG, "get audio parameter "#id " (@%4X, Msk%2X): %u", addr, mask,  *(value));     \
        }                                                                      \
    }                                                                          \
    break;

#define AUDIO_PARAM_NULL(id)                                                   \
    case id: break;

#endif

typedef enum dth_slot_output {
    DTH_SLOT_OUT_FROM_AD_OUT1,
    DTH_SLOT_OUT_FROM_AD_OUT2,
    DTH_SLOT_OUT_FROM_AD_OUT3,
    DTH_SLOT_OUT_FROM_AD_OUT4,
    DTH_SLOT_OUT_FROM_AD_OUT5,
    DTH_SLOT_OUT_FROM_AD_OUT6,
    DTH_SLOT_OUT_FROM_AD_OUT7,
    DTH_SLOT_OUT_FROM_AD_OUT8,
    DTH_SLOT_OUT_TRISTATE,
    DTH_SLOT_OUT_RESERVED1,
    DTH_SLOT_OUT_RESERVED2,
    DTH_SLOT_OUT_RESERVED3,
    DTH_SLOT_OUT_ZEROS
} DTH_SLOT_OUTPUT_T ;



Dthaudio_path  a_dthaudio_fulllistpath[NB_MAX]
#if defined TATL03PARAM_C
= {
    "/DTH/AUDIO/Param/out_gains"
    , "/DTH/AUDIO/Param/out_cfg"
    , "/DTH/AUDIO/Param/out"
    , "/DTH/AUDIO/Param/analog_in"
    , "/DTH/AUDIO/Param/digital_in"
    , "/DTH/AUDIO/Param/analog_in_gains"
    , "/DTH/AUDIO/Param/ad_gains"
    , "/DTH/AUDIO/Param/da_gains"
    , "/DTH/AUDIO/Param/ad_mux"
    , "/DTH/AUDIO/Param/sidetone"
    , "/DTH/AUDIO/Param/da_mux"
    , "/DTH/AUDIO/Param/in_cfg"
    , "/DTH/AUDIO/Param/analog_loop_gains"
    , "/DTH/AUDIO/Param/usb"
    , ""
}
#endif
;

Dthaudio_path  a_dthaudio_ADDAlistpath[NB_MAX]
#if defined TATL03PARAM_C
= {
    "/DTH/AUDIO/Param/out_gains"
    , "/DTH/AUDIO/Param/out_cfg"
    , "/DTH/AUDIO/Param/out"
    , "/DTH/AUDIO/Param/analog_in"
    , "/DTH/AUDIO/Param/digital_in"
    , "/DTH/AUDIO/Param/analog_in_gains"
    , "/DTH/AUDIO/Param/ad_gains"
    , "/DTH/AUDIO/Param/da_gains"
    , "/DTH/AUDIO/Param/ad_mux"
    , "/DTH/AUDIO/Param/sidetone"
    , "/DTH/AUDIO/Param/da_mux"
    , "/DTH/AUDIO/Param/in_cfg"
    , "/DTH/AUDIO/Param/usb"
    , ""
}
#endif
;


Dthaudio_path  a_dthaudio_AnalogLooplistpath[NB_MAX]
#if defined TATL03PARAM_C
= {
    "/DTH/AUDIO/Param/out"
    , "/DTH/AUDIO/Param/analog_in"
    , "/DTH/AUDIO/Param/analog_loop_gains"
    , ""
}
#endif
;

Dthaudio_path  a_dthaudio_DAlistpath[NB_MAX]
#if defined TATL03PARAM_C
= {
    "/DTH/AUDIO/Param/out_gains"
    , "/DTH/AUDIO/Param/out_cfg"
    , "/DTH/AUDIO/Param/out"
    , "/DTH/AUDIO/Param/da_gains"
    , "/DTH/AUDIO/Param/da_mux"
    , "/DTH/AUDIO/Param/usb"
    , ""
}
#endif
;

int v_tataudio_digital_sel;

/* Get the current value of an audio parameter.
 * @param elem[in] address of the DTH element identifying the audio parameter.
 * The member struct user_data should be set to a e_tatlaudio_param value.
 * @param value[out] address of a uint32_t to receive the 0-based index
 * corresponding to the current value of the audio parameter.
 * @return TAT_ERROR_OFF if success else a e_taterror error code
 */
int DthAudio_01ParamGet(struct dth_element *elem, void *value);

/* Set the value of an audio parameter.
 * The function does not verify the validity of *value for the audio parameter.
 * @param elem[in] address of the DTH element identifying the audio parameter.
 * The member struct user_data should be set to a e_tatlaudio_param value.
 * @param value[in] address of a uint32_t set to a 0-based index
 * corresponding to the current value of the audio parameter.
 * @return TAT_ERROR_OFF if success else a e_taterror error code
 */
int DthAudio_02ParamSet(struct dth_element *elem, void *value);

/* Write a register field (corresponding to a parameter) into an AB register.
 * @param addr  register address
 * @param mask  bitfield mask
 * @param pp_value pointer on the field value to write
 * @return TAT_ERROR_OFF if success else a e_taterror error code
 */
int DthAudio_WriteInReg(uint16_t addr, uint8_t mask, uint8_t* pp_value);

/* Read a register field (corresponding to a parameter) from an AB register.
 * @param addr  register address
 * @param mask  bitfield mask
 * @param pp_value pointer on the read field value
 * @return TAT_ERROR_OFF if success else a e_taterror error code
 */
int DthAudio_ReadInReg(uint16_t addr, uint8_t mask, uint8_t* pp_value);

/* Retrieve the start bit of a register field from the mask value.
 * @param mask  bitfield mask
 * @param pp_value pointer on the value of the start bit
 * @return TAT_ERROR_OFF if success else a e_taterror error code
 */
int DthAudio_GetOffsetFromMask(uint8_t mask, uint8_t* pp_value);

/* Initiate asynchronous events for all parameters blocks .
 * @param type  type of the async event
 * @return TAT_ERROR_OFF if success else a e_taterror error code
 */
int DthAudio_SendAsyncEventParameters(Dthaudio_path* listpath, unsigned char type, mode_t mode);

/* Configure a GPIO  .
 * @param AddressDir    Direction Register Address
 * @param AddressVal    Value Register Address
 * @param AddressPud    Pull Up/down Register Address
 * @param Mask          Register Bit Position
 * @param Direction     Direction Value to set
 * @param Value         Value to set
 * @param Pud           Pull up/down value to set
 * @return TAT_ERROR_OFF if success else a e_taterror error code
 */
int DthAudio_GpioConfig(int AddressDir, int AddressVal, int AddressPud, int Mask, uint8_t Direction, uint8_t Value, uint8_t Pud);

/* a verifier */
/* Register GPIO , VIDEOCTRL GPIO is GPIO 13 */
#define TATAUDIO_GPIO_VIDEOCTRL_DIR 0x1011
#define TATAUDIO_GPIO_VIDEOCTRL_OUT 0x1021
#define TATAUDIO_GPIO_VIDEOCTRL_PUD 0x1031

/* Register GPIO , MICCTRL GPIO is GPIO 15 */
#define TATAUDIO_GPIO_MICCTRL_DIR 0x1011
#define TATAUDIO_GPIO_MICCTRL_OUT 0x1021
#define TATAUDIO_GPIO_MICCTRL_PUD 0x1031

/* Masks and offsets of inside register */
#define TATAUDIO_GPIO_VIDEOCTRL_OFFSET_IN_REGISTER 0x10 /* bit 4*/
#define TATAUDIO_GPIO_MICCTRL_OFFSET_IN_REGISTER 0x40  /* bit 6*/

#endif /* !defined(TATL03PARAM_H) */
