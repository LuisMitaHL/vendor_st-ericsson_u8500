/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dmaout_usefull.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "dmaout.h"

void fillBufferWithTriangle(int * buff, unsigned int nb_sample, unsigned int nb_channel){
#define TOP  (1000<<8)
    int val =  - TOP;
    int increment = ( ( TOP / AB_BLOCKSIZE_MONO_1MS ) << 2 );

    int i=0,j=0,k=0, index=0;

    for (i = 0; i < nb_sample ; i++) {
        for (k = 0; k < nb_channel ; k++) {
            buff[index++] = val;
        }
        j++;
        if(j == (AB_BLOCKSIZE_MONO_1MS/2)){
            increment = - increment;
            j=0;
        }
        val  += increment;
    }
}
