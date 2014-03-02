/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <debug/mpc/dsp_printf.nmf>
#include <stdarg.h>

int	vsprintf( char *buf, const char *fmt, va_list ap );
int write_sim( int fildes, char *buf, unsigned int nbyte );

static unsigned int mystrlen2(char *s)
{
 unsigned int n=0;
 if (s==0) return 0;
 while(*s++) n++;
 return n;
}

// Display limitted to one line 
// If this is changed the interface file msg.idl must be modified accordinghly
//
#define MPC_MAX_STRING_SIZE	80
char s0[MPC_MAX_STRING_SIZE];
#pragma force_dcumode
void METH(printf)(char *format,va_list ap)
{
  volatile MMIO unsigned short * is_simulator = (volatile MMIO unsigned short *)0xF523U;
	char	*s=s0;
	vsprintf(s,format,ap);

  /* If we are on simulator (so on SVP), we output printfs  through the simulator, 
    otherwise we send them to ARM through NMF interface */
  if(*is_simulator == 0x7BAD)  // magic number to detect we are on sim
  {
   write_sim(1, s0, (unsigned int)mystrlen2(s0) );  // 1 is stdout  
  } 
  else
  {
	  NmfPrint0(EE_DEBUG_LEVEL_ERROR,s);	
	}
}


 



