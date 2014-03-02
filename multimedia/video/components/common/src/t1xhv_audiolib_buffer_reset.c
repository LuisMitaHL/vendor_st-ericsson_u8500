/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************
  * \note 	copied from: audiolibs/libvector/src
 *  
 *  This function performs "memset" using MMDSP harware loop functionnalities
 */
/*****************************************************************************/

#include "t1xhv_audiolib_vector.h"
#include "stwdsp.h" 

#ifdef SUN
/*****************************************************************************/
/**
 * \brief 	MMDSP memset function
 * \author 	HAMAC A team
 * \param 	*p: buffer address
 * \param 	val: value to set
 * \param 	size: size (sizeof) of the buffer pointed to by *p
 * 
 * \note defined for \b SUN compilation
 * val is set on the range of *p buffer
 */
/*****************************************************************************/

void buffer_reset(char *p, char val, int size)
{
	int             local_size;

	while (size > 0) {

		local_size = wmin(size, 1023);

		HWLOOP(local_size) {
			*p = val;
			p++;
		} END_HWLOOP;
		size -= local_size;
	}
}/* end buffer_reset() */

#else /* not SUN */

/*****************************************************************************/
/**
 * \brief 	MMDSP memset function
 * \author 	HAMAC A team
 * \param 	*p: buffer address
 * \param 	val: value to set
 * \param 	size: size (sizeof) of the buffer pointed to by *p
 * \note defined for other than \b SUN compilation
 *
 * val is set on the range of *p buffer
 */
/*****************************************************************************/
void buffer_reset(char *p, char val, int size)
{
	/* The compiler generates ubyte access, cast to int to force reset
	 * of msb as well */
	int            *p_int = (int *) p;
	int             local_size;

	while (size > 0) {

		local_size = wmin(size, 1023);

		HWLOOP(local_size) {
			*p_int = val;
			p_int++;
		} END_HWLOOP;
		size -= local_size;
	}
}/* end buffer_reset() */

#endif
