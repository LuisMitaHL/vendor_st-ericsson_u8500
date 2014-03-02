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

#include "RedEyes.h"

typedef unsigned char byte ;


#define MIN(a,b) ( (a)<(b) ? (a) : (b) )


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@ DEFINES FOR  THE FIXED POINT VERSION @@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define BIT_PER_PIXEL 8

#define FRAC_BIT 16

#define FILTER_RESOLUTION (BIT_PER_PIXEL+2)

#define FRACT(a) ( (a) & 0x0FFF)
#define INT(a)  ((a)>>FRAC_BIT)
#define UNITY (1<<FILTER_RESOLUTION)
#define LAST_SHIFT (FILTER_RESOLUTION) 


void BilinearInterp(unsigned char *in_img, unsigned char *out_img, int width, int height, int new_width, int new_height);


