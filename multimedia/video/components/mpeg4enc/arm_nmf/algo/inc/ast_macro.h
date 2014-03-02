/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*   
#############################################################
File:     ast_macro.h

Author:   Daniele Bagni
email:    daniele.bagni@st.com  

Org:      Agrate Advanced System Technologies lab.
          STMicroelectronics S.r.l.

Created:  04.03.99

Purpose:  macros visible from all the modules 

Notes:    

Status:   

Modified: 26.07.2000

COPYRIGHT:
This program is property of Agrate Advanced System Technology
lab, from STMicroelectronics S.r.l. It should not be 
communicated outside STMicroelectronics without authorization.
################################################################
*/

#ifndef H_AST_MACROS_H
#define H_AST_MACROS_H

//extern int RCONTROL;



#if defined (AST_PRINT)

#  include <stdio.h>     /* fopen, fread, fwrite, fclose, printf */
#  include "ast_glob.h"

#  ifdef PRN_ON_DP
#  include "dprintf.h"
#  endif

#  define PRINTOUT stderr
#  define PRINTERROR stderr
#  define PRINTST statfile

#  ifndef PRN_ON_DP

#  define PRINTMES1(l,a) if (guc_verbose >= (l)) fprintf(PRINTOUT,(a)) 
#  define PRINTMES2(l,a,b) if (guc_verbose >= (l)) fprintf(PRINTOUT,(a),(b))
#  define PRINTMES3(l,a,b,c) if (guc_verbose >= (l)) fprintf(PRINTOUT,(a),(b),(c)) 
#  define PRINTMES4(l,a,b,c,d) if (guc_verbose >= (l)) fprintf(PRINTOUT,(a),(b),(c),(d))
#  define PRINTMES5(l,a,b,c,d,e) if (guc_verbose >= (l)) fprintf(PRINTOUT,(a),(b),(c),(d),(e))
#  define PRINTMES6(l,a,b,c,d,e,f) if (guc_verbose >= (l)) fprintf(PRINTOUT,(a),(b),(c),(d),(e),(f))

#  else

#  define PRINTMES1(l,a) if (guc_verbose >= (l)) DP((a)) 
#  define PRINTMES2(l,a,b) if (guc_verbose >= (l)) DP((a),(b))
#  define PRINTMES3(l,a,b,c) if (guc_verbose >= (l)) DP((a),(b),(c)) 
#  define PRINTMES4(l,a,b,c,d) if (guc_verbose >= (l)) DP((a),(b),(c),(d))
#  define PRINTMES5(l,a,b,c,d,e) if (guc_verbose >= (l)) DP((a),(b),(c),(d),(e))
#  define PRINTMES6(l,a,b,c,d,e,f) if (guc_verbose >= (l)) DP((a),(b),(c),(d),(e),(f))

#  endif

#  define PRINTSTAT1(l,a) if (guc_verb >= (l)) fprintf(PRINTST,(a)) 
#  define PRINTSTAT2(l,a,b) if (guc_verb >= (l)) fprintf(PRINTST,(a),(b))
#  define PRINTSTAT3(l,a,b,c) if (guc_verb >= (l)) fprintf(PRINTST,(a),(b),(c)) 
#  define PRINTSTAT4(l,a,b,c,d) if (guc_verb >= (l)) fprintf(PRINTST,(a),(b),(c),(d))
#  define PRINTSTAT5(l,a,b,c,d,e) if (guc_verb >= (l)) fprintf(PRINTST,(a),(b),(c),(d),(e))
#  define PRINTSTAT6(l,a,b,c,d,e,f) if (guc_verb >= (l)) fprintf(PRINTST,(a),(b),(c),(d),(e),(f))

#  define PRINTER1(a) if (guc_severity > 0) fprintf(PRINTERROR,(a)) 
#  define PRINTER2(a,b) if (guc_severity > 0) fprintf(PRINTERROR,(a),(b))
#  define PRINTER3(a,b,c) if (guc_severity > 0) fprintf(PRINTERROR,(a),(b),(c)) 
#  define PRINTER4(a,b,c,d) if (guc_severity > 0) fprintf(PRINTERROR,(a),(b),(c),(d))
#  define PRINTER5(a,b,c,d,e) if (guc_severity > 0) fprintf(PRINTERROR,(a),(b),(c),(d),(e))

#ifdef  PRN_ON_SCRN
/* print on the screen */
#  define PRINTMSG0(x) printf(x)
#  define PRINTMSG1(x,a1) printf(x,a1)
#  define PRINTMSG2(x,a1,a2) printf(x,a1,a2)
#  define PRINTMSG3(x,a1,a2,a3) printf(x,a1,a2,a3)
#  define PRINTMSG4(x,a1,a2,a3,a4) printf(x,a1,a2,a3,a4)
#  define PRINTMSG5(x,a1,a2,a3,a4,a5) printf(x,a1,a2,a3,a4,a5)
#  define PRINTMSG6(x,a1,a2,a3,a4,a5,a6) printf(x,a1,a2,a3,a4,a5,a6)
#  define PRINTMSG7(x,a1,a2,a3,a4,a5,a6,a7) printf(x,a1,a2,a3,a4,a5,a6,a7)
#  define PRINTMSG8(x,a1,a2,a3,a4,a5,a6,a7,a8) printf(x,a1,a2,a3,a4,a5,a6,a7,a8)
#  define PRINTMSG9(x,a1,a2,a3,a4,a5,a6,a7,a8,a9) printf(x,a1,a2,a3,a4,a5,a6,a7,a8,a9)
#elif   defined(PRN_ON_DP)
/* print on dumped memory */
#  define PRINTMSG0(x) DP((x))
#  define PRINTMSG1(x,a1) DP((x,a1))
#  define PRINTMSG2(x,a1,a2) DP((x,a1,a2))
#  define PRINTMSG3(x,a1,a2,a3) DP((x,a1,a2,a3))
#  define PRINTMSG4(x,a1,a2,a3,a4) DP((x,a1,a2,a3,a4))
#  define PRINTMSG5(x,a1,a2,a3,a4,a5) DP((x,a1,a2,a3,a4,a5))
#  define PRINTMSG6(x,a1,a2,a3,a4,a5,a6) DP((x,a1,a2,a3,a4,a5,a6))
#  define PRINTMSG7(x,a1,a2,a3,a4,a5,a6,a7) DP((x,a1,a2,a3,a4,a5,a6,a7))
#  define PRINTMSG8(x,a1,a2,a3,a4,a5,a6,a7,a8) DP((x,a1,a2,a3,a4,a5,a6,a7,a8))
#  define PRINTMSG9(x,a1,a2,a3,a4,a5,a6,a7,a8,a9) DP((x,a1,a2,a3,a4,a5,a6,a7,a8,a9))
#endif

#else

#  define PRINTMES1(l,a)                       /* blanck */
#  define PRINTMES2(l,a,b)                     /* blanck */  
#  define PRINTMES3(l,a,b,c)                   /* blanck */
#  define PRINTMES4(l,a,b,c,d)                 /* blanck */
#  define PRINTMES5(l,a,b,c,d,e)               /* blanck */
#  define PRINTMES6(l,a,b,c,d,e,f)             /* blanck */  
      
#  define PRINTSTAT1(l,a)                      /* blanck */
#  define PRINTSTAT2(l,a,b)                    /* blanck */
#  define PRINTSTAT3(l,a,b,c)                  /* blanck */
#  define PRINTSTAT4(l,a,b,c,d)                /* blanck */
#  define PRINTSTAT5(l,a,b,c,d,e)              /* blanck */
#  define PRINTSTAT6(l,a,b,c,d,e,f)            /* blanck */

#  define PRINTER1(a)                          /* blanck */
#  define PRINTER2(a,b)                        /* blanck */
#  define PRINTER3(a,b,c)                      /* blanck */
#  define PRINTER4(a,b,c,d)                    /* blanck */
#  define PRINTER5(a,b,c,d,e)                  /* blanck */

#  define PRINTMSG0(x) 
#  define PRINTMSG1(x,a1) 
#  define PRINTMSG2(x,a1,a2) 
#  define PRINTMSG3(x,a1,a2,a3) 
#  define PRINTMSG4(x,a1,a2,a3,a4) 
#  define PRINTMSG5(x,a1,a2,a3,a4,a5)
#  define PRINTMSG6(x,a1,a2,a3,a4,a5,a6)
#  define PRINTMSG7(x,a1,a2,a3,a4,a5,a6,a7)
#  define PRINTMSG8(x,a1,a2,a3,a4,a5,a6,a7,a8)
#  define PRINTMSG9(x,a1,a2,a3,a4,a5,a6,a7,a8,a9)

#endif  /* AST_PRINT */      

#undef ABS
#undef IMIN
#define ABS(x)  ( ((x) < 0) ? - (x) : (x) )
#define IMIN(x,y) ((x) < (y)) ? (x) : (y)
#define MIN(x,l) ((x)<(l)) ? (x) : (l)
#define MAX(x,h) ((x)>(h)) ? (x) : (h)


#define SWAP_PUI(a,b)				\
{						\
    unsigned char *temp;			\
						\
    temp = a;					\
    a    = b;					\
    b    = temp;				\
}

#define SWAP_PSI(a,b)				\
{						\
    signed char *temp;				\
						\
    temp = a;					\
    a    = b;					\
    b    = temp;				\
}




#endif /* H_AST_MACROS_H */
