/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __JPEGDEC_PARSER_H
#define __JPEGDEC_PARSER_H


#include <stdio.h>

#ifdef __JPEGDEC_SOFTWARE //for ARM-NMF
	#define JPEGDEC_CLASS jpegdec_arm_nmf_parser_arm
	#define JPEGDec_ARMNMF_ParamAndConfig_CLASS JPEGDec_ARMNMF_ParamAndConfig_SW
	#define JPEGDec_ARMNMF_Parser_CLASS JPEGDec_ARMNMF_Parser_SW

#elif __JPEGDEC_DUAL
	#define JPEGDEC_CLASS jpegdec_arm_nmf_parser_dual
	#define JPEGDec_ARMNMF_ParamAndConfig_CLASS JPEGDec_ARMNMF_ParamAndConfig_Dual
	#define JPEGDec_ARMNMF_Parser_CLASS JPEGDec_ARMNMF_Parser_Dual
#else
	#define JPEGDEC_CLASS jpegdec_arm_nmf_parser_mpc
	#define JPEGDec_ARMNMF_ParamAndConfig_CLASS JPEGDec_ARMNMF_ParamAndConfig
	#define JPEGDec_ARMNMF_Parser_CLASS JPEGDec_ARMNMF_Parser

#endif

#endif // __JPEGDEC_PARSER_H
