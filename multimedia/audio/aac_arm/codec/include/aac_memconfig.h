#ifndef _aac_memconfig_h_
#define _aac_memconfig_h_

#ifdef __flexcc2__
#define EXTERN __EXTERN
#else
#define EXTERN 
#endif

#define FT_MEM  YMEM  /* Filter related tables are in YMEM     */


#define CONST_MEM    EXTERN
#define TNS_MEM      EXTERN
#define INV_SQRT_NRG EXTERN
#define PNS_MEM      EXTERN
#define HUF_MEM      EXTERN
#define LUT_MEM      EXTERN
#define SR_MEM       EXTERN  
#define CODEBOOK_MEM EXTERN

#endif /* Do not edit below this line */


/* Define location of ROM */
