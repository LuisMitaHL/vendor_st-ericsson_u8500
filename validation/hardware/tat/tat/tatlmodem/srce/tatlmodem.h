/*
* =====================================================================================
* 
*        Filename:  tatlmodem.h
* 
*     Description:  TATLMODEM global header
* 
*         Version:  1.0
*         Created:  06-16-2010
*
*        Compiler:  gcc
* 
*        Company:  ST-Ericsson
* 		
*	  Copyright (C) ST-Ericsson 2009
* =====================================================================================
*/
#ifndef TATL01MAIN_H__
#define TATL01MAIN_H__

#include <tatl00modemdth.h>

#if defined (TATL01MAIN_C)
#define GLOBAL 
#else
#define GLOBAL extern
#endif

#define SYSLOGLOC   MODEM_SYSLOGLOC
#define SYSLOGSTR   MODEM_SYSLOGSTR
#define SYSLOG      MODEM_SYSLOG

GLOBAL int dth_init_service(void);

GLOBAL int DthModem_exec(struct dth_element *elem);
GLOBAL int DthModem_get(struct dth_element *elem, void *value);
GLOBAL int DthModem_set(struct dth_element *elem, void *value);
GLOBAL int DthModem_setDebug(struct dth_element *elem, void *value);

#undef GLOBAL

#endif
