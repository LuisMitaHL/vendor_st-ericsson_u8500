/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * Author: Szymon Bigos <szymon.bigos@tieto.com> for ST-Ericsson
 *         Tomasz Hliwiak <tomasz.hliwiak@tieto.com> for ST-Ericsson
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STE_PCSC_H
#define STE_PCSC_H

#if defined RIL
#define ADAPTER_NAME "ste_pcsc_ril"
#elif defined CAIF
#define ADAPTER_NAME "ste_pcsc_caif"
#else
#define ADAPTER_NAME "ste_pcsc_unknown"
#endif

struct _SCARD_IO_REQUEST{
	unsigned long 	dwProtocol; //Protocol identifier.
	unsigned long 	cbPciLength; //Protocol Control Inf Length.
};

typedef struct _SCARD_IO_REQUEST SCARD_IO_REQUEST;
typedef long SCARDCONTEXT;
typedef long SCARDHANDLE;
typedef unsigned long DWORD;

#define MAX_ATR_SIZE 33

struct SCARD_READERSTATE{
	const char * 	szReader;
	void * 	pvUserData;
	unsigned long dwCurrentState;
	unsigned long dwEventState;
	unsigned long cbAtr;
	unsigned char rgbAtr [MAX_ATR_SIZE];
};


/**
 * SCardEstablishContext - creates application context for PCSC.
 *
 * @scope: Scope of established context.
 * @reserved_1: Reserved value. Ignored.
 * @reserved_2: Reserved value. Ignored.
 * @context: Returned context.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardEstablishContext(
		unsigned long scope,
		const void *reserved_1,
		const void *reserved_2,
		long *context);

/**
 * SCardReleaseContext - destroys given application context.
 *
 * @context: Context to release.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardReleaseContext(
		long context);

/**
 * SCardListReaders - Lists available readers. If groups and readers are NULL,
 * readers_len contains length required to store readers names.
 *
 * @context: Application context.
 * @groups: Readers group. Ignored.
 * @readers: Returned reader names.
 * @readers_len: Length of returned readers names or max available readers buff.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardListReaders(
		long context,
		const char *groups,
		char *readers,
		unsigned long *readers_len);

/**
 * SCardConnect - Establish connection to given reader.
 *
 * @context: Application context.
 * @reader: Reader name.
 * @mode: Reader share mode. Ignored.
 * @pref_proto: Preffered protocol to use. Ignored.
 * @card: Returned card handler.
 * @active_proto: Used protocol. Always T0.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardConnect(
		long context,
		const char *reader,
		unsigned long mode,
		unsigned long pref_proto,
		long *card,
		unsigned long *active_proto);

/**
 * SCardDisconnect - Terminate connection to given reader.
 *
 * @card: Card handler.
 * @disposition: Reader action. Ignored.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardDisconnect(
		long card,
		unsigned long disposition);

/**
 * SCardBeginTransaction - Begins card transaction.
 *
 * @card: Card handler.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardBeginTransaction(
		long card);

/**
 * SCardEndTransaction - Ends card transaction.
 *
 * @card: Card handler.
 * @disposition: Reader action. Ignored.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardEndTransaction(
		long card,
		unsigned long disposition);

/**
 * SCardTransmit - Transmits APDU frame to card.
 *
 * @card: Card handler.
 * @in_io_mode: Input IO mode. Ignored.
 * @in_buff: Frame to send.
 * @in_len: in_buff length.
 * @out_io_mode: Output IO mode. Ignored.
 * @out_buff: Buffer for response.
 * @out_len: Max out_buff length.
 *
 * Returns: SCARD_S_SUCCESS on success or other value on failure
 */
long SCardTransmit(
		long card,
		const SCARD_IO_REQUEST *in_io_mode,
		const unsigned char *in_buff,
		unsigned long in_len,
		SCARD_IO_REQUEST *out_io_mode,
		unsigned char *out_buff,
		unsigned long *out_len);

//PCSC-Lite definitions:

// Error codes:

// No error was encountered.
#define SCARD_S_SUCCESS 0x00000000L
// An internal consistency check failed
#define SCARD_F_INTERNAL_ERROR 0x80100001L
// The action was cancelled by a SCardCancel request.
#define SCARD_E_CANCELLED 0x80100002L
// The supplied handle was invalid.
#define SCARD_E_INVALID_HANDLE 0x80100003L
// One or more of the supplied parameters could not be properly interpreted.
#define SCARD_E_INVALID_PARAMETER 0x80100004L
// Registry startup information is missing or invalid.
#define SCARD_E_INVALID_TARGET 0x80100005L
// Not enough memory available to complete this command.
#define SCARD_E_NO_MEMORY 0x80100006L
// An internal consistency timer has expired.
#define SCARD_F_WAITED_TOO_LONG 0x80100007L
// The data buffer to receive returned data is too small for the returned data.
#define SCARD_E_INSUFFICIENT_BUFFER 0x80100008L
// The specified reader name is not recognized.
#define SCARD_E_UNKNOWN_READER 0x80100009L
// The user-specified timeout value has expired.
#define SCARD_E_TIMEOUT 0x8010000AL
// The smart card cannot be accessed because of other connections outstanding.
#define SCARD_E_SHARING_VIOLATION 0x8010000BL
// The operation requires a smart card, but no smart card is currently in the device.
#define SCARD_E_NO_SMARTCARD 0x8010000CL
// The specified smart card name is not recognized.
#define SCARD_E_UNKNOWN_CARD 0x8010000DL
// The system could not dispose of the media in the requested manner.
#define SCARD_E_CANT_DISPOSE 0x8010000EL
// The requested protocols are incompatible with the protocol currently in use with the smart card.
#define SCARD_E_PROTO_MISMATCH 0x8010000FL
// The reader or smart card is not ready to accept commands.
#define SCARD_E_NOT_READY 0x80100010L
// One or more of the supplied parameters values could not be properly interpreted.
#define SCARD_E_INVALID_VALUE 0x80100011L
// The action was cancelled by the system, presumably to log off or shut down.
#define SCARD_E_SYSTEM_CANCELLED 0x80100012L
// An internal communications error has been detected.
#define SCARD_F_COMM_ERROR 0x80100013L
// An internal error has been detected, but the source is unknown.
#define SCARD_F_UNKNOWN_ERROR 0x80100014L
// An ATR obtained from the registry is not a valid ATR string.
#define SCARD_E_INVALID_ATR 0x80100015L
// An attempt was made to end a non-existent transaction.
#define SCARD_E_NOT_TRANSACTED 0x80100016L
// The specified reader is not currently available for use.
#define SCARD_E_READER_UNAVAILABLE 0x80100017L
// The operation has been aborted to allow the server application to exit.
#define SCARD_P_SHUTDOWN 0x80100018L
// The PCI Receive buffer was too small.
#define SCARD_E_PCI_TOO_SMALL 0x80100019L
// The reader driver does not meet minimal requirements for support.
#define SCARD_E_READER_UNSUPPORTED 0x8010001AL
// The reader driver did not produce a unique reader name.
#define SCARD_E_DUPLICATE_READER 0x8010001BL
// The smart card does not meet minimal requirements for support.
#define SCARD_E_CARD_UNSUPPORTED 0x8010001CL
// The Smart Card Resource Manager is not running.
#define SCARD_E_NO_SERVICE 0x8010001DL
// The Smart Card Resource Manager has shut down.
#define SCARD_E_SERVICE_STOPPED 0x8010001EL
// An unexpected card error has occurred.
#define SCARD_E_UNEXPECTED 0x8010001FL
// No primary provider can be found for the smart card.
#define SCARD_E_ICC_INSTALLATION 0x80100020L
// The requested order of object creation is not supported.
#define SCARD_E_ICC_CREATEORDER 0x80100021L
// This smart card does not support the requested feature.
#define SCARD_E_UNSUPPORTED_FEATURE 0x80100022L
// The identified directory does not exist in the smart card.
#define SCARD_E_DIR_NOT_FOUND 0x80100023L
// The identified file does not exist in the smart card.
#define SCARD_E_FILE_NOT_FOUND 0x80100024L
// The supplied path does not represent a smart card directory.
#define SCARD_E_NO_DIR 0x80100025L
// The supplied path does not represent a smart card file.
#define SCARD_E_NO_FILE 0x80100026L
// Access is denied to this file.
#define SCARD_E_NO_ACCESS 0x80100027L
// The smart card does not have enough memory to store the information.
#define SCARD_E_WRITE_TOO_MANY 0x80100028L
// There was an error trying to set the smart card file object pointer.
#define SCARD_E_BAD_SEEK 0x80100029L
// The supplied PIN is incorrect.
#define SCARD_E_INVALID_CHV 0x8010002AL
// An unrecognized error code was returned from a layered component.
#define SCARD_E_UNKNOWN_RES_MNG 0x8010002BL
// The requested certificate does not exist.
#define SCARD_E_NO_SUCH_CERTIFICATE 0x8010002CL
// The requested certificate could not be obtained.
#define SCARD_E_CERTIFICATE_UNAVAILABLE 0x8010002DL
// Cannot find a smart card reader.
#define SCARD_E_NO_READERS_AVAILABLE 0x8010002EL
// A communications error with the smart card has been detected. Retry the operation.
#define SCARD_E_COMM_DATA_LOST 0x8010002FL
// The requested key container does not exist on the smart card.
#define SCARD_E_NO_KEY_CONTAINER 0x80100030L
// The Smart Card Resource Manager is too busy to complete this operation.
#define SCARD_E_SERVER_TOO_BUSY 0x80100031L
// The reader cannot communicate with the card, due to ATR string configuration conflicts.
#define SCARD_W_UNSUPPORTED_CARD 0x80100065L
// The smart card is not responding to a reset.
#define SCARD_W_UNRESPONSIVE_CARD 0x80100066L
// Power has been removed from the smart card, so that further communication is not possible.
#define SCARD_W_UNPOWERED_CARD 0x80100067L
// The smart card has been reset, so any shared state information is invalid.
#define SCARD_W_RESET_CARD 0x80100068L
// The smart card has been removed, so further communication is not possible.
#define SCARD_W_REMOVED_CARD 0x80100069L
// Access was denied because of a security violation.
#define SCARD_W_SECURITY_VIOLATION 0x8010006AL
// The card cannot be accessed because the wrong PIN was presented.
#define SCARD_W_WRONG_CHV 0x8010006BL
// The card cannot be accessed because the maximum number of PIN entry attempts has been reached.
#define SCARD_W_CHV_BLOCKED 0x8010006CL
// The end of the smart card file has been reached.
#define SCARD_W_EOF 0x8010006DL
// The action was cancelled by the user.
#define SCARD_W_CANCELLED_BY_USER 0x8010006EL
// No PIN was presented to the smart card.
#define SCARD_W_CARD_NOT_AUTHENTICATED 0x8010006FL

//Scope in user space.
#define SCARD_SCOPE_USER 0x0000
// Scope in terminal.
#define SCARD_SCOPE_TERMINAL 0x0001
// Scope in system.
#define SCARD_SCOPE_SYSTEM 0x0002
// Scope is global
#define SCARD_SCOPE_GLOBAL 0x0003

// Exclusive mode only.
#define SCARD_SHARE_EXCLUSIVE 0x0001
// Shared mode only.
#define SCARD_SHARE_SHARED 0x0002
// Raw mode only.
#define SCARD_SHARE_DIRECT 0x0003

// Protocols:
#define SCARD_PROTOCOL_UNDEFINED 0x0000
#define SCARD_PROTOCOL_UNSET SCARD_PROTOCOL_UNDEFINED
#define SCARD_PROTOCOL_T0 0x0001
#define SCARD_PROTOCOL_T1 0x0002
#define SCARD_PROTOCOL_RAW 0x0004
#define SCARD_PROTOCOL_T15 0x0008
#define SCARD_PROTOCOL_ANY (SCARD_PROTOCOL_T0|SCARD_PROTOCOL_T1)

// Do nothing on close.
#define SCARD_LEAVE_CARD 0x0000
// Reset on close.
#define SCARD_RESET_CARD 0x0001
// Power down on close.
#define SCARD_UNPOWER_CARD 0x0002
// Eject on close.
#define SCARD_EJECT_CARD 0x0003

#endif /* STE_PCSC_H */
