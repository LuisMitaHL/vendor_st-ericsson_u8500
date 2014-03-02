/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



//#define MYMEM_IN   YMEM
//#define MYMEM_OUT  YMEM


#ifdef __flexcc2__
#define MYMEM_IN    __EXTERN   
#define MYMEM_OUT   __EXTERN
#else
#define MYMEM_IN  
#define MYMEM_OUT 
#endif




extern Float MYMEM_OUT const *out[9];
extern Float MYMEM_OUT const out_16[8];
extern Float MYMEM_OUT const out_32[16];
extern Float MYMEM_OUT const out_64[32];
extern Float MYMEM_OUT const out_128[64];
extern Float MYMEM_OUT const out_256[128];
extern Float MYMEM_OUT const out_512[256];
extern Float MYMEM_OUT const out_1024[512];
extern Float MYMEM_OUT const out_2048[1024];
extern Float MYMEM_OUT const out_4096[2048];



extern Float MYMEM_IN  *in[9];
extern Float MYMEM_OUT  in_4096[4096];
extern Float MYMEM_OUT  in_2048[2048];
extern Float MYMEM_OUT  in_1024[1024];
extern Float MYMEM_OUT  in_512[512];
extern Float MYMEM_OUT  in_256[256];
extern Float MYMEM_OUT  in_128[128];
extern Float MYMEM_OUT  in_64[64];
extern Float MYMEM_OUT  in_32[32];
extern Float MYMEM_OUT  in_16[16];



