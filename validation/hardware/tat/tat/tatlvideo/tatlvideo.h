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

#ifndef TATLVIDEO_H_
#define TATLVIDEO_H_

#include "tatl02videogen.h"
#include "VideoServices.h"
#include "tatl01videofunc.h"

#if defined (TATLVIDEO_C)
#define GLOBAL 
#else
#define GLOBAL extern
#endif

GLOBAL int fd_input
#if defined (TATLVIDEO_C)
=0
#endif
;

GLOBAL int fd_output
#if defined (TATLVIDEO_C)
=0
#endif
;

#if !defined(PIPE_VERSION)
GLOBAL int fd_output_mmte_task
#if defined (TATLVIDEO_C)
=0
#endif
;
#endif

#if !defined(PIPE_VERSION)
GLOBAL int fd_popen;
#else
GLOBAL FILE *fd_popen;
#endif

#define SYSLOG					VIDEO_SYSLOG
#define SYSLOGLOC				VIDEO_SYSLOGLOC
#define SYSLOGSTR				VIDEO_SYSLOGSTR

#endif /* TATLVIDEO_H_
*/
