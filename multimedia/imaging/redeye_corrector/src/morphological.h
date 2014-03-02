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

typedef struct{
   int min_light;
   int area;
   int max_x;
   int max_y;
   int min_x;
   int min_y;
   int cluster;
   bool redeye;
   int modality;
}ClusterInfo;

#define false 0
#define true 1


