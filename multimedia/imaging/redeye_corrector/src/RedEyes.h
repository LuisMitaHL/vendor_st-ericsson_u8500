/*******************************************************************************
 *                                                                                         
 * STMicroelectronics - Company Confidential                                                
 *                                                                                         
 *******************************************************************************
 *                                                                                         
 * Project name		: RedEyes
 * File name			: interpolation.cpp
 * Goal					: Red-Eyes Removal
 * Date					: 11 July 2008           
 * Version				: Release 2.0
 * Author				: Giuseppe Messina - Mirko Guarnera 
 * Company				: AST - STMicroelectronics - Catania                                             
 *                                                                                                     
 *******************************************************************************/


//#include "resource.h"
//#include "atlstr.h"
//#include "atlimage.h"

#pragma once

#ifndef __REDEYES_H

#define  __REDEYES_H
#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
//#include <crtdbg.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
//#include <direct.h>
#include <math.h>
//#include <memory.h>


#include "AIQ_Memory.h" 
#include "AIQ_PerfTracer.h" 
#define AIQ_REC_ID 2

#include "REC_Api.h"

typedef unsigned short IMG16 ;
typedef unsigned char IMG8 ;
#define IMG IMG8

typedef struct 
{
	IMG8 *red_plane;
	IMG8 *green_plane;
	IMG8 *blue_plane;
	int width;
	int height;
	int colors;
} planar_img;

typedef struct REC_InternalContextStruct
{
   unsigned char  GLINT_WHITENESS;
   float          STRENGHT;
   float          ATTENUATION;
} REC_InternalContext;

#define max(a,b) ( ( a > b ) ? a : b)
#define min(a,b) ( ( a < b ) ? a : b)

#endif

