/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



/*******************************************************************************
 * File:     ast_defines.h
 *
 * Author:   Antonio Borneo
 * email:    antonio.borneo@st.com
 *
 * Org:      Agrate Advanced System Technologies lab.
 *           STMicroelectronics S.r.l.
 *
 * Created:  16.06.99
 *
 * Purpose:  defines values for the C pre-processor
 *
 * Notes:
 *
 * Status:   AB 25.08.2000: added little endian for Lx
 *
 * Modified: 26.07.2000 by Daniele Bagni (e-mail: Daniele.Bagni@st.com)
 *           25.08.2000 by Antonio Borneo
 *
 * COPYRIGHT:
 * This program is property of Agrate Advanced System Technology
 * lab, from STMicroelectronics S.r.l. It should not be
 * communicated outside STMicroelectronics without authorization.
 ******************************************************************************/


#ifndef H_AST_DEFINES_H
#define H_AST_DEFINES_H


/* Verbosity levels */

#define PRINT_ENCODER  1
#define PRINT_COMPLIA  2
#define PRINT_FRAME    3
#define PRINT_MOTION   4
#define PRINT_PARAM    5
#define AST_WRITEBITS  6

#define SET            0
#define RESET          1

#define OFF            0
#define ON             1

#define FILENAME_PSNR "stat.enc"

/*******************************************************************************
 * This flag signals the writing of statistics on the file stat.enc
 */

#if 0
 #define STATISTIC
#endif


/*******************************************************************************
 * This flag compiles a version of the encoder with modified
 * on-line help and some features disabled
 */

//We should have all features
#if 0
 #define NOMADIK_VERSION
#endif


/* New Simple Profile Level */

#if 1
 #define _NEW_SP_LEVELS
#endif

/* GT: Automatic Level Detection for Simple Profile, 02 Apr 2007 */
#if 0
#define _AUTOMATIC_LEVEL_DETECTION
#endif

/* GT: I and P Picture QP Correlation, 02 Apr 2007 */
#if 0
#define _VBR_IP_QP_CORRELATION
#endif

/* GT: enables Enhanced MB control for better QP distribution, 02 Apr 2007 */
#if 0
#define _CBR_ENHANCED_MB_CONTROL
#endif

/* GT: Motion Estimation Search Window Extesion in Simple Profile Mode, 02 Apr 2007 */
#if 0
#define _SEARCH_WINDOW_EXTENSION
#endif

/* End */

/*******************************************************************************
 * comment the following line if you don't want to use restricted pointers
 */

#if 0
 #define AST_RSTR
#endif


#define SKIP_SAD_THRESHOLD              0

/* GT: enable 4Mb/s Overcome for CBR mode, 02 Apr 07 */
#define _4MBPS_OVERCOME_ON 1
#define _ORIGINAL 2
#define _4MBPS_OVERCOME  _4MBPS_OVERCOME_ON


/*******************************************************************************
 * uncomment the following line if you want to use asm instructions
 * defined in ast_asm.h and ast_asm_xxx.h modules.
 * This will provide processor specific defines (e.g. #define AST_ASM_LX1 or
 * #define AST_ASM_TM1).
 * uncomment the next one if you want asm instructions emulated
 * by ANSI-C macros defined in ast_asm.h and ast_asm_emul.h
 */

#define AST_ASM_EMUL

/*******************************************************************************
 *  uncomment the following line if you want to store the VP position in an
 *  external file
 */

#if 0
 #define VP_STORE_POSITION
 #define VP_ARRAY_SIZE 32
#endif


/*******************************************************************************
 * uncomment the following line if you want to force the not_coded encoding
 * indipendently on the dquant
 */
#if 0
 #define ENFORCE_NOT_CODED
#endif


/*******************************************************************************
 * uncomment the following line if you don't want to generate Motion-Vector
 * report
 */
#if 0
 #define MP4_REPORT
#endif

/*******************************************************************************
 * Algo for MB type (intra/inter) decision.
 * Uncomment ONLY the one you want to use.
 */
/* MAD based algorithm: original algo */
#define MBTYPE_USE_MAD
/* SAD based algorithm: very low computational cost */
//#define MBTYPE_USE_SAD
/* Motion Field based algorithm */
/*#define MBTYPE_USE_MOTION_FIELD*/
/* Algorithm using both Motion Field and SAD*/
/*#define MBTYPE_USE_MOTION_FIELD_AND_SAD*/

//#define ALTERNATE_SUB_SAMPLING

/*******************************************************************************
 * AST standard defines
 */
#define AST_USED     1
#define AST_NOT_USED 0


/*******************************************************************************
 * The followings are architecture dependent - automatic defines
 */


/*
 * Linux on i386 and Cygnus on Win-NT
 */


# define AST_LITTLE_ENDIAN

/*******************************************************************************
 * The followings are user inserted defines
 */

#ifdef AST_LITTLE_ENDIAN
#define CYG_LITTLE_ENDIAN
#else
#define CYG_BIG_ENDIAN
#endif


/*******************************************************************************
 *
 *                      PRINTING DEBUGGING OPTIONS
 *
 *******************************************************************************/

/*******************************************************************************
 * comment the following line if you don't want to use printf messages defined
 * in module ast_macro.h
 */
#define AST_PRINT

/* internal encoder defines */

#define H263_QUANT    0
#define SHDR_QUANT    2


#define ME_PROBES     0  /* 0: No ME Probes , 1: ME Probes Activated */



#endif /* AST_DEFINES_H */

