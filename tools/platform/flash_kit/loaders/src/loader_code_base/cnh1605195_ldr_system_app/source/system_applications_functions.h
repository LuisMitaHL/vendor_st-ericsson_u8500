/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _SYSTEM_APPLICATIONS_FUNCTIONS_H_
#define _SYSTEM_APPLICATIONS_FUNCTIONS_H_

/**
 *  @addtogroup ldr_system_app
 *  @{
 */
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "error_codes.h"
#include "t_communication_service.h"

/*******************************************************************************
 * Types, constants
 ******************************************************************************/
/** Defines the Major revision version of the protocol */
#define PROTOCOL_VERSION_MAJOR 0x31
/** Defines the Minor revision version of the protocol */
#define PROTOCOL_VERSION_MINOR 0x30

/** Protocol Version response size */
#define PROTOCOLVERSION_SIZE                0x02

/**  Number of IMEI digits in decimal radix representation. */
#define IMEI_LENGTH        14

/** Maximal Length of Loader Version String is limited by
 *  maximal length of payload when using
 *  command communication protocol
 */
#define LOADER_VERSION_MAX_SIZE                 65530

/** Number fo control keys */
#define NO_OF_CONTROL_KEYS    5

/** length of the control key */
#define CONTROL_KEY_LENGTH    16

typedef enum {
    /** Control Keys Authentication type */
    CONTROL_KEYS_AUTHENTICATION = 0,
    /** Certificate Authentication type */
    CERTIFICATE_AUTHENTICATION,
    /** Permanent Authentication type */
    PERMANENT_AUTHENTICATION,
    /** Control Keys Persistent Authentication type */
    CONTROL_KEYS_AUTHENTICATION_PERSISTENT,
    /** Certificate Persistent Authentication type */
    CERTIFICATE_AUTHENTICATION_PERSISTENT,
    /** Get Device Authentication state*/
    GET_AUTHENTICATION_STATE
} AuthenticationType_t;

typedef enum {
    /** De-Authenticate for current session */
    DEAUTHENTICATION = 0,
    /** De-Authenticate Persistently */
    DEAUTHENTICATION_PERSISTENT
} DeAuthenticationType_t;

/***/
typedef struct {
    boolean ValidControlKeys;
    uint8 ControlKeys[NO_OF_CONTROL_KEYS *(CONTROL_KEY_LENGTH + 1)];
    uint32 Length;
} ControlKeysData_t;

/***/
typedef struct {
    boolean ValidDataBlock;
    uint8 *Data_p;
    uint32 Length;
} CA_AuthData_t;

/***/
typedef enum {
    AUTHENTICATION_TYPE_SELECTION = 0,
    PROCESS_CONTROL_KEYS_AUTHENTICATION,
    PROCESS_CERTIFICATE_AUTHENTICATION,
    PROCESS_PERMANENT_AUTHENTICATION,
    CONTROL_KEYS_SET_VERIFICATION,
    CHALLENGE_DATA_BLOCK_VERIFICATION,
    NO_ATTEMPT_LEFT_SHUTDOWN
} Authentication_t;
/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * Function implemented in system ram.
 * Copies memory area of size copyLength from one location (copyFrom)
 * to another (copyTo) and transfers execution to jumpTo address afterwards
 *
 *
 * @param [in]  copyFrom    Source address in memory area
 * @param [in]  copyTo      Destination address in memory area
 * @param [in]  copyLength  Data length
 * @param [in]  jumpTo      When transfer is finished jump to this address
 *                          and start executing
 */
void CopyAndJumpTo(uint8 const *const copyFrom, uint8 *copyTo, uint32 copyLength, uint32 jumpTo);

/***************************************************************************//**
* @brief Read Real Time Clock initialization status.
*
* @retval TRUE   if RTC is initialized.
* @retval FALSE  if RTC is not initialized.
*
*******************************************************************************/
boolean Read_RTC_Status(void);


/** @} */
#endif /*_SYSTEM_APPLICATIONS_FUNCTIONS_H_*/
