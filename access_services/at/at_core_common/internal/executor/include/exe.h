/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef EXE_H
#define EXE_H 1

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define EXE_COPS_OPERATOR_MAX_ACT_LEN       (2)
#define EXE_COPS_OPERATOR_MAX_LONG_LEN      (60)
#define EXE_COPS_OPERATOR_MAX_SHORT_LEN     (30)
#define EXE_COPS_OPERATOR_MAX_STAT_LEN      (1)
#define EXE_IMEI_LENGTH   (16)   /* Including space for null termination */
#define EXE_SIM_IMSI_LEN  (15)
#define EXE_CEER_MAX_CHAR  (90)   /* including space for null termination */
#define EXE_INVALID_LAC (0x0000) /* Reserved value to indicate that no valid LAC exists, as defined by 3GPP 23.003 */

/* Max length for the MCC (Country Code) and MNC (Network Code) on numeric format */
#define EXE_OPERATOR_NUMERIC_STRING_MAX_LENGTH (6)

#define EXE_PSCC_UPLINK_DATA_MAX_SIZE (262143)
#define EXE_PSCC_DEFAULT_UPLINK_DATA_SIZE (512)
#define EXE_COPS_IMEI_MAX_LEN (15)

/* Default values for APN and PDP address */
#define DEFAULT_APN ""
#define DEFAULT_PDP_ADDR ""

/* Note on default values:
 * Default values have the value set by the ST-Ericsson AT specifications. In those cases there are no defaults,
 * 0 will be used (which means that subscribed values will be used.
 * Default values will also be used for parameters omitted from the CGEQMIN/REQ SET command.
 */
#define EXE_QOS3G_PROFILE_TRAFFIC_CLASS (3)
#define EXE_QOS3G_PROFILE_TRAFFIC_CLASS_REQ (4)
#define EXE_QOS3G_PROFILE_MAX_BITRATE_UL (0)
#define EXE_QOS3G_PROFILE_MAX_BITRATE_DL (0)
#define EXE_QOS3G_PROFILE_GUARANTEED_BITRATE_UL (0)
#define EXE_QOS3G_PROFILE_GUARANTEED_BITRATE_DL (0)
#define EXE_QOS3G_PROFILE_DELIVERY_ORDER (0)
#define EXE_QOS3G_PROFILE_DELIVERY_ORDER_REQ (2)
#define EXE_QOS3G_PROFILE_SDU_SIZE (0)
#define EXE_QOS3G_PROFILE_SDU_ERROR_RATIO ""
#define EXE_QOS3G_PROFILE_RESIDUAL_BER ""
#define EXE_QOS3G_PROFILE_DELIVERY_ERR_SDU (2)
#define EXE_QOS3G_PROFILE_DELIVERY_ERR_SDU_REQ (3)
#define EXE_QOS3G_PROFILE_TRANSFER_DELAY (0)
#define EXE_QOS3G_PROFILE_TRAFFIC_HANDLING_PRIORITY (0)
#define EXE_QOS3G_PROFILE_SOURCE_STATISTICS_DESCRIPTOR (0)
#define EXE_QOS3G_PROFILE_SIGNALLING_INDICATION (0)

/* Maximum values supported */
#define EXE_QOS3G_PROFILE_MAX_TRAFFIC_CLASS (3)
#define EXE_QOS3G_PROFILE_MAX_MAX_BITRATE_UL (5696)
#define EXE_QOS3G_PROFILE_MAX_MAX_BITRATE_DL (21000)
#define EXE_QOS3G_PROFILE_MAX_GUARANTEED_BITRATE_UL (5696)
#define EXE_QOS3G_PROFILE_MAX_GUARANTEED_BITRATE_DL (21000)
#define EXE_QOS3G_PROFILE_MAX_DELIVERY_ORDER (1)
#define EXE_QOS3G_PROFILE_MAX_SDU_SIZE (1520)
#define EXE_QOS3G_PROFILE_MAX_DELIVERY_ERR_SDU (2)
#define EXE_QOS3G_PROFILE_MAX_TRANSFER_DELAY (4000)
#define EXE_QOS3G_PROFILE_MAX_TRAFFIC_HANDLING_PRIO (3)
#define EXE_QOS3G_PROFILE_MAX_SOURCE_STATISTICS_DESCRIPTOR (1)
#define EXE_QOS3G_PROFILE_MAX_SIGNALLING_INDICATION (1)

/* Maximum values supported for 2G qos. Default=min=0 for all parameters. */
#define EXE_QOS2G_PROFILE_DEFAULT (0)
#define EXE_QOS2G_PROFILE_MAX_PRECEDENCE (3)
#define EXE_QOS2G_PROFILE_MAX_DELAY (4)
#define EXE_QOS2G_PROFILE_MAX_RELIABILITY (5)
#define EXE_QOS2G_PROFILE_MAX_PEAK (9)
#define EXE_QOS2G_PROFILE_MAX_MEAN (18)
#define EXE_QOS2G_PROFILE_MAX_MEAN_EXTRA (31)

#define EXE_QOS3G_PROFILE_ACCEPTED_SDU_ERRORS {{0,0},{1,2},{1,3},{1,4},{1,5},{1,6},{1,1},{7,3}}
#define EXE_QOS3G_PROFILE_ACCEPTED_BER_RATIOS {{0,0},{1,2},{1,3},{1,4},{1,5},{1,6},{4,3},{5,2},{5,3},{6,8}}

#define EXE_COPS_MAX_AUTH_CHALLENGE_LEN (512)

#define EXE_MAX_DICT_SIZE (2048)

typedef struct exe_s exe_t;

typedef enum {
    EXE_SERVICE_NONE    = -1,
    EXE_SERVICE_EXE     = 0,    /* Internal */
#ifdef EXE_USE_AUDIO_SERVICE
    EXE_SERVICE_AUDIO,
#endif
#ifdef EXE_USE_CN_SERVICE
    EXE_SERVICE_CN,
#endif
#ifdef EXE_USE_COPS_SERVICE
    EXE_SERVICE_COPS,
#endif
#ifdef EXE_USE_CSPSA_SERVICE
    EXE_SERVICE_CSPSA,
#endif
#ifdef EXE_USE_PSCC_SERVICE
    EXE_SERVICE_PSCC,
#endif
#ifdef EXE_USE_SIM_SERVICE
    EXE_SERVICE_SIM,
#endif
#ifdef EXE_USE_SIMPB_SERVICE
    EXE_SERVICE_SIMPB,
#endif
#ifdef EXE_USE_SMS_SERVICE
    EXE_SERVICE_SMS,
#endif
    EXE_SERVICE_LAST,
} exe_service_t;

typedef enum {
    EXE_A       = 0,
    EXE_CCFC,
    EXE_CCWA_SET,
    EXE_CCWA_QUERY,
    EXE_CFUN_RADIO_ON,
    EXE_CFUN_RADIO_OFF,
    EXE_CFUN_MODEM_SHUTDOWN,
    EXE_CFUN_SET_NW,
    EXE_CFUN_READ,
    EXE_CGACT,
    EXE_CGACT_READ,
    EXE_CGATT_SET_GPRS_STATE,
    EXE_CGATT_GET_GPRS_STATE,
    EXE_CGCMOD,
    EXE_CGDCONT,
    EXE_CGDCONT_READ,
    EXE_CGEQMIN_SET,
    EXE_CGEQMIN_GET,
    EXE_CGEQREQ_SET,
    EXE_CGEQREQ_GET,
    EXE_CGQMIN_SET,
    EXE_CGQMIN_GET,
    EXE_CGQREQ_SET,
    EXE_CGQREQ_GET,
    EXE_CGEQNEG_SET,
    EXE_CGPADDR,
    EXE_CGREG_READ,
    EXE_CGREG_SET,
    EXE_CGSMS_READ,
    EXE_CGSMS_SET,
    EXE_CGSN,
    EXE_CHLD,
    EXE_CIMI,
    EXE_CLCC,
    EXE_CLCK_AL,
    EXE_CLCK_BARRING,
    EXE_CLCK_CS,
    EXE_CLCK_FD,
    EXE_CLCK_EL,
    EXE_CLCK_LL,
    EXE_CLCK_NA,
    EXE_CLCK_NM,
    EXE_CLCK_NS,
    EXE_CLCK_NT,
    EXE_CLCK_P2,
    EXE_CLCK_PC,
    EXE_CLCK_PF,
    EXE_CLCK_PN,
    EXE_CLCK_PP,
    EXE_CLCK_PS,
    EXE_CLCK_PU,
    EXE_CLCK_SC,
    EXE_CLIP_READ,
    EXE_CLIR,
    EXE_CLIR_READ,
    EXE_CMGD_DELETE,
    EXE_CMGD_DELETE_ALL,
    EXE_CMGL,
    EXE_CMGR,
    EXE_CMGS,
    EXE_CMGW,
    EXE_CPMS,
    EXE_CMMS_READ,
    EXE_CMMS_SET,
    EXE_CMSS,
    EXE_CMUT_READ,
    EXE_CMUT_SET,
    EXE_CNAP_READ,
    EXE_COLR_DO,
    EXE_CNMA,
    EXE_CNMI,
    EXE_COPS_AUTOMATIC_REGISTER,
    EXE_COPS_MANUAL_REGISTER,
    EXE_COPS_MANUAL_REGISTER_AUTO_FALLBACK,
    EXE_COPS_DEREGISTER,
    EXE_COPS_GET_SETTINGS,
    EXE_COPS_SEARCH_NETWORKS,
    EXE_CSCA_SET_SMSC,
    EXE_CSCA_GET_SMSC,
    EXE_CRES,
    EXE_CRES_TEST,
    EXE_CSAS,
    EXE_CSAS_TEST,
    EXE_CSCB_SET,
    EXE_CSCB_READ,
    EXE_CSMS_SET,
    EXE_CSMS_READ,
    EXE_CUSD_REQUEST_USSD,
    EXE_CUSD_CANCEL_USSD,
    EXE_CPIN_GET_SIM_STATE,
    EXE_CPOL_SET,
    EXE_CPOL_READ,
    EXE_CPOL_TEST,
    EXE_CPIN_GET_PIN_INFO,
    EXE_CPIN_VERIFY_SIM_PIN,
    EXE_CPIN_VERIFY_SIM_PIN2,
    EXE_CPIN_VERIFY_SIM_PUK,
    EXE_CPIN_VERIFY_SIM_PUK2,
    EXE_CPWD_BARRING,
    EXE_CPWD_NONE,
    EXE_CPWD_P2,
    EXE_CPWD_SC,
    EXE_CREG_READ,
    EXE_CREG_SET,
    EXE_CRSM_SIM_IO_GET_RESPONSE,
    EXE_CRSM_SIM_IO_READ_BINARY,
    EXE_CRSM_SIM_IO_READ_RECORD,
    EXE_CRSM_SIM_IO_UPDATE_BINARY,
    EXE_CRSM_CHECK_SERVICE_SIM_IO_READ_BINARY,
    EXE_CRSM_SIM_IO_UPDATE_RECORD,
    EXE_CSIM,
    EXE_CRSM_SIM_IO_STATUS,
    EXE_CUAD_APPS,
    EXE_CCHC,
    EXE_CCHO,
    EXE_CGLA,
    EXE_CSQ,
    EXE_CSSN,
    EXE_DIAL_UP,
    EXE_PS_DIAL_UP,
    EXE_ECRAT_SET,
    EXE_ECRAT_READ,
    EXE_ENNIR_SET,
    EXE_EFSIM,
    EXE_EHSTACT,
    EXE_EIAAUW,
    EXE_ELIN_READ,
    EXE_ELIN_SET,
    EXE_ENAP_CONNECT,
    EXE_ENAP_DISCONNECT,
    EXE_ENAP_READ,
    EXE_EPINR,
    EXE_EPINR_FULL,
    EXE_EPPSD,
    EXE_EPWRRED_SET,
    EXE_EREG_READ,
    EXE_EREG_SET,
    EXE_ESHLVOCR,
    EXE_ESVN_READ,
    EXE_SIM_RESTART,
    EXE_ESMSFULL,
    EXE_ESMSFULL_READ,
    EXE_ESIMSR_GET_STATE,
    EXE_ETTY_GET,
    EXE_ETTY_SET,
    EXE_EUPLINK,
    EXE_HOOK,
    EXE_HOOK_ALL,
    EXE_SET_DTMF_DURATION,
    EXE_STKE,
    EXE_GET_DTMF_DURATION,
    EXE_VTS,
    EXE_STKC_PC_CONTROL,
    EXE_GET_TERMPROFILE,
    EXE_GET_CHALLENGE_DATA,
    EXE_COPS_AUTHENTICATE,
    EXE_COPS_DEAUTHENTICATE,
    EXE_COPS_SET_IMEI,
    EXE_COPS_SET_SIMLOCK,
    EXE_COPS_WRITE_SL_DATA,
    EXE_COPS_READ_SL_DATA,
    EXE_COPS_UNLOCK_SIM_LOCK,
    EXE_PROACTIVE_COMMAND_RESPONSE,
    EXE_CNUM,
    EXE_CEER,
    EXE_CMER,
    EXE_CSPSA_WRITE_DATA,
    EXE_CSPSA_READ_DATA,
    EXE_PSCC_EVENT_DISCONNECTED,
    EXE_NEIGH_CELLINFO_2G_GET,
    EXE_NEIGH_CELLINFO_3G_GET,
    EXE_SERVING_CELLINFO_2G_GET,
    EXE_SERVING_CELLINFO_3G_GET,
    EXE_EMPAGE_SET,
    EXE_COPS_AUTH_ENG_MODE,
    EXE_COPS_DEAUTH_ENG_MODE,
    EXE_COPS_LOCK_BOOTPARTITION,
    EXE_COPS_INIT_ANTIROLLBACK,
    EXE_COPS_WRITE_D_AND_T,
    EXE_EMSR,
    EXE_MODEM_ESLEEP,
    EXE_RADIOVER_READ,
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <inc_exe_extended.h>
#endif
    EXE_LAST
} exe_request_id_t;

typedef enum {
    EXE_PENDING,        /* The request is still executing, the handler must not finish yet */
    EXE_SUCCESS,        /* The request completed successfully, the handler can report a positive result */
    EXE_FAILURE,        /* The request could not be successfully completed, the handler can report a negative result */
    EXE_NOT_SUPPORTED,  /*  The request, or the parameters to the request, is not supported */
} exe_request_result_t;

typedef enum {
    EXE_EVENT_CALL_RESPONSE,        /* Named differently as it's not a specific unsolicited. */
    EXE_UNSOLICITED_CBM,
    EXE_UNSOLICITED_CCWA,
    EXE_UNSOLICITED_CDS,
    EXE_UNSOLICITED_CDSI,
    EXE_UNSOLICITED_CGEV,
    EXE_UNSOLICITED_CIEV_SIGNAL,
    EXE_UNSOLICITED_CIEV_STORAGE,
    EXE_UNSOLICITED_CLIP,
    EXE_UNSOLICITED_CNAP,
    EXE_UNSOLICITED_CMT,
    EXE_UNSOLICITED_CMTI,
    EXE_UNSOLICITED_CR,
    EXE_UNSOLICITED_CREG,
    EXE_UNSOLICITED_CGREG,
    EXE_UNSOLICITED_CSSI,
    EXE_UNSOLICITED_CSSU,
    EXE_UNSOLICITED_CUSD,
    EXE_UNSOLICITED_CUUS1I,
    EXE_UNSOLICITED_CUUS1U,
    EXE_UNSOLICITED_ECAV,
    EXE_UNSOLICITED_ECME,
    EXE_UNSOLICITED_ECRAT,
    EXE_UNSOLICITED_ENNIR,
    EXE_UNSOLICITED_ELAT,
    EXE_UNSOLICITED_EPEV,
    EXE_UNSOLICITED_EREG,
    EXE_UNSOLICITED_ESIMSR,
    EXE_UNSOLICITED_ESIMRF,
    EXE_UNSOLICITED_ETZV,
    EXE_UNSOLICITED_RING,
    EXE_UNSOLICITED_RSSI,
    EXE_UNSOLICITED_SAT_EVENT_NOTIFY,
    EXE_UNSOLICITED_SAT_PROACTIVE_COMMAND,
    EXE_UNSOLICITED_SAT_SESSION_COMPLETED,
    EXE_UNSOLICITED_ERFSTATE,
    EXE_UNSOLICITED_ATD,
    EXE_UNSOLICITED_EEMPAGE,
    EXE_UNSOLICITED_SIMPB_FILE_IDS,
#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <inc_atc_extended_exe_event_t.h>
#endif
} exe_event_t;

typedef struct {
    union {
        struct {
            int8_t rxlev;             /**< RXLEV - Received signal Level.  INTEGER (0..63). */
            uint8_t timing_advance;    /**< Network Timing Advance. INTEGER (0..255).*/
            uint8_t bsic;              /**< BSIC  - Base Station Identity Code.  INTEGER (0..63). */
            uint16_t mcc;               /**< PLMN-MCC value Mobile Country Code.  INTEGER (0..999). Invalid Value -1*/
            uint16_t mnc;               /**< PLMN-MNC value Mobile Network Code.  INTEGER (0..999). Invalid Value -1*/
            uint16_t lac;               /**< Location Area Code of the serving cell.  INTEGER (0..65535). */
            uint16_t cid;               /**< GSM Cell number identifier of the serving cell. INTEGER (0..65535).*/
            uint16_t arfcn;             /**< ARFCN - Absolute Radio Frequency Channel Number.  INTEGER (0..1023). */
        } gsm_info;

        struct {
            uint8_t cpich_ecno;       /**< Received energy per chip/power density.INTEGER  (0..63).*/
            uint8_t cpich_pathloss;   /**< Path Loss.INTEGER (46..173).*/
            int8_t cpich_rscp;       /**< Received Signal Code Power.INTEGER  (-4 to 127 ). */
            uint16_t dl_uarfcn;
            uint16_t psc;              /**< Primary Scrambling Code.  INTEGER (0..511). */
            uint16_t mcc;              /**< PLMN-MCC value Mobile Country Code.  INTEGER (0..999). Invalid Value -1*/
            uint16_t mnc;              /**< PLMN-MNC value Mobile Network Code.  INTEGER (0..999). Invalid Value -1*/
            uint32_t ucid;             /**< Cell number identifier of the serving cell, i.e. UC-ID Identity.
                                                                            INTEGER (0..268435455).	UC-ID is composed of RNC-ID and C-ID. */
        } umts_info;
    };
} exe_cell_information_response_t;

typedef struct {
    union {
        struct {
            uint8_t timing_advance;
            uint8_t bsic;
            uint16_t arfcn;
            int16_t rssi;
        } gsm_info;

        struct {
            uint8_t cpich_ecno;
            uint8_t cpich_pathloss;
            uint16_t uarfcn;
            uint32_t psc;
            int16_t cpich_rscp;
        } umts_info;
    };
} exe_serving_cell_information_response_t;

/**
 * \enum exe_neighbouring_rat_type
 * \brief RAT types associated with Neighbour Cell Information
 *
 * \n This enum defines RAT types associated with Neighbour Cell Information.
 */
typedef enum {
    EXE_NEIGHBOURING_RAT_TYPE_UNKNOWN = 0x00, /**< RAT type unknown */
    EXE_NEIGHBOURING_RAT_TYPE_2G      = 0x01, /**< RAT type 2G (GSM) */
    EXE_NEIGHBOURING_RAT_TYPE_3G      = 0x02  /**< RAT type 3G (UMTS) */
} exe_neighbouring_rat_type_t;

/**
 * \struct exe_2g_3g_neigh_cell_response_t
 * \brief Information related to neighbour cells
 *
 * \n This structure provides cell information for all neighbour cells.
 */
typedef struct {
    exe_cell_information_response_t *neigh_cell_info_p;
    exe_cell_information_response_t  serving_cell_info;
    exe_neighbouring_rat_type_t      rat_info;
    uint8_t                          num_of_cells;
} exe_2g_3g_neigh_cell_response_t;

/**
 * \struct exe_2g_3g_serving_cell_response_t
 * \brief Information related to neighbour cells
 *
 * \n This structure provides cell information for all neighbour cells.
 */
typedef struct {
    exe_serving_cell_information_response_t  serving_cell_info;
    exe_neighbouring_rat_type_t      rat_info;
} exe_2g_3g_serving_cell_response_t;

typedef struct {
    uint8_t imeisv;
} exe_esvn_response_t;

typedef struct {
    char imei[EXE_IMEI_LENGTH];
} exe_cgsn_response_t;

typedef struct {
    char ceer_string[EXE_CEER_MAX_CHAR + 1];
} exe_ceer_request_t;

/* AT+CREG/AT+CGREG/AT*EREG <stat> values */
typedef enum {
    EXE_NET_REG_STAT_NOT_REGISTERED = 0,
    EXE_NET_REG_STAT_REGISTERED_HOME,
    EXE_NET_REG_STAT_NOT_REGISTERED_SEARCH,
    EXE_NET_REG_STAT_REGISTRATION_DENIED,
    EXE_NET_REG_STAT_UNKNOWN,
    EXE_NET_REG_STAT_REGISTERED_ROAMING,
    EXE_NET_REG_STAT_NOT_REGISTERED_EMERGENCY_CALLS_ALLOWED        = 0x0A,
    EXE_NET_REG_STAT_NOT_REGISTERED_SEARCH_EMERGENCY_CALLS_ALLOWED = 0x0C,
    EXE_NET_REG_STAT_REGISTRATION_DENIED_EMERGENCY_CALLS_ALLOWED   = 0x0D,
    EXE_NET_REG_STAT_UNKNOWN_EMERGENCY_CALLS_ALLOWED               = 0x0E,
} exe_net_reg_stat_t;

/* AcT values */
typedef enum {
    EXE_ACT_GSM                 = 0,
    EXE_ACT_GSM_COMPACT         = 1, /* Not Supported */
    EXE_ACT_UTRAN               = 2,
    EXE_ACT_GSM_EGPRS           = 3,
    EXE_ACT_UTRAN_HDSPA         = 4,
    EXE_ACT_UTRAN_HSUPA         = 5,
    EXE_ACT_UTRAN_HSUPA_HSDPA   = 6,
    EXE_ACT_EPS                 = 7, /* Not supported */
    EXE_ACT_GSM_GPRS_DTM        = 16, /* Not 3GPP, out of spec */
    EXE_ACT_GSM_EDGE_DTM        = 19, /* Not 3GPP, out of spec */
    EXE_ACT_LAST
} exe_act_t;

/* Limited AcT values for *EREG/+COPS */
typedef enum {
    EXE_BASIC_ACT_GSM = 0,
    EXE_BASIC_ACT_UTRAN = 2,
    EXE_BASIC_ACT_UNKNOWN = 255
} exe_basic_act_t;

/* AT+COPS <format> values */
typedef enum {
    EXE_COPS_FORMAT_LONG = 0,
    EXE_COPS_FORMAT_SHORT,
    EXE_COPS_FORMAT_NUMERIC,
} exe_cops_operator_format_t;

/* Pin/Puk types for Pin/Puk info */
typedef enum {
    EXE_PIN_1 = 1,
    EXE_PIN_2,
    EXE_PUK_1,
    EXE_PUK_2,
} exe_sim_pin_puk_t;

typedef enum {
    EXE_RAT_NAME_UNKNOWN = 0,
    EXE_RAT_NAME_GSM     = 1,
    EXE_RAT_NAME_UMTS    = 2,
    EXE_RAT_NAME_EPS     = 4
} exe_rat_name_t;

typedef enum {
    EXE_TX_BACK_OFF_EVENT_NO_EVENT                        = 0,
    EXE_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_CLOSED     = 1,
    EXE_TX_BACK_OFF_EVENT_FOLD_SLIDE_MECHANISM_OPEN       = 2,
    EXE_TX_BACK_OFF_EVENT_DVBH_INACTIVE                   = 3,
    EXE_TX_BACK_OFF_EVENT_DVBH_ACTIVE                     = 4,
    EXE_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_FAR            = 5,
    EXE_TX_BACK_OFF_EVENT_PROXIMITY_SENSOR_NEAR           = 6,
    EXE_TX_BACK_OFF_EVENT_GPS_INACTIVE                    = 7,
    EXE_TX_BACK_OFF_EVENT_GPS_ACTIVE                      = 8,
    EXE_TX_BACK_OFF_EVENT_ANTENNA_INACTIVE                = 9,
    EXE_TX_BACK_OFF_EVENT_ANTENNA_ACTIVE                  = 10,
    EXE_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_INACTIVE     = 11,
    EXE_TX_BACK_OFF_EVENT_THERMAL_MANAGEMENT_ACTIVE       = 12,
    EXE_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_INACTIVE           = 13,
    EXE_TX_BACK_OFF_EVENT_WLAN_HOTSPOT_ACTIVE             = 14,
    EXE_TX_BACK_OFF_EVENT_AUTOMATIC_THERMAL_CONTROL_INACTIVE = 15,
    EXE_TX_BACK_OFF_EVENT_AUTOMATIC_THERMAL_CONTROL_ACTIVE   = 16
} exe_tx_back_off_event_t;


typedef enum {
    EXE_NETWORK_NAME_DCS_GSM7 = 0,
    EXE_NETWORK_NAME_DCS_UCS2 = 1,
} exe_network_name_dcs_t;


typedef struct {
    int n;
} exe_chld_t;

typedef struct {
    int n;
} exe_clir_t;

typedef struct {
    int n;
} exe_modem_sleep_t;

typedef struct {
    int      status;       /* Status of the SIM service. */
    int      sim_type;     /* Fetched SIM Type. */
} exe_get_sim_type_response_t;

typedef struct {
    int n;
} exe_ecrat_set_t;

typedef struct {
    int n;
} exe_ennir_set_t;


typedef struct {
    exe_rat_name_t rat;
} exe_ecrat_read_t;

typedef struct {
    exe_rat_name_t rat;
} exe_ecrat_ind_t;

typedef struct {
    char mcc_mnc[EXE_OPERATOR_NUMERIC_STRING_MAX_LENGTH +1];
    char text_long[EXE_COPS_OPERATOR_MAX_LONG_LEN +1];
    char text_short[EXE_COPS_OPERATOR_MAX_SHORT_LEN +1];
    uint8_t             name_char_long_length;
    uint8_t             name_char_short_length;
    exe_network_name_dcs_t    long_dcs;
    exe_network_name_dcs_t    short_dcs;
} exe_ennir_ind_t;


typedef struct {
    int n;
} exe_etty_t;

typedef struct {
    int cid;
    int size;
} exe_euplink_t;

typedef struct {
    exe_tx_back_off_event_t event;
} exe_epwrred_t;

typedef struct {
    int n;
    int m;
} exe_clir_read_t;

/* CMS error codes definition */
typedef enum {
    /* GSM 04.11 annex E-2 values */
    EXE_CMS_UNASSIGNED_NUMBER                                        = 1,
    EXE_CMS_OPERATOR_DETERMINED_BARRING                              = 8,
    EXE_CMS_CALL_BARRED                                              = 10,
    EXE_CMS_SHORT_MESSAGE_TRANSFER_REJECTED                          = 21,
    EXE_CMS_DESTINATION_OUT_OF_SERVICE                               = 27,
    EXE_CMS_UNIDENTIFIED_SUBSCRIBER                                  = 28,
    EXE_CMS_FACILITY_REJECTED                                        = 29,
    EXE_CMS_UNKNOWN_SUBSCRIBER                                       = 30,
    EXE_CMS_NETWORK_OUT_OF_ORDER                                     = 38,
    EXE_CMS_TEMPORARY_FAILURE                                        = 41,
    EXE_CMS_CONGESTION                                               = 42,
    EXE_CMS_RESOURCES_UNAVAILABLE_UNSPECIFIED                        = 47,
    EXE_CMS_REQUESTED_FACILITY_NOT_SUBSCRIBED                        = 50,
    EXE_CMS_REQUESTED_FACILITY_NOT_IMPLEMENTED                       = 69,
    EXE_CMS_INVALID_SHORT_MESSAGE_TRANSFER_REFERENCE_VALUE           = 81,
    EXE_CMS_INVALID_MESSAGE_UNSPECIFIED                              = 95,
    EXE_CMS_INVALID_MANDATORY_INFORMATION                            = 96,
    EXE_CMS_MESSAGE_TYPE_NON_EXISTENT_OR_NOT_IMPLEMENTED             = 97,
    EXE_CMS_MESSAGE_NOT_COMPATIBLE_WITH_SHORT_MESSAGE_PROTOCOL_STATE = 98,
    EXE_CMS_INFORMATION_ELEMENT_NON_EXISTENT_OR_NOT_IMPLEMENTED      = 99,
    EXE_CMS_PROTOCOL_ERROR_UNSPECIFIED                               = 111,
    EXE_CMS_INTERWORKING_UNSPECIFIED                                 = 127,

    /* GSM 03.40 values */
    EXE_CMS_TELEMATIC_INTERWORKING_NOT_SUPPORTED  = 0x80,
    EXE_CMS_SHORT_MESSAGE_TYPE_0_NOT_SUPPORTED    = 0x81,
    EXE_CMS_CANNOT_REPLACE_SHORT_MESSAGE          = 0x82,
    EXE_CMS_UNSPECIFIED_TP_PID_ERROR              = 0x8F,
    EXE_CMS_DATA_CODING_SCHEME_NOT_SUPPORTED      = 0x90,
    EXE_CMS_MESSAGE_CLASS_NOT_SUPPORTED           = 0x91,
    EXE_CMS_UNSPECIFIED_TP_DCS_ERROR              = 0x9F,
    EXE_CMS_COMMAND_CANNOT_BE_ACTIONED            = 0xA0,
    EXE_CMS_COMMAND_UNSUPPORTED                   = 0xA1,
    EXE_CMS_UNSPECIFIED_TP_COMMAND_ERROR          = 0xAF,
    EXE_CMS_TPDU_NOT_SUPPORTED                    = 0xB0,
    EXE_CMS_SC_BUSY                               = 0xC0,
    EXE_CMS_NO_SC_SUBSCRIPTION                    = 0xC1,
    EXE_CMS_SC_SYSTEM_FAILURE                     = 0xC2,
    EXE_CMS_INVALID_SME_ADDRESS                   = 0xC3,
    EXE_CMS_DESTINATION_SME_BARRED                = 0xC4,
    EXE_CMS_SM_REJECTED_DUPLICATE_SM              = 0xC5,

    EXE_CMS_SIM_SMS_STORAGE_FULL                  = 0xD0,
    EXE_CMS_NO_SMS_STORAGE_CAPABILITY_IN_SIM      = 0xD1,
    EXE_CMS_ERROR_IN_MS                           = 0xD2,
    EXE_CMS_MEMORY_CAPACITY_EXCEEDED              = 0xD3,

    EXE_CMS_UNSPECIFIED_ERROR_CAUSE               = 0xFF,

    /* GSM 07.05 values */
    EXE_CMS_ME_FAILURE                            = 300,
    EXE_CMS_SMS_SERVICE_OF_ME_RESERVED            = 301,
    EXE_CMS_OPERATION_NOT_ALLOWED                 = 302,
    EXE_CMS_OPERATION_NOT_SUPPORTED               = 303,
    EXE_CMS_INVALID_PDU_MODE_PARAMETER            = 304,
    EXE_CMS_INVALID_TEXT_MODE_PARAMETER           = 305,
    EXE_CMS_SIM_NOT_INSERTED                      = 310,
    EXE_CMS_SIM_PIN_REQUIRED                      = 311,
    EXE_CMS_PH_SIM_PIN_REQUIRED                   = 312,
    EXE_CMS_SIM_FAILURE                           = 313,
    EXE_CMS_SIM_BUSY                              = 314,
    EXE_CMS_SIM_WRONG                             = 315,
    EXE_CMS_SIM_PUK_REQUIRED                      = 316,
    EXE_CMS_SIM_PIN2_REQUIRED                     = 317,
    EXE_CMS_SIM_PUK2_REQUIRED                     = 318,
    EXE_CMS_MEMORY_FAILURE                        = 320,
    EXE_CMS_INVALID_MEMORY_INDEX                  = 321,
    EXE_CMS_MEMORY_FULL                           = 322,
    EXE_CMS_SMSC_ADDRESS_UNKNOWN                  = 330,
    EXE_CMS_NO_NETWORK_SERVICE                    = 331,
    EXE_CMS_NETWORK_TIMEOUT                       = 332,
    EXE_CMS_NO_PLUS_CNMA_ACKNOWLEDGEMENT_EXPECTED = 340,
    EXE_CMS_UNKNOWN_ERROR                         = 500,

    /* Manufacturer defined values in range 512 ... */
    EXE_CMS_FIXED_DIALLING_NUMBER_CHECK_FAILURE   = 512,
    EXE_CMS_OK                                    = 999
} exe_cms_error_t;

/* CMEE error codes definition */
typedef enum {
    CMEE_PHONE_FAILURE                      =   0,
    CMEE_NO_CONNECTION_TO_PHONE             =   1,
    CMEE_PHONE_ADAPTOR_LINK_RESERVED        =   2,
    CMEE_OPERATION_NOT_ALLOWED              =   3,
    CMEE_OPERATION_NOT_SUPPORTED            =   4,
    CMEE_PH_SIM_PIN_REQUIRED                =   5,
    CMEE_PH_FSIM_PIN_REQUIRED               =   6,
    CMEE_PH_FSIM_PUK_REQUIRED               =   7,
    CMEE_SIM_NOT_INSERTED                   =  10,
    CMEE_SIM_PIN_REQUIRED                   =  11,
    CMEE_SIM_PUK_REQUIRED                   =  12,
    CMEE_SIM_FAILURE                        =  13,
    CMEE_SIM_BUSY                           =  14,
    CMEE_SIM_WRONG                          =  15,
    CMEE_INCORRECT_PASSWORD                 =  16,
    CMEE_SIM_PIN2_REQUIRED                  =  17,
    CMEE_SIM_PUK2_REQUIRED                  =  18,
    CMEE_MEMORY_FULL                        =  20,
    CMEE_INVALID_INDEX                      =  21,
    CMEE_NOT_FOUND                          =  22,
    CMEE_MEMORY_FAILURE                     =  23,
    CMEE_TEXT_STRING_TOO_LONG               =  24,
    CMEE_INVALID_CHARACTER_IN_TEXT_STRING   =  25,
    CMEE_DIAL_STRING_TOO_LONG               =  26,
    CMEE_INVALID_CHARACTER_IN_DIAL_STRING   =  27,
    CMEE_NO_NETWORK_SERVICE                 =  30,
    CMEE_NETWORK_TIMEOUT                    =  31,
    CMEE_NETWORK_NOT_ALLOWED_EMERGENCY_ONLY =  32,
    CMEE_PH_NET_PIN_REQUIRED                =  40,
    CMEE_PH_NET_PUK_REQUIRED                =  41, /* Not supported */
    CMEE_PH_NETSUB_PIN_REQUIRED             =  42,
    CMEE_PH_NETSUB_PUK_REQUIRED             =  43,
    CMEE_PH_SP_PIN_REQUIRED                 =  44,
    CMEE_PH_SP_PUK_REQUIRED                 =  45,
    CMEE_PH_CORP_PIN_REQUIRED               =  46,
    CMEE_PH_CORP_PUK_REQUIRED               =  47,
    CMEE_HIDDEN_KEY_REQUIRED                =  48,
    CMEE_EAP_METHOD_NOT_SUPPORTED           =  49,
    CMEE_INCORRECT_PARAMETERS               =  50,
    CMEE_UNKNOWN                            = 100,
    /* Errors related to a failure to perform an Attach */
    CMEE_ILLEGAL_MS                         = 103, /* Illegal MS (#3) */
    CMEE_ILLEGAL_ME                         = 106, /* Illegal ME (#6) */
    CMEE_GPRS_SERVICE_NOT_ALLOWED           = 107, /* GPRS services not allowed (#7) */
    CMEE_PLMN_NOT_ALLOWED                   = 111, /* PLMN not allowed (#11) */
    CMEE_LA_NOT_ALLOWED                     = 112, /* Location area not allowed (#12) */
    CMEE_ROAMING_NOT_ALLOWED                = 113, /* Roaming not allowed in this location area (#13) */

    /* Errors related to a failure to Activate a Context */
    CMEE_SERVICE_OPTION_NOT_SUPPORTED       = 132, /* service option not supported (#32) */
    CMEE_SERVICE_OPTION_NOT_SUBSCRIBED      = 133, /* requested service option not subscribed (#33) */
    CMEE_SERVICE_OPTION_TEMP_OUT_OF_ORDER   = 134, /* service option temporarily out of order (#34) */
    CMEE_PDP_AUTH_FAILURE                   = 149, /* PDP authentication failure */

    /* Other GPRS errors */
    CMEE_INVALID_MOBILE_CLASS               = 150, /* invalid mobile class */
    CMEE_UNSPEC_GPRS_ERROR                  = 148, /* unspecified GPRS error */

    /* VBS / VGCS and eMLPP -related errors */
    CMEE_VBS_VGCS_NOT_SUPPORTED             = 151, /* VBS/VGCS not supported by the network */
    CMEE_NO_SERIVCE_SUBSCRIPTION            = 152, /* No service subscription on SIM */
    CMEE_NO_SUBSCRIPTION_FOR_GROUP_ID       = 153, /* No subscription for group ID */
    CMEE_GROUP_ID_NOT_ACTIVATED             = 154, /* Group Id not activated on SIM */
    CMEE_NO_MATCHING_NOTIFICATION           = 155, /* No matching notification */
    CMEE_VBS_VGCS_CALL_ALREADY_PRESENT      = 156, /* VBS/VGCS call already present */
    CMEE_CONGESTION                         = 157, /* Congestion */
    CMEE_NETWORK_FAILURE                    = 158, /* Network failure */
    CMEE_UPLINK_BUSY                        = 159, /* Uplink busy */
    CMEE_NO_ACCESS_RIGHTS_FOR_SIM_FILE      = 160, /* No access rights for SIM file */
    CMEE_NO_SUBSCRIPTION_FOR_PRIO           = 161, /* No subscription for priority */
    CMEE_OP_NOT_APPLICABLE                  = 162, /* operation not applicable or not possible */

    CMEE_PH_SIMLOCK_PIN_REQUIRED            = 200,

    CMEE_MODIFIED_TO_DIAL                   = 210,
    CMEE_MODIFIED_TO_SS                     = 211,
    CMEE_MODIFIED_TO_USSD                   = 212,
    CMEE_FDN_BLOCKED                        = 213,
    CMEE_SIM_BLOCKED                        = 262,


    CMEE_OK                                 = 255
} exe_cmee_error_t;

typedef enum {
    MSG_STORAGE_ME = 0,
    MSG_STORAGE_SM = 1,
    MSG_STORAGE_MT = 2,
    MSG_STORAGE_UNKNOWN = 0xFF
} msg_storage_t;

typedef struct  MSG_StorageInfo_s {
    msg_storage_t memory;
    uint16_t used_space;
    uint16_t total_space;
} exe_msg_storage_info_t;

typedef struct {
    exe_msg_storage_info_t mem1;
    exe_msg_storage_info_t mem2;
    exe_msg_storage_info_t mem3;
} exe_cpms_msg_storage_area_t;

typedef uint16_t exe_msg_position_t;
#define EXE_MSG_POSITION_INVALID (0)

typedef struct {
    msg_storage_t      storage;
    exe_msg_position_t position;
} exe_msg_slot_t;

typedef struct {
    unsigned char *pdu_data_p;
    int length;
    int mr;
    exe_cms_error_t error;
} exe_cmgs_t;

typedef struct {
    unsigned char *pdu_data_p;
    int length;
    int stat;
    exe_msg_slot_t slot;
    exe_cms_error_t error;
} exe_cmgw_t;

typedef struct {
    char *buf_p;
    unsigned len;
} exe_apdu_data_t;

typedef enum {
    MSG_STATUS_READ          = 0x01,
    MSG_STATUS_UNREAD        = 0x03,
    MSG_STATUS_SENT          = 0x05,
    MSG_STATUS_UNSENT        = 0x07,
    MSG_STATUS_UNKNOWN       = 0x00,
    MSG_STATUS_ANY_STATUS    = 0x99
} exe_msg_status_t;

typedef enum {
    MSG_FIRST,
    MSG_NEXT,
    MSG_PREVIOUS,
    MSG_LAST
} exe_msg_browse_option_t;

typedef struct {
    exe_msg_status_t     status;
    exe_msg_slot_t       slot;
    exe_msg_browse_option_t   browse_option;
} exe_msg_search_info_t;

typedef struct {
    msg_storage_t storage;
    uint16_t index;
    exe_cms_error_t error;
} exe_cmgd_del_t;

typedef struct {
    exe_cms_error_t error;
} exe_cmgd_response_t;

typedef struct {
    msg_storage_t storage;

    exe_msg_status_t status;
    exe_cms_error_t error;
} exe_cmgd_del_all_t;

typedef struct {
    bool sms;
    bool cbs;
} exe_message_subscribe_t;

typedef struct {
    char *dcss;
    char *mids;
} exe_cscb_param_t;

typedef enum {
    EXE_NETWORK_WCDMA_GSM_PREFERRED_WCDMA,
    EXE_NETWORK_GSM_ONLY,
    EXE_NETWORK_WCDMA_ONLY
} exe_network_t;

typedef struct {
    exe_network_t network;
} exe_cfun_request_t;

typedef struct {
    int value;
} exe_cfun_response_t;

typedef struct {
    int value;
} exe_cmut_response_t;

#define EXE_MAX_NUMBER_CALLS 7          /* Can have 5 calls in a conference + 1 held + 1 incoming */
#define EXE_MAX_LENGTH_PHONE_NUMBER 40   /* TODO Should be from a define in CN. */

typedef enum {
    EXE_CLI_VALIDITY_VALID              = 0,
    EXE_CLI_VALIDITY_WITHHELD           = 1,
    EXE_CLI_VALIDITY_NA_INTERWORKING    = 2,
    EXE_CLI_VALIDITY_NA_PAYPHONE        = 3,
    EXE_CLI_VALIDITY_NA_OTHER           = 4,
} exe_cli_validity_t;

typedef enum {
    EXE_CLCC_MODE_VOICE             = 0,
    EXE_CLCC_MODE_DATA              = 1,
    EXE_CLCC_MODE_FAX               = 2,
    EXE_CLCC_MODE_VO_FO_BY_DA_VOICE = 3,
    EXE_CLCC_MODE_ALT_VO_DA_VOICE   = 4,
    EXE_CLCC_MODE_ALT_VO_FA_VOICE   = 5,
    EXE_CLCC_MODE_VO_FO_DA_DATA     = 6,
    EXE_CLCC_MODE_ALT_VO_DA_DATA    = 7,
    EXE_CLCC_MODE_ALT_VO_FA_FAX     = 8,
    EXE_CLCC_MODE_UNKNOWN           = 9
} exe_clcc_mode_t;

typedef struct {
    int     id;
    int     direction;
    int     state;
    int     mode;
    int     multipart;
    char    number[EXE_MAX_LENGTH_PHONE_NUMBER + 1];
    int     type;
    int     alpha;
    int     priority;
    exe_cli_validity_t cli_validity;
} exe_clcc_call_info_t;

typedef struct {
    int                     number_of_calls;
    exe_clcc_call_info_t    call_info[EXE_MAX_NUMBER_CALLS];
} exe_clcc_response_t;

typedef enum {
    EXE_CLCK_MODE_UNLOCK,
    EXE_CLCK_MODE_LOCK,
    EXE_CLCK_MODE_QUERY_STATUS
} exe_clck_mode_t;

typedef enum {
    EXE_STKC_PC_DISABLE = 0x0,
    EXE_STKC_PC_ENABLE,
} exe_stkc_pc_request_mode_t;

typedef struct {
    exe_stkc_pc_request_mode_t mode;
} exe_stkc_pc_request_data_t;

typedef enum {
    EXE_CLCK_STATUS_NOT_ACTIVE,
    EXE_CLCK_STATUS_ACTIVE,
    EXE_CLCK_STATUS_NOT_AVAILABLE
} exe_clck_status_t;

typedef struct {
    exe_net_reg_stat_t stat;
    int32_t lac;
    int32_t cid;
    exe_basic_act_t act;
    uint8_t cs_attached;
} exe_creg_reg_info_t;

typedef struct {
    exe_net_reg_stat_t stat;
    int32_t lac;
    int32_t cid;
    exe_basic_act_t act;
    uint8_t cs_attached;
} exe_ereg_reg_info_t;

typedef struct {
    int mode;
    int requested_classes;
} exe_call_waiting_request_t;

typedef struct {
    uint32_t  enabled;
    uint32_t  requested_classes;
    uint32_t  classes_status;
    int       mode;
} exe_call_waiting_response_t;

typedef struct {
    exe_net_reg_stat_t stat;
    int32_t lac;
    int32_t ci;
    exe_act_t act;
} exe_cgreg_reg_info_t;

typedef struct {
    exe_clck_mode_t mode;
    char *passwd_p;
    char *facility_p;
    exe_cmee_error_t status;
    exe_clck_status_t lock_status;
    int class;
} exe_clck_t;

typedef struct {
    uint8_t rsa_challenge_data[EXE_COPS_MAX_AUTH_CHALLENGE_LEN];
    size_t  challenge_len;
} exe_cops_challenge_t;

/* Enum will probably be extended with more input-types */
typedef enum {
    COPS_D_AND_T_INPUT_TYPE_1 = 1,
} exe_cops_d_and_t_input_types_t;

/*
 * This struct is for sending data for the d&t hashkeys,
 * and should match in types most of cops_write_secprofile_arg_t.
 */
typedef struct {
    uint32_t version;
    uint32_t flags;
    size_t hashlist_len;
    uint8_t *hashlist;
} exe_cops_d_and_t_data_t;

typedef enum {
    EXE_COPS_AUTH_TYPE_RSA = 0,
    EXE_COPS_AUTH_TYPE_SIMLOCK,
} exe_cops_auth_types_t;

typedef enum {
    COPS_OPERATION_TYPE_ABORT = 0,
    COPS_OPERATION_TYPE_INIT,
    COPS_OPERATION_TYPE_COMPLETE,
} exe_cops_operation_types_t;

typedef struct {
    exe_cops_auth_types_t auth_type;
    uint8_t *rsa_challenge_response_p;
    uint32_t rsa_size;
    char *nl_key_p;
    char *nsl_key_p;
    char *spl_key_p;
    char *cl_key_p;
    char *esl_key_p;
} exe_cops_auth_data_t;

typedef struct {
    uint8_t imei[EXE_COPS_IMEI_MAX_LEN];
} exe_cops_imei_t;

typedef struct {
    char *nl_key_p;
    char *nsl_key_p;
    char *spl_key_p;
    char *cl_key_p;
    char *esl_key_p;
} exe_cops_simlock_keys_t;

typedef enum {
    EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_QUERY_STATUS = 0,
    EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_UNLOCK,
    EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_LOCK,
    EXE_COPS_FACILITY_PERSONALIZATION_OPERATION_NO_OP
} exe_cops_facility_personalization_operation_t;

typedef enum {
    EXE_COPS_SIM_LOCK_TYPE_NETWORK_LOCK = 0,
    EXE_COPS_SIM_LOCK_TYPE_NETWORK_SUBSET_LOCK,
    EXE_COPS_SIM_LOCK_TYPE_SERVICE_PROVIDER_LOCK,
    EXE_COPS_SIM_LOCK_TYPE_CORPORATE_LOCK,
    EXE_COPS_SIM_LOCK_TYPE_USER_LOCK,
    EXE_COPS_SIM_LOCK_TYPE_FLEXIBLE_ESL_LOCK
} exe_cops_sim_lock_type_t;

typedef struct {
    exe_cops_facility_personalization_operation_t operation;
    exe_cops_sim_lock_type_t sim_lock_type;
    char *passwd_p;
    exe_cmee_error_t error_code; /* Output parameter */
} exe_cops_facility_personalization_data_t;

typedef struct {
    uint8_t *simlock_data;
    uint32_t simlock_data_size;
} exe_cops_simlock_data_t;

typedef struct {
    uint32_t key;
    uint8_t *value;
    uint32_t data_len;
    char *storage;
} exe_cspsa_data_t;

typedef enum {
    EXE_SIM_PIN_ID_PIN1 = 0, /* Target pin1/puk1 */
    EXE_SIM_PIN_ID_PIN2, /* Target pin2/puk2 */
    EXE_SIM_PIN_ID_UNKNOWN /* Target pin is state dependent */
} exe_sim_pin_id_t;

typedef struct {
    char *old_passwd_p;
    char *new_passwd_p;
    exe_sim_pin_id_t pin_id;
    char *facility_p;
    exe_cmee_error_t error_code;
} exe_change_pin_t;

typedef struct {
    exe_cmee_error_t   error_code;
    char *state_content_p;
} exe_full_sim_t;

typedef struct {
    int            n;
    int            length;
    unsigned char *pdu_data_p;
} exe_cnma_t;

typedef struct {
    int n;
} exe_cmms_t;

typedef struct {
    int n;
} exe_csms_t;

typedef struct {
    bool full;
} exe_esmsfull_t;

typedef struct {
    int memfull;
} exe_esmsfull_read_t;

typedef struct {
    exe_cops_operator_format_t format;
    uint8_t act;
    char operator[EXE_COPS_OPERATOR_MAX_LONG_LEN + 1];
} exe_cops_request_data_t;

typedef struct {
    int32_t ussd_length;
    uint8_t dcs;
    unsigned char *str_p;
} exe_cusd_request_data_t;

typedef struct {
    int mode;
} exe_cssn_request_data_t;

/**
 * \enum  exe_ussd_type_t
 * \brief This enum describe USSD type codes.
 */
typedef enum {
    EXE_USSD_TYPE_USSD_NOTIFY                  = 0, /**< USSD-Notify                                        */
    EXE_USSD_TYPE_USSD_REQUEST                 = 1, /**< USSD-Request                                       */
    EXE_USSD_TYPE_USSD_SESSION_TERMINATED      = 2, /**< Session terminated by network                      */
    EXE_USSD_TYPE_USSD_OTHER_LOCAL_CLIENT      = 3, /**< other local client (eg, SIM Toolkit) has responded */
    EXE_USSD_TYPE_USSD_OPERATION_NOT_SUPPORTED = 4, /**< Operation not supported                            */
    EXE_USSD_TYPE_USSD_NETWORK_TIMEOUT         = 5  /**<  Network timeout                                   */
} exe_ussd_type_t;

/**
 * \enum exe_ussd_received_type_t
 * \brief This enum describe the ussd received type
*/
typedef enum {
    EXE_USSD_RECEIVED_TYPE_GSM_USSD_REQUEST, /**< An USSD request has been received from the network */
    EXE_USSD_RECEIVED_TYPE_GSM_USSD_NOTIFY,  /**< An USSD notification has been received from the network */
    EXE_USSD_RECEIVED_TYPE_GSM_USSD_COMMAND, /**< An USSD command has been received from the network */
    EXE_USSD_RECEIVED_TYPE_GSM_USSD_END      /**< The USSD request has been terminated by the network */
} exe_ussd_received_type_t;

typedef struct {
    exe_ussd_type_t type;
    exe_ussd_received_type_t  received_type;
    uint32_t length;
    uint8_t dcs;
    uint8_t me_initiated;
    unsigned char *ussd_str_p;
    uint8_t suppress_ussd_on_end; /**<Valid when received_type is CN_USSD_RECEIVED_TYPE_GSM_USSD_END for SAT initiated USSD */
} exe_cusd_response_t;

typedef struct {
    exe_cmee_error_t error;
} exe_error_response_t;

/* Call state as defined and used by EXE_UNSOLICITED_ECAV */
typedef enum {
    ECAM_IDLE         = 0,
    ECAM_CALLING      = 1,
    ECAM_CONNECTING   = 2,
    ECAM_ACTIVE       = 3,
    ECAM_HOLD         = 4,
    ECAM_WAITING      = 5,
    ECAM_ALERTING     = 6,
    ECAM_BUSY         = 7
} exe_call_status_t;

typedef enum {
    CALL_EMPTY  = 0,
    CALL_SPEECH = 1,
    CALL_DATA    = 2,
    CALL_ALS    = 128
} exe_call_type_t;

typedef enum {
    EXE_CCFC_REASON_UNCONDITIONAL = 0,
    EXE_CCFC_REASON_MOBILE_BUSY = 1,
    EXE_CCFC_REASON_NO_REPLY = 2,
    EXE_CCFC_REASON_NOT_REACHABLE = 3,
    EXE_CCFC_REASON_ALL_CALL_FORWARDING = 4,
    EXE_CCFC_REASON_ALL_CONDITIONAL_CALL_FORWARDING = 5,
    EXE_CCFC_REASON_UNKNOWN = 6
} exe_ccfc_reason_t;

typedef enum {
    EXE_CCFC_MODE_DISABLE = 0,
    EXE_CCFC_MODE_ENABLE = 1,
    EXE_CCFC_MODE_QUERY_STATUS = 2,
    EXE_CCFC_MODE_REGISTRATION = 3,
    EXE_CCFC_MODE_ERASURE = 4,
    EXE_CCFC_MODE_UNKNOWN = 5
} exe_ccfc_mode_t;

typedef enum {
    EXE_CCFC_STATUS_DISABLED = 0,
    EXE_CCFC_STATUS_ENABLED = 1,
} exe_ccfc_status_t;

#define EXE_CCFC_MAX_NUMBER_OF_QUERY_INFOS (48)                      /* MAX(reason)*MAX(class) = 48 */
#define EXE_CCFC_TIME_DEFAULT              (20)                      /* seconds */
#define EXE_CCFC_TIME_MIN                  (1)                       /* seconds */
#define EXE_CCFC_TIME_MAX                  (30)                      /* seconds */
#define EXE_CCFC_TYPE_MIN                  (128)                     /* min value of numbering plan  */
#define EXE_CCFC_TYPE_NONE                 (0)                       /* no number plan given  */
#define EXE_CCFC_NUM_TYPE_PLUS             (145)                     /* ISDN/telephony international number  */
#define EXE_CCFC_NUM_TYPE_NO_PLUS          (129)                     /* ISDN/telephony international/national number unknown */
#define EXE_CCFC_NUM_TYPE_NATIONAL         (161)                     /* ISDN/telephony national number */
#define EXE_CCFC_ALLOWED_DIGITS            "0123456789*#abc"         /* Allowed digits (4bit BCD) found in 3gpp 24.008 v7.15 (10.5.4.7: table 10.5.118) */
#define EXE_CCFC_MAX_NUM_DIGITS            (28)                      /* Maximum number of digits in call forwarding according to 3gpp 22.082 */

typedef enum {
    EXE_CLASSX_UNDEFINED = 0,
    EXE_CLASSX_VOICE = 1,
    EXE_CLASSX_DATA = 2,
    EXE_CLASSX_FAX = 4,
    EXE_CLASSX_DEFAULT = 7,
    EXE_CLASSX_SMS = 8,
    EXE_CLASSX_DATA_CIRCUIT_SYNC = 16,
    EXE_CLASSX_DATA_CIRCUIT_ASYNC = 32,
    EXE_CLASSX_PACKET_ACCESS = 64,
    EXE_CLASSX_PAD_ACCESS = 128,
    EXE_CLASSX_MASK_SMS = 0xF7,
    EXE_CLASSX_ALL = 0xFF
} exe_classx_t;


typedef struct exe_ccfc_data_s {
    exe_ccfc_status_t     status;
    exe_ccfc_reason_t     reason;
    exe_ccfc_mode_t       mode;
    char                 *number_p;
    int                   type;
    int                   class;
    int                   time;
    struct exe_ccfc_data_s *next_p;
} exe_ccfc_data_t;


typedef struct exe_ccfc_head_s {
    exe_ccfc_data_t   *data_p;
    exe_cmee_error_t  error_code;
} exe_ccfc_head_t;

typedef struct {
    int len;
    int type;
    uint8_t *buff;
} exe_cuus1_response_t;

typedef struct {
    int                 ccid;
    exe_call_status_t   call_status;
    exe_call_type_t     call_type;
    int                 process_id;
    int                 exit_cause;
    char               *subscriber_number_p;
    int                 subscriber_number_addr_type;
} exe_ecav_response_t;

/* Call state as defined by CN and used by EXE_EVENT_CALL_RESPONSE.
 * Could also be used by CLCC and/or merged with exe_call_status_t. */
typedef enum {
    EXE_CALL_ACTIVE,
    EXE_CALL_HOLD,
    EXE_CALL_DIALING,
    EXE_CALL_ALERTING,
    EXE_CALL_INCOMING,
    EXE_CALL_WAITING,
    EXE_CALL_IDLE
} exe_call_state_t;

typedef enum {
    EXE_CALL_RESPONSE_NO_DIALTONE,
    EXE_CALL_RESPONSE_BUSY,
    EXE_CALL_RESPONSE_NO_ANSWER,
    EXE_CALL_RESPONSE_NO_CARRIER,
    EXE_CALL_RESPONSE_NONE
} exe_call_exit_response_t;

typedef struct {
    exe_call_state_t call_state;
    exe_call_exit_response_t exit_response;
} exe_call_response_t;

typedef struct {
    char               *number_p;
    int32_t             type;
} exe_clip_response_t;

typedef enum {
    EXE_CNI_VALIDITY_VALID              = 0,
    EXE_CNI_VALIDITY_WITHHELD           = 1,
    EXE_CNI_VALIDITY_NA_INTERWORKING    = 2,
} exe_cni_validity_t;

/* CNAP unsolicited response */
typedef struct {
    uint16_t           *name_p;             /* Name in UCS-2 coding scheme */
    uint8_t             name_char_length;   /* Number of characters in name */
    exe_cni_validity_t  cni_validity;
} exe_cnap_response_t;

typedef struct {
    int32_t code1;
    int32_t index;
} exe_cssi_response_t;

typedef struct {
    int32_t code2;
    int32_t index;
    int32_t address_type;
    char   *number;
} exe_cssu_response_t;

typedef struct {
    char *number_p;
    int32_t type;
    int32_t class;
} exe_ccwa_response_t;

#define TIME_INFO_NOT_AVAILABLE 0x64
typedef struct {
    uint8_t year;                  /**< Year;   [0-99], TIME_INFO_NOT_AVAILABLE. */
    uint8_t month;                 /**< Month;  [1-12], TIME_INFO_NOT_AVAILABLE. */
    uint8_t day;                   /**< Day;    [1-31], TIME_INFO_NOT_AVAILABLE. */
    uint8_t hour;                  /**< Hour;   [0-23], TIME_INFO_NOT_AVAILABLE. */
    uint8_t minute;                /**< Minute; [0-59], TIME_INFO_NOT_AVAILABLE. */
    uint8_t second;                /**< Second; [0-59], TIME_INFO_NOT_AVAILABLE. */
    uint8_t time_zone_sign;        /**< Time zone sign; [0-1],
                                       *  0 - Positive (+) sign.
                                       *  1 - Negative (-) sign.
                                       */
    uint8_t time_zone_value;       /**< Number of quarter-hours */
    uint8_t day_light_saving_time; /**< Day light saving time;
                                       * [0-2], TIME_INFO_NOT_AVAILABLE:
                                       * 0 - No adjustment.
                                       * 1 - One hour adjustment.
                                       * 2 - Two hour adjustment.
                                       */
} exe_etzv_response_t;

#define CME_3GPP_REJECT_CAUSE_OFFSET (100)
typedef struct {
    int32_t cause;
    char *cause_text_p;
} exe_ecme_response_t;

typedef struct {
    int mode;
    int format;
    char operator[EXE_COPS_OPERATOR_MAX_LONG_LEN + 1];
    exe_act_t act;
    bool operator_selected;
    exe_cmee_error_t error_code;
} exe_cops_response_data_t;

typedef struct {
    char long_alphanumeric[EXE_COPS_OPERATOR_MAX_LONG_LEN + 1];
    char short_alphanumeric[EXE_COPS_OPERATOR_MAX_SHORT_LEN + 1];
    char numeric[EXE_OPERATOR_NUMERIC_STRING_MAX_LENGTH + 1];
    char stat[EXE_COPS_OPERATOR_MAX_STAT_LEN + 1];
    char act[EXE_COPS_OPERATOR_MAX_ACT_LEN + 1];
} exe_cops_operator_info_t;

typedef struct {
    uint32_t num_of_networks;
    exe_cops_operator_info_t *completelist_p;
} exe_cops_network_list_t;

typedef enum {
    EXE_TYPE_OF_NUMBER_UNKNOWN = 129,
    EXE_TYPE_OF_NUMBER_INTERNATIONAL = 145,
    EXE_TYPE_OF_NUMBER_NATIONAL = 161,
} exe_type_of_number_t;

typedef struct {
    exe_type_of_number_t ton;
    void *text_p;
} exe_csca_request_data_t;

typedef struct {
    char *text_p;
    exe_type_of_number_t tosca;
    exe_cmee_error_t error_codes;
} exe_csca_response_data_t;

typedef enum {
    EXE_CRSM_STATUS_INFO,
    EXE_CRSM_RESPONSE_INFO
} exe_crsm_info_t;

typedef struct {
    uint16_t file_id;
    char *path_p;
    uint8_t p_1;
    uint8_t p_2;
    uint8_t p_3;
    char *data_p;
    exe_crsm_info_t info_type;
    /* Response data */
    uint8_t sw_1;
    uint8_t sw_2;
    char *sim_io_response_p;
    uint16_t length;
    exe_cmee_error_t error_codes;
} exe_sim_io_t;

typedef struct {
    int                     status;        /**< Status of the CAT service. */
    char                   *profile_p;     /**< Fetched profile data. */
    unsigned                profile_len;   /**< Length of fetched profile data. */
} exe_stkc_get_terminal_profile_response_t;


typedef struct {
    char *imsi_p;
    exe_cmee_error_t error_code;
} exe_imsi_t;

typedef enum /* Possibly CPIN codes: */
{
    EXE_CPIN_CODE_READY,           /* ... no pending request for any password.   */
    EXE_CPIN_CODE_SIM_PIN,         /* ... requires a SIM PIN to be entered.      */
    EXE_CPIN_CODE_SIM_PUK,         /* ... requires a SIM PUK to be entered.      */
    EXE_CPIN_CODE_PH_SIM_PIN,      /* ... requires a Phone -to-SIM card password *
                                    *     to be entered (phone lock).            */
    EXE_CPIN_CODE_SIM_PIN2,        /* ... requires a SIM PIN2 to be entered.     */
    EXE_CPIN_CODE_SIM_PUK2,        /* ... requires a SIM PUK2 to be entered.     */
    EXE_CPIN_CODE_PH_NET_PIN,      /* ... requires a network personalization     *
                                    *     password to be entered.                */
    EXE_CPIN_CODE_PH_NETSUB_PIN,   /* ... requires a network subset              *
                                    *     personalization password to be entered.*/
    EXE_CPIN_CODE_PH_SP_PIN,       /* ... requires a service provider            *
                                    *     personalization password to be entered.*/
    EXE_CPIN_CODE_PH_CORP_PIN,     /* ... requires a corporate personalization   *
                                    *     password to be entered.                */
    EXE_CPIN_CODE_PH_SIMLOCK_PIN,  /* ... requires a SIM lock password           *
                                    *     to be entered.                         */
    EXE_CPIN_CODE_PH_ESL_PIN,      /* ... requires extended SIM lock
                                          to be entered.                         */
    EXE_CPIN_CODE_BLOCKED,         /* ... the SIM card is present but blocked.   */
    EXE_CPIN_CODE_NO_CODE          /* ... no matching CPIN code was found.       */
} exe_cpin_code_t;


typedef struct {
    exe_cpin_code_t code;
    exe_cmee_error_t    error_code;
} exe_cpin_sim_get_state_t;

typedef enum {
    EXE_CPIN_RESP_STATE_NORMAL,
    EXE_CPIN_RESP_STATE_QUERY_APP_STATE,
    EXE_CPIN_RESP_STATE_RESTART_SIM
} exe_cpin_response_state_t;

typedef enum {
    EXE_SIM_STATE_NULL = 0,
    EXE_SIM_STATE_AWAITING_APPLICATION_SELECTION = 1,
    EXE_SIM_STATE_BLOCKED = 2,
    EXE_SIM_STATE_BLOCKED_FOREVER = 3,
    EXE_SIM_STATE_WAIT_FOR_PIN = 4,
    EXE_SIM_STATE_ACTIVE = 5,
    EXE_SIM_STATE_TERMINATING = 6,
    EXE_SIM_STATE_POWER_OFF = 7,
} exe_esimsr_state_t;

typedef struct {
    exe_esimsr_state_t sim_state;
} exe_esimsr_sim_state_t;

typedef enum {
    EXE_SIM_REFRESH_INIT_FULL_FILE_CHANGE,
    EXE_SIM_REFRESH_FILE_CHANGE,
    EXE_SIM_REFRESH_INIT_FILE_CHANGE,
    EXE_SIM_REFRESH_INIT,
    EXE_SIM_REFRESH_RESET,
    EXE_SIM_REFRESH_3G_APP_RESET,
    EXE_SIM_REFRESH_3G_SESSION_RESET,
    EXE_SIM_REFRESH_UNKNOWN,
} exe_esimrf_type_t;

typedef struct {
    exe_esimrf_type_t refresh_type;
    uint16_t file_id;
    char *path_p;
} exe_esimrf_unsol_t;

typedef struct {
    exe_sim_pin_puk_t   pin_puk;
    exe_cmee_error_t    error_code;
    int             pin_status;
    int             attempts;
} exe_sim_pin_info_t;

typedef struct {
    exe_cmee_error_t    error_code;
    int                 pin1_attempts;
    int                 pin2_attempts;
    int                 puk1_attempts;
    int                 puk2_attempts;
} exe_sim_full_pin_info_t;

typedef struct {
    char *code_p;    /* Code when verifying PIN, PIN2, PUK, PUK2 and SIM locks */
    char *new_pin_p; /* New PIN or PIN2 when PUK and PUK2 is verified*/
    exe_sim_pin_id_t pin_id; /* Pin id to target pin1/puk1 or pin2/puk2 */
} exe_sim_verify_codes_t;

typedef enum {
    CPOL_FORMAT_LONG = 0,
    CPOL_FORMAT_SHORT = 1,
    CPOL_FORMAT_NUMERIC = 2,
} atc_sim_cpol_format_t;

/* exe_sim_cpol_info_t */
typedef struct {
    int index;
    int format;
    char oper_string[EXE_OPERATOR_NUMERIC_STRING_MAX_LENGTH + 1];
    int gsm_act;
    int gsm_compact_act;
    int utran_act;
} exe_sim_cpol_info_t;

typedef struct exe_sim_cpol_info_s {
    exe_sim_cpol_info_t cpol_info;
    struct exe_sim_cpol_info_s *next_p;
    bool valid_act;
} exe_sim_cpol_info_response_t;

typedef struct {
    int answer;
} exe_setup_call_answer_data_t;

typedef enum {
    EXE_SMS_CATEGORY_CLASS_ZERO,
    EXE_SMS_CATEGORY_CLASS_ONE,
    EXE_SMS_CATEGORY_CLASS_TWO,
    EXE_SMS_CATEGORY_CLASS_THREE,
    EXE_SMS_CATEGORY_UNKNOWN
} exe_sms_class_t;

#define EXE_SMS_CODING_GROUP_MASK                     0xf0      /* 0b11110000 */
/* in accordance with 3GPP TS 23.038 */
#define EXE_SMS_CODING_GROUP_GEN_UCOMP_CODE           0x10      /* 0b00010000 */
#define EXE_SMS_CODING_GROUP_GEN_UCOMP_CODE_NO_CLASS  0x00      /* 0b00000000 */
#define EXE_SMS_CODING_GROUP_GEN_COMP_CODE            0x30      /* 0b00110000 */
#define EXE_SMS_CODING_GROUP_GEN_COMP_CODE_NO_CLASS   0x20      /* 0b00100000 */
#define EXE_SMS_CODING_GROUP_AUTO_DELETE_COMP_CODE    0x70      /* 0b01110000 */
#define EXE_SMS_CODING_GROUP_AUTO_DELETE_UCOMP_CODE   0x50      /* 0b01010000 */
#define EXE_SMS_CODING_GROUP_MWI_DISCARD              0xc0      /* 0b11000000 */
#define EXE_SMS_CODING_GROUP_MWI_STORE                0xd0      /* 0b11010000 */
#define EXE_SMS_CODING_GROUP_MWI_STORE_16BIT          0xe0      /* 0b11100000 */
#define EXE_SMS_CODING_GROUP_DATA_CODING              0xf0      /* 0b11110000 */

typedef struct {
    exe_sms_class_t class;
    uint8_t         group;
    exe_msg_slot_t  slot;
    uint8_t        *pdu_p;
    uint8_t         total_length;
    uint8_t         tpdu_length;
} exe_sms_mt_t;

typedef struct {
    exe_msg_slot_t slot;
    uint8_t       *pdu_p;
    uint8_t        total_length;
    uint8_t        tpdu_length;
} exe_sms_status_report_t;

typedef struct {
    uint16_t length;
    uint8_t pdu[1];
} exe_cbm_t;

typedef struct {
    uint16_t first_message_identifier;
    uint16_t last_message_identifier;
    uint8_t  first_data_coding_scheme;
    uint8_t  last_data_coding_scheme;
} exe_cscb_subscription_t;

typedef struct {
    exe_cscb_subscription_t *subscription_p;
    uint8_t                  num_of_subs;
} exe_cscb_data_t;

typedef struct {
    int rssi; /* Received Signal Strength Indicator */
    int ber; /* Bit Error Rate */
} exe_csq_data_t;

typedef enum {
    CLIR_CMD_DEFAULT = 0,
    CLIR_CMD_INVOKE = 1,
    CLIR_CMD_SUPPRESS = 2
} exe_clir_cmd_t;

typedef struct {
    uint8_t length;
    uint8_t dcs;
    uint8_t *data;
} cuus1_message_t;


typedef struct {
    char *phone_number_p;
    exe_clir_cmd_t clir;
    cuus1_message_t *cuus1_data_p;
} exe_atd_data_t;

typedef struct {
    int cid;
    int l2p;
} exe_atd_psdun_t;

typedef struct {
    exe_cmee_error_t error_code;
} exe_atd_response_t;

typedef enum {
    EXE_PDP_TYPE_IPV4    = 0,
    EXE_PDP_TYPE_IPV6    = 1,
    EXE_PDP_TYPE_IPV4V6  = 2,
    EXE_PDP_TYPE_TEST    = 3,
    EXE_PDP_TYPE_INVALID = 4
} exe_pdp_type_cmd_t;

typedef struct exe_cgcmod_set_request_cid_s {
    int cid;
    struct exe_cgcmod_set_request_cid_s *next_p;
} exe_cgcmod_set_request_cid_t;

typedef struct {
    exe_cgcmod_set_request_cid_t *cid_p;
} exe_cgcmod_set_request_t;

typedef struct {
    int     cid;
    exe_pdp_type_cmd_t pdp_type;
    char   *apn_p;
    char   *pdp_addr_p;
    char   *ipv6_pdp_addr_p;
    int     d_comp;
    int     h_comp;
} exe_cgdcont_t;

typedef struct cgdcont_response_s {
    exe_cgdcont_t *cgdcont_p;
    struct cgdcont_response_s *next_p;
} exe_cgdcont_response_t;

/* Rel-99 (3G) QoS parameters. Used by AT+CGEQMIN, AT+CGEQNEG  and AT+CGEQREQ */
typedef struct {
    int cid;
    int traffic_class;
    unsigned long max_bitrate_ul;
    unsigned long max_bitrate_dl;
    unsigned long guaranteed_bitrate_ul;
    unsigned long guaranteed_bitrate_dl;
    int delivery_order;
    unsigned long max_sdu_size;
    char *sdu_error_ratio_p;
    char *residual_ber_p;
    int delivery_err_sdu;
    unsigned long transfer_delay;
    int traffic_handling_priority;
    int source_statistics_descriptor;
    int signalling_indication;
} exe_pdp_qos_t;

typedef struct {
    int mantissa;
    int exponent;
} exe_exponential_t;

typedef struct exe_pdp_qos_s {
    exe_pdp_qos_t pdp_qos;
    struct exe_pdp_qos_s *next_p;
} exe_pdp_qos_response_t;

/* Rel-97 (2G) parameters. Used by AT+CGQMIN and AT+CGQREQ */
typedef struct {
    int cid;
    int precedence;
    int delay;
    int reliability;
    int peak;
    int mean;
} exe_pdp_qos_2G_t;

typedef struct exe_pdp_qos_2G_s {
    exe_pdp_qos_2G_t pdp_qos;
    struct exe_pdp_qos_2G_s *next_p;
} exe_pdp_qos_2G_response_t;

typedef struct {
    bool state;
    int cid;
} exe_enap_t;

typedef enum {
    EXE_PSCC_AUTH_NONE = 1,
    EXE_PSCC_AUTH_PAP = 3,
    EXE_PSCC_AUTH_CHAP = 5,
    EXE_PSCC_AUTH_PAP_OR_CHAP = 7
} exe_pscc_auth_prot_t;

typedef enum {
    EXE_PSCC_BEARER_TYPE_PS = 1,
    EXE_PSCC_BEARER_TYPE_CS,
    EXE_PSCC_BEARER_TYPE_BT,
    EXE_PSCC_BEARER_TYPE_EXT
} exe_pscc_bearer_type_t;

typedef struct {
    int cid;
    exe_pscc_bearer_type_t bearer_type;
    char *userid_p;
    char *password_p;
    exe_pscc_auth_prot_t auth_prot;
    bool ask4pwd;
} exe_eiaauw_t;

typedef enum {
    EXE_PSCC_STATE_OFF = 0,
    EXE_PSCC_STATE_ON = 1
} exe_pscc_state_t;

typedef struct {
    exe_pscc_state_t state;
    unsigned long connection_id;
    int cid;
} exe_eppsd_t;

typedef struct {
    char *netdev_name_p;
    char *ip_address_p;
    char *ipv6_address_p;
    char *subnet_netmask_p;
    int   mtu;
    char *default_gateway_p;
    char *dns_server_p;
    char *ipv6_dns_server_p;
    char *dns_server_2_p;
    char *ipv6_dns_server_2_p;
    char *p_cscf_server_p;
    char *p_cscf_server_2_p;
    int   connection_status;
    int   activation_cause;
} exe_pscc_connection_response_t;

typedef struct cgact_response_s {
    int cid;
    int state;
    struct cgact_response_s *next_p;
} exe_cgact_read_response_t;

typedef struct exe_cgact_set_request_cid_s {
    int cid;
    struct exe_cgact_set_request_cid_s *next_p;
} exe_cgact_set_request_cid_t;

typedef struct exe_cgpaddr_response_s {
    int cid;
    char *pdp_address_p;
    char *ipv6_pdp_address_p;
    struct exe_cgpaddr_response_s *next_p;
} exe_cgpaddr_response_t;

typedef struct exe_cgpaddr_cid_s {
    int cid;
    struct exe_cgpaddr_cid_s *next_p;
} exe_cgpaddr_cid_t;

typedef enum {
    PPP_AUTH_PROTOCOL_NONE = 1,
    PPP_AUTH_PROTOCOL_PAP,
    PPP_AUTH_PROTOCOL_CHAP,
} exe_ppp_auth_prot_t;

typedef struct exe_auth_list_response_s {
    int cid;
    exe_ppp_auth_prot_t auth_prot;
    struct exe_auth_list_response_s *next_p;
} exe_auth_list_response_t;

typedef struct exe_cgact_set_request_cid_s exe_cgdata_t;

typedef enum {
    EXE_CGACT_ASYNC_MODE_SYNCHRONOUS,
    EXE_CGACT_ASYNC_MODE_ASYNCHRONOUS
} exe_cgact_async_mode_t;

typedef struct exe_cgact_set_request_s {
    int state;
    exe_cgact_async_mode_t async_mode;
    exe_cgact_set_request_cid_t *cid_p;
} exe_cgact_set_request_t;

typedef enum {
    EXE_CGATT_DETACH,
    EXE_CGATT_ATTACH
} exe_cgatt_gprs_state_t;

typedef struct {
    exe_cgatt_gprs_state_t state;
} exe_cgatt_request_data_t;

typedef struct {
    exe_cgatt_gprs_state_t state;
} exe_cgatt_response_data_t;

typedef enum {
    CGEV_TYPE_UNDEFINED,
    CGEV_TYPE_ME_ACT,
    CGEV_TYPE_NW_DEACT,
    CGEV_TYPE_ME_DEACT,
    CGEV_TYPE_NW_DETACH,
    CGEV_TYPE_ME_DETACH
} exe_cgev_response_type_t;

typedef struct {
    exe_cgev_response_type_t response_type;
    int                      cid;
    int                      pdp_type;
    char                    *pdp_addr_p;
    char                    *ipv6_pdp_addr_p;
} exe_cgev_response_t;

typedef enum {
    EXE_PCD_RESPONSE_STATUS_DETACHED = 0,
    EXE_PCD_RESPONSE_STATUS_ATTACHED = 1
} exe_pcd_response_status_t;

typedef struct {
    int radio_state;
} exe_erfstate_response_t;

typedef enum {
    DTMF_STATE_UNKNOWN = 0,
    DTMF_STATE_STOPPED = 1,
    DTMF_STATE_STARTED = 2
} dtmf_state_t;

typedef enum {
    EXE_SMS_STATUS_UNREAD = 0,
    EXE_SMS_STATUS_READ = 1,
    EXE_SMS_STATUS_UNSENT = 2,
    EXE_SMS_STATUS_SENT = 3
} exe_sms_status_t;

typedef struct {
    msg_storage_t storage;
    uint16_t index;
} exe_cmgr_t;

typedef struct {
    uint8_t *pdu_data_p;
    uint8_t total_length;
    uint8_t tpdu_length;
    exe_sms_status_t status;
    exe_cms_error_t error;
} exe_cmgr_response_t;


/*
 * Enum value for coding.
 */
typedef enum {
    EXE_CHARSET_UTF8 = 0,
    EXE_CHARSET_HEX,
    EXE_CHARSET_GSM7,
    EXE_CHARSET_GSM8,
    EXE_CHARSET_UCS2,
    EXE_CHARSET_UNDEF_BINARY,
    EXE_CHARSET_MAX /* The number of different character sets. */
} exe_charset_t;

/*
 * Data structure for CNUM.
 */
typedef struct {
    uint8_t  alpha_id_actual_len;
    uint8_t  alpha_coding;
    uint8_t  *alpha_p;
    uint8_t  number_actual_len;
    uint8_t  *number_p;
    uint8_t  type;
    uint8_t  speed;
    uint8_t  service;
    uint8_t  itc;
} exe_cnum_record_data_t;

/**
 * Data structure for CNUM.
 */
typedef struct {
    exe_cmee_error_t cmee_error_code;
    uint16_t number_of_records; /**< Number of records. */
    exe_cnum_record_data_t *record_data_p;
} exe_cnum_response_data_t;

typedef struct {
    uint8_t profile;
} exe_cres_csas_t;

typedef struct {
    exe_cms_error_t error;
} exe_cres_csas_response_t;

typedef struct {
    uint8_t profiles;
    exe_cms_error_t error;
} exe_cres_csas_test_t;

typedef struct {
    uint32_t apdu_length;
    uint8_t  *apdu_p;
    uint16_t session_id;
} exe_generic_sim_io_t;

typedef struct {
    uint8_t *apdu_data_p;
    uint32_t apdu_data_length;
    exe_cmee_error_t error;
} exe_generic_sim_io_response_t;

typedef struct {
    uint8_t *application_id_p;
    uint32_t application_id_length;
} exe_ccho_t;

typedef struct {
    uint16_t session_id;
    exe_cmee_error_t error;
} exe_ccho_response_t;

typedef struct {
    uint16_t session_id;
} exe_cchc_t;

typedef struct {
    exe_cmee_error_t error;
} exe_cchc_response_t;

typedef char exe_vts_t;

typedef struct {
    exe_vts_t *dtmf_p; /* pointer to dtmf characters */
    uint8_t length; /* string length */
} exe_dtmf_data_t;

typedef unsigned long exe_vtd_t;

typedef enum {
    EXE_CGSMS_SERVICE_ROUTE_PS,
    EXE_CGSMS_SERVICE_ROUTE_CS,
    EXE_CGSMS_SERVICE_ROUTE_PS_CS,
} exe_cgsms_service_t;

typedef struct {
    exe_cgsms_service_t service;
} exe_cgsms_request_t;

typedef struct {
    exe_cgsms_service_t service;
} exe_cgsms_response_t;

typedef struct {
    uint16_t index;
    msg_storage_t storage;
    int8_t toda;
    char *da_p;
} exe_cmss_request_t;

typedef struct {
    uint8_t mr;
    exe_cms_error_t error;
} exe_cmss_response_t;

#define MAX_SIM_APPS 8

typedef struct {
    char *sim_io_response_p;
    uint16_t length;
} exe_cuad_app_response_t;

typedef struct {
    int number_of_apps;
    int count;
    exe_cuad_app_response_t *apps[MAX_SIM_APPS];
    exe_cmee_error_t error;
} exe_cuad_response_t;

/**
 * Data structures for CMGL.
 */
typedef enum {
    EXE_CMGL_STATUS_REC_UNREAD = 0,
    EXE_CMGL_STATUS_REC_READ = 1,
    EXE_CMGL_STATUS_STO_UNSENT = 2,
    EXE_CMGL_STATUS_STO_SENT = 3,
    EXE_CMGL_STATUS_ALL = 4
} exe_cmgl_status_t;

typedef struct {
    msg_storage_t storage;
    exe_msg_position_t position;
    exe_msg_browse_option_t browse;
    exe_cmgl_status_t status;
} exe_cmgl_request_t;

typedef struct exe_cmgl_response_pdu_tag {
    struct exe_cmgl_response_pdu_tag *next_p;
    exe_msg_position_t position;
    uint8_t *pdu_p;
    uint8_t total_length;
    uint8_t tpdu_length;
    exe_sms_status_t status;
} exe_cmgl_response_pdu_data_t;

typedef struct {
    uint16_t num_pdus;
    exe_cmgl_response_pdu_data_t *pdu_data_p;
    exe_cms_error_t error;
} exe_cmgl_response_t;

typedef int32_t exe_elat_response_t;

typedef enum {
    EXE_LINE_NORMAL = 1,
    EXE_LINE_TWO = 2
} exe_line_values_t;

typedef enum {
    EXE_LINE_NO_SUPPORT = 0,
    EXE_LINE_SUPPORT = 1
} exe_line_support_values_t;

typedef struct {
    /* Response data */
    exe_line_values_t current_line;
    exe_line_support_values_t supported_lines;
    /* Request data */
    exe_line_values_t line;
} exe_line_request_response_data_t;

typedef struct {
    uint8_t rssi_dbm;
} exe_cmer_ciev_data_t;

typedef struct {
    uint8_t ind;
} exe_cmer_data_t;

typedef struct {
    uint8_t storage_full_SIM;
} exe_ciev_storage_status_t;

typedef enum {
    EXE_EMPAGE_CMD_ONESHOT,
    EXE_EMPAGE_CMD_START_PERIODIC,
    EXE_EMPAGE_CMD_STOP_PERIODIC,
    EXE_EMPAGE_CMD_RESET_MEASUREMENTS
} exe_empage_cmd_mode_t;

typedef struct {
    int32_t mode;
    int32_t page;
    int32_t timer;
} exe_empage_t;

typedef struct {
    int32_t page;
    int32_t seqnr;
} exe_empage_urc_t;

typedef enum {
    EXE_EMPAGE_STATE_IDLE,     /* Used when idle */
    EXE_EMPAGE_STATE_ONESHOT,
    EXE_EMPAGE_STATE_PERIODIC
} exe_empage_state_t;

#ifdef EXE_USE_ATC_CUSTOMER_EXTENSIONS
#include <exe_extended.h>
#endif

typedef void (*exe_final_response_t)(void *caller_data_p, exe_request_result_t result, void *response_p);
typedef void (*exe_event_response_t)(exe_event_t event, void *response_p);

exe_t *exe_create(void);

void exe_destroy(exe_t *exe_p);

exe_request_result_t exe_request(exe_t *exe_p,
                                 exe_request_id_t request_id,
                                 void *request_data_p,
                                 void *caller_data_p,
                                 int *request_handle_p);

exe_request_result_t exe_request_abort(exe_t *exe_p, int request_handle);

void exe_register_callback_final_response(exe_t *exe_p, exe_final_response_t final_response_callback);

void exe_register_callback_event(exe_t *exe_p, exe_event_response_t event_callback);

const char *exe_get_request_description(exe_request_id_t request_id);

exe_service_t exe_get_request_service(exe_request_id_t request_id);

void exe_set_client_context(exe_t *exe_p, exe_service_t service, void *context_p);

#endif
