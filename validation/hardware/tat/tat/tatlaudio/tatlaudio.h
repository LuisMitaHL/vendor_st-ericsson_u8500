/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides routines to manage audio tat functions
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef TATLAUDIO_H_
#define TATLAUDIO_H_

#include "tatl00audiodth.h"

#include "dthaudio.h"

#include "tatl02func.h"

#if defined HATS_AB_8500_HW
#include "hats_audio.h"        /* from audioservices */
#include "tatl03param8500.h"
#include "ab8500_Audio.h"
#else
#include "hats_audio_9540.h"        /* from audioservices */
#include "tatl03param8520.h"
#include "ab9540_Audio.h"
#endif

#include <dthsrvhelper/taterror.h>

#include "libab.h"

#include "tatasync.h"

#include <stdio.h>
#include <dirent.h>

/*#include "modemmngt.h"
#include "modemlst.h"*/

/* audioservices errors */
typedef uint32_t hats_audio_err_t;

/* convert a HATS audio error to a DTH error */
int hats_audio_err_to_dth(hats_audio_err_t);

/* videoservices errors */
typedef uint32_t hats_video_err_t;

/* convert a HATS video error to a DTH error */
int hats_video_err_to_dth(hats_video_err_t);

/* log macros */
#define SYSLOGLOC AUDIO_SYSLOGLOC
#define SYSLOGSTR AUDIO_SYSLOGSTR
#define SYSLOG AUDIO_SYSLOG

/* casting */
#define STATIC_CAST(val, cast_type)     ( (cast_type)(val) )

/* pointer dereferencement */
#define DEREF_PTR(ptr, type)            ( (type)*((type*)(ptr)) )

/* pointer assignment */
#define DEREF_PTR_SET(ptr, val, type)   *((type*)(ptr)) = (type)(val)

#define DECLARE_ERR()                                           \
    int tataudio_err = TAT_ERROR_OFF;

#define RETURN_ERR()                                            \
    return (tataudio_err);

#define LAST_ERR      (tataudio_err)

#define TAT_LAST_OK()       TAT_OK(LAST_ERR)

#define TAT_IF_OK(instr)    if ( TAT_LAST_OK() ) { instr; }

#define TAT_SET_LAST_ERR(code)  TAT_SET_ERR(LAST_ERR, code)

#define TAT_TRY(op)         TAT_ATTEMPT(LAST_ERR, op)

/* common check for auto-numbered enum */
#define VERIFY_ENUM(val, max_enum)                              \
    if ((val) > (max_enum)){                \
        TAT_SET_LAST_ERR(TAT_BAD_REQ);                          \
        SYSLOG(LOG_ERR, "val: not whithin range [0..max_enum]"); \
    }

#define VERIFY_DTH_TYPE(elem, type_)                            \
    if ( (elem)->type != (type_) ) {                            \
        TAT_SET_LAST_ERR(TAT_ERROR_TYPE);                       \
        SYSLOG(LOG_ERR, "%u is not of the expected DTH type (type_)", (elem)->user_data); \
    }

/* service operation failure (will return with provided code) */
#define TAT_FAIL(code_)                                         \
    {                                                           \
    SYSLOG(LOG_ERR, "operation failed with code %s (%d)", #code_, (code_)); \
    return code_;                                               \
    }

#endif /* TATLAUDIO_H_ */
