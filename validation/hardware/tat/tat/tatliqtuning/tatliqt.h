/*
* =====================================================================================
* 
*        Filename:  tatlvideo.h
* 
*     Description:  
* 
*         Version:  1.0
*         Created:  27/08/2009 
*        Revision:  none
*        Compiler:  
* 
*          Author: GVO, 
*         © Copyright ST-Ericsson, 2009. All Rights Reserved
* 
* =====================================================================================
*/

#ifndef TATLIQT_H_
#define TATLIQT_H_

#include "tatl02iqtgen.h"
#include "IqtServices.h"
#include "tatl01iqtfunc.h"

#if defined (TATLIQT_C)
#define GLOBAL 
#else
#define GLOBAL extern
#endif

GLOBAL int fd_input
#if defined (TATLIQT_C)
=0
#endif
;

GLOBAL int fd_output
#if defined (TATLIQT_C)
=0
#endif
;

#if !defined(PIPE_VERSION)
GLOBAL int fd_output_mmte_task
#if defined (TATLIQT_C)
=0
#endif
;
#endif

#if !defined(PIPE_VERSION)
GLOBAL int fd_popen;
#else
GLOBAL FILE *fd_popen;
#endif

#define SYSLOG					iq_tunning_SYSLOG
#define SYSLOGLOC				iq_tunning_SYSLOGLOC
#define SYSLOGSTR				iq_tunning_SYSLOGSTR

#endif /*  TATLIQT_H_ */

