/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
#ifndef GNS_SUPL_TYPEDEFS_H
#define GNS_SUPL_TYPEDEFS_H


/*! \addtogroup Location_gnsSUPL_Constants */
/*@{*/

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsSUPL_MsgType
 *      List of constant used to define type of message
 */
typedef enum
{
    E_gnsSUPL_CONNECT_REQ,        /*!< TCP/IP Connection Requested. Data defined by \ref s_gnsSUPL_ConnectReq*/
    E_gnsSUPL_DISCONNECT_REQ,   /*!< TCP/IP Connection Shutdown Requested. Data defined by \ref s_gnsSUPL_DisconnectReq */
    E_gnsSUPL_SEND_DATA_REQ,    /*!< Data to be sent over TCP/IP Connection. Data defined by \ref s_gnsSUPL_SendDataReq */
#ifndef GNS_CELL_INFO_FTR
    E_gnsSUPL_MOBILE_INFO_REQ,    /*!< Mobile Info Updates Requested. Data defined by \ref s_gnsSUPL_MobileInfoReq */
#endif
    E_gnsSUPL_RECEIVE_DATA_CNF, /*!< Incoming Data over TCP/IP connection handled. Data defined by \ref s_gnsSUPL_SendDataReq */
    E_gnsSUPL_ESTABLISH_BEARER_REQ , /*!< No Data associated with this */
    E_gnsSUPL_CLOSE_BEARER_REQ /*!< No Data associated with this */
} e_gnsSUPL_MsgType;

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsSUPL_RegistrationState
 *      List of constant used to indicate a registration state.
 */
typedef enum{
    E_gnsSUPL_REGISTRATION_MOBILE_OFF            = 0,    /*!< The Cellular system on the Mobile is OFF, eg. Flight mode. */
    E_gnsSUPL_REGISTRATION_NO_CARRIER            = 1,    /*!< No Network Coverage. */
    E_gnsSUPL_REGISTRATION_EMERGENCY_IDLE        = 2,    /*!< Only emergency calls are allowed. */
    E_gnsSUPL_REGISTRATION_SERVICE_ALLOWED       = 3,    /*!< All services Calls,data,SMS etc are allowed. */
} e_gnsSUPL_RegistrationState;

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsSUPL_CellInfoStatus
 *  \brief  List of constant used to indicate The Status of Cell Information (2G/3G).
 */
typedef enum
{
    E_gnsSUPL_CELL_INFO_STALE,      /*!< Cell information is Stale (Last Known Cell Information). */
    E_gnsSUPL_CELL_INFO_CURRENT,    /*!< Cell information is Current. */
    E_gnsSUPL_CELL_INFO_UNKNOWN     /*!< Cell information is Unknown. */
} e_gnsSUPL_CellInfoStatus;


/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsSUPL_DuplexDivisionMode
 *  \brief  List of constant used to indicate The Frequency type ( Frequency Division Duplex/Time Division Duplex).
 */
typedef enum{
    E_gnsSUPL_FDD                   = 1,        /*!< Used to define WCDMA - FDD (Frequency Division Duplexed) Mode of Operation. */
    E_gnsSUPL_TDD                   = 2         /*!< Used to define WCDMA - TDD (Time Division Duplexed) Mode of Operation. */
} e_gnsSUPL_DuplexDivisionMode;


/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsSUPL_CellInfoType
 *  \brief  List of constant used to indicate The Cell Info type (2G/3G).
 */
typedef enum{
    E_gnsSUPL_GSM_CELL              = 1,        /*!< Used to indicate GSM Cell type is present in s_gnsSUPL_MobileInfo::v_CellInfo. */
    E_gnsSUPL_WCDMA_FDD_CELL        = 2,        /*!< Used to indicate WCDMA FDD Cell type is present in s_gnsSUPL_MobileInfo::v_CellInfo. */
    E_gnsSUPL_WCDMA_TDD_CELL        = 3         /*!< Used to indicate WCDMA TDD Cell type is present in s_gnsSUPL_MobileInfo::v_CellInfo. */
} e_gnsSUPL_CellInfoType;

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsSUPL_MobileInfoValidity
 *  \brief  List of constant used to indicate validity of different fields in mobile info structure
 */
typedef enum{
    E_gnsSUPL_MCC_FIELD = 0x1,     /**< MCC field validity. */
    E_gnsSUPL_MNC_FIELD = 0x2,     /**< MNC field validity. */
    E_gnsSUPL_LAC_FIELD = 0x4,     /**< LAC field validity. */
    E_gnsSUPL_CID_FIELD = 0x8,     /**< CellIdentity field validity. */
    E_gnsSUPL_RNC_FIELD = 0x10,     /**< RNCId field validity. */
    E_gnsSUPL_NMR_FIELD = 0x20,     /**< Network Measurements for GMS/WCDMA/TDSCDMA  field validity. */
    E_gnsSUPL_MSISDN_FIELD = 0x40,    /**< MSISDN field validity. */
    E_gnsSUPL_IMSI_FIELD = 0x80    /**< IMSI field validity. */
} e_gnsSUPL_MobileInfoValidity;

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsSUPL_SocketType
 *  \brief  Socket type required for SUPL data connection.
 */
typedef enum{
    E_gnsSUPL_SOCKETTYPE_TCP        = 0,        /*!< Not Secured mode. */
    E_gnsSUPL_SOCKETTYPE_TLS        = 1,        /*!< Secured Mode TLS 1.0. */
} e_gnsSUPL_SocketType;
/*----------------------------------------------------------------------*/

#define E_gnsSUPL_MAX_TIMESLOT_ISCP         14  /*!< Maximum Number of Interference measurement on Time slot.*/
#define E_gnsSUPL_MAX_WCDMA_CELL_MEAS       32  /*!< Maximum number of UTRAN Cell Measurement for a Frequency. */
#define E_gnsSUPL_MAX_NUM_GSM_NMR           15  /*!< Maximum number of GSM cell measurements. */
#define E_gnsSUPL_MAX_NUM_WCDMA_FREQUENCY    3  /*!< Maximum number of UTRAN frequency. */
#define K_gnsSUPL_MAX_IMSI_LENGTH            16  /*!< Maximum length of the IMSI string, including the null terminator. */
#define K_gnsSUPL_MAX_MSISDN_LENGTH          42  /*!< Maximum length of the MSISDN string, including the null terminator. */

/*@}*/

/*! \addtogroup Location_gnsSUPL_Structures */
/*@{*/

typedef uint32_t t_GnsConnectionHandle;    /**< */


/*!
 *  \enum   e_gnsSUPL_AddressType
 *  \brief  SUPL server Address type.
 */

typedef enum
{
    E_gnsSUPL_DEFAULT_ADDRESS = 0, /**<  */
    E_gnsSUPL_FQDN_ADDRESS = 1,    /**< Used for FQDN type address*/
    E_gnsSUPL_IPV4_ADDRESS = 2,    /**< Used for IPV4 address. */
    E_gnsSUPL_IPV6_ADDRESS = 3,    /**<  Used for IPV6 address.*/
} e_gnsSUPL_AddressType;

/*! \struct  s_gnsSUPL_ConnectReq
 *  \brief  Structure which defines the data associated with \ref E_gnsSUPL_CONNECT_REQ
 */typedef struct
{
    t_GnsConnectionHandle     v_Handle; /*!<  Connection Handle supplied by GNS layer*/
    uint8_t                        *p_Apn;
    uint8_t                        *p_UserName;
    uint8_t                        *p_Password;
    uint8_t                        *p_PhoneCalledNumber;
    uint8_t                        *p_ServerAddr;
    uint8_t                            v_SocketType; /*!<  Socket type required for SUPL Connection \ref e_gnsSUPL_SocketType*/
    uint32_t                        v_ServerPortNum;
    e_gnsSUPL_AddressType    v_ServerAddrType;
} s_gnsSUPL_ConnectReq;

/*! \struct  s_gnsSUPL_DisconnectReq
 *  \brief  Structure which defines the data associated with \ref E_gnsSUPL_DISCONNECT_REQ
 */typedef struct
{
    t_GnsConnectionHandle v_Handle; /*!< Connection Handle supplied by GNS layer */
} s_gnsSUPL_DisconnectReq;

/*! \struct  s_gnsSUPL_SendDataReq
 *  \brief  Structure which defines the data associated with \ref E_gnsSUPL_SEND_DATA_REQ
 */
typedef struct
{
    t_GnsConnectionHandle v_Handle; /*!< Connection Handle supplied by GNS layer */
    uint8_t* p_Data;                        /*!< Data to be transmitted */
    uint32_t v_DataLen;                    /*!< Length of the data */
} s_gnsSUPL_SendDataReq;

#ifndef GNS_CELL_INFO_FTR
/*! \struct  s_gnsSUPL_MobileInfoReq
 *  \brief  Structure which defines the data associated with \ref E_gnsSUPL_RECEIVE_DATA_CNF
 */
typedef struct
{
    uint32_t v_RequestedFields; /* \ref  e_gnsSUPL_MobileInfoValidity*/
} s_gnsSUPL_MobileInfoReq;
#endif

/*! \struct  t_GnsReceiveDataCnf
 *  \brief  Structure which defines the data associated with \ref E_gnsSUPL_MOBILE_INFO_REQ
 */
typedef struct
{
    t_GnsConnectionHandle v_Handle; /*!< Connection Handle supplied by GNS layer */
} s_gnsSUPL_ReceiveDataCnf;


/*----------------------------------------------------------------------*/
/*! \struct s_gnsSUPL_GsmNMR
 *  \brief  Structure which defines an NMR (Network Measurement Report) element from a GSM network.
 */
typedef struct
{
    uint16_t v_ARFCN;            /*!< ARFCN - Absolute Radio Frequency Channel Number.  INTEGER (0..1023). */
    uint8_t  v_BSIC;             /*!< BSIC  - Base Station Identity Code.  INTEGER (0..63). */
    uint8_t  v_RXLev;            /*!< RXLEV - Received signal Level.  INTEGER (0..63). */
} s_gnsSUPL_GsmNMR;


/*----------------------------------------------------------------------*/
/*! \struct s_gnsSUPL_MeasuredResultFDD
 *  \brief  Structure which defines the cell measurements from the FDD (Frequency Division Duplexed) domain from a WCDMA network.
 */
typedef struct
{
    uint32_t v_CellIdentity;             /**< Cell Identity.  INTEGER (0..268435455).  OPTIONAL (0xFFFFFFFF = Not Present/Unknown K_gnsUNKNOWN_U32).  */
    uint16_t v_PrimaryScramblingCode;    /*!< Primary Scrambling Code.  INTEGER (0..511). */
    uint8_t  v_CPICH_EcNo;               /*!< Received energy per chip/power density.
                                        INTEGER  (0..63).   OPTIONAL (255 = 0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8).  Values above 49 are spare. */
    uint8_t  v_CPICH_RSCP;               /*!< Received Signal Code Power.
                                        INTEGER  (0..127).  OPTIONAL (255 = 0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8).  Values above 91 are spare. */
    uint8_t  v_CPICH_PathLoss;           /*!< Path Loss.
                                        INTEGER (46..173).  OPTIONAL (255 = 0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8).  Values above 158 are spare.  */
} s_gnsSUPL_MeasuredResultFDD;


/*----------------------------------------------------------------------*/
/*! \struct s_gnsSUPL_MeasuredResultTDD
 *  \brief  Structure which defines the cell measurements from the TDD (Time Division Duplexed) domain from a WCDMA network.
 *  \note   Macros used: \ref E_gnsSUPL_MAX_TIMESLOT_ISCP.
 */
typedef struct
{
    uint32_t v_CellIdentity;             /**< Cell Identity.  INTEGER (0..268435455).  OPTIONAL (0xFFFFFFFF = Not Present/Unknown K_gnsUNKNOWN_U32).  */
    uint8_t  v_CellParametersID;         /*!< Cell Parameters Identity.  INTEGER (0..127). */
    uint8_t  v_ProposedTGSN;             /*!< Transmission Gap Starting Slot Number.
                                            INTEGER  (0..14).   OPTIONAL (255 = 0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8). */
    uint8_t  v_PrimaryCCPCH_RSCP;        /*!< Received Signal Code Power.
                                            INTEGER  (0..127).  OPTIONAL (255 = 0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8). */
    uint8_t  v_PathLoss;                 /*!< Path Loss.
                                            INTEGER (46..173).  OPTIONAL (255 = 0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8).  Values above 158 are spare. */
    uint8_t  v_TimeslotISCP_Count;       /*!< Number of entries in p_TimeslotISCP_List.  INTEGER (0..#E_gnsSUPL_MAX_TIMESLOT_ISCP). */
    uint8_t  a_TimeslotISCP_List[ E_gnsSUPL_MAX_TIMESLOT_ISCP ];  /*!< Interference on Signal Code Power.
                                            INTEGER  (1..127).  OPTIONAL (Present if timeslotISCP_Count > 0). */

} s_gnsSUPL_MeasuredResultTDD;


/*! \struct s_gnsSUPL_WcdmaMeasResultsListFDD
 *  \brief  Structure containing WCDMA FDD (Frequency Division Duplexed) Measurement results list.
 *  \note   Macros used: \ref E_gnsSUPL_MAX_WCDMA_CELL_MEAS.
 */
typedef struct
{
    uint16_t                             v_Uarfcn_DL;                /*!< Down-Link ARFCN (Absolute Radio Frequency Channel Number).
                                                                        INTEGER (0..16383).   OPTIONAL (65535 = 0xFFFF = Not Present/Unknown #K_gnsUNKNOWN_U16). */
    uint16_t                             v_Uarfcn_UL;                /*!< Up-Link   ARFCN (Absolute Radio Frequency Channel Number).
                                                                        INTEGER (0..16383).   OPTIONAL (65535 = 0xFFFF = Not Present/Unknown #K_gnsUNKNOWN_U16).
                                                                        v_Uarfcn_UL may only be present if v_Uarfcn_DL is also present. */
    uint8_t                              v_UTRACarrierRSS;           /*!< RSSI Value of the cell.
                                                                        INTEGER (0..127).  OPTIONAL (255 = 0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8). */
    uint8_t                              v_NumberOfCellsMeasured;    /*!< Number of cell measurement results provided in #s_gnsSUPL_WcdmaMeasResultsListFDD::a_CellMeasuredResultsList.
                                                                        INTEGER (0..#E_gnsSUPL_MAX_WCDMA_CELL_MEAS). */
    s_gnsSUPL_MeasuredResultFDD      a_CellMeasuredResultsList[ E_gnsSUPL_MAX_WCDMA_CELL_MEAS ];    /*!< Cell measurement results. */
} s_gnsSUPL_WcdmaMeasResultsListFDD;


/*! \struct s_gnsSUPL_WcdmaMeasResultsListTDD
 *  \brief  Structure which WCDMA Measured (Time Division Duplexed) results list.
 *  \note   Macros used: \ref E_gnsSUPL_MAX_WCDMA_CELL_MEAS.
 */
typedef struct
{
    uint16_t                             v_Uarfcn_Nt;                /*!< Frequency information.
                                                                        INTEGER (0..16383).  OPTIONAL (65535 = 0xFFFF = Not Present/Unknown #K_gnsUNKNOWN_U16). */
    uint8_t                              v_UTRACarrierRSS;           /*!< RSSI value of the cell.
                                                                        INTEGER (0..127).    OPTIONAL (255 = 0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8). */
    uint8_t                              v_NumberOfCellsMeasured;    /*!<  Number of cell measurement results provided in #s_gnsSUPL_WcdmaMeasResultsListTDD::a_CellMeasuredResultsList.
                                                                        INTEGER (0..#E_gnsSUPL_MAX_WCDMA_CELL_MEAS). */
    s_gnsSUPL_MeasuredResultTDD     a_CellMeasuredResultsList[ E_gnsSUPL_MAX_WCDMA_CELL_MEAS ];     /*!< Cell measurement results. */
} s_gnsSUPL_WcdmaMeasResultsListTDD;

/*=========================== -Mobile and Network Info constants =======================*/

/*----------------------------------------------------------------------*/
/*! \struct s_gnsSUPL_WcdmaFDDCellInfo
 *  \brief  Structure which defines the mobile information for WCDMA-FDD carrier.
 *  \note   Macros used: \ref E_gnsSUPL_MAX_NUM_WCDMA_FREQUENCY.
 */
typedef struct
{
    uint16_t                             v_MCC;                      /*!< PLMN-MCC value Mobile Country Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t                             v_MNC;                      /*!< PLMN-MNC value Mobile Network Code.  INTEGER (0..999). Invalid Value -1*/
    uint32_t                             v_CellIdentity;             /*!< Cell number identifier of the serving cell, i.e. UC-ID Identity.
                                                                        INTEGER (0..268435455).  UC-ID is composed of RNC-ID and C-ID. */
    uint16_t                             v_Uarfcn_DL;                /*!< Down-Link ARFCN (Absolute Radio Frequency Channel Number).
                                                                        INTEGER (0..16383).   OPTIONAL (65535 = 0xFFFF = Not Present/Unknown #K_gnsUNKNOWN_U16). */
    uint16_t                             v_Uarfcn_UL;                /*!< Up-Link   ARFCN (Absolute Radio Frequency Channel Number).
                                                                        INTEGER (0..16383).   OPTIONAL (65535 = 0xFFFF = Not Present/Unknown #K_gnsUNKNOWN_U16).
                                                                        v_Uarfcn_UL may only be present if v_Uarfcn_DL is also present. */
    uint16_t                             v_PrimaryScramblingCode;    /*!< Primary scrambling code of serving cell.  INTEGER (0..511).
                                                                        OPTIONAL (65535 = 0xFFFF = Not Present/Unknown #K_gnsUNKNOWN_U16). */
    uint8_t                              v_NumNeighbouringCells;     /*!< Number of WCDMA Neighbouring cells for which Measurements Results are available in
                                                                        #s_gnsSUPL_WcdmaFDDCellInfo::a_WcdmaMeasResultsListFDD.
                                                                        INTEGER (0..#E_gnsSUPL_MAX_NUM_WCDMA_FREQUENCY). */
    s_gnsSUPL_WcdmaMeasResultsListFDD a_WcdmaMeasResultsListFDD[ E_gnsSUPL_MAX_NUM_WCDMA_FREQUENCY ];  /*!< WCDMA Neighbour cell
                                                                        (Intra-Frequency and Inter-Frequency) Measurement Report. */
} s_gnsSUPL_WcdmaFDDCellInfo;


/*----------------------------------------------------------------------*/
/*! \struct s_gnsSUPL_WcdmaTDDCellInfo
 *  \brief  Structure which defines the mobile information for WCDMA-TDD carrier.
 *  \note   Macros used: \ref E_gnsSUPL_MAX_NUM_WCDMA_FREQUENCY.
 */
typedef struct
{
    uint16_t                             v_MCC;                      /*!< PLMN-MCC value Mobile Country Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t                             v_MNC;                      /*!< PLMN-MNC value Mobile Network Code.  INTEGER (0..999). Invalid Value -1*/
    uint32_t                             v_CellIdentity;             /*!< Cell number identifier of the serving cell, i.e. UC-ID Identity.
                                                                        INTEGER (0..268435455).  UC-ID is composed of RNC-ID and C-ID. */
    uint16_t                             v_Uarfcn_Nt;                /*!< Frequency information.  INTEGER (0..16383).
                                                                        OPTIONAL (65535 = 0xFFFF = Not Present/Unknown #K_gnsUNKNOWN_U16). */
    uint16_t                             v_CellParameterID;    /*!< Cell Parameters Identity.  INTEGER (0..127).
                                                                        OPTIONAL (65535 = 0xFFFF = Not Present/Unknown #K_gnsUNKNOWN_U16). */
    uint8_t                              v_NumNeighbouringCells;     /*!< Number of WCDMA Neighbouring cells for which Measurement Results are available in
                                                                        #s_gnsSUPL_WcdmaTDDCellInfo::a_WcdmaMeasResultsListTDD.
                                                                        INTEGER (0..#E_gnsSUPL_MAX_NUM_WCDMA_FREQUENCY). */
    s_gnsSUPL_WcdmaMeasResultsListTDD a_WcdmaMeasResultsListTDD[ E_gnsSUPL_MAX_NUM_WCDMA_FREQUENCY ];  /*!< WCDMA Neighbour cell
                                                                        (Intra-Frequency and Inter-Frequency) Measurement Report. */
} s_gnsSUPL_WcdmaTDDCellInfo;


/*----------------------------------------------------------------------*/
/*! \struct s_gnsSUPL_GSMCellInfo
 *  \brief  Structure which defines the mobile information for GSM carrier.
 *  \note   Macros used: \ref E_gnsSUPL_MAX_NUM_GSM_NMR.
 */
typedef struct
{
    uint16_t                             v_MCC;                      /*!< PLMN-MCC value Mobile Country Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t                             v_MNC;                      /*!< PLMN-MNC value Mobile Network Code.  INTEGER (0..999). Invalid Value -1*/
    uint32_t                             v_LAC;                      /*!< Location Area Code of the serving cell.  INTEGER (0..65535). Invalid value is #K_gnsUNKNOWN_U32 */
    uint16_t                             v_CellIdentity;             /*!< GSM Cell number identifier of the serving cell. INTEGER (0..65535).*/
    uint16_t                             v_TimingAdvance;            /*!< Network Timing Advance. INTEGER (0..255).
                                                                    <br>OPTIONAL (65535 = 0xFFFF = Not Present/Unknown #K_gnsUNKNOWN_U16).
                                                                    <br>This information is valid only when Mobile is in dedicated mode. */
    uint8_t                              v_NumNeighbouringCells;     /*!< Number of GSM Neighbouring cells for which Newtork Measurement Reports are available in
                                                                        #s_gnsSUPL_GSMCellInfo::a_GsmNMR.
                                                                        INTEGER (0..#E_gnsSUPL_MAX_NUM_GSM_NMR). */
    s_gnsSUPL_GsmNMR                a_GsmNMR[ E_gnsSUPL_MAX_NUM_GSM_NMR ];  /*!< GSM Neighbour cell Measurement Reports. The number of which is provided in v_NumNeighbouringCells. */
} s_gnsSUPL_GSMCellInfo;


/*----------------------------------------------------------------------*/
/*! \union  u_GnsSuplCellInfo
 *  \brief  Union which defines the cell information.
 */
typedef union
{
    s_gnsSUPL_GSMCellInfo           v_GsmCellInfo;              /*!< GSM Cell Information.
                                                                            <br>Corresponding to #e_gnsSUPL_CellInfoType value #E_gnsSUPL_GSM_CELL. */
    s_gnsSUPL_WcdmaFDDCellInfo      v_WcdmaFDDCellInfo;         /*!< WCDMA-FDD (Frequency Division Duplex) Cell Information.
                                                                            Corresponding to #e_gnsSUPL_CellInfoType value #E_gnsSUPL_WCDMA_FDD_CELL. */
    s_gnsSUPL_WcdmaTDDCellInfo      v_WcdmaTDDCellInfo;         /*!< WCDMA-TDD (Time Division Duplex) Cell Information.
                                                                            Corresponding to #e_gnsSUPL_CellInfoType value #E_gnsSUPL_WCDMA_TDD_CELL. */
} u_GnsSuplCellInfo;


/*----------------------------------------------------------------------*/
/*! \struct s_gnsSUPL_MobileInfo
 *  \brief  Structure which defines mobile information.
 */
typedef struct
{
    e_gnsSUPL_CellInfoType          v_CellInfoType;                 /*!< Cell Id type (GSM, WCDMA-FDD or WCDMA-TDD).  Defines which type of cell the mobile
                                                                        is camped on, and which type of cell information is provided in #u_GnsSuplCellInfo. */
    e_gnsSUPL_RegistrationState     v_RegistrationState;        /*!< Registration state. */
    e_gnsSUPL_CellInfoStatus        v_CellInfoStatus;           /*!< Indicates the validity of the Location Id. */
    u_GnsSuplCellInfo               v_CellInfo;                 /*!< CellInformation. */
    char                               a_IMSI[ K_gnsSUPL_MAX_IMSI_LENGTH ]; /*!< IMSI . If not valid set to zero */
    char                               a_MSISDN[ K_gnsSUPL_MAX_MSISDN_LENGTH ]; /*!< MSISDN . If not valid set to zero */
} s_gnsSUPL_MobileInfo;

/*! \union  u_gnsSUPL_MsgData
 *  \brief  Union which defines the data associated with \ref e_gnsSUPL_MsgType
 */
typedef union
{
    s_gnsSUPL_ConnectReq         v_GnsSuplConnectReq;    /*!< Corresponds to message type #E_gnsSUPL_CONNECT_REQ */
    s_gnsSUPL_DisconnectReq     v_GnsSuplDisconnectReq; /*!< Corresponds to message type #E_gnsSUPL_DISCONNECT_REQ */
    s_gnsSUPL_SendDataReq        v_GnsSuplSendDataReq;   /*!< Corresponds to message type #E_gnsSUPL_SEND_DATA_REQ */
#ifndef GNS_CELL_INFO_FTR
    s_gnsSUPL_MobileInfoReq        v_GnsSuplMobileInfoReq; /*!< Corresponds to message type #E_gnsSUPL_MOBILE_INFO_REQ */
#endif
    s_gnsSUPL_ReceiveDataCnf    v_GnsSuplReceiveDataCnf;/*!< Corresponds to message type #E_gnsSUPL_RECEIVE_DATA_CNF */
} u_gnsSUPL_MsgData ;
/*@}*/

#endif /* GNS_SUPL_TYPEDEFS_H */
