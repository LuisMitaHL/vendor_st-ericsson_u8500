/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 *
 *
 *
 *
 *
 *
 */

#ifdef MMDSP

#ifdef _NMF_MPC_
#include <libeffects/mpc/libdBconv.nmf>
#endif //#ifdef _NMF_MPC_
#include "audiolibs_common.h"
#include "dBconv_include.h"
#include "dB_tab_gain.h"

/*
  dB to linear conversion

  input parameter : gaindB is in Q8 representation
  principle : gain_linear(6,02dB) = 2
  => gain_linear(x) = gain_linear(x' + y*6.02) = gain_linear(x')*2^y with 0<x'<6.02dB
  process :
          gaindB is put in [0;6,02dB[ with sucessive add (or sub) of 6.02dB (=sixdB).
		  the number of add (or sub) is store in shift.
		  Once gaindB is in the range [0;6.02dB] its value is read in the convsersion table tab_gain
		  tab_gain is computed at compilation time according the NB_STEP value (define in dBconv_include.h)
		  gain_linear(x) = tab_gain[x*(NB_STEP/6.02)].   STEP_FACT = NB_STEP / 6.02dB
 */
MMshort get_gain_from_dB(MMshort gaindB, MMshort *shift_res)
{
  MMshort shift = 1; /* all value of tab_gain are already divided by two */
  MMshort step;

  if(gaindB == MINIMUM_GAIN)
  { /* minimum gain see as -infinity dB*/
	shift = 0;
	return 0;
  }
  else
  {
	while (gaindB < 0)
	{
	  gaindB += sixdB;
	  shift -= 1;
	}
	while (gaindB >= sixdB)
	{
	  gaindB -= sixdB;
	  shift +=1;
	}

	step = wfmulr(gaindB,STEP_FACT);
	*shift_res = shift;

	return tab_gain[step];
  }
}


/*
 * gainl in Q23 with shift
 * result is in Q8 representation
 */
MMshort get_dB_from_gain(MMshort gainl, MMshort shift)
{
  MMshort index;
  MMshort gaindB;
  MMlong gainldB,tmp;

  if(gainl == 0) gainl = 1;

  while(gainl < FORMAT_FLOAT(0.5,MAXVAL))
  {
	gainl <<= 1;
	shift --;
  }

  index= 0;
  while ((gainl > tab_gain[index])&&(index<63))
  {
	index ++;
  }

  gainldB = wL_imul(index,STEP_FACT_INV);

  gaindB = wimul(shift-1,sixdB);
  gaindB += wextract_l(gainldB);

  tmp = wL_imul(shift-1,sixdB);
  gaindB = wextract_l(gainldB) + wextract_h(wL_msr(wL_msl(tmp,32),8));


  return gaindB;
}


#endif // MMDSP
