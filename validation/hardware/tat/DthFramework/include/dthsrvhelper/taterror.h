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

#ifndef TAT_ERROR_INC
#define TAT_ERROR_INC

/* TAT error codes definitions.
 * Error codes are supposed to be different to linux errno codes.
 * Any change in the table i is to be reported in 9pserver/srce/9psrverror.c.
 */
/***********************/
enum {
   TAT_ERROR_OFF = 0 ,            /*0x00*/
   /* error codes from 1 to 1023 are reserved for errno codes next are the TAT
    * error codes */
   TAT_ERROR_MIN = 1024,
   TAT_ERROR_ON = TAT_ERROR_MIN,
   TAT_ERROR_VERSION,
   TAT_ERROR_REVISION,
   TAT_ERROR_CHECKSUM,
   TAT_ERROR_PROCESS_NULL,
   TAT_ERROR_NUM_PROCESS,
   TAT_ERROR_BAD_OPERATION,
   TAT_ERROR_CASE,
   TAT_ERROR_RESOURCE,
   TAT_ERROR_PARAMETERS,
   TAT_ERROR_OUT_OF_MEM,
   TAT_ERROR_BAD_COMMAND,
   TAT_ERROR_BAD_REQ,
   TAT_ERROR_DEADLOCK,
   TAT_ERROR_INIT_LENGHT,
   TAT_ERROR_INIT,
   TAT_ERROR_NOT_AVAILABLE,
   TAT_ERROR_BAD_DEV_ID,
   TAT_ERROR_BAD_MANUFACT_ID,
   TAT_ERROR_IT,
   TAT_ERROR_CHK_DIFFERED,
   TAT_ERROR_MEM_ALLOC,
   TAT_ERROR_MEM_FREE,
   TAT_ERROR_ENTRY_POINT,
   TAT_ERROR_FILE_OPEN,
   TAT_ERROR_FILE_WRITE,
   TAT_ERROR_FILE_READ,
   TAT_ERROR_NULL_POINTER,
   TAT_ERROR_NOT_FOUND,
   TAT_ERROR_ALIGN,
   TAT_ERROR_TYPE,
   TAT_ERROR_TOO_MANY,
   TAT_ERROR_SEND,
   TAT_ERROR_RECEIVE,

   /* keep this last of standard DTH error codes */
   TAT_ERROR_MAX,

   /* service specific error codes begins here */
   TAT_ERROR_USER = 0x7FFF,

   /* modem specific errors */
   TAT_ERROR_NOT_MATCHING_MSG,
   TAT_ERROR_ISI_LINK,
   TAT_ERROR_MISC,

   /* deprecated (to be replaced) */
   TAT_BAD_REQ = TAT_ERROR_BAD_REQ,
   TAT_ERROR_NO_MSG_RECEIVED = TAT_ERROR_RECEIVE,
   TAT_ISI_HANDLER_ERROR = TAT_ERROR_ISI_LINK,
   TAT_MISC_ERR = TAT_ERROR_MISC
} e_taterror;

/* macro to check if the result is success or not (boolean) */
#define TAT_OK(taterr)	(TAT_ERROR_OFF == (taterr))

/* process operation if err is TAT_ERROR_OFF else does nothing */
#define TAT_ATTEMPT(err, operation) \
	do {  \
		if (TAT_OK((err))) \
			(err) = (operation); \
	} while (0);

/* sets x to err ONLY if previously TAT_ERROR_OFF else x is unchanged */
#define TAT_SET_ERR(err, code)     \
	do { \
		if (TAT_OK((err))) \
			(err) = (code); \
	} while (0);

#endif   /* TAT_ERROR_CODES_DEF */

