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

typedef struct{
   int max_x;
   int max_y;
   int min_x;
   int min_y;
}eyeStruct;

// mirko adding 18/07/2008 to avoid the _msize() windows function
//ClusterInfo **new_Clustering(int *Image,short int Hsize, short int Vsize, ClusterInfo **info, int* number );
ClusterInfo **new_Clustering(IMG8 *Image,short int Hsize, short int Vsize, ClusterInfo **info, int* number );

//ClusterInfo **new_Clustering(int *Image,short int Hsize, short int Vsize, ClusterInfo **info);

#define WHITE  255
#define BLACK    0
#define ZERO     0

#define false 0
#define true 1


unsigned short *binary_Erosion(unsigned short *Input,int H, int V, int dim, int type);
unsigned short *binary_Dilatation(unsigned short *Input,int H, int V, int dim, int type);

//IMG8* bit_Dilate_mirko_new(IMG8 *imgptr, int X, int Y,  IMG8 *outptr);
//IMG8* bit_Erode_mirko_new(IMG8 *imgptr, int X, int Y,  IMG8 *outptr);
IMG8* bit_Fake_mirko_new(IMG8 *imgptr, int X, int Y,  IMG8 *outptr);

IMG8* bit_Dilate_3x3(IMG8 *imgptr, int X, int Y,  IMG8 *outptr);
IMG8* bit_Erode_3x3(IMG8 *imgptr, int X, int Y,  IMG8 *outptr);
IMG8* margeMaps(IMG8* map1,IMG8* map2,IMG8* mapOut,int bit_Hsize,int Vsize);

IMG8 *MetodoOtsu(IMG8 *img,IMG8 *out_img, int Hsize, int Vsize, int colors);
//IMG8 *histo_stretch(IMG8 *in_img, int width, int height, int colors,int threshold,int gmax,int gmin);
IMG8 lightness(IMG8 *ptr);
IMG8 lightnessss(IMG8 R, IMG8 G, IMG8 B);
bool eyeIsAlreadyInsert(ClusterInfo* cluster,eyeStruct* arrayEyes,int* numEyes);
bool AddEyeToArray (ClusterInfo* curCluster,eyeStruct* arrayEyes,int* numEyes);
//ClusterInfo **bit_new_Clustering(IMG8 *Image,short int Hsize, short int Vsize, ClusterInfo **info, int* number );

