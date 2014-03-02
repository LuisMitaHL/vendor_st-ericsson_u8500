#ifndef INCLUSION_GUARD_T_MAI_H
#define INCLUSION_GUARD_T_MAI_H

/*************************************************************************
* $Copyright ST-Ericsson 2010 $
**************************************************************************
*
* DESCRIPTION:  Defines the Modem Audio Interface (MAI) types and messages.
*               It is based on the document 26/0363-1/FCP 121 0014. Rel B
*
**************************************************************************
**************************************************************************
*/

// Definitions of portable types (uint8, ENUM8...)
// #include "t_basicdefinitions.h"

typedef unsigned char uint8;

#define TYPEDEF_ENUM enum
#define ENUM8(t) ;typedef uint8 t

/** This is used to uniquely identify the MAI SWBP category */
#define CAT_ID_MAI 520

/**************************************/
/* Defines                            */
/**************************************/

/**
 * Defines the frame sizes for the different speech codecs.
 */

/** The size in number of bytes (octets) for a Full Rate speech frame.*/
#define MAI_FR_FRAME_SIZE    33

/** The size in number of bytes (octets) for a Half Rate speech frame.*/
#define MAI_HR_FRAME_SIZE    14

/** The size in number of bytes (octets) for an Enhanced Full Rate speech frame.*/
#define MAI_EFR_FRAME_SIZE   33

/** The (maximum) size in number of bytes (octets) for an AMR-NB speech frame.*/
#define MAI_AMR_NB_FRAME_SIZE 31

/** The (maximum) size in number of bytes (octets) for an AMR-WB speech frame.*/
#define MAI_AMR_WB_FRAME_SIZE 60

/** The size in number of bytes (octets) for the PCM 8 KHz frame.*/
#define MAI_PCM_FRAME_SIZE_8KHZ  320

/** The size in number of bytes (octets) for the PCM 16 KHz frame.*/
#define MAI_PCM_FRAME_SIZE_16KHZ 640

/** The length of the Modem Identity in number of bytes (octets).*/
#define MAI_MODEM_ID_STRING_LENGTH 32 /* Note it must be an even number of 32 bit bit words, i.e. dividable by 4. */

/** The length of the Modem Version in number of bytes (octets). */
#define MAI_MODEM_VERSION_STRING_LENGTH 32 /* Note it must be an even number of 32 bit bit words, i.e. dividable by 4. */

/**************************************/
/* Enums                              */
/**************************************/

/**
 * This data type defines the message type identifiers for all messages.
 *
 * @param MAI_MESSAGE_INFORMATION_REQUEST          Message type identifier for "Information Request". \n
 *                                                 Direction: AudioSub -> ModemSub
 * @param MAI_MESSAGE_INFORMATION_RESPONSE         Message type identifier for "Information Response".\n
 *                                                 Direction: ModemSub -> AudioSub
 * @param MAI_MESSAGE_CODING_FORMAT                Message type identifier for "Coding Format".\n
 *                                                 Direction: ModemSub -> AudioSub
 * @param MAI_MESSAGE_CODING_FORMAT_RESPONSE       Message type identifier for "Coding Format Response".\n
 *                                                 Direction: AudioSub -> ModemSub
 * @param MAI_MESSAGE_UPLINK_TIMING_CONFIG         Message type identifier for "Uplink Timing Configuration".\n
 *                                                 Direction: ModemSub -> AudioSub
 * @param MAI_MESSAGE_DOWNLINK_TIMING_CONFIG       Message type identifier for "Downlink Timing Configuration".\n
 *                                                 Direction: ModemSub -> AudioSub
 * @param MAI_MESSAGE_TIMING_REPORT                Message type identifier for "Timing Report".\n
 *                                                 Direction: ModemSub -> AudioSub
 * @param MAI_MESSAGE_DOWNLINK_SPEECH_DATA         Message type identifier for "Downlink Speech Data".\n
 *                                                 Direction: ModemSub -> AudioSub
 * @param MAI_MESSAGE_UPLINK_SPEECH_DATA           Message type identifier for "Uplink Speech Data".\n
 *                                                 Direction: AudioSub -> ModemSub
 * @param MAI_MESSAGE_VOICE_CALL_STATUS            Message type identifier for "Voice Call Status".\n
 *                                                 Direction: AudioSub -> ModemSub
 * @param MAI_MESSAGE_TEST_MODEM_LOOPBACK          Message type identifier for "Test Modem Loopback".\n
 *                                                 Direction: AudioSub -> ModemSub
 * @param MAI_MESSAGE_TEST_MODEM_LOOPBACK_RESPONSE Message type identifier for "Test Modem Loopback Response".\n
 *                                                 Direction: ModemSub -> AudioSub
 * @param MAI_MESSAGE_TEST_AUDIO_LOOPBACK          Message type identifier for "Test Audio Loopback".\n
 *                                                 Direction: ModemSub -> AudioSub
 * @param MAI_MESSAGE_TEST_AUDIO_LOOPBACK_RESPONSE Message type identifier for "Test Audio Loopback Response".\n
 *                                                 Direction: AudioSub -> ModemSub
 * @param MAI_MESSAGE_NUMBER_OF_MESSAGES           Parameter to count the number of messages
 *
 */
TYPEDEF_ENUM
{
  MAI_MESSAGE_INFORMATION_REQUEST          = 0, /* AudioSub -> ModemSub */
  MAI_MESSAGE_INFORMATION_RESPONSE         = 1, /* ModemSub -> AudioSub */
  MAI_MESSAGE_CODING_FORMAT                = 2, /* ModemSub -> AudioSub */
  MAI_MESSAGE_CODING_FORMAT_RESPONSE       = 3, /* AudioSub -> ModemSub */
  MAI_MESSAGE_UPLINK_TIMING_CONFIG         = 4, /* AudioSub -> ModemSub */
  MAI_MESSAGE_DOWNLINK_TIMING_CONFIG       = 5, /* ModemSub -> AudioSub */
  MAI_MESSAGE_TIMING_REPORT                = 6, /* ModemSub -> AudioSub */
  MAI_MESSAGE_DOWNLINK_SPEECH_DATA         = 7, /* ModemSub -> AudioSub */
  MAI_MESSAGE_UPLINK_SPEECH_DATA           = 8, /* AudioSub -> ModemSub */
  MAI_MESSAGE_VOICE_CALL_STATUS            = 9, /* ModemSub -> AudioSub */
  MAI_MESSAGE_TEST_MODEM_LOOPBACK          = 10,/* AudioSub -> ModemSub */
  MAI_MESSAGE_TEST_MODEM_LOOPBACK_RESPONSE = 11,/* ModemSub -> AudioSub */
  MAI_MESSAGE_TEST_AUDIO_LOOPBACK          = 12,/* ModemSub -> AudioSub */
  MAI_MESSAGE_TEST_AUDIO_LOOPBACK_RESPONSE = 13,/* AudioSub -> ModemSub */
  MAI_MESSAGE_NUMBER_OF_MESSAGES
} ENUM8(MAI_Message_t);

/**
 * This data type defines if the coding format was acceptable.
 *
 * @param MAI_CODING_CONFIG_OK        The coding format configuration is ok.
 * @param MAI_CODING_CONFIG_FAILURE   The coding format configuration is not ok.
 */
TYPEDEF_ENUM
{
  MAI_CODING_CONFIG_OK      = 0,
  MAI_CODING_CONFIG_FAILURE = 1
} ENUM8(MAI_CodingConfig_t);

/**
 * This data type defines if a speech data message shall be transmitted when nothing is transmitted/received over the air interface.
 *
 * @param MAI_NO_DATA_USAGE_FALSE      It is allowed to skip sending any UL/DL speech data if speech contains no data.
 * @param MAI_NO_DATA_USAGE_TRUE       It is not allowed to skip sending any UL/DL speech data if speech contains no data.
 */
TYPEDEF_ENUM
{
  MAI_NO_DATA_USAGE_FALSE = 0,
  MAI_NO_DATA_USAGE_TRUE  = 1
} ENUM8(MAI_NoDataUsage_t);

/**
 * This data type defines the cause of the generated timing message.
 *
 * @param MAI_TIMING_CAUSE_TOO_LATE         The arrival of the UL speech frame was too late.
 * @param MAI_TIMING_CAUSE_TOO_EARLY        The arrival of the UL speech frame was too early.
 * @param MAI_TIMING_CAUSE_PACKAGE_DISCARD  The arrival of the UL speech frame resulted in a discard of a speech frame not yet sent out on the air interface.
 */
TYPEDEF_ENUM
{
  MAI_TIMING_CAUSE_TOO_LATE         = 0,
  MAI_TIMING_CAUSE_TOO_EARLY        = 1,
  MAI_TIMING_CAUSE_PACKAGE_DISCARD  = 2
} ENUM8(MAI_TimingCause_t);

/**
 * This data type defines the voice call the status.
 *
 * @param MAI_CALL_STATUS_NOT_CONNECTED    The call is not yet connected.
 * @param MAI_CALL_STATUS_CONNECTED        The call is connected.
 */
TYPEDEF_ENUM
{
  MAI_CALL_STATUS_NOT_CONNECTED = 0,
  MAI_CALL_STATUS_CONNECTED     = 1
} ENUM8(MAI_CallStatus_t);

/**
 * This data type defines on what access technology (or system) the speech call is running.
 *
 * @param MAI_SYSTEM_NONE      No system is currently running. This is set when the call disconnects.
 * @param MAI_SYSTEM_2G        The system is 2G (GSM).
 * @param MAI_SYSTEM_3G        The system is 3G (WCDMA running speech call over dedicated channels).
 * @param MAI_SYSTEM_CS_O_HS   The system is run over 3G HSPA channels (CS over HS).
 */
TYPEDEF_ENUM
{
  MAI_SYSTEM_NONE    = 0,
  MAI_SYSTEM_2G      = 1,
  MAI_SYSTEM_3G      = 2,
  MAI_SYSTEM_CS_O_HS = 3
} ENUM8(MAI_System_t);

/**
 * This data type defines a speech codec format.
 *
 * @param MAI_SPEECH_CODEC_NONE      No Speech codec.
 * @param MAI_SPEECH_CODEC_HR        Half Rate (HR) Speech codec.
 * @param MAI_SPEECH_CODEC_FR        Full Rate (FR) Speech codec.
 * @param MAI_SPEECH_CODEC_EFR       Enhanced Full Rate (EFR) Speech codec.
 * @param MAI_SPEECH_CODEC_AMR_NB    Adaptive Multi-Rate Narrow Band(AMR-NB) Speech codec.
 * @param MAI_SPEECH_CODEC_AMR_WB    Adaptive Multi-Rate Wide Band (AMR-WB) Speech codec.
 * @param MAI_SPEECH_CODEC_PCM8      Pulse Coded Modulation (PCM) 8 kHz.
 * @param MAI_SPEECH_CODEC_PCM16     Pulse Coded Modulation (PCM) 16 kHz.
 */
TYPEDEF_ENUM
{
  MAI_SPEECH_CODEC_NONE   = 0,
  MAI_SPEECH_CODEC_HR     = 1,
  MAI_SPEECH_CODEC_FR     = 2,
  MAI_SPEECH_CODEC_EFR    = 3,
  MAI_SPEECH_CODEC_AMR_NB = 4,
  MAI_SPEECH_CODEC_AMR_WB = 5,
  MAI_SPEECH_CODEC_PCM8   = 6,
  MAI_SPEECH_CODEC_PCM16  = 7
} ENUM8(MAI_SpeechCodec_t);

/**
 * This data type defines if DTX (Discontinuous transmission) is on/off.
 *
 * @param MAI_DTX_OFF       DTX is off.
 * @param MAI_DTX_ON        DTX is on.
 */
TYPEDEF_ENUM
{
  MAI_DTX_OFF = 0,
  MAI_DTX_ON  = 1
} ENUM8(MAI_DTX_t);

/**
 * This data type defines if the Noise Suppression Control Bit (NSCB) is on/off. This is only valid for (2G) GSM AMR-NB/AMR-WB.
 *
 * @param MAI_NSCB_OFF     NSCB is off.
 * @param MAI_NSCB_ON      NSCB is on.
 */
TYPEDEF_ENUM
{
  MAI_NSCB_OFF = 0,
  MAI_NSCB_ON  = 1
} ENUM8(MAI_NSCB_t);


/**
 * This data type defines the frame quality.
 *
 * @param MAI_FQI_FRAME_BAD    The frame is corrupted.
 * @param MAI_FQI_FRAME_GOOD   The frame is ok
 */
TYPEDEF_ENUM
{
  MAI_FQI_FRAME_BAD  = 0,
  MAI_FQI_FRAME_GOOD = 1
} ENUM8(MAI_FQI_t);

/**
 * This data type defines if the UL frame for HR/FR/EFR is a silence (SID) or a speech frame.
 *
 * @param MAI_SP_SID_FRAME       Current UL frame is a SID frame.
 * @param MAI_SP_SPEECH_FRAME    Current UL frame is a Speech frame.
 */
TYPEDEF_ENUM
{
  MAI_SP_SID_FRAME    = 0,
  MAI_SP_SPEECH_FRAME = 1
} ENUM8(MAI_SP_t);

/**
 * This data type defines the values of the Bad Frame Indicator (BFI) for a DL HR/FR/EFR frame.
 *
 * @param MAI_BFI_FALSE       The BFI is false.
 * @param MAI_BFI_TRUE        The BFI is true.
 */
TYPEDEF_ENUM
{
  MAI_BFI_FALSE = 0,
  MAI_BFI_TRUE  = 1
} ENUM8(MAI_BFI_t);

/**
 * This data type defines the values of the Dangerous Frame Indication (DFI) used to indicate for the decoder that the DL FR frame might be bad.
 *
 * @param MAI_FR_DFI_FALSE    The DFI is false.
 * @param MAI_FR_DFI_TRUE     The DFI is true.
 * @param MAI_FR_DFI_NOT_USED The BFI is not used.
 */
TYPEDEF_ENUM
{
  MAI_FR_DFI_FALSE    = 0,
  MAI_FR_DFI_TRUE     = 1,
  MAI_FR_DFI_NOT_USED = 2
} ENUM8(MAI_FR_DFI_t);

/**
 * This data type defines the values of the Unreliable Frame Indicator (UFI) used to indicate for the decoder that the DL HR frame might be bad.
 *
 * @param MAI_UFI_FALSE       The UFI is false.
 * @param MAI_UFI_TRUE        The UFI is true.
 */
TYPEDEF_ENUM
{
  MAI_UFI_FALSE = 0,
  MAI_UFI_TRUE  = 1
} ENUM8(MAI_UFI_t);

/**
 * This data type defines the type of DL speech frame for HR/FR/EFR.
 *
 * @param MAI_FRAME_TYPE_SPEECH        Speech.
 * @param MAI_FRAME_TYPE_INVALID_SID   Invalid SID.
 * @param MAI_FRAME_TYPE_VALID_SID     Valid SID.
 */
TYPEDEF_ENUM
{
  MAI_FRAME_TYPE_SPEECH      = 0,
  MAI_FRAME_TYPE_INVALID_SID = 1,
  MAI_FRAME_TYPE_VALID_SID   = 2
} ENUM8(MAI_FrameType_t);

/**
 * This data type defines the Timing Alignment Flag (TAF) used to indicate for the decoder that the comfort noise parameters should be updated (FR/HR/EFR).
 *
 * @param MAI_TAF_NO_UPDATE_COMFORT_NOISE     No update of comfort noise.
 * @param MAI_TAF_UPDATE_COMFORT_NOISE        Update comfort noise.
 */
TYPEDEF_ENUM
{
  MAI_TAF_NO_UPDATE_COMFORT_NOISE = 0,
  MAI_TAF_UPDATE_COMFORT_NOISE    = 1
} ENUM8(MAI_TAF_t);

/**
 * This data type defines the behavior of TX/RX DTX during handover synchronization in 2G (GSM). See 3GPP TS 26.093 Annex A5.
 *
 * @param MAI_AMR_NSYNC_OFF   NSYNC is off.
 * @param MAI_AMR_NSYNC_ON    NSYNC is on.
 */
TYPEDEF_ENUM
{
  MAI_AMR_NSYNC_OFF = 0,
  MAI_AMR_NSYNC_ON  = 1
} ENUM8(MAI_AMR_NSYNC_t);

/**
 * This data type defines the AMR (both NB and WB) speech frame type for TX (UL). Note that this is only used in 2G.
 *
 * @param MAI_AMR_TX_TYPE_SPEECH       Speech frame type is speech.
 * @param MAI_AMR_TX_TYPE_SID_FIRST    Speech frame type is SID First.
 * @param MAI_AMR_TX_TYPE_SID_UPDATE   Speech frame type is SID Update.
 * @param MAI_AMR_TX_TYPE_NO_DATA      Speech frame type is No Data.
 */
TYPEDEF_ENUM
{
  MAI_AMR_TX_TYPE_SPEECH      = 0,
  MAI_AMR_TX_TYPE_SID_FIRST   = 1,
  MAI_AMR_TX_TYPE_SID_UPDATE  = 2,
  MAI_AMR_TX_TYPE_NO_DATA     = 3
} ENUM8(MAI_AMR_TX_Type_t);

/**
 * This data type defines the AMR-NB speech frame type for RX (DL) according to 3GPP TS 26.093. Note that this is only applicable for 2G.
 *
 * @param MAI_AMR_NB_RX_TYPE_SPEECH          The contents of the frame is SPEECH.
 * @param MAI_AMR_NB_RX_TYPE_SPEECH_DEGRADED The contents of the frame is SPEECH_DEGRADED.
 * @param MAI_AMR_NB_RX_TYPE_ONSET           The contents of the frame is ONSET.
 * @param MAI_AMR_NB_RX_TYPE_SPEECH_BAD      The contents of the frame is SPEECH_BAD.
 * @param MAI_AMR_NB_RX_TYPE_SID_FIRST       The contents of the frame is SID_FIRST.
 * @param MAI_AMR_NB_RX_TYPE_SID_UPDATE      The contents of the frame is SID_UPDATE.
 * @param MAI_AMR_NB_RX_TYPE_SID_BAD         The contents of the frame is SID_BAD.
 * @param MAI_AMR_NB_RX_TYPE_NO_DATA         The contents of the frame is NO_DATA.
 */
TYPEDEF_ENUM
{
  MAI_AMR_NB_RX_TYPE_SPEECH          = 0,
  MAI_AMR_NB_RX_TYPE_SPEECH_DEGRADED = 1,
  MAI_AMR_NB_RX_TYPE_ONSET           = 2,
  MAI_AMR_NB_RX_TYPE_SPEECH_BAD      = 3,
  MAI_AMR_NB_RX_TYPE_SID_FIRST       = 4,
  MAI_AMR_NB_RX_TYPE_SID_UPDATE      = 5,
  MAI_AMR_NB_RX_TYPE_SID_BAD         = 6,
  MAI_AMR_NB_RX_TYPE_NO_DATA         = 7
} ENUM8(MAI_AMR_NB_RX_Type_t);

/**
 * This data type defines the AMR-WB speech frame type for RX (DL) in 2G according to 3GPP TS 26.193. Note that this is only applicable for 2G.
 *
 * @param MAI_AMR_WB_RX_TYPE_SPEECH                    The contents of the frame is SPEECH.
 * @param MAI_AMR_WB_RX_TYPE_SPEECH_DEGRADED           The contents of the frame is SPEECH_DEGRADED.
 * @param MAI_AMR_WB_RX_TYPE_SPEECH_LOST               The contents of the frame is SPEECH_LOST.
 * @param MAI_AMR_WB_RX_TYPE_SPEECH_BAD                The contents of the frame is SPEECH_BAD.
 * @param MAI_AMR_WB_RX_TYPE_SID_FIRST                 The contents of the frame is SID_FIRST.
 * @param MAI_AMR_WB_RX_TYPE_SID_UPDATE                The contents of the frame is SID_UPDATE.
 * @param MAI_AMR_WB_RX_TYPE_SID_BAD                   The contents of the frame is SID_BAD.
 * @param MAI_AMR_WB_RX_TYPE_NO_DATA                   The contents of the frame is NO_DATA.
 */
TYPEDEF_ENUM
{
  MAI_AMR_WB_RX_TYPE_SPEECH                   = 0,
  MAI_AMR_WB_RX_TYPE_SPEECH_DEGRADED          = 1,
  MAI_AMR_WB_RX_TYPE_SPEECH_LOST              = 2,
  MAI_AMR_WB_RX_TYPE_SPEECH_BAD               = 3,
  MAI_AMR_WB_RX_TYPE_SID_FIRST                = 4,
  MAI_AMR_WB_RX_TYPE_SID_UPDATE               = 5,
  MAI_AMR_WB_RX_TYPE_SID_BAD                  = 6,
  MAI_AMR_WB_RX_TYPE_NO_DATA                  = 7
} ENUM8(MAI_AMR_WB_RX_Type_t);

/**
 * This data type defines the AMR-NB rates.
 *
 * @param MAI_AMR_NB_RATE_4_75        4.75 kbps.
 * @param MAI_AMR_NB_RATE_5_15        5.15 kbps.
 * @param MAI_AMR_NB_RATE_5_90        5.95 kbps.
 * @param MAI_AMR_NB_RATE_6_70        6.70 kbps.
 * @param MAI_AMR_NB_RATE_7_40        7.40 kbps.
 * @param MAI_AMR_NB_RATE_7_95        7.95 kbps.
 * @param MAI_AMR_NB_RATE_10_2        10.2 kbps.
 * @param MAI_AMR_NB_RATE_12_2        12.2 kbps.
 * @param MAI_AMR_NB_RATE_SID         SID frame.
 * @param MAI_AMR_NB_RATE_NO_DATA     No Data frame.
 */
TYPEDEF_ENUM
{
  MAI_AMR_NB_RATE_4_75    = 0,
  MAI_AMR_NB_RATE_5_15    = 1,
  MAI_AMR_NB_RATE_5_90    = 2,
  MAI_AMR_NB_RATE_6_70    = 3,
  MAI_AMR_NB_RATE_7_40    = 4,
  MAI_AMR_NB_RATE_7_95    = 5,
  MAI_AMR_NB_RATE_10_2    = 6,
  MAI_AMR_NB_RATE_12_2    = 7,
  MAI_AMR_NB_RATE_SID     = 8,
  MAI_AMR_NB_RATE_NO_DATA = 15
} ENUM8(MAI_AMR_NB_Rate_t);

/**
 * This data type defines the AMR-WB rates.
 *
 * @param MAI_AMR_WB_RATE_6_60         6.60 kbps.
 * @param MAI_AMR_WB_RATE_8_85         8.85 kbps.
 * @param MAI_AMR_WB_RATE_12_65        12.65 kbps.
 * @param MAI_AMR_WB_RATE_14_25        14.25 kbps.
 * @param MAI_AMR_WB_RATE_15_85        15.85 kbps.
 * @param MAI_AMR_WB_RATE_18_25        18.25 kbps.
 * @param MAI_AMR_WB_RATE_19_85        19.85 kbps.
 * @param MAI_AMR_WB_RATE_23_05        23.05 kbps.
 * @param MAI_AMR_WB_RATE_23_85        23.85 kbps.
 * @param MAI_AMR_WB_RATE_23_85        23.85 kbps.
 * @param MAI_AMR_WB_RATE_SID          SID frame.
 * @param MAI_AMR_WB_RATE_SPEECH_LOST  Lost frame.
 * @param MAI_AMR_WB_RATE_NO_DATA      No Data frame.
 */
TYPEDEF_ENUM
{
  MAI_AMR_WB_RATE_6_60        = 0,
  MAI_AMR_WB_RATE_8_85        = 1,
  MAI_AMR_WB_RATE_12_65       = 2,
  MAI_AMR_WB_RATE_14_25       = 3,
  MAI_AMR_WB_RATE_15_85       = 4,
  MAI_AMR_WB_RATE_18_25       = 5,
  MAI_AMR_WB_RATE_19_85       = 6,
  MAI_AMR_WB_RATE_23_05       = 7,
  MAI_AMR_WB_RATE_23_85       = 8,
  MAI_AMR_WB_RATE_SID         = 9,
  MAI_AMR_WB_RATE_SPEECH_LOST = 14,
  MAI_AMR_WB_RATE_NO_DATA     = 15
} ENUM8(MAI_AMR_WB_Rate_t);

/**
 * This data type defines the frame number for 2G (GSM) when AMR-NB or AMR-WB is used.\n
 * The parameter is a counter that should be set to zero at first UL frame and then be incremented at each subsequent frame modulus 8.
 *
 * @param MAI_FRAME_NUMBER_0    Frame number 0.
 * @param MAI_FRAME_NUMBER_1    Frame number 1.
 * @param MAI_FRAME_NUMBER_2    Frame number 2.
 * @param MAI_FRAME_NUMBER_3    Frame number 3.
 * @param MAI_FRAME_NUMBER_4    Frame number 4.
 * @param MAI_FRAME_NUMBER_5    Frame number 5.
 * @param MAI_FRAME_NUMBER_6    Frame number 6.
 * @param MAI_FRAME_NUMBER_7    Frame number 7.
 */
TYPEDEF_ENUM
{
  MAI_FRAME_NUMBER_0 = 0,
  MAI_FRAME_NUMBER_1 = 1,
  MAI_FRAME_NUMBER_2 = 2,
  MAI_FRAME_NUMBER_3 = 3,
  MAI_FRAME_NUMBER_4 = 4,
  MAI_FRAME_NUMBER_5 = 5,
  MAI_FRAME_NUMBER_6 = 6,
  MAI_FRAME_NUMBER_7 = 7
} ENUM8(MAI_FrameNumber_t);

/**
 * This data type defines the loopback response.\n
 *
 * @param MAI_LOOP_NOT_SUPPORTED  Loop not supported.
 * @param MAI_LOOP_OK             Loop supported.
 */
TYPEDEF_ENUM
{
  MAI_LOOPBACK_NOT_SUPPORTED = 0,
  MAI_LOOPBACK_OK            = 1
} ENUM8(MAI_LoopBackSupported_t);

/**
 * This data type defines the loopback mode.\n
 *
 * @param MAI_MODEM_LOOPBACK_MODE_OFF          Loopback off.
 * @param MAI_MODEM_LOOPBACK_MODE_RAW          Loopback RAW.
 * @param MAI_MODEM_LOOPBACK_MODE_SPEECH_CODEC Loopback speech data.
 */
TYPEDEF_ENUM
{
  MAI_MODEM_LOOPBACK_MODE_OFF          = 0,
  MAI_MODEM_LOOPBACK_MODE_RAW          = 1,
  MAI_MODEM_LOOPBACK_MODE_SPEECH_CODEC = 2
} ENUM8(MAI_ModemLoopBackMode_t);

/**
 * This data type defines the loopback mode.\n
 *
 * @param MAI_AUDIO_LOOPBACK_MODE_OFF  Loopback off.
 * @param MAI_AUIDO_LOOPBACK_MODE_RAW  Loopback RAW.
 * @param MAI_AUDIO_LOOPBACK_MODE_PCM  Loopback PCM.
 */
TYPEDEF_ENUM
{
  MAI_AUDIO_LOOPBACK_MODE_OFF = 0,
  MAI_AUDIO_LOOPBACK_MODE_RAW = 1,
  MAI_AUDIO_LOOPBACK_MODE_PCM = 2
} ENUM8(MAI_AudioLoopBackMode_t);

/**************************************/
/* Structs                            */
/**************************************/

/**
 * This data type defines the UL Full Rate data format.
 *
 * @param SP          Speech flag, used to indicate if the current frame is a speech or silence frame.
 * @param SpeechData  D0-D259.
 */
typedef struct
{
  MAI_SP_t        SP;
  uint8           SpeechData[MAI_FR_FRAME_SIZE];
} MAI_FR_UL_Buffer_t;

/**
 * This data type defines the DL Full Rate data format.
 *
 * @param SID         Silence descriptor.
 * @param TAF         Timing alignment flag, used to indicate for the decoder that the comfort noise parameters should be updated.
 * @param BFI         Bad frame indicator, used with DFI to indicate for the decoder that the received frame is bad.
 * @param DFI         Dangerous frame indication, used to indicate for the decoder that the received frame might be bad.
 * @param SpeechData  D0-D259.
 */
typedef struct
{
  MAI_FrameType_t SID;
  MAI_TAF_t       TAF;
  MAI_BFI_t       BFI;
  MAI_FR_DFI_t    DFI;
  uint8           SpeechData[MAI_FR_FRAME_SIZE];
} MAI_FR_DL_Buffer_t;

/**
 * This data type defines the UL Half Rate data format.
 *
 * @param SP          Speech flag, used to indicate if the current frame is a speech or silence frame.
 * @param SpeechData  D0-D111.
 */
typedef struct
{
  MAI_SP_t        SP;
  uint8           SpeechData[MAI_HR_FRAME_SIZE];
} MAI_HR_UL_Buffer_t;

/**
 * This data type defines the DL Half Rate data format.
 *
 * @param SID         Silence descriptor.
 * @param TAF         Timing alignment flag, used to indicate for the decoder that the comfort noise parameters should be updated.
 * @param BFI         Bad frame indicator, used with UFI to indicate for the decoder that the received frame is bad.
 * @param UFI         Unreliable frame indicator, used to indicate for the decoder that the received frame might be bad.
 * @param SpeechData  D0-D111.
 */
typedef struct
{
  MAI_FrameType_t SID;
  MAI_TAF_t       TAF;
  MAI_BFI_t       BFI;
  MAI_UFI_t       UFI;
  uint8           SpeechData[MAI_HR_FRAME_SIZE];
} MAI_HR_DL_Buffer_t;

/**
 * This data type defines the UL Enhanced Full Rate data format.
 *
 * @param SP          Speech flag, used to indicate if the current frame is a speech or silence frame.
 * @param SpeechData  D0-D259.
 */
typedef struct
{
  MAI_SP_t        SP;
  uint8           SpeechData[MAI_EFR_FRAME_SIZE];
} MAI_EFR_UL_Buffer_t;

/**
 * This data type defines the DL Enhanced Full Rate data format.
 *
 * @param SID         Silence descriptor.
 * @param TAF         Timing alignment flag, used to indicate for the decoder that the comfort noise parameters should be updated.
 * @param BFI         Bad frame indicator, to indicate for the decoder that the received frame is bad.
 * @param SpeechData  D0-D259.
 */
typedef struct
{
  MAI_FrameType_t SID;
  MAI_TAF_t       TAF;
  MAI_BFI_t       BFI;
  uint8           SpeechData[MAI_EFR_FRAME_SIZE];
} MAI_EFR_DL_Buffer_t;

/**
 * This data type defines UL AMR-NB data format.
 *
 * @param TX_Type       The frame TX_TYPE. The parameter is only valid in 2G.
 * @param Rate          The AMR-NB rate.
 * @param SpeechData    Variable size that depends on the AMR-NB rate.
 *                      The size (in bits) is given by Rate (in kbps) times 20 (ms), e.g. for AMR 12.2 kbps 12.2x20=244 bits or D0-D243.
 *                      The SID frame size is always 39 bits for AMR-NB.
 */
typedef struct
{
  MAI_AMR_TX_Type_t TX_Type;
  MAI_AMR_NB_Rate_t Rate;
  uint8             SpeechData[MAI_AMR_NB_FRAME_SIZE];
} MAI_AMR_NB_UL_Buffer_t;

/**
 * This data type defines DL AMR-NB data format.
 *
 * @param RX_Type       The frame RX_TYPE. The parameter is only valid in 2G.
 * @param Rate          The AMR-NB rate.
 * @param FQI           The Frame Quality Indicator, i.e. if there was a CRC error on the class A bits for 3G. The parameter is only valid for 3G.
 * @param SpeechData    Variable size that depends on the AMR-NB rate.
 *                      The size (in bits) is given by Rate (in kbps) times 20 (ms), e.g. for AMR 12.2 kbps 12.2x20=244 bits or D0-D243.
 *                      The SID frame size is always 39 bits for AMR-NB.
 */
typedef struct
{
  MAI_AMR_NB_RX_Type_t RX_Type;
  MAI_AMR_NB_Rate_t    Rate;
  MAI_FQI_t            FQI;
  uint8                SpeechData[MAI_AMR_NB_FRAME_SIZE];
} MAI_AMR_NB_DL_Buffer_t;

/**
 * This data type defines UL AMR-WB data format.
 *
 * @param TX_Type       The frame TX_TYPE. The parameter is only valid in 2G.
 * @param Rate          The AMR-WB rate.
 * @param SpeechData    Variable size that depends on the AMR-WB rate.
 *                      The size (in bits) is given by Rate (in kbps) times 20 (ms), e.g. for AMR 12.65 kbps 12.65x20=253 bits or D0-D252.
 *                      The SID frame size is always 40 bits for AMR-WB.
 */
typedef struct
{
  MAI_AMR_TX_Type_t TX_Type;
  MAI_AMR_WB_Rate_t Rate;
  uint8             SpeechData[MAI_AMR_WB_FRAME_SIZE];
} MAI_AMR_WB_UL_Buffer_t;

/**
 * This data type defines DL AMR-WB data format.
 *
 * @param RX_Type       The frame RX_TYPE. The parameter is only valid in 2G.
 * @param Rate          The AMR-WB rate.
 * @param FQI           The Frame Quality Indicator, i.e. if there was a CRC error on the class A bits for 3G. The parameter is only valid for 3G.
 * @param SpeechData    Variable size that depends on the AMR-WB rate.
 *                      The size (in bits) is given by Rate (in kbps) times 20 (ms), e.g. for AMR 12.65 kbps 12.65x20=253 bits or D0-D252.
 *                      The SID frame size is always 40 bits for AMR-WB.
 */
typedef struct
{
  MAI_AMR_WB_RX_Type_t  RX_Type;
  MAI_AMR_WB_Rate_t     Rate;
  MAI_FQI_t             FQI;
  uint8                 SpeechData[MAI_AMR_WB_FRAME_SIZE];
} MAI_AMR_WB_DL_Buffer_t;


/**************************************/
/* Unions                             */
/**************************************/

/**
 * This data type defines the union of the AMR-NB and AMR-WB rate types that is used in the "Coding Format" message.
 *
 * @param AMR_NB     AMR-NB rate.
 * @param AMR_WB     AMR-WB rate
 */
typedef union
{
  MAI_AMR_NB_Rate_t AMR_NB;
  MAI_AMR_WB_Rate_t AMR_WB;
} MAI_AMR_BitRate_t;

/**************************************/
/*Structure                           */
/**************************************/

/**
 * This data type defines the DL PCM 8 kHz mono data format.
 *
 * @param PCM8_Data    20 ms of PCM data 8 kHz, mono, each sample occupying 2 bytes (big endian)
 * @param SpeechCodec  Current used codec to decode  PCM_Dta
 * @param RX_Type_SID  Speech decoder input flags, codec dependant
 * @param Bitrate_TAF  Speech decoder input flags, codec dependant
 * @param FQI_BFI      Speech decoder input flags, codec dependant
 */
typedef struct
{
  uint8              PCM8_Data[MAI_PCM_FRAME_SIZE_8KHZ];
  MAI_SpeechCodec_t  SpeechCodec;
  uint8              RX_Type_SID;
  uint8              Bitrate_TAF;
  uint8              FQI_BFI;
} MAI_PCM8_DL_Buffer_t;

/**
 * This data type defines the UL PCM 8 kHz mono data format.
 *
 * @param PCM8_Data     20 ms of PCM data 8 kHz, mono, each sample occupying 2 bytes (big endian)
 */
typedef struct
{
  uint8 PCM8_Data[MAI_PCM_FRAME_SIZE_8KHZ];
} MAI_PCM8_UL_Buffer_t;

/**
 * This data type defines the DL PCM 16 kHz mono data format.
 *
 * @param PCM16_Data   20 ms of PCM data 16 kHz, mono, each sample occupying 2 bytes (big endian)
 * @param SpeechCodec  Current used codec to decode  PCM_Dta
 * @param RX_Type_SID  Speech decoder input flags, codec dependant
 * @param Bitrate_TAF  Speech decoder input flags, codec dependant
 * @param FQI_BFI      Speech decoder input flags, codec dependant
 */
typedef struct
{
  uint8             PCM16_Data[MAI_PCM_FRAME_SIZE_16KHZ];
  MAI_SpeechCodec_t SpeechCodec;
  uint8             RX_Type_SID;
  uint8             Bitrate_TAF;
  uint8             FQI_BFI;
} MAI_PCM16_DL_Buffer_t;

/**
 * This data type defines the UL PCM 16 kHz mono data format.
 *
 * @param PCM16_Data     20 ms of PCM data 16 kHz, mono, each sample occupying 2 bytes (big endian)
 * @param SpeechCodec    Current used codec to decode  PCM_Dta
 * @param RX_Type        Speech decoder input flag, AMR_WB RX frame type
 * @param BitRate        Speech decoder input flag, AMR_WB bit rate
 * @param FQI            Speech decoder input flag, frame quality indicatior, FQI
 */
typedef struct
{
  uint8 PCM16_Data[MAI_PCM_FRAME_SIZE_16KHZ];
} MAI_PCM16_UL_Buffer_t;

/**************************************/
/* Buffers                            */
/**************************************/

/**
 * This data type defines the union of the different UL buffer types that is used in the "Uplink Speech Data" message.
 *
 * @param FR     Full Rate UL buffer.
 * @param HR     Half Rate UL buffer.
 * @param EFR    Enhanced Full Rate UL buffer.
 * @param AMR_NB AMR-NB UL buffer.
 * @param AMR_WB AMR-WB UL buffer.
 * @param PCM8   PCM 8 kHz UL buffer.
 * @param PCM16  PCM 16 kHz UL buffer.
 */
typedef union
{
  MAI_FR_UL_Buffer_t      FR;
  MAI_HR_UL_Buffer_t      HR;
  MAI_EFR_UL_Buffer_t     EFR;
  MAI_AMR_NB_UL_Buffer_t  AMR_NB;
  MAI_AMR_WB_UL_Buffer_t  AMR_WB;
  MAI_PCM8_UL_Buffer_t    PCM8;
  MAI_PCM16_UL_Buffer_t   PCM16;
} MAI_UL_Buffers_t;

/**
 * This data type defines the union of the different DL buffer types that is used in the "Downlink Speech Data" message.
 *
 * @param FR      Full Rate DL buffer.
 * @param HR      Half Rate DL buffer.
 * @param EFR     Enhanced Full Rate DL buffer.
 * @param AMR_NB  AMR-NB DL buffer.
 * @param AMR_WB  AMR-WB DL buffer.
 * @param PCM8    PCM 8 kHz DL buffer.
 * @param PCM16   PCM 16 kHz DL buffer.
 */
typedef union
{
  MAI_FR_DL_Buffer_t      FR;
  MAI_HR_DL_Buffer_t      HR;
  MAI_EFR_DL_Buffer_t     EFR;
  MAI_AMR_NB_DL_Buffer_t  AMR_NB;
  MAI_AMR_WB_DL_Buffer_t  AMR_WB;
  MAI_PCM8_DL_Buffer_t    PCM8;
  MAI_PCM16_DL_Buffer_t   PCM16;
} MAI_DL_Buffers_t;

/**************************************/
/* Messages                           */
/**************************************/

/**
 * This data type defines the message header used for all messages. Note that the header must be aligned to even number of 32 bits to get it portable.
 *
 * @param MessageSize       The size if the total message in number of bytes. Coded as big endian two-complemented (network byte order).
 * @param MessageId         The message identity
 * @param MessagePadding    Unused parameter just to get the start of the rest of the message aligned to an even 32 bit boundary
 */
typedef struct
{
  uint8         MessageSize[2];
  MAI_Message_t MessageId;
  uint8         MessagePadding; // to get 32 bit aligned start of the "Data"
} MAI_MessageHeader_t;

/**************************************/
/* Information Messages               */
/**************************************/

/**
 * This data type defines the "Information Request" message.\n
 * It is sent when information about the Msub is requested.
 *
 * @param MessageHeader   The common message header.
 */
typedef struct
{
  MAI_MessageHeader_t MessageHeader;
} MAI_InformationRequest_t;

/**
 * This data type defines the "Information Response" message.\n
 * It contains Msub information and is sent as response to the "Modem Information Request" message.
 *
 * @param MessageHeader       The common message header.
 * @param ModemId             Modem Identity String.
 * @param ModemVersionString  Modem Version String (e.g. freeze label).
 * @param VersionMajor        Version Major.
 * @param VersionMinor        Version Minor.
 */
typedef struct
{
  MAI_MessageHeader_t MessageHeader;
  uint8               ModemId[MAI_MODEM_ID_STRING_LENGTH];
  uint8               ModemVersionString[MAI_MODEM_VERSION_STRING_LENGTH];
  uint8               VersionMajor;
  uint8               VersionMinor;
} MAI_InformationResponse_t;

/**************************************/
/* Coding Format Messages             */
/**************************************/

/**
 * This data type defines the "Coding Format" message.\n
 * The message is always sent from the Msub when an audio path is connected by the Msub.\n
 * It may also be sent at handover (Intra/Inter-System) as well as when a parameter is changed. Example of parameter changes are:\n
 *   - The AMR-NB/AMR-WB rate that shall be used for encoding a speech frame is changed.\n
 *   - The System changes when an Inter-system handover occurs.\n
 *   - When the audio path is disconnected, it shall be sent with system set to 'None' and Speech Codec set to 'None'.\n\n
 *
 * The speech coding format is valid for both uplink and downlink. During a speech codec handover there might be a short overlap.\n
 * Consider a switch from AMR-NB to AMR-WB is at time T. Then at time T-x an AMR-WB speech frame needs to be built and sent to NS.\n
 * At time T the last AMR-NB speech frame is received over the air and will be delivered to the Asub at T+y.\n
 * Thus, during the time [T-x, T+y] one codec is active in UL/TX and another is active in DL/RX.
 *
 * @param MessageHeader               The common message header.
 * @param System                      The parameter defines the system or access technology used for the following speech data.
 *                                    The value of the parameter gives what parameters that is valid in the message.
 * @param SpeechCodec                 Parameter defines the speech codec to be used in uplink and downlink.
 * @param NoiseSupressionControlBit   The Noise Suppression Control Bit is defined in 3GPP 44.018 and toggles noise suppression
 *                                    in the uplink path. It is only valid in GSM AMR-NB and GSM AMR-WB.
 * @param NSYNC                       Defines behavior of TX/RX DTX during handover synchronization in 2G (GSM), see 3GPP 26.093 Annex A5..
 * @param FrameNumber                 Parameter is only used in 2G for AMR-NB/AMR-WB. It is used in conjunction with Frame Number in the
 *                                    "Speech Data Uplink" message. The number specifies at what encoded frame the new bit rate shall be used.
 * @param DTX                         The parameter defines if DTX (Discontinuous transmission) is enabled/disabled.
 * @param NoDataUsage                 Controls if the Asub is allowed to skip sending any uplink data if speech contains no data
 *                                    (for example "No data" in AMR-NB/AMR-WB).
 * @param UnusedPadding               Padding to get the next "complex" parameter aligned to a 32 bit boundary for portability.
 * @param AMR_Rate                    The used AMR-NB/AMR-WB rate. The parameter is only valid when SpeechCodec is AMR-NB or AMR-WB.
 */
typedef struct
{
  MAI_MessageHeader_t MessageHeader;
  MAI_System_t        System;
  MAI_SpeechCodec_t   SpeechCodec;
  MAI_NSCB_t          NoiseSupressionControlBit;
  MAI_AMR_NSYNC_t     NSYNC;
  MAI_FrameNumber_t   FrameNumber;
  MAI_DTX_t           DTX;
  MAI_NoDataUsage_t   NoDataUsage;
  uint8               UnusedPadding;
  MAI_AMR_BitRate_t   AMR_Rate;
} MAI_CodingFormat_t;

/**
 * This data type defines the "Coding Format Response" message.\n
 * The message is sent as a response to a "Coding Format" message.
 *
 * @param MessageHeader         The common message header.
 * @param CodingConfigResponse  Response to if coding format was acceptable.
 * @param NoDataUsage           Controls if the Msub is allowed to skip sending any uplink data if speech contains no data
 *                              (for example "No data" in AMR-NB/AMR-WB).
 *                              Note that when the system is set to MAI_SYSTEM_CS_O_HS no message will be generated even if this is set to TRUE.
 */
typedef struct
{
  MAI_MessageHeader_t MessageHeader;
  MAI_CodingConfig_t  CodingConfigResponse;
  MAI_NoDataUsage_t   NoDataUsage;
} MAI_CodingFormatResponse_t;

/**************************************/
/* VoiceCall Status Message           */
/**************************************/

/**
 * This data type defines the "Voice Call Status" message.\n
 * The message should be sent after the "Coding Format" message when a call is setup and in addition
 * each time the status of a voice call is changed.
 *
 * @param MessageHeader     The common message header.
 * @param CallStatus        The status of the Voice call. Both uplink and downlink shall be muted
 *                          when status is "Not Connected" and un-muted when status is "Connected".
 */
typedef struct
{
  MAI_MessageHeader_t     MessageHeader;
  MAI_CallStatus_t        CallStatus;
} MAI_VoiceCallStatus_t;

/**************************************/
/* Test modem loopback Messages       */
/**************************************/

/**
 * This data type defines the "Test modem loopback" message.\n
 * The message should be sent by the APE to ask for a modem loopback.
 *
 * @param MessageHeader     The common message header.
 * @param LoopBackMode      The requested modem loopback mode
 * @param System            The parameter defines the system or access technology used for the following speech data.
 *                          The value of the parameter gives what parameters that is valid in the message.
 * @param SpeechCodec       Parameter defines the speech codec to be used in uplink and downlink.
 * @param DTX               The parameter defines if DTX (Discontinuous transmission) is enabled/disabled.
 * @param AMR_Rate          The used AMR-NB/AMR-WB rate. The parameter is only valid when SpeechCodec is AMR-NB or AMR-WB.
 * @param NoDataUsage       Controls if the Msub is allowed to skip sending any uplink data if speech contains no data
 *                          (for example "No data" in AMR-NB/AMR-WB).
 *                          Note that when the system is set to MAI_SYSTEM_CS_O_HS no message will be generated even if this is set to TRUE.
 */
typedef struct
{
  MAI_MessageHeader_t     MessageHeader;
  MAI_ModemLoopBackMode_t LoopBackMode;
  MAI_System_t            System;
  MAI_SpeechCodec_t       SpeechCodec;
  MAI_DTX_t               DTX;
  MAI_AMR_BitRate_t       AMR_Rate;
  MAI_NoDataUsage_t       NoDataUsage;
} MAI_TestModemLoopBack_t;

/**
 * This data type defines the "Test modem loopback reponse" message.\n
 * The message should be sent by the Modem to say whether this feature is supported or not.
 *
 * @param MessageHeader     The common message header.
 * @param Result            The loopback support result
 */
typedef struct
{
  MAI_MessageHeader_t     MessageHeader;
  MAI_LoopBackSupported_t Result;
} MAI_TestModemLoopBackResponse_t;

/**************************************/
/* Test audio loopback Messages       */
/**************************************/

/**
 * This data type defines the "Test audio loopback" message.\n
 * The message should be sent by the modem to ask for a APE loopback.
 *
 * @param MessageHeader     The common message header.
 * @param LoopBackMode      The requested audio loopback mode
 */
typedef struct
{
  MAI_MessageHeader_t     MessageHeader;
  MAI_AudioLoopBackMode_t LoopBackMode;
} MAI_TestAudioLoopBack_t;

/**
 * This data type defines the "Test audio loopback reponse" message.\n
 * The message should be sent by the APE to say whether this feature is supported or not.
 *
 * @param MessageHeader     The common message header.
 * @param Result            The loopback request result
 */
typedef struct
{
  MAI_MessageHeader_t     MessageHeader;
  MAI_LoopBackSupported_t Result;
} MAI_TestAudioLoopBackResponse_t;

/**************************************/
/* Uplink Timing Message              */
/**************************************/

/**
 * This data type defines the "Uplink Timing" message.\n
 * The message is sent from the Asub to the Msub and contains the timing window range within which an encoded message
 * sent to the Msub is considered to be in time. It should be sent before the first speech data is sent to the Msub
 * and can be sent whenever the Asub wants to change the timing window range.\n
 * If a "Speech Data Uplink" message arrives outside the range a "Timing Report" message is sent with time and cause for the event.\n
 * If Low = High a "Timing Report" message will be sent for each "Speech Data Uplink" message.\n
 * If Low = 0 and High \> 20 ms no time report will be sent.
 *
 * @param MessageHeader   The common message header.
 * @param WaterMarkLow    The parameter defines the low watermark.
 *                        If a "Speech Data Uplink" message arrives later than Low relative to when the Msub needs to start processing
 *                        a "Timing Report" is generated.
 *                        The value is given in micro-seconds.
 * @param WaterMarkHigh   The parameter defines the high watermark.
 *                        If a "Speech Data Uplink" arrives earlier than High relative to when the Msub needs to start processing
 *                        a "Timing Report" is generated.
 *                        The value is given in micro-seconds.
 */
typedef struct
{
  MAI_MessageHeader_t MessageHeader;
  uint8               WaterMarkLow[4];
  uint8               WaterMarkHigh[4];
} MAI_UplinkTiming_t;

/**************************************/
/* Downlink Timing Message            */
/**************************************/

/**
 * This data type defines the "Downlink Timing Message" message.\n
 * The message contains the current worst-case time configuration within the Msub.
 * This includes the worst-case Msub processing time including the worst-case network jitter.
 *
 * @param MessageHeader   The common message header.
 * @param ProcessingTime  The worst-case processing time including network jitter for a "Speech Data Downlink" message.
 *                        The value is given in micro-seconds.
 */
typedef struct
{
  MAI_MessageHeader_t MessageHeader;
  uint8               ProcessingTime[4];
} MAI_DownlinkTiming_t;

/**************************************/
/* Timing Report Message              */
/**************************************/

/**
 * This data type defines the "Timing Report" message.
 *
 * @param MessageHeader   The common message header.
 * @param TimeDiff        The difference in micro-seconds between when the Msub received the "Speech Data Uplink" message until
 *                        the Msub needs to start the Msub related speech processing.
 * @param Cause           Gives the reason for the generation of the "Timing Report" message.\n
 *                        The "Speech Data Uplink" message was either outside the valid range as specified in the latest "Uplink Timing"
 *                        message or it was discarded due to that a new "Speech Data Uplink" message was received before the Msub
 *                        processing started.
 */
typedef struct
{
  MAI_MessageHeader_t MessageHeader;
  uint8               TimeDiff[4];
  MAI_TimingCause_t   Cause;
} MAI_TimingReport_t;

/**************************************/
/* Speech Data Uplink Message         */
/**************************************/

/**
 * This data type defines the "Speech Data Uplink" message.\n
 * Note that when allocating memory for this message a sizeof() will return the largest possible message for the available codecs.\n
 * Thus, to reduce the memory it is recommended that the provided C macros of format:\n
 * MAI_X_UL_DATA_MSG_SIZE (X different for different codecs)\n
 * should be used.
 *
 * @param MessageHeader   The common message header.
 * @param System          The parameter defines the system or access technology used for speech data and defines which parameters that is valid.
 * @param SpeechCodec     Specifies the speech codec used for UL_Data.
 * @param FrameNumber     Parameter only valid for 2G AMR-NB/AMR-WB. A counter that is used in conjunction with Frame Number in "Coding Format" message. It should be set to zero at first UL frame, and then be incremented at each subsequent frame modulus 8.
 * @param UnusedPadding   Padding to get the next "complex" parameter aligned to a 32 bit boundary for portability.
 * @param UL_Data         Speech data depending on speech codec.
 */
typedef struct
{
  MAI_MessageHeader_t MessageHeader;
  MAI_System_t        System;
  MAI_SpeechCodec_t   SpeechCodec;
  MAI_FrameNumber_t   FrameNumber;
  uint8               UnusedPadding;
  MAI_UL_Buffers_t    UL_Data;
} MAI_SpeechDataUL_t;

/** Macro to get the size of a "Speech Data Uplink Message" for the Full Rate speech codec. */
#define MAI_FR_UL_DATA_MSG_SIZE     (sizeof(MAI_SpeechDataUL_t) - sizeof(MAI_UL_Buffers_t) + sizeof(MAI_FR_UL_Buffer_t))

/** Macro to get the size of a "Speech Data Uplink Message" for the Half Rate speech codec.  */
#define MAI_HR_UL_DATA_MSG_SIZE     (sizeof(MAI_SpeechDataUL_t) - sizeof(MAI_UL_Buffers_t) + sizeof(MAI_HR_UL_Buffer_t))

/** Macro to get the size of a "Speech Data Uplink Message" for the Enhanced Full Rate speech codec.  */
#define MAI_EFR_UL_DATA_MSG_SIZE    (sizeof(MAI_SpeechDataUL_t) - sizeof(MAI_UL_Buffers_t) + sizeof(MAI_EFR_UL_Buffer_t))

/** Macro to get the size of a "Speech Data Uplink Message" for the AMR-NB speech codec.  */
#define MAI_AMR_NB_UL_DATA_MSG_SIZE (sizeof(MAI_SpeechDataUL_t) - sizeof(MAI_UL_Buffers_t) + sizeof(MAI_AMR_NB_UL_Buffer_t))

/** Macro to get the size of a "Speech Data Uplink Message" for the AMR-WB speech codec.  */
#define MAI_AMR_WB_UL_DATA_MSG_SIZE (sizeof(MAI_SpeechDataUL_t) - sizeof(MAI_UL_Buffers_t) + sizeof(MAI_AMR_WB_UL_Buffer_t))

/** Macro to get the size of a "Speech Data Uplink Message" for the PCM 8 kHz speech codec.  */
#define MAI_PCM8_UL_DATA_MSG_SIZE   (sizeof(MAI_SpeechDataUL_t) - sizeof(MAI_UL_Buffers_t) + sizeof(MAI_PCM8_UL_Buffer_t))

/** Macro to get the size of a "Speech Data Uplink Message" for the PCM 16 kHz speech codec.  */
#define MAI_PCM16_UL_DATA_MSG_SIZE  (sizeof(MAI_SpeechDataUL_t) - sizeof(MAI_UL_Buffers_t) + sizeof(MAI_PCM16_UL_Buffer_t))


/**************************************/
/* Speech Data Downlink Message       */
/**************************************/

/**
 * This data type defines the "Speech Data Downlink" message.\n
 * Note that when allocating memory for this message a sizeof() will return the largest possible message for the available codecs.\n
 * Thus, to reduce the memory it is recommended that the provided C macros of format:\n
 * MAI_X_DL_DATA_MSG_SIZE (X different for different codecs)\n
 * should be used.
 *
 * @param MessageHeader   The common message header.
 * @param System          The parameter defines the system or access technology used for speech data and defines which parameters that is valid.
 * @param SpeechCodec     Specifies the speech codec used for UL_Data.
 * @param UnusedPadding   Padding to get the next parameter aligned to a 32 bit boundary for portability.
 * @param ProcessingTime  Parameter contains the processing time including network jitter for the frame. The value shall be within the range 0 to worst case processing time as defined in the "Downlink Timing Configuration" message.
 * @param DL_Data         Speech data depending on speech codec.
 */
typedef struct
{
  MAI_MessageHeader_t MessageHeader;
  MAI_System_t        System;
  MAI_SpeechCodec_t   SpeechCodec;
  uint8               UnusedPadding[2];
  uint8               ProcessingTime[4];
  MAI_DL_Buffers_t    DL_Data;
} MAI_SpeechDataDL_t;

/** Macro to get the size of a "Speech Data Downlink Message" for the Full Rate speech codec.  */
#define MAI_FR_DL_DATA_MSG_SIZE     (sizeof(MAI_SpeechDataDL_t) - sizeof(MAI_DL_Buffers_t) + sizeof(MAI_FR_DL_Buffer_t))

/** Macro to get the size of a "Speech Data Downlink Message" for the Half Rate speech codec.  */
#define MAI_HR_DL_DATA_MSG_SIZE     (sizeof(MAI_SpeechDataDL_t) - sizeof(MAI_DL_Buffers_t) + sizeof(MAI_HR_DL_Buffer_t))

/** Macro to get the size of a "Speech Data Downlink Message" for the Enhanced Full Rate speech codec.  */
#define MAI_EFR_DL_DATA_MSG_SIZE    (sizeof(MAI_SpeechDataDL_t) - sizeof(MAI_DL_Buffers_t) + sizeof(MAI_EFR_DL_Buffer_t))

/** Macro to get the size of a "Speech Data Downlink Message" for the AMR-NB speech codec.  */
#define MAI_AMR_NB_DL_DATA_MSG_SIZE (sizeof(MAI_SpeechDataDL_t) - sizeof(MAI_DL_Buffers_t) + sizeof(MAI_AMR_NB_DL_Buffer_t))

/** Macro to get the size of a "Speech Data Downlink Message" for the AMR-WB speech codec.  */
#define MAI_AMR_WB_DL_DATA_MSG_SIZE (sizeof(MAI_SpeechDataDL_t) - sizeof(MAI_DL_Buffers_t) + sizeof(MAI_AMR_WB_DL_Buffer_t))

/** Macro to get the size of a "Speech Data Downlink Message" for the PCM 8 kHz speech codec.  */
#define MAI_PCM8_DL_DATA_MSG_SIZE   (sizeof(MAI_SpeechDataDL_t) - sizeof(MAI_DL_Buffers_t) + sizeof(MAI_PCM8_DL_Buffer_t))

/** Macro to get the size of a "Speech Data Downlink Message" for the PCM 16 kHz speech codec. */
#define MAI_PCM16_DL_DATA_MSG_SIZE  (sizeof(MAI_SpeechDataDL_t) - sizeof(MAI_DL_Buffers_t) + sizeof(MAI_PCM16_DL_Buffer_t))

#endif /* INCLUSION_GUARD_T_MAI_H */
