/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief   Type definistion of Service category Packet Switched Services (PSS)
 * 
 */

#ifndef INCLUSION_GUARD_T_PSS_H
#define INCLUSION_GUARD_T_PSS_H

#include "r_sys.h"
#include "t_basicdefinitions.h"
#include "t_catdefinitions.h"
//#include "t_roaming.h"

#ifndef SKIP_IN_REF_MAN
/** This is used to uniquely identify the PSS SWBP category */
#define CAT_ID_PSS 146
CAT_INTERVAL_DEFINITION(CAT_ID_PSS)
#endif //SKIP_IN_REF_MAN

/*************************************************************************/
// Types and constants shared by Control Plane Processes and User Plane 
// Processes

/** The value of an undefined NSAPI. */
#define PS_NSAPI_UNDEFINED 255

/** Bitmask used to extract NSAPI value for PDP context from 32 bit variable. */
#define PS_EXTRACT_PDP_CONTEXT_NSAPI 0x000000FF

/** The value of an undefined broadcast MBMS service NSAPI. */
#define PS_BROADCAST_MBMS_NSAPI_UNDEFINED 0xFF00

/**
 * Bitmask used to extract NSAPI value for (enhanced) broadcast MBMS bearer services
 * service from 32 bit variable.
 */
#define PS_EXTRACT_BROADCAST_MBMS_BEARER_SERVICE_NSAPI 0x0000FF00

/** This data type indicates the identity of an activated PDP context. */
typedef uint8 PS_NSAPI_t;

/**
 * This data type indicates the identity of an activated PDP context or MBMS bearer service.
 * | byte 4 not used | byte 3 to be used for multicast | byte 2 for (enhanced) broadcast | byte 1 for PDP contexts |
 * Possible values are: |0x00|0x00|0x01-0x03|0x05-0x0F|
 * Note that all bytes except the one used will contain all zeros.
 * This construction enables unique NSAPI values within UE even for broadcast MBMS bearer services.
 */
typedef uint32 PS_Extended_NSAPI_t;



/**
 * This data type defines result codes when addressing an activated PDP context or MBMS bearer service.
 *
 * @param PS_GENERAL_RESULT_OK                      Ok.
 * @param PS_GENERAL_RESULT_UNKNOWN_NSAPI           Unknown NSAPI (for PDP or MBMS).
 * @param PS_GENERAL_RESULT_NSAPI_DEACTIVATED       NSAPI deactivated (for PDP or MBMS).
 * @param PS_GENERAL_RESULT_UNSPECIFIED             Unspecified.
 * @param PS_GENERAL_RESULT_NOT_SUPPORTED           Not supported.
 * @param PS_GENERAL_RESULT_REJECTED                Rejected.
 * @param PS_GENERAL_RESULT_FAILED_PARAMETER        Input parameter faulty/out of range.
 * @param PS_GENERAL_RESULT_MBMS_NW_NOT_SUPPORTED   NW does not support MBMS service.
 * @param PS_GENERAL_RESULT_MBMS_NO_RESOURCE        Maximum number of concurrent MBMS services activated.
 * @param PS_GENERAL_RESULT_MBMS_SERVICE_ALREADY_ACTIVATED Particular MBMS service has already been activated.
 * @param PS_GENERAL_RESULT_MBMS_SERVICE_NOT_ACTIVATED   MBMS service is not activated.
 * @param PS_GENERAL_RESULT_MBMS_NO_ACTIVE_SESSION  Session receive is requested in an invalid state.
 */
TYPEDEF_ENUM
{
  PS_GENERAL_RESULT_OK = 0,
  PS_GENERAL_RESULT_UNKNOWN_NSAPI,
  PS_GENERAL_RESULT_NSAPI_DEACTIVATED,
  PS_GENERAL_RESULT_UNSPECIFIED,
  PS_GENERAL_RESULT_NOT_SUPPORTED,
  PS_GENERAL_RESULT_REJECTED,
  PS_GENERAL_RESULT_FAILED_PARAMETER,
  PS_GENERAL_RESULT_MBMS_NW_NOT_SUPPORTED,
  PS_GENERAL_RESULT_MBMS_NO_RESOURCE,
  PS_GENERAL_RESULT_MBMS_SERVICE_ALREADY_ACTIVATED,
  PS_GENERAL_RESULT_MBMS_SERVICE_NOT_ACTIVATED,
  PS_GENERAL_RESULT_MBMS_NO_ACTIVE_SESSION
} SIGNED_ENUM8(PS_GeneralResult_t);

/**
 * Defines the maximum SDU size (in bytes) of an uplink or a downlink SDU that 
 * can be exchanged between the service user and the service provider
 *
 * PS_MAX_SDU_SIZE is the maximum integer value in bytes/octets
 *
 * The maximum SDU size differs depending on the PDP type. The values are 1500
 * bytes for IPv4/IPv6 and 1502 for PPP. 
 * Note due to 3GPP supporting values up to 1520 bytes this value is used as max. 
 * This in order to facilitate interoperability despite no use case currently exist.
 */
#define PS_MAX_SDU_SIZE 1520

/**
 * Defines the maximum SDU size of an uplink or a downlink SDU that 
 * can be exchanged between the service user and the service provider
 *
 * PS_MAX_QOS_SDU_SIZE is the equivalent 3GPP coded value (to the PS_MAX_SDU_SIZE)
 * that can be set in the parameter MaxSDU_Size in PS_QOS_Profile_t.
 *
 * The maximum SDU size differs depending on the PDP type. The values are 1500
 * bytes for IPv4/IPv6 and 1502 for PPP.
 * Note due to 3GPP supporting values up to 1520 bytes this value is used as max. 
 * This in order to facilitate interoperability despite no use case currently exist.
 */
#define PS_MAX_QOS_SDU_SIZE 153

/**
 * This data type defines the PS user plane suspend cause. If the PS user
 * plane is suspended then this type describes the reason for the suspend.
 *
 * @param PS_USER_PLANE_SUSPEND_CAUSE_UNSPECIFIED
 *        The PS user plane is not suspended or no active PDP context
 *
 * @param PS_USER_PLANE_SUSPEND_CAUSE_CHANNEL_RECONFIGURATION
 *        When the network requests the UE to change RRC state in Connected
 *        mode or to change RAB configurations. This is normal control signalling
 *        cases where short suspend times are expected
 *
 * @param PS_USER_PLANE_SUSPEND_CAUSE_CHANNEL_FAILURE
 *        When the Radio Link is broken due to poor radio conditions. Longer
 *        recovery times could be expected
 * 
 * @param PS_USER_PLANE_SUSPEND_CAUSE_CHANNEL_ESTABLISHMENT
 *        RAB establishment in WCDMA is ongoing
 * 
 * @param PS_USER_PLANE_SUSPEND_CAUSE_CELL_RESELECTION
 *        Cell selection/reselection is ongoing
 *
 * @param PS_USER_PLANE_SUSPEND_CAUSE_REGISTRATION_PROCEDURE
 *        Ongoing PS or CS registration procedure (e.g. Routing Area Update
 *        procedure or Location Updating procedure in GSM/GPRS when no DTM support
 *        in cell)
 * 
 * @param PS_USER_PLANE_SUSPEND_CAUSE_INTER_SYSTEM_CHANGE
 *        Inter-system change is ongoing
 *
 * @param PS_USER_PLANE_SUSPEND_CAUSE_CS_CALL
 *        A CS call in GSM/GPRS is ongoing, no DTM support in cell
 *
 * @param PS_USER_PLANE_SUSPEND_CAUSE_NO_CELL_OR_NO_ACCESS_IN_CELL
 *        No coverage, no GPRS support in cell, forbidden PLMN/LA, cell barred or PLMN scan
 */
TYPEDEF_ENUM
{
  PS_USER_PLANE_SUSPEND_CAUSE_UNSPECIFIED = 0,
  PS_USER_PLANE_SUSPEND_CAUSE_CHANNEL_RECONFIGURATION,
  PS_USER_PLANE_SUSPEND_CAUSE_CHANNEL_FAILURE,
  PS_USER_PLANE_SUSPEND_CAUSE_CHANNEL_ESTABLISHMENT,
  PS_USER_PLANE_SUSPEND_CAUSE_CELL_RESELECTION,
  PS_USER_PLANE_SUSPEND_CAUSE_REGISTRATION_PROCEDURE,
  PS_USER_PLANE_SUSPEND_CAUSE_INTER_SYSTEM_CHANGE,
  PS_USER_PLANE_SUSPEND_CAUSE_CS_CALL,
  PS_USER_PLANE_SUSPEND_CAUSE_NO_CELL_OR_NO_ACCESS_IN_CELL
}SIGNED_ENUM8(PS_UserPlaneSuspendCause_t);

/**
 * In the current implementation the PS user plane estimated suspend time
 * is always set to unknown.
 */
#define PS_USER_PLANE_SUSPEND_TIME_UNKNOWN 0

/**
 * This data type defines the PS user plane type. It is a bitmap where each
 * bit defines a certain property of the user plane.
 * 
 * @param PS_USER_PLANE_TYPE_HSDPA    HSDPA
 * @param PS_USER_PLANE_TYPE_GPRS     GPRS
 * @param PS_USER_PLANE_TYPE_EGPRS    E-GPRS
 * @param PS_USER_PLANE_TYPE_HSUPA    HSUPA
 * @param PS_USER_PLANE_TYPE_WCDMA    WCDMA
 */ 
typedef uint32 PS_UserPlaneType_t; 

#define PS_USER_PLANE_TYPE_HSDPA  0x00000001
#define PS_USER_PLANE_TYPE_GPRS   0x00000002
#define PS_USER_PLANE_TYPE_EGPRS  0x00000004
#define PS_USER_PLANE_TYPE_HSUPA  0x00000008
#define PS_USER_PLANE_TYPE_WCDMA  0x00000010

/*************************************************************************/
// Types and constants used only by Control Plane Processes

// Type of the Packet Data Protocol (PDP)

/**
 * This data type defines the type of the Packet Data Protocol.
 *
 * @param PS_PDP_TYPE_UNSPECIFIED     Unspecified.
 * @param PS_PDP_TYPE_IP_V4           IP-V4.
 * @param PS_PDP_TYPE_IP_V6           IP-V6.
 * @param PS_PDP_TYPE_PPP             PPP.
 */
TYPEDEF_ENUM
{
  PS_PDP_TYPE_UNSPECIFIED = 0,
  PS_PDP_TYPE_IP_V4,
  PS_PDP_TYPE_IP_V6,
  PS_PDP_TYPE_PPP
}SIGNED_ENUM8(PS_PDP_Type_t);

// PDP Address

/** Defines the maximum length of a PDP address. */
#define PS_MAX_PDP_ADDRESS_LENGTH         16

/**
 * The number of valid elements for the PDP address of PDP type IPv4
 * Valid elements are addressed in ascending order from location zero upwards
 * with the most significant bit of the IP address contained in bit 8 of
 * octet 0 (zero).
 */
#define PS_PDP_TYPE_IP_V4_LENGTH           4

/**
 * The number of valid elements for the PDP address of PDP type IPv6
 * Valid elements are addressed in ascending order from location zero upwards
 * with the most significant bit of the IP address contained in bit 8 of
 * octet 0 (zero). 
 */
#define PS_PDP_TYPE_IP_V6_LENGTH          16

/** This data type defines the PDP address. */
typedef uint8 PS_PDP_Address_t[PS_MAX_PDP_ADDRESS_LENGTH];

/**
 * Value that a Service User wanting a dynamic IP address shall set
 * when requesting the activation of a PDP context
 */
#define PS_PDP_ADDRESS_UNSPECIFIED         0

/**
 * Defines the maximum length of an Access Point Name (APN) parameter string.
 */
#define PS_MAX_APN_LENGTH                 99

/**
 * The maximum length of a protocol configuration options parameter string.
 */ 
#define PS_MAX_PROTOCOL_OPTIONS_LENGTH   251

// Header compression flag

/**
 * This data type defines a flag that the service user can use to request
 * that the service provider tries to negotiate a header compression
 * algorithm for the user plane when setting up a PDP context over GSM or
 * when a handover to GSM is performed.
 *
 * @param PS_HEADER_COMPRESSION_NO_COMPRESSION  No header compression.
 * @param PS_HEADER_COMPRESSION_REQUESTED       Header compression requested.
 */
TYPEDEF_ENUM
{
  PS_HEADER_COMPRESSION_NO_COMPRESSION = 0,
  PS_HEADER_COMPRESSION_REQUESTED
}SIGNED_ENUM8(PS_HeaderCompression_t);

#ifndef SKIP_IN_REF_MAN
#define PS_NO_HEADER_COMPRESSION PS_HEADER_COMPRESSION_NO_COMPRESSION
#define PS_HdrCompression_t PS_HeaderCompression_t
#endif //SKIP_IN_REF_MAN

// Data compression flag

/**
 * This data type defines a flag that the service user can use to request
 * that the service provider tries to negotiate a data compression
 * algorithm for the user plane when setting up a PDP context over GSM or
 * when a handover to GSM is performed.
 *
 * @param PS_DATA_COMPRESSION_NO_COMPRESSION   No data compression.
 * @param PS_DATA_COMPRESSION_REQUESTED        Data compression requested.
 */
TYPEDEF_ENUM
{
  PS_DATA_COMPRESSION_NO_COMPRESSION = 0,
  PS_DATA_COMPRESSION_REQUESTED
}SIGNED_ENUM8(PS_DataCompression_t);

#ifndef SKIP_IN_REF_MAN
#define PS_NO_DATA_COMPRESSION PS_DATA_COMPRESSION_NO_COMPRESSION
#endif //SKIP_IN_REF_MAN

// Quality of Service (QoS)

/**
 * This data type specifies relative importance compared to other UMTS bearers for 
 * allocation and retention of the UMTS bearer. 
 *
 * This is an R97 QoS attribute, for UEs based on R99 (or later) this corresponds to 
 * allocation / retention priority
 *
 * @param PS_PRECEDENCE_CLASS_UNSPECIFIED         unspecified, use subscribed value
 * @param PS_PRECEDENCE_CLASS_HIGH                High.
 * @param PS_PRECEDENCE_CLASS_NORMAL              Normal.
 * @param PS_PRECEDENCE_CLASS_LOW                 Low.
 */
TYPEDEF_ENUM
{
  PS_PRECEDENCE_CLASS_UNSPECIFIED = 0,
  PS_PRECEDENCE_CLASS_HIGH,
  PS_PRECEDENCE_CLASS_NORMAL,
  PS_PRECEDENCE_CLASS_LOW
}SIGNED_ENUM8(PS_PrecedenceClass_t);

#ifndef SKIP_IN_REF_MAN
#define PS_PRECEDENCE_UNSPECIFIED PS_PRECEDENCE_CLASS_UNSPECIFIED
#define PS_PRECEDENCE_HIGH PS_PRECEDENCE_CLASS_HIGH
#define PS_PRECEDENCE_NORMAL PS_PRECEDENCE_CLASS_NORMAL
#define PS_PRECEDENCE_LOW PS_PRECEDENCE_CLASS_LOW
#endif //SKIP_IN_REF_MAN

/**
 * This data type is used to specify requested delay for a PDP context.
 *
 * This is an R97 QoS attribute, for UEs based on R99 (or later) this corresponds to 
 * a combination of Traffic class and traffic handling priority
 *
 * @param PS_DELAY_CLASS_UNSPECIFIED        unspecified, use subscribed value
 * @param PS_DELAY_CLASS_1                  Class 1 (least delay)
 * @param PS_DELAY_CLASS_2                  Class 2.
 * @param PS_DELAY_CLASS_3                  Class 3.
 * @param PS_DELAY_CLASS_4                  Class 4.(best effort)
 */
TYPEDEF_ENUM
{
  PS_DELAY_CLASS_UNSPECIFIED = 0,
  PS_DELAY_CLASS_1,
  PS_DELAY_CLASS_2,
  PS_DELAY_CLASS_3,
  PS_DELAY_CLASS_4
}SIGNED_ENUM8(PS_DelayClass_t);

/**
 * This data type is used to specify reliability class for PDP contexts
 *
 * This is an R97 QoS attribute, for UEs based on R99 (or later) this corresponds to 
 * a combination of SDU error ratio, residual bit error ratio and delivery of 
 * erroneous SDUs
 *
 * @param PS_RELIABILITY_CLASS_UNSPECIFIED                    use subscribed value
 *
 * @param PS_RELIABILITY_CLASS_ACK_GTP_LLC_RLC_PROTECT        acknowledged protocols (GTP, LLC and RLC), 
 *                                                            data is protected
 *
 * @param PS_RELIABILITY_CLASS_UNACK_GTP_ACK_LLC_RLC_PROTECT  both unacknowledged (GTP) and acknowledged 
 *                                                            protocols (LLC, RLC), data is protected
 *
 * @param PS_RELIABILITY_CLASS_UNACK_GTP_LLC_ACK_RLC_PROTECT  both unacknowledged (GTP and LLC) and acknowledged 
 *                                                            protocols (RLC), data is protected
 *
 * @param PS_RELIABILITY_CLASS_UNACK_GTP_LLC_RLC_PROTECT      only unacknowledged protocols (GTP, LLC and RLC), 
 *                                                            data is protected
 *
 * @param PS_RELIABILITY_CLASS_UNACK_GTP_LLC_RLC_UNPROTECT    only unacknowledged protocols (GTP, LLC and RLC), 
 *                                                            data is not protected
 */
TYPEDEF_ENUM
{
  PS_RELIABILITY_CLASS_UNSPECIFIED = 0,
  PS_RELIABILITY_CLASS_ACK_GTP_LLC_RLC_PROTECT,
  PS_RELIABILITY_CLASS_UNACK_GTP_ACK_LLC_RLC_PROTECT,
  PS_RELIABILITY_CLASS_UNACK_GTP_LLC_ACK_RLC_PROTECT,
  PS_RELIABILITY_CLASS_UNACK_GTP_LLC_RLC_PROTECT,
  PS_RELIABILITY_CLASS_UNACK_GTP_LLC_RLC_UNPROTECT
}SIGNED_ENUM8(PS_ReliabilityClass_t);

#ifndef SKIP_IN_REF_MAN
#define PS_RELIABILITY_UNSPECIFIED PS_RELIABILITY_CLASS_UNSPECIFIED
#define PS_ACK_GTP_LLC_RLC_PROTECT PS_RELIABILITY_CLASS_ACK_GTP_LLC_RLC_PROTECT
#define PS_UNACK_GTP_ACK_LLC_RLC_PROTECT PS_RELIABILITY_CLASS_UNACK_GTP_ACK_LLC_RLC_PROTECT
#define PS_UNACK_GTP_LLC_ACK_RLC_PROTECT PS_RELIABILITY_CLASS_UNACK_GTP_LLC_ACK_RLC_PROTECT
#define PS_UNACK_GTP_LLC_RLC_PROTECT PS_RELIABILITY_CLASS_UNACK_GTP_LLC_RLC_PROTECT
#define PS_UNACK_GTP_LLC_RLC_UNPROTECT PS_RELIABILITY_CLASS_UNACK_GTP_LLC_RLC_UNPROTECT
#endif //SKIP_IN_REF_MAN

/**
 * This data type is used to specify peak throughput values for PDP contexts.
 *
 * This is an R97 QoS attribute, for UEs based on R99 (or later) this corresponds to 
 * maximum bitrate (same for both UL and DL)
 *
 * @param PS_PEAK_THROUGHPUT_CLASS_UNSPECIFIED         unspecified, use subscribed value
 * @param PS_PEAK_THROUGHPUT_CLASS_1K_OCTETS_PR_SEC    1K octets/s
 * @param PS_PEAK_THROUGHPUT_CLASS_2K_OCTETS_PR_SEC    2K octets/s
 * @param PS_PEAK_THROUGHPUT_CLASS_4K_OCTETS_PR_SEC    4K octets/s
 * @param PS_PEAK_THROUGHPUT_CLASS_8K_OCTETS_PR_SEC    8K octets/s
 * @param PS_PEAK_THROUGHPUT_CLASS_16K_OCTETS_PR_SEC   16K octets/s
 * @param PS_PEAK_THROUGHPUT_CLASS_32K_OCTETS_PR_SEC   32K octets/s
 * @param PS_PEAK_THROUGHPUT_CLASS_64K_OCTETS_PR_SEC   64K octets/s
 * @param PS_PEAK_THROUGHPUT_CLASS_128K_OCTETS_PR_SEC  128K octets/s
 * @param PS_PEAK_THROUGHPUT_CLASS_256K_OCTETS_PR_SEC  256K octets/s
 */
TYPEDEF_ENUM
{
  PS_PEAK_THROUGHPUT_CLASS_UNSPECIFIED = 0,
  PS_PEAK_THROUGHPUT_CLASS_1K_OCTETS_PR_SEC,
  PS_PEAK_THROUGHPUT_CLASS_2K_OCTETS_PR_SEC,
  PS_PEAK_THROUGHPUT_CLASS_4K_OCTETS_PR_SEC,
  PS_PEAK_THROUGHPUT_CLASS_8K_OCTETS_PR_SEC,
  PS_PEAK_THROUGHPUT_CLASS_16K_OCTETS_PR_SEC,
  PS_PEAK_THROUGHPUT_CLASS_32K_OCTETS_PR_SEC,
  PS_PEAK_THROUGHPUT_CLASS_64K_OCTETS_PR_SEC,
  PS_PEAK_THROUGHPUT_CLASS_128K_OCTETS_PR_SEC,
  PS_PEAK_THROUGHPUT_CLASS_256K_OCTETS_PR_SEC
}SIGNED_ENUM8(PS_PeakThroughputClass_t); 

#ifndef SKIP_IN_REF_MAN
#define PS_PEAK_UNSPECIFIED PS_PEAK_THROUGHPUT_CLASS_UNSPECIFIED
#define PS_PEAK_1K_OCTETS_PR_SEC PS_PEAK_THROUGHPUT_CLASS_1K_OCTETS_PR_SEC
#define PS_PEAK_2K_OCTETS_PR_SEC PS_PEAK_THROUGHPUT_CLASS_2K_OCTETS_PR_SEC
#define PS_PEAK_4K_OCTETS_PR_SEC PS_PEAK_THROUGHPUT_CLASS_4K_OCTETS_PR_SEC
#define PS_PEAK_8K_OCTETS_PR_SEC PS_PEAK_THROUGHPUT_CLASS_8K_OCTETS_PR_SEC
#define PS_PEAK_16K_OCTETS_PR_SEC PS_PEAK_THROUGHPUT_CLASS_16K_OCTETS_PR_SEC
#define PS_PEAK_32K_OCTETS_PR_SEC PS_PEAK_THROUGHPUT_CLASS_32K_OCTETS_PR_SEC
#define PS_PEAK_64K_OCTETS_PR_SEC PS_PEAK_THROUGHPUT_CLASS_64K_OCTETS_PR_SEC
#define PS_PEAK_128K_OCTETS_PR_SEC PS_PEAK_THROUGHPUT_CLASS_128K_OCTETS_PR_SEC
#define PS_PEAK_256K_OCTETS_PR_SEC PS_PEAK_THROUGHPUT_CLASS_256K_OCTETS_PR_SEC
#endif //SKIP_IN_REF_MAN

/**
 * This data type is used to specify mean throughput values for PDP contexts.
 *
 * This is an R97 QoS attribute, for UEs based on R99 (or later) there are no 
 * corresponding attribute
 *
 * @param PS_MEAN_THROUGHPUT_CLASS_UNSPECIFIED           unspecified, use subscribed value
 * @param PS_MEAN_THROUGHPUT_CLASS_100_OCTETS_PR_HOUR    100 octets/h
 * @param PS_MEAN_THROUGHPUT_CLASS_200_OCTETS_PR_HOUR    200 octets/h
 * @param PS_MEAN_THROUGHPUT_CLASS_500_OCTETS_PR_HOUR    500 octets/h
 * @param PS_MEAN_THROUGHPUT_CLASS_1K_OCTETS_PR_HOUR     1K octets/h
 * @param PS_MEAN_THROUGHPUT_CLASS_2K_OCTETS_PR_HOUR     2K octets/h
 * @param PS_MEAN_THROUGHPUT_CLASS_5K_OCTETS_PR_HOUR     5K octets/h
 * @param PS_MEAN_THROUGHPUT_CLASS_10K_OCTETS_PR_HOUR    10K octets/h
 * @param PS_MEAN_THROUGHPUT_CLASS_20K_OCTETS_PR_HOUR    20K octets/h
 * @param PS_MEAN_THROUGHPUT_CLASS_50K_OCTETS_PR_HOUR    50K octets/h
 * @param PS_MEAN_THROUGHPUT_CLASS_100K_OCTETS_PR_HOUR   100K octets/h.
 * @param PS_MEAN_THROUGHPUT_CLASS_200K_OCTETS_PR_HOUR   200K octets/h.
 * @param PS_MEAN_THROUGHPUT_CLASS_500K_OCTETS_PR_HOUR   500K octets/h.
 * @param PS_MEAN_THROUGHPUT_CLASS_1M_OCTETS_PR_HOUR     1M octets/h.
 * @param PS_MEAN_THROUGHPUT_CLASS_2M_OCTETS_PR_HOUR     2M octets/h.
 * @param PS_MEAN_THROUGHPUT_CLASS_5M_OCTETS_PR_HOUR     5M octets/h.
 * @param PS_MEAN_THROUGHPUT_CLASS_10M_OCTETS_PR_HOUR    10M octets/h.
 * @param PS_MEAN_THROUGHPUT_CLASS_20M_OCTETS_PR_HOUR    20M octets/h.
 * @param PS_MEAN_THROUGHPUT_CLASS_50M_OCTETS_PR_HOUR    50M octets/h.
 * @param PS_MEAN_THROUGHPUT_CLASS_BEST_EFFORT           Best effort.
 */
TYPEDEF_ENUM
{
  PS_MEAN_THROUGHPUT_CLASS_UNSPECIFIED = 0,
  PS_MEAN_THROUGHPUT_CLASS_100_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_200_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_500_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_1K_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_2K_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_5K_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_10K_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_20K_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_50K_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_100K_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_200K_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_500K_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_1M_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_2M_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_5M_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_10M_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_20M_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_50M_OCTETS_PR_HOUR,
  PS_MEAN_THROUGHPUT_CLASS_BEST_EFFORT = 31
}SIGNED_ENUM8(PS_MeanThroughputClass_t);

#ifndef SKIP_IN_REF_MAN
#define PS_MEAN_UNSPECIFIED PS_MEAN_THROUGHPUT_CLASS_UNSPECIFIED
#define PS_MEAN_100_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_100_OCTETS_PR_HOUR
#define PS_MEAN_200_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_200_OCTETS_PR_HOUR
#define PS_MEAN_500_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_500_OCTETS_PR_HOUR
#define PS_MEAN_1K_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_1K_OCTETS_PR_HOUR
#define PS_MEAN_2K_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_2K_OCTETS_PR_HOUR
#define PS_MEAN_5K_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_5K_OCTETS_PR_HOUR
#define PS_MEAN_10K_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_10K_OCTETS_PR_HOUR
#define PS_MEAN_20K_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_20K_OCTETS_PR_HOUR
#define PS_MEAN_50K_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_50K_OCTETS_PR_HOUR
#define PS_MEAN_100K_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_100K_OCTETS_PR_HOUR
#define PS_MEAN_200K_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_200K_OCTETS_PR_HOUR
#define PS_MEAN_500K_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_500K_OCTETS_PR_HOUR
#define PS_MEAN_1M_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_1M_OCTETS_PR_HOUR
#define PS_MEAN_2M_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_2M_OCTETS_PR_HOUR
#define PS_MEAN_5M_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_5M_OCTETS_PR_HOUR
#define PS_MEAN_10M_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_10M_OCTETS_PR_HOUR
#define PS_MEAN_20M_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_20M_OCTETS_PR_HOUR
#define PS_MEAN_50M_OCTETS_PR_HOUR PS_MEAN_THROUGHPUT_CLASS_50M_OCTETS_PR_HOUR
#define PS_MEAN_BEST_EFFORT PS_MEAN_THROUGHPUT_CLASS_BEST_EFFORT
#endif //SKIP_IN_REF_MAN

/**
 * This data type is used to specify traffic class for PDP contexts.
 * Based on traffic class UTRAN can make assumptions about the traffic 
 * source and optimize transport for that traffic type
 *
 * @param PS_TRAFFIC_CLASS_UNSPECIFIED      unspecified, use subscribed value
 * @param PS_TRAFFIC_CLASS_CONVERSATIONAL   Conversational.
 * @param PS_TRAFFIC_CLASS_STREAMING        Streaming.
 * @param PS_TRAFFIC_CLASS_INTERACTIVE      Interactive.
 * @param PS_TRAFFIC_CLASS_BACKGROUND       Background.
 */
TYPEDEF_ENUM
{
  PS_TRAFFIC_CLASS_UNSPECIFIED = 0,
  PS_TRAFFIC_CLASS_CONVERSATIONAL,
  PS_TRAFFIC_CLASS_STREAMING,
  PS_TRAFFIC_CLASS_INTERACTIVE,
  PS_TRAFFIC_CLASS_BACKGROUND
}SIGNED_ENUM8(PS_TrafficClass_t);

/**
 * This data type is used to specify if out-of-sequence SDUs are acceptable 
 * or not.
 *
 * @param PS_DELIVERY_ORDER_UNSPECIFIED   unspecified, use subscribed value
 * @param PS_DELIVERY_ORDER_YES           Delivery order.
 * @param PS_DELIVERY_ORDER_NO            No delivery order.
 */
TYPEDEF_ENUM
{
  PS_DELIVERY_ORDER_UNSPECIFIED = 0,
  PS_DELIVERY_ORDER_YES,
  PS_DELIVERY_ORDER_NO
}SIGNED_ENUM8(PS_DeliveryOrder_t);

/**
 * This data type is used to indicate whether SDUs detected as erroneous 
 * shall be delivered or discarded.
 *
 * @param PS_DELIVERY_ERRONEOUS_SDU_UNSPECIFIED   unspecified, use subscribed value
 * @param PS_DELIVERY_ERRONEOUS_SDU_NO_DETECT     Erroneous SDUs are not detected
 * @param PS_DELIVERY_ERRONEOUS_SDU_YES           Erroneous SDUs are delivered
 * @param PS_DELIVERY_ERRONEOUS_SDU_NO            Erroneous SDUs are not delivered
 */
TYPEDEF_ENUM
{
  PS_DELIVERY_ERRONEOUS_SDU_UNSPECIFIED = 0,
  PS_DELIVERY_ERRONEOUS_SDU_NO_DETECT,
  PS_DELIVERY_ERRONEOUS_SDU_YES,
  PS_DELIVERY_ERRONEOUS_SDU_NO
}SIGNED_ENUM8(PS_DeliveryErroneousSDU_t);

/** 
 * This define is used when clients want to request subscribed SDU size when setting
 * up a PDP context 
 */
#define PS_MAX_SDU_SIZE_UNSPECIFIED 0

/** 
 * The maximum SDU size is coded within an 8 bit wide parameter. 
 * Assume the value X of maximum SDU size parameter then the following coding applies:
 *\code
 *
 *  0       subscribed value
 *
 *  1-150   value range of 10-1500 octets binary coded with 10 octets granularity, 
 *          i.e. (X*10) octets
 *
 *  151     1502 octets
 *
 *  152     1510 octets
 *
 *  153     1520 octets
 *
 *\endcode
 * Note that the value range supported by the service provider is 0-151, where the last value 151 only is
 * allowed in case the PDP type is PPP.
 */
typedef uint8 PS_MaxSDU_Size_t;

/** Value used when setting max bitrate in QoS (request or minimum) to subscribed value. */
#define PS_MAX_BITRATE_UNSPECIFIED    0
/** Value used to indicate that extended (3GPP Rel5) max bitrates are not used in QoS. */
#define PS_MAX_EXTENDED_BITRATE_DOWNLINK_UNSPECIFIED 0
/** Value used to indicate max value of extended (3GPP Rel5) max bitrates used in QoS. */
#define PS_MAX_EXTENDED_BITRATE_DOWNLINK 0x4a

/** 
 * The bit rate is coded within an 8 bit wide parameter. 
 * Assume the value X of the bit rate parameter then the following coding applies:
 *\code
 *
 *  0         subscribed value
 *
 *  1-63      value range of 1-63 kbps binary coded with 1 kbps granularity,
 *            i.e. (X*1) kbps
 *
 *  64-127    value range of 64-568 kbps binary coded with 8 kbps granularity, 
 *            i.e. (64+(X-64)*8) kbps
 *
 *  128-254   value range of 576-8640 kbps binary coded with 64 kbps granularity, 
 *            i.e. (576+(X-128)*64) kbps
 *
 *  255       0 kbps
 *
 *\endcode
 */
typedef uint8 PS_MaxBitRateUplink_t;

/** 
 * The bit rate is coded according to the definition for PS_MaxBitRateUplink_t. 
 * If a Maximum bit rate for downlink higher than 8640 kbps is wanted this parameter shall be set to 254, i.e. 8640 kbps 
 * and the parameter of type PS_ExtendedMaxBitRateDownlink_t should be set to a non-zero value.
 * @see PS_MaxBitRateUplink_t
 * @see PS_ExtendedMaxBitRateDownlink_t
 */
typedef uint8 PS_MaxBitRateDownlink_t;

/** 
 * The extended bit rate is coded within 7 bits of an 8 bit wide parameter. 
 * Assume the value X of the extended bit rate parameter then the following coding applies:
 * \code
 *
 *   0        the bit rate is not extended, i.e. use PS_MaxBitRateDownlink_t. 
 *
 *   1-74     value range of 8700-16000 kbps binary coded with 100 kbps granularity, 
 *            i.e. (8600+X*100) kbps. 
 *
 *   75-255   reserved
 *
 * \endcode
 * @see PS_MaxBitRateDownlink_t
 * @see PS_MAX_EXTENDED_BITRATE_DOWNLINK_UNSPECIFIED
 * @see PS_MAX_EXTENDED_BITRATE_DOWNLINK
 */
typedef uint8 PS_ExtendedMaxBitRateDownlink_t;

/**
 * This data type is used to indicate undetected bit error ratio in the delivered SDUs. 
 * If no error detection is requested, Residual bit error ratio indicates the bit error 
 * ratio in the delivered SDUs. It is used by client to request a certain value during 
 * PDP context setup 
 *
 * @param PS_RESIDUAL_BER_UNSPECIFIED   unspecified, use subscribed value
 * @param PS_RESIDUAL_BER_5_E_NEG2      5E-2
 * @param PS_RESIDUAL_BER_1_E_NEG2      1E-2
 * @param PS_RESIDUAL_BER_5_E_NEG3      5E-3
 * @param PS_RESIDUAL_BER_4_E_NEG3      4E-3
 * @param PS_RESIDUAL_BER_1_E_NEG3      1E-3
 * @param PS_RESIDUAL_BER_1_E_NEG4      1E-4
 * @param PS_RESIDUAL_BER_1_E_NEG5      1E-5
 * @param PS_RESIDUAL_BER_1_E_NEG6      1E-6
 * @param PS_RESIDUAL_BER_6_E_NEG8      6E-8
 */
TYPEDEF_ENUM
{
  PS_RESIDUAL_BER_UNSPECIFIED = 0,
  PS_RESIDUAL_BER_5_E_NEG2,
  PS_RESIDUAL_BER_1_E_NEG2,
  PS_RESIDUAL_BER_5_E_NEG3,
  PS_RESIDUAL_BER_4_E_NEG3,
  PS_RESIDUAL_BER_1_E_NEG3,
  PS_RESIDUAL_BER_1_E_NEG4,
  PS_RESIDUAL_BER_1_E_NEG5,
  PS_RESIDUAL_BER_1_E_NEG6,
  PS_RESIDUAL_BER_6_E_NEG8
}SIGNED_ENUM8(PS_ResidualBER_t);

/**
 * This data type is used to indicate the fraction of SDUs lost or detected as erroneous. 
 * SDU error ratio is defined only for conforming traffic.
 * 
 * By reserving resources, SDU error ratio performance is independent of the loading conditions, 
 * whereas without reserved resources, such as in Interactive and Background classes, SDU error 
 * ratio is used as target value.
 *
 * @param PS_SDU_ERROR_RATIO_UNSPECIFIED   unspecified, use subscribed value
 * @param PS_SDU_ERROR_RATIO_1_E_NEG2      1E-2
 * @param PS_SDU_ERROR_RATIO_7_E_NEG3      7E-3
 * @param PS_SDU_ERROR_RATIO_1_E_NEG3      1E-3
 * @param PS_SDU_ERROR_RATIO_1_E_NEG4      1E-4
 * @param PS_SDU_ERROR_RATIO_1_E_NEG5      1E-5
 * @param PS_SDU_ERROR_RATIO_1_E_NEG6      1E-6
 * @param PS_SDU_ERROR_RATIO_1_E_NEG1      1E-1
 */
TYPEDEF_ENUM
{
  PS_SDU_ERROR_RATIO_UNSPECIFIED = 0,
  PS_SDU_ERROR_RATIO_1_E_NEG2,
  PS_SDU_ERROR_RATIO_7_E_NEG3,
  PS_SDU_ERROR_RATIO_1_E_NEG3,
  PS_SDU_ERROR_RATIO_1_E_NEG4,
  PS_SDU_ERROR_RATIO_1_E_NEG5,
  PS_SDU_ERROR_RATIO_1_E_NEG6,
  PS_SDU_ERROR_RATIO_1_E_NEG1
}SIGNED_ENUM8(PS_SDU_ErrorRatio_t);

/** 
 * This define is used when clients want to request subscribed transfer delay when setting
 * up a PDP context 
 */
#define PS_TRANSFER_DELAY_UNSPECIFIED 0

/** 
 * The transfer delay is coded within 6 bits of an 8 bit wide parameter. 
 * Assume the value X of the transfer delay parameter then the following coding applies:
 *\code
 *
 *   0       subscribed value
 *
 *   1-15    value range of 10-150 ms binary coded with 10 ms granularity, 
 *           i.e. (X*10) ms
 *
 *   16-31   value range of 200-950 ms binary coded with 50 ms granularity, 
 *           i.e. (200+(X-16)*50) ms
 *
 *   32-62   value range of 1000-4000 ms binary coded with 100 ms granularity, 
 *           i.e. (1000+(X-32)*100) ms
 *
 *   63      reserved
 *
 *\endcode
 */
typedef uint8 PS_TransferDelay_t;

/**
 * This data type is used to specify the relative importance for handling of all SDUs belonging 
 * to the UMTS bearer compared to the SDUs of other bearers. 
 * 
 * This is only valid for Interactive traffic class. 
 *
 * @param PS_TRAFFIC_HANDLING_PRIORITY_UNSPECIFIED   unspecified, use subscribed value.
 * @param PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_1       Level 1.(highest)
 * @param PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_2       Level 2.
 * @param PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_3       Level 3.(lowest)
 */
TYPEDEF_ENUM
{
  PS_TRAFFIC_HANDLING_PRIORITY_UNSPECIFIED = 0,
  PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_1,
  PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_2,
  PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_3
}SIGNED_ENUM8(PS_TrafficHandlingPriority_t);

#ifndef SKIP_IN_REF_MAN
#define PS_PRIORITY_UNSPECIFIED PS_TRAFFIC_HANDLING_PRIORITY_UNSPECIFIED 
#define PS_PRIORITY_LEVEL_1 PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_1
#define PS_PRIORITY_LEVEL_2 PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_2
#define PS_PRIORITY_LEVEL_3 PS_TRAFFIC_HANDLING_PRIORITY_LEVEL_3
#endif //SKIP_IN_REF_MAN

/** Value used when setting guaranteed bitrate in QoS to subscribed value. */
#define PS_GUARANTEED_BITRATE_UNSPECIFIED    0
/** Value used to indicate that extended (3GPP Rel5) guaranteed bitrates are not used in QoS. */
#define PS_GUARANTEED_EXTENDED_BITRATE_DOWNLINK_UNSPECIFIED 0
/** Value used to indicate max value of extended (3GPP Rel5) guaranteed bitrates used in QoS. */
#define PS_MAX_GUARANTEED_EXTENDED_BITRATE_DOWNLINK 0x4a

/** The bit rate is coded according to the definition for PS_MaxBitRateUplink_t. 
 * @see PS_MaxBitRateUplink_t 
 */
typedef uint8 PS_GuaranteedBitRateUplink_t;

/** 
 * The bit rate is coded according to the definition for PS_MaxBitRateUplink_t.
 * If a Guaranteed bit rate for downlink higher than 8640 kbps is wanted this parameter shall be set to 254, i.e. 8640 kbps 
 * and the parameter of type PS_ExtendedGuaranteedBitRateDownlink_t should be set to a non-zero value. 
 * @see PS_ExtendedMaxBitRateDownlink_t 
 */
typedef uint8 PS_GuaranteedBitRateDownlink_t;

/** 
 * The extended bit rate is coded within 7 bits of an 8 bit wide parameter. 
 * Assume the value X of the extended bit rate parameter then the following coding applies:
 * \code
 *
 *   0       the bit rate is not extended, i.e. use PS_GuaranteedBitRateDownlink_t. 
 *           
 *
 *   1-74    value range of 8700-16000 kbps binary coded with 100 kbps granularity,
 *           i.e. (8600+X*100) kbps. 
 *
 *   75-255  reserved
 *
 * \endcode
 * @see PS_GuaranteedBitRateDownlink_t 
 * @see PS_GUARANTEED_EXTENDED_BITRATE_DOWNLINK_UNSPECIFIED 
 * @see PS_MAX_GUARANTEED_EXTENDED_BITRATE_DOWNLINK
 */
typedef uint8 PS_ExtendedGuaranteedBitRateDownlink_t;

/**
 * This type is used to indicate signalling content of submitted SDUs for
 * a PDP context. 
 *
 * @param PS_SIGNALLING_INDICATION_NOT_OPTIMIZED_FOR_SIGNALLING_TRAFFIC 
 *          PDP context is not optimized for IMS signalling
 * @param PS_SIGNALLING_INDICATION_OPTIMIZED_FOR_SIGNALLING_TRAFFIC
 *          PDP context is optimized for IMS signalling
 */
TYPEDEF_ENUM
{
  PS_SIGNALLING_INDICATION_NOT_OPTIMIZED_FOR_SIGNALLING_TRAFFIC = 0,
  PS_SIGNALLING_INDICATION_OPTIMIZED_FOR_SIGNALLING_TRAFFIC
}SIGNED_ENUM8(PS_SignallingIndication_t);

/**
 * This type specifies characteristics of the source of the submitted SDUs
 * for a PDP context
 *
 * @param PS_SOURCE_STATISTICS_DESCRIPTOR_UNKNOWN
 *          Characteristics of SDUs is unknown
 * @param PS_SOURCE_STATISTICS_DESCRIPTOR_SPEECH
 *          Characteristics of SDUs corresponds to a speech source
 */
TYPEDEF_ENUM
{
  PS_SOURCE_STATISTICS_DESCRIPTOR_UNKNOWN = 0,
  PS_SOURCE_STATISTICS_DESCRIPTOR_SPEECH
}SIGNED_ENUM8(PS_SourceStatisticsDescriptor_t);

/**
 * This data type defines the set of attributes comprising the Quality of
 * Service of a PDP context. It will be used for both Release '97 [3] and
 * Release '99 [2] Quality of Service profiles.
 *
 * When specifying a minimum Quality of Service profile a value of zero
 * (i.e. unspecified value) for any of the parameters
 * (e.g. DeliveryOrder = PS_DELIVERY_ORDER_UNSPECIFIED) will result in that
 * particular parameter being excluded from any QoS negotiation that takes
 * place at PDP context activation or modification. If a value of zero is
 * given to any QoS parameter for the requested Quality of Service this will
 * result in the subscribed value for that parameter being requested from
 * the network at PDP context activation or modification. 
 *
 * @param PrecedenceClass           Enum value specifying the precedence class
 *
 * @param DelayClass                Numeric value specifying the delay class                                
 *
 * @param ReliabilityClass          Enum value specifying the reliability class                                  
 *
 * @param PeakThroughputClass       Enum value specifying the peak 
 *                                  throughput class
 *
 * @param MeanThroughputClass       Enum value specifying the mean
 *                                  throughput class
 *
 * @param TrafficClass              Enum value specifying the type of
 *                                  application for which the bearer service
 *                                  is optimized
 *
 * @param DeliveryOrder             Enum value specifying whether the SDUs
 *                                  will be delivered in order
 *
 * @param DeliveryErroneousSDU      Enum value specifying whether erroneous
 *                                  SDUs will be delivered
 *
 * @param MaxSDU_Size               range: 0-151
 *                                  Numeric value specifying the max. SDU size
 *
 * @param MaxBitRateUplink          range: 0-255
 *                                  Numeric value specifying the max.
 *                                  uplink bit rate
 *
 * @param MaxBitRateDownlink        range: 0-255
 *                                  Numeric value specifying the max.
 *                                  downlink bit rate
 *
 * @param ResidualBER               Enum value specifying the undetected bit
 *                                  error ratio in the delivered SDUs 
 *
 * @param SDU_ErrorRatio            Enum value specifying the fraction of
 *                                  SDUs lost or detected as erroneous
 *
 * @param TransferDelay             range: 0-254
 *                                  Numeric value specifying the delay
 *                                  tolerated by the application
 *
 * @param TrafficHandlingPriority   Enum value specifying the priority level
 *
 * @param GuaranteedBitRateUplink   range: 0-255
 *                                  Numeric value specifying the guaranteed
 *                                  uplink bit rate (dependant on traffic class)
 *
 * @param GuaranteedBitRateDownlink range: 0-255
 *                                  Numeric value specifying the guaranteed
 *                                  downlink bit rate (dependant on traffic class)
 *
 * @param SignallingIndication      Enum value specifying if PDP context is 
 *                                  optimized for signalling
 *
 * @param SourceStatisticsDescriptor Enum value is used to specify statistical 
 *                                   content of submitted SDUs
 *
 * @param ExtendedMaxBitRateDownlink range 0-74
 *                                   Numeric value specifying an extended max
 *                                   downlink bit rate
 *
 * @param ExtendedGuaranteedBitRateDownlink range 0-74
 *                                          Numeric value specifying an extended
 *                                          guaranteed downlink bit rate
 */
typedef struct
{
  PS_PrecedenceClass_t PrecedenceClass;
  PS_DelayClass_t DelayClass;
  PS_ReliabilityClass_t ReliabilityClass;
  PS_PeakThroughputClass_t PeakThroughputClass;
  PS_MeanThroughputClass_t MeanThroughputClass;
  PS_TrafficClass_t TrafficClass;
  PS_DeliveryOrder_t DeliveryOrder;
  PS_DeliveryErroneousSDU_t DeliveryErroneousSDU;
  PS_MaxSDU_Size_t MaxSDU_Size;
  PS_MaxBitRateUplink_t MaxBitRateUplink;
  PS_MaxBitRateDownlink_t MaxBitRateDownlink;
  PS_ResidualBER_t ResidualBER;
  PS_SDU_ErrorRatio_t SDU_ErrorRatio;
  PS_TransferDelay_t TransferDelay;
  PS_TrafficHandlingPriority_t TrafficHandlingPriority;
  PS_GuaranteedBitRateUplink_t GuaranteedBitRateUplink;
  PS_GuaranteedBitRateDownlink_t GuaranteedBitRateDownlink;
  PS_SignallingIndication_t       SignallingIndication; 
  PS_SourceStatisticsDescriptor_t SourceStatisticsDescriptor;
  PS_ExtendedMaxBitRateDownlink_t ExtendedMaxBitRateDownlink;
  PS_ExtendedGuaranteedBitRateDownlink_t ExtendedGuaranteedBitRateDownlink;
} PS_QOS_Profile_t;


// Switch to identify which release QoS application is using 

/**
 * This data type defines the type of Quality of Service Profile, i.e. to
 * distinguish between Release '97 [3] and Release '99 [2] Quality of Service
 * profiles or to specify subscribed or unspecified values.
 *
 * @param PS_QOS_TYPE_UNSPECIFIED   Used for subscribed or unspecified value
 *
 * @param PS_QOS_TYPE_RELEASE_97    Results in the release 99 parts of the Quality
 *                                  of Service profile having values derived from
 *                                  the mapping of the specified release 97 values
 *                                  (see reference [4] for mapping table).
 *
 * @param PS_QOS_TYPE_RELEASE_99    Results in the release 97 parts of the Quality 
 *                                  of Service profile being derived by the mapping
 *                                  of the release 99 values.
 */
TYPEDEF_ENUM
{
  PS_QOS_TYPE_UNSPECIFIED = 0,
  PS_QOS_TYPE_RELEASE_97,
  PS_QOS_TYPE_RELEASE_99
}SIGNED_ENUM8(PS_QOS_Type_t);

/** 
 * Internal error codes used to extend SM cause values specifeid in 
 * 3GPP 24.008 ch. 10.5.6.6
 */
/** Cause value used to indicate that there is no error*/
#define PS_NO_ERROR                           0x00
/** Cause value used to indicate that it was not possible to perform PS attach */
#define PS_GPRS_ATTACH_NOT_POSSIBLE           0x01
/** Cause value used to indicate that it was not possible to establish a PS signalling connection */
#define PS_NO_PS_SIGNALLING_CONNECTION        0x02
/** Cause value used to indicate to client that PDP context reactivation is possible */
#define PS_PDP_CONTEXT_REACTIVATION_POSSIBLE  0x03
#ifndef SKIP_IN_REF_MAN
/** Cause value used to indicate to client that PS access is barred */
#define PS_ACCESS_CLASS_BARRED                0x04
#endif //SKIP_IN_REF_MAN

/**
 * This data type defines different cause codes and consists of both own
 * defined causes and cause codes defined in 3GPP 24.008.
 */
typedef uint8 PS_CauseCode_t;

// Result codes from requesting activation of a PDP context

/**
 * This data type defines possible result codes when requesting activation
 * of a PDP context.
 *
 * @param PS_PDP_CONTEXT_ACTIVATE_RESULT_OK               OK
 *
 * @param PS_PDP_CONTEXT_ACTIVATE_RESULT_NO_RESOURCES     Not possible to activate another
 *                                                        PDP context
 *
 * @param PS_PDP_CONTEXT_ACTIVATE_RESULT_IN_PROGRESS      The activation procedure is in progress
 * 
 * @param PS_PDP_CONTEXT_ACTIVATE_RESULT_REJECTED         The request has been rejected by
 *                                                        the network
 * 
 * @param PS_PDP_CONTEXT_ACTIVATE_RESULT_UNSPECIFIED      Unspecified error.
 * 
 * @param PS_PDP_CONTEXT_ACTIVATE_RESULT_NOT_SUPPORTED    The n/w doesn't support this feature
 *                                                        e.g. secondary PDP contexts
 * 
 * @param PS_PDP_CONTEXT_ACTIVATE_RESULT_NSAPI_NOT_ACTIVE Request to activate secondary PDP
 *                                                        context is rejected because the
 *                                                        given NSAPI with the same PDP
 *                                                        address is not active
 * @param PS_PDP_CONTEXT_ACTIVATE_RESULT_REJ_RADIO_OFF    Request to activate PDP context is 
 *                                                        rejected because NS is not activated
 */
TYPEDEF_ENUM
{
  PS_PDP_CONTEXT_ACTIVATE_RESULT_OK = 0,
  PS_PDP_CONTEXT_ACTIVATE_RESULT_NO_RESOURCES,
  PS_PDP_CONTEXT_ACTIVATE_RESULT_IN_PROGRESS,
  PS_PDP_CONTEXT_ACTIVATE_RESULT_REJECTED,
  PS_PDP_CONTEXT_ACTIVATE_RESULT_UNSPECIFIED,
  PS_PDP_CONTEXT_ACTIVATE_RESULT_NOT_SUPPORTED,
  PS_PDP_CONTEXT_ACTIVATE_RESULT_NSAPI_NOT_ACTIVE,
  PS_PDP_CONTEXT_ACTIVATE_RESULT_REJ_RADIO_OFF
}
SIGNED_ENUM8(PS_PDP_ContextActivateResult_t);

#ifndef SKIP_IN_REF_MAN
#define PS_ACTIVATION_OK PS_PDP_CONTEXT_ACTIVATE_RESULT_OK 
#define PS_ACTIVATION_NO_RESOURCES PS_PDP_CONTEXT_ACTIVATE_RESULT_NO_RESOURCES
#define PS_ACTIVATION_IN_PROGRESS PS_PDP_CONTEXT_ACTIVATE_RESULT_IN_PROGRESS
#define PS_ACTIVATION_REJECTED PS_PDP_CONTEXT_ACTIVATE_RESULT_REJECTED
#define PS_ACTIVATION_UNSPECIFIED PS_PDP_CONTEXT_ACTIVATE_RESULT_UNSPECIFIED
#define PS_ACTIVATION_NOT_SUPPORTED PS_PDP_CONTEXT_ACTIVATE_RESULT_NOT_SUPPORTED
#define PS_ACTIVATION_NSAPI_NOT_ACTIVE PS_PDP_CONTEXT_ACTIVATE_RESULT_NSAPI_NOT_ACTIVE
#define PS_ACTIVATION_REJ_RADIO_OFF PS_PDP_CONTEXT_ACTIVATE_RESULT_REJ_RADIO_OFF
#endif //SKIP_IN_REF_MAN

// Type of events associated to a PDP context

/**
 * This data type defines the nature of the occurred event.
 *
 * @param PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_STARTED    MS activation started.
 * @param PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_FAILED     MS activation failed.
 * @param PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_REJECTED   MS activation rejected.
 * @param PS_PDP_CONTEXT_EVENT_MS_ACTIVATED             MS activated.
 * @param PS_PDP_CONTEXT_EVENT_MS_DEACTIVATE_STARTED    MS deactivation started.
 * @param PS_PDP_CONTEXT_EVENT_MS_DEACTIVATED           MS deactivated.
 * @param PS_PDP_CONTEXT_EVENT_NW_DEACTIVATED           NW deactivated.
 * @param PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_STARTED    NW activation started.
 * @param PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_FAILED     NW activation failed.
 * @param PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_REJECTED   NW activation rejected.
 * @param PS_PDP_CONTEXT_EVENT_NW_ACTIVATED             NW activated.
 * @param PS_PDP_CONTEXT_EVENT_NETWORK_MODIFICATION     Network modification.
 */
TYPEDEF_ENUM
{
  PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_STARTED = 0,
  PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_FAILED,
  PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_REJECTED,
  PS_PDP_CONTEXT_EVENT_MS_ACTIVATED,
  PS_PDP_CONTEXT_EVENT_MS_DEACTIVATE_STARTED,
  PS_PDP_CONTEXT_EVENT_MS_DEACTIVATED,
  PS_PDP_CONTEXT_EVENT_NW_DEACTIVATED,
  PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_STARTED,
  PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_FAILED,
  PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_REJECTED,
  PS_PDP_CONTEXT_EVENT_NW_ACTIVATED,
  PS_PDP_CONTEXT_EVENT_NETWORK_MODIFICATION
}SIGNED_ENUM8(PS_PDP_ContextEvent_t);

#ifndef SKIP_IN_REF_MAN
#define PS_EVENT_MS_ACTIVATION_STARTED PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_STARTED
#define PS_EVENT_MS_ACTIVATION_FAILED PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_FAILED
#define PS_EVENT_MS_ACTIVATION_REJECTED PS_PDP_CONTEXT_EVENT_MS_ACTIVATION_REJECTED
#define PS_EVENT_MS_ACTIVATED PS_PDP_CONTEXT_EVENT_MS_ACTIVATED
#define PS_EVENT_MS_DEACTIVATE_STARTED PS_PDP_CONTEXT_EVENT_MS_DEACTIVATE_STARTED
#define PS_EVENT_MS_DEACTIVATED PS_PDP_CONTEXT_EVENT_MS_DEACTIVATED
#define PS_EVENT_NW_DEACTIVATED PS_PDP_CONTEXT_EVENT_NW_DEACTIVATED
#define PS_EVENT_NW_ACTIVATION_STARTED PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_STARTED
#define PS_EVENT_NW_ACTIVATION_FAILED PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_FAILED
#define PS_EVENT_NW_ACTIVATION_REJECTED PS_PDP_CONTEXT_EVENT_NW_ACTIVATION_REJECTED
#define PS_EVENT_NW_ACTIVATED PS_PDP_CONTEXT_EVENT_NW_ACTIVATED
#define PS_EVENT_NETWORK_MODIFICATION PS_PDP_CONTEXT_EVENT_NETWORK_MODIFICATION
#endif //SKIP_IN_REF_MAN

// Request to Tear Down all active contexts with this PDP address

/**
 * This data type defines whether only the PDP context associated with the
 * specific request shall be deactivated or whether all active PDP contexts
 * sharing the same PDP address as the PDP context shall be deactivated.
 *
 * @param PS_TEAR_DOWN_INDICATOR_TEARDOWN_NOT_REQUESTED   Tear-down not requested.
 * @param PS_TEAR_DOWN_INDICATOR_TEARDOWN_REQUESTED       Tear-down requested.
 */
TYPEDEF_ENUM
{
  PS_TEAR_DOWN_INDICATOR_TEARDOWN_NOT_REQUESTED = 0,
  PS_TEAR_DOWN_INDICATOR_TEARDOWN_REQUESTED
}SIGNED_ENUM8(PS_TearDownIndicator_t);

#ifndef SKIP_IN_REF_MAN
#define PS_TEARDOWN_NOT_REQUESTED PS_TEAR_DOWN_INDICATOR_TEARDOWN_NOT_REQUESTED
#define PS_TEARDOWN_REQUESTED PS_TEAR_DOWN_INDICATOR_TEARDOWN_REQUESTED
#endif //SKIP_IN_REF_MAN

/**
 * Data type to distinguish the type of operation to be performed on a Traffic
 * Flow Template (TFT) when a PDP context is modified or when setting up a
 * secondary PDP context.
 *
 * @param PS_TFT_OPERATION_CODE_NO_OPERATION          No operation.
 * @param PS_TFT_OPERATION_CODE_CREATE_NEW            Create new.
 * @param PS_TFT_OPERATION_CODE_DELETE_EXISTING       Delete existing.
 * @param PS_TFT_OPERATION_CODE_ADD_TO_EXISTING       Add to existing.
 * @param PS_TFT_OPERATION_CODE_REPLACE_IN_EXISTING   Replace in existing.
 * @param PS_TFT_OPERATION_CODE_DELETE_FROM_EXISTING  Delete from existing.
 */
TYPEDEF_ENUM
{
  PS_TFT_OPERATION_CODE_NO_OPERATION = 0,
  PS_TFT_OPERATION_CODE_CREATE_NEW,
  PS_TFT_OPERATION_CODE_DELETE_EXISTING,
  PS_TFT_OPERATION_CODE_ADD_TO_EXISTING,
  PS_TFT_OPERATION_CODE_REPLACE_IN_EXISTING,
  PS_TFT_OPERATION_CODE_DELETE_FROM_EXISTING
}
SIGNED_ENUM8(PS_TFT_OperationCode_t);

#ifndef SKIP_IN_REF_MAN
#define PS_TFT_OPCODE_NO_OPERATION PS_TFT_OPERATION_CODE_NO_OPERATION
#define PS_TFT_OPCODE_CREATE_NEW PS_TFT_OPERATION_CODE_CREATE_NEW
#define PS_TFT_OPCODE_DELETE_EXISTING PS_TFT_OPERATION_CODE_DELETE_EXISTING
#define PS_TFT_OPCODE_ADD_TO_EXISTING PS_TFT_OPERATION_CODE_ADD_TO_EXISTING
#define PS_TFT_OPCODE_REPLACE_IN_EXISTING PS_TFT_OPERATION_CODE_REPLACE_IN_EXISTING
#define PS_TFT_OPCODE_DELETE_FROM_EXISTING PS_TFT_OPERATION_CODE_DELETE_FROM_EXISTING
#endif //SKIP_IN_REF_MAN

/**
 * This data type defines the maximum number of packet filters that may exist
 * within a Traffic Flow Template.
 */
#define PS_MAX_PACKET_FILTERS 8

//Defines made to help setting and reading the different attributes within a 
//packet filter combination variable.
/** 
 *  Read/Write mask for "source address" packet filter in the PS_PacketFilterCombination_t
 */
#define PS_SOURCE_ADDRESS_AND_SUBNET_MASK    0x80

 /** 
 *  Read/Write mask for "protocol (IPv4) or next header (IPv6)" packet filter in the PS_PacketFilterCombination_t
 */
#define PS_PROTOCOL_NUMBER_OR_NEXT_HEADER    0x40

 /** 
 *  Read/Write mask for "destination port range" packet filter in the PS_PacketFilterCombination_t
 */
#define PS_DESTINATION_PORT_RANGE            0x20

 /** 
 *  Read/Write mask for "source port range" packet filter in the PS_PacketFilterCombination_t
 */
#define PS_SOURCE_PORT_RANGE                 0x10

/** 
 *  Read/Write mask for "IPSEC parameter index" packet filter in the PS_PacketFilterCombination_t
 */
#define PS_IPSEC_SPI                         0x08

/** 
 *  Read/Write mask for "type of service (IPv4) or traffic class (IPv6)" packet filter in the PS_PacketFilterCombination_t
 */
#define PS_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS  0x04

 /** 
 *  Read/Write mask for "IPv6 flow label" packet filter in the PS_PacketFilterCombination_t
 */
#define PS_FLOW_LABEL                        0x02

#ifndef SKIP_IN_REF_MAN
#define SOURCE_ADDRESS_AND_SUBNET_MASK PS_SOURCE_ADDRESS_AND_SUBNET_MASK
#define PROTOCOL_NUMBER_OR_NEXT_HEADER PS_PROTOCOL_NUMBER_OR_NEXT_HEADER
#define DESTINATION_PORT_RANGE PS_DESTINATION_PORT_RANGE
#define SOURCE_PORT_RANGE PS_SOURCE_PORT_RANGE
#define IPSEC_SPI PS_IPSEC_SPI
#define TYPE_OF_SERVICE_OR_TRAFFIC_CLASS PS_TYPE_OF_SERVICE_OR_TRAFFIC_CLASS
#define FLOW_LABEL PS_FLOW_LABEL
#endif //SKIP_IN_REF_MAN
/**
 * This data type gives the combination of attributes within a packet filter.
 * The type shall be seen as a bit map (of size 8) where each bit indicates
 * if the packet filter attribute is present in the packet filter. A one
 * indicates that the attribute is present.
 *
 * There are a couple of defines made to help setting and reading the different
 * attributes within a packet filter combination variable.
 */
typedef uint8 PS_PacketFilterCombination_t;

/**
 * This data type defines a packet filter of a Traffic Flow Template. Note that
 * the values of all structure members are not valid at the same time. The
 * valid members are given by the value of PacketFilterCombination. This must
 * always be set to a non-zero value. A packet filter has a mandatory
 * identifier named PacketFilterIdentifier in the structure and this may hold
 * a value between: 
 *
 * 0-(PS_MAX_PACKET_FILTERS-1)
 *
 * A packet filter is required to have an EvaluationPrecedenceIndex attached to
 * it. This gives the precedence for the packet filter among all packet filters
 * in all TFTs associated with a PDP address. The structure members of type
 * uint16 and uint32 shall be coded in host byte order, e.g. a port number of
 * 80 shall have the value 0x0050 and not 0x5000 (NW byte order). 
 *
 * @param PacketFilterIdentifier            The packet filter identifier
 *
 * @param PacketFilterCombination           The packet filter combination that
 *                                          gives the valid filter parameters.
 *                                          There are defines that can be used 
 *                                          to help set/read this.                                           
 *
 * @param EvaluationPrecedenceIndex         The precedence for the packet filter
 *                                          among all packet filters in all TFTs 
 *                                          associated with this PDP address.
 *
 * @param SourcePDP_Type                    Source PDP address type. 
 *                                          The only valid types are IPv4/IPv6
 *
 * @param SourceAddress                     Source PDP address
 *
 * @param SubNetMask                        Source PDP address mask
 *
 * @param ProtocolNumberOrNextHeader        The IPv4 protocol identifier or 
 *                                          IPv6 next header
 *
 * @param DestinationPortRangeFrom          First value in destination port number range  
 *
 * @param DestinationPortRangeTo            Last value in destination port number range
 *
 * @param SourcePortRangeFrom               First value in source port number range
 *
 * @param SourcePortRangeTo                 Last value in source port number rangeS
 *
 * @param IP_SecurityParameterIndex         IPSEC parameter index
 *
 * @param TypeOfServiceOrTrafficClass       Type of Service (IPv4) or 
 *                                          Traffic Class (IPv6)
 *
 * @param TypeOfServiceOrTrafficClassMask   Type of Service/ Traffic class mask
 *
 * @param FlowLabel                         IPv6 Flow Label. 
 *                                          The bits 8 through 5 of the first octet 
 *                                          shall be spare whereas the remaining
 *                                          20 bits shall contain the IPv6 flow label
 */
typedef struct
{
  uint8 PacketFilterIdentifier;
  PS_PacketFilterCombination_t PacketFilterCombination;
  uint8 EvaluationPrecedenceIndex;
  PS_PDP_Type_t SourcePDP_Type;
  PS_PDP_Address_t SourceAddress;
  PS_PDP_Address_t SubNetMask;
  uint8 ProtocolNumberOrNextHeader;
  uint16 DestinationPortRangeFrom;
  uint16 DestinationPortRangeTo;
  uint16 SourcePortRangeFrom;
  uint16 SourcePortRangeTo;
  uint32 IP_SecurityParameterIndex;
  uint8 TypeOfServiceOrTrafficClass;
  uint8 TypeOfServiceOrTrafficClassMask;
  uint32 FlowLabel; 
} PS_PacketFilter_t;

/**
 * Temporary(?) for backwards compatibility
 */
#define PS_TrafficFlowTemplate_t  PS_TFT_PacketFilter_t

/**
 * This data type defines packet filters for a Traffic Flow Template for a PDP context.
 * It consists of a list of packet filters and the number of packet filters.
 *
 * @param NumberOfPacketFilters  Specifies number of packet filters
 *
 * @param PacketFilter           Contains packet filters
 */
typedef struct
{
  uint8             NumberOfPacketFilters;
  PS_PacketFilter_t PacketFilter[PS_MAX_PACKET_FILTERS];
}PS_TFT_PacketFilter_t;

/**
 * This data type defines the maximum length of parameter list that may exist
 * within a Traffic Flow Template.
 */
#define PS_MAX_TFT_PARAMETERS_LIST_LENGTH 255

/**
 * This data type defines parameters for a Traffic Flow Template.
 * It consists of a list of parameters and the length of the list.
 *
 * @param ParametersListLength  Specifies length of parameter list
 *
 * @param ParametersList        Contains list of parameters
 */
typedef struct
{
  uint8 ParametersListLength;
  uint8 ParametersList[PS_MAX_TFT_PARAMETERS_LIST_LENGTH];
} PS_TFT_ParameterList_t;

/**
 * There is a macro defined that calculates the size in bytes of a variable of
 * type PS_TrafficFlowTemplate_t. The input parameter of this macro is the
 * number of packet filters in the Traffic Flow Template. A service user
 * should use this macro when allocating memory for a variable of this type in
 * case it includes less than 8 packet filters.
 *
 * This is probably true for most situations.
 *
 * Parameters:
 * \li[in] NUMBER_OF_FILTERS The number of filters in the traffic flow template
 * @return int
 *
 * @return The macro returns the size in bytes
 */
#define PS_TRAFFIC_TEMPLATE_SIZE(NUMBER_OF_FILTERS)\
  sizeof(PS_TFT_PacketFilter_t) - (sizeof(PS_PacketFilter_t)*(PS_MAX_PACKET_FILTERS - (NUMBER_OF_FILTERS)))

/**
 * This data type defines the time and data volume counter that is measured
 * during the lifetime of an activated PDP context.
 *
 * @param ActivationTimeInSeconds   Activation time in seconds.
 * @param BytesSent                 Bytes sent.
 * @param BytesReceived             Bytes received.
 */
typedef struct
{
  uint32 ActivationTimeInSeconds; 
  uint32 BytesSent;
  uint32 BytesReceived;  
} PS_TimeAndDataVolume_t;

/**
 * This data type specifies whether, in the event the radio access technology 
 * does not support simultaneous packet and circuit switched bearers, the
 * packet switched bearers will be prioritized over the circuit switched
 * bearers. As an example(see 2) of the use of this parameter a setting of
 * PS_PRIORITY_NOT_REQUESTED (the default setting) will result in packet switch
 * services being suspended due to an incoming circuit switch call over a GSM
 * bearer, whereas the setting PS_PRIORITY_REQUESTED would result in the
 * incoming circuit switched call being rejected.
 *
 * A priority setting is fixed for the active duration of a particular PDP
 * context and once a PDP context is active with PS_PRIORITY_REQUESTED, that
 * setting will apply to all PS services that are active during that time.
 * When all PDP contexts with a PS_PRIORITY_REQUESTED setting have been
 * deactivated, then the UE will revert to prioritizing circuit switched
 * services over packet switched services (i.e. if the radio access technology
 * does not support simultaneous packet and circuit switch bearers then PS
 * services will be suspended due to an incoming circuit switch call).
 *
 * 2.This example is applicable only if the UE and the NW do not support DTM.
 *
 * @param PS_PRIORITY_NOT_REQUESTED       Priority not requested.
 * @param PS_PRIORITY_REQUESTED           Priority requested.
 */
TYPEDEF_ENUM
{
  PS_PRIORITY_NOT_REQUESTED = 0,
  PS_PRIORITY_REQUESTED
}SIGNED_ENUM8(PS_Priority_t);


//  The defines below shall be used for the parameters:
//  - PS_SDU_Type in the type PS_SDU_Annotation_t 
//  - SDU_Prioritization in the type PS_UserPlaneSettings_t

/** This define is used for two purposes:
- in the parameter SDU_Prioritization in the type PS_UserPlaneSettings_t when no SDU types shall be prioritized
- in the parameter PS_SDU_Type in the type PS_SDU_Annotation_t when an SDU classification has been done for the SDU but the type is unknown */
#define PS_SDU_TYPE_UNSPECIFIED    0x00000000 

/** This define is used for two purposes:
- in the parameter SDU_Prioritization in the type PS_UserPlaneSettings_t when TCP ACK SDU types shall be prioritized
- in for the parameter PS_SDU_Type in the type PS_SDU_Annotation_t when the SDU is classified as a TCP ACK SDU */
#define PS_SDU_TYPE_TCP_ACK         0x00000001 

/** This define is used for two purposes:
- in the parameter SDU_Prioritization in the type PS_UserPlaneSettings_t when TCP SYN SDU types shall be prioritized
- in for the parameter PS_SDU_Type in the type PS_SDU_Annotation_t when the SDU is classified as a TCP SYN SDU */
#define PS_SDU_TYPE_TCP_SYN         0x00000002

/** This define is used for two purposes:
- in the parameter SDU_Prioritization in the type PS_UserPlaneSettings_t when TCP FIN SDU types shall be prioritized
- in for the parameter PS_SDU_Type in the type PS_SDU_Annotation_t when the SDU is classified as a TCP FIN SDU */
#define PS_SDU_TYPE_TCP_FIN         0x00000004

/** This define is used for two purposes:
- in the parameter SDU_Prioritization in the type PS_UserPlaneSettings_t when UDP SDU types shall be prioritized
- in for the parameter PS_SDU_Type in the type PS_SDU_Annotation_t when the SDU is classified as a UDP SDU */
#define PS_SDU_TYPE_UDP             0x00000008 

/** This define is used for two purposes:
- in the parameter SDU_Prioritization in the type PS_UserPlaneSettings_t when RTP/RTCP SDU types shall be prioritized
- in for the parameter PS_SDU_Type in the type PS_SDU_Annotation_t when the SDU is classified as an RTP/RTCP SDU */
#define PS_SDU_TYPE_RTP_RTCP          0x00000010

/** This define is used for two purposes:
- in the parameter SDU_Prioritization in the type PS_UserPlaneSettings_t when a SDU type is unknown but is set to be prioritized
- in for the parameter PS_SDU_Type in the type PS_SDU_Annotation_t when the SDU is classified as a unknown prioritized SDU type
Note that this must be set as a separate socket option and an SDU is not classified according to this without the socket option set.
An example of the usage of this is if a special protocol like SIP or MSRP should be prioritized. Then the socket option will be set and all 
packets (SDUs) originating from the socket will be prioritized by the service provider. */
#define PS_SDU_TYPE_UNKNOWN_PRIORITIZED 0x00000020

/** This define is used for the parameter PS_SDU_Type in the type PS_SDU_Annotation_t when no SDU classification has been set for the SDU. 
When the PS_SDU_Type parameter is set to this value no other bit field is allowed to be set. 
Note also that this define is not allowed to be used for the parameter SDU_Prioritization in the type PS_UserPlaneSettings_t. */
#define PS_SDU_TYPE_UNCLASSIFIED     0x80000000


/** This define is used for the parameter PS_SDU_ExpireTime in the type PS_SDU_Annotation_t when the SDU has no expire time, i.e. TTL is not used for this SDU*/
#define PS_TIME_TO_LIVE_NO_EXPIRE_TIME 0x00000000  

/**
 * This type is used to specify different kind of SDUs, e.g. in SDU prioritization. 
 */
typedef uint32 PS_SDU_TypeBitField_t; 

/**
 * This type specifies user plane settings regarding AQM settings and SDU prioritization. 
 * AQM is a function designed to maintain as high TCP/IP throughput as possible in rate varying links.
 * SDU prioritization is a function that allows certain SDUs, e.g. TCP ACK SDUs, 
 * media SDUs like RTP/RTCP and others to be placed in front of ordinary non-prioritized SDUs. 
 *
 * @param SDU_Prioritization  This is a bit field used to enabling SDU prioritization for certain SDUs
 *                            The different bit field values are specified as defines and is named PS_SDU_TYPE_xxx
 *                            Several of these defines can be used at the same time. For example if RTP/RTCP and 
 *                            TCP ACK SDUs shall be prioritized on a PDP context the SDU_Prioritization parameter
 *                            shall be set to (PS_SDU_TYPE_TCP_ACK | PS_SDU_TYPE_RTP_RTCP). 
 *
 * @param EnableAQM           This parameter turns AQM on/off (true/false) for a PDP context. 
 */
typedef struct 
{
  PS_SDU_TypeBitField_t SDU_Prioritization;
  boolean               EnableAQM;
} PS_UserPlaneSettings_t;

/*************************************************************************/
// Types and constants used only by User Plane Processes 

/** Reserved space for signal parameters when SDU is re-used as a signal. */
/** Reserved length is extended to 12 bytes to support extended NSAPI. */
#ifndef PS_SDU_RESERVED_LENGTH
#define PS_SDU_RESERVED_LENGTH 12
#endif

/**
 * This define is used in the Uplink by the WCDMA RLC Layer. Memory allocated for SDU is increased by
 * the defined value (given in bytes) to enable concatenation.
*/
#ifndef PS_WCDMA_UPLINK_L2_TAIL 
#define PS_WCDMA_UPLINK_L2_TAIL  20
#endif 

/**
 * This define is used by NS in the event EVENT_PS_UPLINKCREDITS_GRANTED to notify Datacom that there is NO 
 * flowcontrol in the Uplink.
*/
#ifndef PS_DISABLE_UPLINK_FLOWCONTROL 
#define PS_DISABLE_UPLINK_FLOWCONTROL  MAX_UINT8
#endif 

/**
 * This type specifies the packet annotation attached on every SDU. 
 *  
 * @param PS_SDU_ExpireTime    Specifies how long an UL SDU shall be kept before thrown away. 
 *                             This is used for the so called packet time-to-live (TTL) function.
 *
 * @param PS_SDU_Type          This is used to display SDU type for the packet.  
 *                             It can consist of one or several of the PS_SDU_TYPE_xxx defines, 
 *                             e.g. (PS_SDU_TYPE_UDP | PS_SDU_TYPE_RTP_RTCP).
 *                             If PS_SDU_TYPE_UNCLASSIFIED is set no other bit fields should be set.                             
 */
typedef struct 
{
  OSTICK                 PS_SDU_ExpireTime;
  PS_SDU_TypeBitField_t  PS_SDU_Type;
} PS_SDU_Annotation_t;

/**
 *  This data type is used to transfer PS SDUs
 *
 * @param SDU_Size        SDU size in bytes.
 *
 * @param SDU_Offset      SDU offset in bytes.
 *
 * @param MaxDataSize     The size in bytes of the "Data"-area. Note the SDU size can never be 
 *                        larger than this. MaxDataSize >= SDU_Offset + SDU_Size
 *
 * @param Unused          This parameter is not used and is only present to get 32-bit alignment.
 *
 * @param SDU_Annotation  Packet annotation belonging to the SDU
 *
 * @param Data            The first octet in the PS SDU, note that the actual allocated size is 
 *                        not 1 but is given by MaxDataSize
 *
 */
typedef struct
{
  uint16                SDU_Size;
  uint16                SDU_Offset;
  uint16                MaxDataSize;
  uint16                Unused;
  PS_SDU_Annotation_t   SDU_Annotation;
  uint8                 Data[1];
} PS_SDU_Basic_t;

/** 
 * The SDU type that are used for transporting data between the user and NS.
 *
 * @param Reserved      This parameter is for internal use and holds for example the client tag
 * @param NextSDU_p     Pointer to next SDU in list of SDUs.
 * @param SDU           The SDU carried in this structure.
 */
typedef struct PS_SDU
{
  uint8 Reserved[PS_SDU_RESERVED_LENGTH];
  struct PS_SDU *NextSDU_p;
  PS_SDU_Basic_t SDU;
} PS_SDU_t;

// Defines the space needed for protocol headers uplink through the Network
// Signalling module:
// GPRS:  4 octets in SNDCP + 3 octets in LLC or 
// WCDMA: 3 octets in PDCP

/**
 * This data type defines the minimum number of octets in front of the SDUs
 * that the service user must reserve, i.e. the value of the parameter
 * SDU_Offset must not be smaller than this constant when an UL SDU buffer
 * is sent from the service user. The service provider uses this space to
 * append potential protocol headers. 
 *
 * The actual value of this parameter is not specified here and may change
 * during implementation. There is a minimum value specified in this
 * reference manual and that is 8.
 *
 * Note, that if this value is changed the new value must be chosen with care 
 * and must be an even number of 32 bits and the configured OSE buffer sizes
 * might be needed to be changed to get optimized buffer sizes for 1500 byte
 * SDUs
 */
#ifndef PS_SDU_OFFSET_FOR_UPLINK_HEADERS
#define PS_SDU_OFFSET_FOR_UPLINK_HEADERS 8
#endif

// Defines the space needed for protocol headers downlink through the user 
// modules: 2 octets for PPP

/**
 * This data type defines the minimum number of octets in front of the SDUs
 * that the service provider must reserve, i.e. the value of the parameter
 * SDU_Offset must not be smaller than this constant when an DL SDU buffer
 * is sent from the service provider. The service user uses this space to
 * append potential protocol headers.
 *
 * The actual value of this parameter may change during implementation. 
 *
 * The value should be chosen so that 32-bit alignment for the IP packet
 * is achieved.
 *
 * Note, that if this value is changed the new value must be chosen with care 
 * and must be an even number of 32 bits and the configured OSE buffer sizes
 * might be needed to be changed to get optimized buffer sizes for 1500 byte
 * SDUs 
*/
#ifndef PS_SDU_OFFSET_FOR_DOWNLINK_HEADERS
#define PS_SDU_OFFSET_FOR_DOWNLINK_HEADERS 108
#endif

/** Macro for PS SDU NULL pointer. */
#ifdef USE_PS_HEAP
#define PS_SDU_NULL NULL
#else 
#define PS_SDU_NULL NIL
#endif

/**
 * This data type defines the maximum size that the parameter MaxDataSize can have.
 *
 * The actual value of this parameter is not specified here and may change
 * during implementation. There is a minimum value specified in this reference
 * manual and that is 1704
 */
#define PS_MAX_DATA_SIZE 1704


/****************************** MBMS START *******************************/

/** Defines default MBMS NSAPI value used for MBMS (enhanced) broadcast */
#define PS_MBMS_NSAPI_BROADCAST 255

/** Bitmask used to check if TMGI is present */
#define PS_MBMS_TMGI_MASK 0x02

/** Bitmask used to check if SessionID is present */
#define PS_MBMS_SESSIONID_MASK 0x01

/**
 * This data type is used to notify service user about upcoming or stopped MBMS sessions.
 * Note that only activated MBMS bearer services are notified.
 *
 * @param PS_MBMS_EVENT_CAUSE_SERVICE_ACTIVATED                            MBMS service activation confirmed
 * @param PS_MBMS_EVENT_CAUSE_SERVICE_DEACTIVATED                          MBMS service deactivation confirmed
 * @param PS_MBMS_EVENT_CAUSE_SESSION_ACTIVE                               MBMS session start imminent.
 * @param PS_MBMS_EVENT_CAUSE_SESSION_NOT_ACTIVE                           MBMS active session is not found while
 *                                                                         being in MBMS coverage (MCCH found)
 * @param PS_MBMS_EVENT_CAUSE_NO_MBMS_COVERAGE                             UE is out of MBMS coverage (MCCH lost)
 */
TYPEDEF_ENUM
{
  PS_MBMS_EVENT_CAUSE_SERVICE_ACTIVATED = 0,
  PS_MBMS_EVENT_CAUSE_SERVICE_DEACTIVATED,
  PS_MBMS_EVENT_CAUSE_SESSION_ACTIVE,
  PS_MBMS_EVENT_CAUSE_SESSION_NOT_ACTIVE,
  PS_MBMS_EVENT_CAUSE_NO_MBMS_COVERAGE
}SIGNED_ENUM8(PS_MBMS_EventCause_t);

/**
 * This data type is used to indicate if current serving NW (SGSN) supports MBMS or not.
 *
 * Note that the SGSN can indicate MBMS support even if the current serving cell does
 * not support MBMS (i.e. UE being out of MBMS coverage)
 *
 * @param PS_MBMS_NW_FEATURE_SUPPORT_UNKNOWN_NOT_PS_ATTACHED   Network feature unknown.
 * @param PS_MBMS_NW_FEATURE_SUPPORT_MBMS_NOT_SUPPORTED        MBMS not supported by NW.
 * @param PS_MBMS_NW_FEATURE_SUPPORT_MBMS_SUPPORTED            MBMS supported by NW.
 */
TYPEDEF_ENUM
{
  PS_MBMS_NW_FEATURE_SUPPORT_UNKNOWN_NOT_PS_ATTACHED = 0,
  PS_MBMS_NW_FEATURE_SUPPORT_MBMS_NOT_SUPPORTED,
  PS_MBMS_NW_FEATURE_SUPPORT_MBMS_SUPPORTED
}SIGNED_ENUM8(PS_MBMS_NW_FeatureSupport_t);

/** Defines the length (bytes) of the MBMS service ID */
#define PS_MBMS_SERVICE_ID_LENGTH 3

/**
 * Type holding the Service ID of an MBMS bearer service.
 * @param ServiceID        The Service ID is part of the Temporary Group Mobile Identity (TMGI),
 *                         which uniquely identifies the MBMS bearer service.
 *                         The TMGI consists of the Service ID and the PLMN (MCC + MNC).
 *                         The PLMN in the TMGI is optional.
 */
typedef struct
{
  uint8 ServiceID[PS_MBMS_SERVICE_ID_LENGTH];
}PS_MBMS_ServiceID_t;

/**
 * Type holding the Temporary Mobile Group Identity (TMGI) of an MBMS bearer service.
 * @param PLMN_IsSet       Indicates if the PLMN param contains a valid PLMN (the PLMN part of TMGI is optional).
 * @param ServiceID        The Service ID part of the TMGI.
 * @param PLMN             The PLMN code in octet array format, part of the TMGI.
 */
typedef struct
{
  boolean PLMN_IsSet;
  PS_MBMS_ServiceID_t ServiceID;
//  Roaming_PLMN_t PLMN;
}PS_MBMS_TMGI_t;

/**
 * This data type defines the MBMS Multicast address and type.
 * @param MulticastAddress_PDP_Type   Type of PDP address, the only valid types are IPv4/IPv6.
 * @param MulticastAddress            The IP multicast address identifies an MBMS multicast service/group.
 */
typedef struct
{
  PS_PDP_Type_t MulticastAddress_PDP_Type;
  uint8 MulticastAddress[PS_MAX_PDP_ADDRESS_LENGTH];
}PS_MBMS_MulticastAddress_t;

/**
 * This data type is used to indicate the session identity of an activated MBMS bearer service.
 */
typedef uint8 PS_MBMS_SessionID_t;

#ifndef SKIP_IN_REF_MAN //Multicast only
/**
 * This data type defines the mode in which an MBMS bearer service is delivered.
 *
 * @param PS_MBMS_MODE_BROADCAST            MBMS bearer service is run in ordinary broadcast delivery mode
 * @param PS_MBMS_MODE_ENHANCED_BROADCAST   MBMS bearer service is run in enhanced broadcast delivery mode
 * @param PS_MBMS_MODE_MULTICAST            MBMS bearer service is run in multicast delivery mode
 */
#else //SKIP_IN_REF_MAN
/**
 * This data type defines the mode in which an MBMS bearer service is delivered.
 *
 * @param PS_MBMS_MODE_BROADCAST            MBMS bearer service is run in ordinary broadcast delivery mode
 * @param PS_MBMS_MODE_ENHANCED_BROADCAST   MBMS bearer service is run in enhanced broadcast delivery mode
 */
#endif //SKIP_IN_REF_MAN
TYPEDEF_ENUM
{
  PS_MBMS_MODE_BROADCAST = 0,
  PS_MBMS_MODE_ENHANCED_BROADCAST,
#ifndef SKIP_IN_REF_MAN //Multicast only
  PS_MBMS_MODE_MULTICAST
#endif //SKIP_IN_REF_MAN
}SIGNED_ENUM8(PS_MBMS_Mode_t);

#ifndef SKIP_IN_REF_MAN //Multicast only
/**
 * This data type defines possible result codes when requesting activation
 * of a MBMS context.
 *
 * @param PS_MBMS_CONTEXT_ACTIVATE_RESULT_NO_RESOURCES     Not possible to activate another
 *                                                         MBMS context due to limited UE resources.
 * @param PS_MBMS_CONTEXT_ACTIVATE_RESULT_IN_PROGRESS      The activation procedure is in progress, msg
 *                                                         has been sent to NW.
 * @param PS_MBMS_CONTEXT_ACTIVATE_RESULT_REJ_RADIO_OFF    Request to activate MBMS context is 
 *                                                         rejected because NS is not activated.
 */
TYPEDEF_ENUM
{
  PS_MBMS_CONTEXT_ACTIVATE_RESULT_NO_RESOURCES = 0,
  PS_MBMS_CONTEXT_ACTIVATE_RESULT_IN_PROGRESS,
  PS_MBMS_CONTEXT_ACTIVATE_RESULT_REJ_RADIO_OFF
}SIGNED_ENUM8(PS_MBMS_ContextActivateResult_t);

/**
 * This data type defines the nature of the occurred event regarding MBMS context activation.
 * NOTE: this is only used for MBMS multicast
 *
 * @param PS_MBMS_CONTEXT_EVENT_NW_CONTEXT_ACTIVATED            MBMS context activate accepted by NW, No UP connected.
 * @param PS_MBMS_CONTEXT_EVENT_MS_UP_CONNECTED                 UP connected.
 * @param PS_MBMS_CONTEXT_EVENT_NW_NSAPI_NOT_ACTIVE             Request to activate MBMS context
 *                                                              is rejected because the given linked NSAPI
 *                                                              does not belong to an active PDP context.
 * @param PS_MBMS_CONTEXT_EVENT_NW_ACTIVATION_REJECTED          The request has been explicitly rejected by the NW.
 * @param PS_MBMS_CONTEXT_EVENT_NW_REJECT_UNSPECIFIED           Unspecified error.
 * @param PS_MBMS_CONTEXT_EVENT_NW_REJECT_NO_RESOURCES          Not possible to activate another
 *                                                              MBMS context due to limited UE resources.
 * @param PS_MBMS_CONTEXT_EVENT_NW_REJECT_FEATURE_NOT_SUPPORTED The NW doesn't support this feature.
 */
TYPEDEF_ENUM
{
  PS_MBMS_CONTEXT_EVENT_NW_CONTEXT_ACTIVATED = 0,
  PS_MBMS_CONTEXT_EVENT_MS_UP_CONNECTED,
  PS_MBMS_CONTEXT_EVENT_NW_NSAPI_NOT_ACTIVE,
  PS_MBMS_CONTEXT_EVENT_NW_ACTIVATION_REJECTED,
  PS_MBMS_CONTEXT_EVENT_NW_REJECT_UNSPECIFIED,
  PS_MBMS_CONTEXT_EVENT_NW_REJECT_NO_RESOURCES,
  PS_MBMS_CONTEXT_EVENT_NW_REJECT_FEATURE_NOT_SUPPORTED
}SIGNED_ENUM8(PS_MBMS_ContextEvent_t);
#endif //SKIP_IN_REF_MAN

/******************************* MBMS END ********************************/

#endif // INCLUSION_GUARD_T_PSS_H 

/* End of file */
