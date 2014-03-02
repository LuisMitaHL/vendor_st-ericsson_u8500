/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   volctrl_util.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/

/*
 *
 *
 *
 *
 *
 *
 */
//#define my_debug
#ifdef _NMF_MPC_
#include <libeffects/libvolctrl.nmf>
#endif //#ifdef _NMF_MPC_
#include "volctrl_include.h"
#include "volctrl.h"
#include <math.h>
#ifdef my_debug
unsigned long startcount, stopcount;
#include <stdio.h>
#endif
#ifndef MMDSP
extern float volctrl_get_dB_from_gain(float gain_lin);

double volctrl_get_gain_from_dB(float gain_dB) {
    return pow((double)10, ((double)gain_dB)/((double)20));
}

float volctrl_get_dB_from_gain(float gain_lin) {
    return (float)20 * (float)(log10((double)gain_lin));
}

float volctrl_convert_Q8_to_float(int gain) {
    return ((float)gain)/((float)onedB_Q8);
}

void volctrl_set_one_gain(VOLCTRL_LOCAL_STRUCT_T *volctrl, int gain_idx, unsigned int init_current_gain, float gain_cmd) {
    double tmp_gain;
    double delta_gain;
    int iter = (int)((double)(volctrl->duration[gain_idx]) * ((double)(volctrl -> sample_freq)/100000.));
	volctrl->current_duration[gain_idx]=volctrl->duration[gain_idx];
	if (iter<22) iter=0;
    tmp_gain = volctrl_get_gain_from_dB(gain_cmd);
    volctrl->gain[gain_idx] = (float)tmp_gain;

    if ((init_current_gain != 0) || 
			(fabs(volctrl->gain[gain_idx] - (float)volctrl->gain_smoothed[gain_idx]) < VOLCTRL_EPSILON) ||
			(iter==0)
			) 
	{
        // No need for ramp
        volctrl->nb_smooth_iter[gain_idx] = 0;
        volctrl->gain_smoothed[gain_idx] = (double)volctrl->gain[gain_idx];
        if (gain_cmd <= VOLCTRL_MINIMUM_GAIN) {
            volctrl->gain_mask &= ~(1<<gain_idx);
        }
        else
        {
            volctrl->gain_mask |= (1<<gain_idx);
        }
    }
	else
	{
		//compute delta gain
		//nb_iter is fixed here (use multiple of nb_iter for mmdsp)
		if (volctrl->db_ramp) {
			delta_gain = (double)gain_cmd;
			delta_gain -= (double)volctrl_get_dB_from_gain((float)(volctrl->gain_smoothed[gain_idx]));
			delta_gain /= iter;
			delta_gain = volctrl_get_gain_from_dB((float)delta_gain);
		}
		else {
			delta_gain = tmp_gain;
			delta_gain -= volctrl->gain_smoothed[gain_idx];
			delta_gain /= iter;
		}
		volctrl->alpha[gain_idx] = delta_gain;
		volctrl->nb_smooth_iter[gain_idx] = iter;
		if (gain_cmd > VOLCTRL_MINIMUM_GAIN) {
			volctrl->gain_mask |= (1<<gain_idx);
		}
	}
	volctrl->current_gaindB[gain_idx]=(int)(256.*(volctrl_get_dB_from_gain((float)(volctrl->gain_smoothed[gain_idx]))));
//	printf("\n starting current_gaindB: %d \n",volctrl->current_gaindB[gain_idx]);
}

int volctrl_add_2_dB_gain(float gain_one, float gain_two) {
    double gain_one_lin = volctrl_get_gain_from_dB(gain_one);
    double gain_two_lin = volctrl_get_gain_from_dB(gain_two);

    return (volctrl_get_dB_from_gain((float)(gain_one_lin+gain_two_lin)));
}


void volctrl_set_new_config(VOLCTRL_LOCAL_STRUCT_T *volctrl)
{
    int i,config_index ;
    unsigned int init_current_gain;
    if(volctrl->new_config & VOLCTRL_IMMEDIAT_CMD_PENDING)
    {
        volctrl->new_config &= (~VOLCTRL_IMMEDIAT_CMD_PENDING);
		init_current_gain=1;
		config_index=1;
		volctrl_set_new_config_core(volctrl,config_index,init_current_gain); //program initial current gain
    }
    else
    {
        volctrl->new_config = 0;
		config_index=0;
/*		if (volctrl->duration==0) init_current_gain=1;
			else*/ init_current_gain=0;
		volctrl_set_new_config_core(volctrl,config_index,init_current_gain); //progrem end gain for ramp
		volctrl->configL &= ~1;volctrl->configR &= ~1; //bit0 for nmf bit1 for standalone
		for (i=0;i<MAX_MCH;i++)
			volctrl->configX[i] &= ~1;
    }
}


void volctrl_set_new_config_core(VOLCTRL_LOCAL_STRUCT_T *volctrl,int config_index,unsigned int init_current_gain)
{
    int i;
    volctrl->gain_mask   = 0;
#ifndef MMDSP
	if (volctrl->multichannel!=0)
	{
		for(i=0;i<volctrl->input_mode;i++)
		{
			//----------- only set one gain if setramp was called for 1 of the N channel --------------------
			if ((volctrl->configX[i]!=0)&&(volctrl->terminate[i+START_IDX_MCH]==0))
			{
				float gain_asked = volctrl_convert_Q8_to_float(volctrl->gain_cmd[config_index][i+START_IDX_MCH]);
				volctrl_set_one_gain(volctrl, i+START_IDX_MCH, init_current_gain, gain_asked);
			}
		}
	}
	else
	{
#endif
    	if( volctrl->input_mode == VOLCTRL_INPUT_MONO)
    	{
        	/* Mono case, only apply gll */  
        	volctrl_set_one_gain(volctrl, GLL, init_current_gain, volctrl_convert_Q8_to_float(volctrl->gain_cmd[config_index][GLL]));
    	}
    	else if (volctrl->downmix)
    	{
        	if (volctrl->input_mode == VOLCTRL_INPUT_STEREO)
        	{
            	/* in this case we do : Left  = 0.707(gll+glr)Left + 0.707(grr+grl)Right
               		Right = 0
               	*/
            	int gain_cmd;
            	gain_cmd = volctrl_add_2_dB_gain(volctrl_convert_Q8_to_float(volctrl->gain_cmd[config_index][GLL]),
                    volctrl_convert_Q8_to_float(volctrl->gain_cmd[config_index][GLR]));
            	gain_cmd -= 6.0; // 0.5 in dB  //3.0; // 0.707 in dB

            	volctrl_set_one_gain(volctrl, GLL, init_current_gain, gain_cmd);
            	gain_cmd =  volctrl_add_2_dB_gain(volctrl_convert_Q8_to_float(volctrl->gain_cmd[config_index][GRR]),
                    volctrl_convert_Q8_to_float(volctrl->gain_cmd[config_index][GRL]));
            	gain_cmd -= 6.0;  //3.0;

            	volctrl_set_one_gain(volctrl, GRL, init_current_gain, gain_cmd);
        	}
        	else
        	{
            	/* mono to mono, only applied gll */  
            	volctrl_set_one_gain(volctrl, GLL, init_current_gain, volctrl->gain_cmd[config_index][GLL]);
            	volctrl->gain_mask = GLL_MASK;
        	}
    	}
		else
		{
			/* generic case : Left  = (gll * Left) + (grl * Right)
		   		Right = (glr * Left) + (grr * Right)
		   	*/
			for(i=0;i<4;i++)
			{
				//----------- only set gain on L or R channel if terminate was not configured or setramp was called for L or R channel resp
				if ((((i!=GLL)||(volctrl->configL!=0))&&(volctrl->terminate[GLL]==0))&&(((i!=GRR)||(volctrl->configR!=0))&&(volctrl->terminate[GRR]==0)))
				{
					float gain_asked = volctrl_convert_Q8_to_float(volctrl->gain_cmd[config_index][i]);
					volctrl_set_one_gain(volctrl, i, init_current_gain, gain_asked);
				}
			}
		}
#ifndef MMDSP
	} //if (volctrl.multichannel!=0)
#endif
}

int EXTERN volctrl_freq[18]=
{
    96000  /*ESAA_FREQ_UNKNOWNKHZ*/,
    96000  /*ESAA_FREQ_192KHZ*/,
    96000  /*ESAA_FREQ_176_4KHZ*/,
    96000  /*ESAA_FREQ_128KHZ*/,
    96000  /*ESAA_FREQ_96KHZ*/,
    88200  /*ESAA_FREQ_88_2KHZ*/,
    64000  /*ESAA_FREQ_64KHZ*/,
    48000  /*ESAA_FREQ_48KHZ*/,
    44100  /*ESAA_FREQ_44_1KHZ*/,
    32000  /*ESAA_FREQ_32KHZ*/,
    24000  /*ESAA_FREQ_24KHZ*/,
    22050  /*ESAA_FREQ_22_05KHZ*/,
    16000  /*ESAA_FREQ_16KHZ*/,
    12000  /*ESAA_FREQ_12KHZ*/,
    11025  /*ESAA_FREQ_11_025KHZ*/,
    8000   /*ESAA_FREQ_8KHZ*/,
    7200   /*ESAA_FREQ_7_2KHZ*/,
    8000   /*ESAA_FREQ_LAST_IN_LIST*/
};


void volctrl_init(VOLCTRL_LOCAL_STRUCT_T *volctrl,int freq_enum)
{
    volctrl->sample_freq =  volctrl_freq[freq_enum];
}
#else  //#ifndef MMDSP
#define REMOVEFLOAT
#ifdef REMOVEFLOAT
void volctrl_div(int *res, int num, int den);

#include "volctrl_powtab.c"

#endif

//#define DISABLE_RAMP // AB: temporary remove ramp for V1 wake up
void volctrl_set_one_gain(VOLCTRL_LOCAL_STRUCT_T *volctrl, int gain_idx, unsigned int init_current_gain, int gain_cmd)
{
    MMshort tmp_short;
    MMshort shift;
    MMlong  tmp_long;
    MMshort delta_gain_db;
#ifdef REMOVEFLOAT
	int y,expval,yexp,delta_gain_dB_normalized,nb_iter_normalized;
	Word56 accu;
#else
    double  delta_gain_lin;
#endif
	int nb_iter = wextract_h(wL_msl(wL_add(wL_imuluu(volctrl->duration[gain_idx],volctrl->sample_freq),0x400000),1));
	volctrl->current_duration[gain_idx]=volctrl->duration[gain_idx];
//    duration is in 10*microseconds (1=10^(-5) seconds)
//    sample_freq is in Hz/100000 in Q23 fractionary format
	if (nb_iter<22) nb_iter=0;
//	  nb_iter is kept to 0 (no ramp) if it is less than 22 or .5msec(fs=44100)
//	  this enables alpha maximum value of .45


//  compute final linear gain
    if(gain_cmd > VOLCTRL_MINIMUM_GAIN)
    {
        tmp_long = get_gain_from_dB(gain_cmd, &shift);
        volctrl->gain[gain_idx] = wL_msl(tmp_long,(shift - GUARD_BIT + 24));
    }
    else
    {
        gain_cmd = VOLCTRL_MINIMUM_GAIN;
        volctrl->gain[gain_idx] = 0;
    }

#ifdef DISABLE_RAMP
	init_current_gain=1;
#endif
    if((init_current_gain != 0) ||
            (volctrl->gain[gain_idx] == volctrl->gain_smoothed[gain_idx]) ||
				(nb_iter==0)
			)
    {
        // no need for ramp 
        volctrl->nb_smooth_iter[gain_idx] = 0;
        volctrl->gain_smoothed[gain_idx]  = volctrl->gain[gain_idx];
    }
#ifndef DISABLE_RAMP
    else
    {
        // compute gain delta in dB (between two samples)

        // first get current gain in dB
        if(volctrl->gain_smoothed[gain_idx] == 0)
        {
            delta_gain_db = VOLCTRL_MINIMUM_GAIN;
            tmp_long = get_gain_from_dB(VOLCTRL_MINIMUM_GAIN, &shift);
            volctrl->gain_smoothed[gain_idx] = wL_msl(tmp_long,(shift - GUARD_BIT + 24));
        }
        else
        {
            delta_gain_db = get_dB_from_gain(wextract_h(volctrl->gain_smoothed[gain_idx]), GUARD_BIT);
        }

        if (volctrl->db_ramp) {
            delta_gain_db = gain_cmd - delta_gain_db;
#ifndef REMOVEFLOAT
            tmp_long = (MMlong)(1<<8) * (MMlong)nb_iter;
            delta_gain_lin = (double)(delta_gain_db)/(double)tmp_long;

            // force delta gain in [-6.02,6.02] so that linear gain is in [0.5,2]
            if(delta_gain_lin > 0)
            {
                while(delta_gain_lin > 6.02)
                {
                    nb_iter  += nb_iter;
                    tmp_long = (MMlong)(1<<8) * (MMlong)nb_iter;
                    delta_gain_lin = (double)(delta_gain_db)/(double)tmp_long;
                }
            }
            else
            {
                while(delta_gain_lin < -6.02)
                {
                    nb_iter  += nb_iter;
                    tmp_long = (MMlong)(1<<8) * (MMlong)nb_iter;
                    delta_gain_lin = (double)(delta_gain_db)/(double)tmp_long;
                }
			}
#endif
//#ifndef _NMF_MPC_
#ifdef REMOVEFLOAT
//--------------------------- Formula to be executed ----------------------------------------------------
//			alpha=10^(delta_gain_dB/(20/nb_iter))-1=e^(delta_gain_dB/(20*log10(e)/nb_iter))-1=e^y-1 
//			where: e^y = 1 + y + y^2/2! + y^3/3! +....
//--------------------------- Implementation ------------------------------------------------------------			
//			delta_gain_dB_code=delta_gain_dB*256;
//			coef=2^11/(256*20.*log10(e));
//			y=2^(-11)*delta_gain_dB_code*coef/nb_iter;  
//			alpha= y + y^2/2 + y^3/6 + y^4/24 + ... 
//-------------------------------------------------------------------------------------------------------
			yexp=Exp_coef;
			accu=wL_fmul(delta_gain_db,coef_volctrl);//  num=delta_gain_dB_code*(2^11/(256*20.*log10(e)))
			/*------------------------- delta_gain_db normalized ---------------------------------------*/
			expval=wedge_X(accu);
			yexp=yexp+expval; 
			delta_gain_dB_normalized=wround_X(wX_msl(accu,expval));
			/*-------------------------- nb_iter_normalized -------------------------------------------*/
			expval=wedge(nb_iter);
			yexp=yexp-expval;
			nb_iter_normalized=wmsl(nb_iter,expval);
			/*------------------- delta_gain_dB_normalized/nb_iter_normalized ------------------------ */
			volctrl_div(&y,delta_gain_dB_normalized,nb_iter_normalized); // num/nb_ter
			expval=wedge(y);
			yexp=yexp+expval-1; //-1 division result was attenuated by 2
			y=wmsl(y,expval); // y=(num/nb_iter)*2^(-11)
			/*------------------------- y + y^2/2 + y^3/6 + y^4/24 ------------------------------------*/
			accu=wX_depmsp(y);
			accu=wX_msr(accu,yexp); //1st term
			accu=wX_add(accu,wX_msr(wX_fmul(y,y),2*yexp+1)); //2nd term
			accu=wX_add(accu,wX_msr(wX_fmul(wfmulr(wfmulr(y,y),y),0x155555),3*yexp)); //3rd term
			accu=wX_add(accu,wX_msr(wX_fmul(wfmulr(wfmulr(wfmulr(y,y),y),y),0x55555),4*yexp)); //4th term
			/*----------------------------- store in tmp_long -----------------------------------------*/
			tmp_long=accu;
#ifdef my_debug
			printf("\n");
			printf("alpha_float: %e alpha_bt: %e nb_iter_float: %d nb_iter_bt: %d",
					pow(10.0,((double)(delta_gain_db)/(double)((MMlong)(1<<8)*(MMlong)nb_iter))/20.0) - 1.0,
				    (double)tmp_long/(pow(2.0,47.0)),
					(int)((double)(volctrl->duration[gain_idx]) * (44100./100000.)),
					nb_iter
					);
#endif
#else //REMOVEFLOAT
			// now compute linear gain. Do not use get_gain_from_dB because we need more precision
			delta_gain_lin = pow(10.0, (delta_gain_lin)/20.0) - 1.0; // -1.0 so that it is in ]-0.5,1[
			// and convert it into fractional
            tmp_long  = (delta_gain_lin * (0x800000000000UL));
#endif //REMOVEFLOAT
        } else {
            tmp_long = volctrl->gain[gain_idx] - volctrl->gain_smoothed[gain_idx];
            tmp_long /= nb_iter;
        }
        tmp_short = wedge_X(tmp_long);
        volctrl->alpha[gain_idx]       = wextract_h(tmp_long << tmp_short);
        volctrl->alpha_shift[gain_idx] = -tmp_short + 1; // +1 because in volctrl_smooth_one_gain we do not use fmul
        volctrl->nb_smooth_iter[gain_idx] = nb_iter;
    }
#endif // DISABLE_RAMP
	volctrl->current_gaindB[gain_idx]=get_dB_from_gain(wextract_h(volctrl->gain_smoothed[gain_idx]), GUARD_BIT);
//	printf("\n starting current_gaindB: %d \n",volctrl->current_gaindB[gain_idx]);
}



MMshort volctrl_add_2_dB_gain(MMshort gain_one, MMshort gain_two)
{
    MMlong gain_one_lin;
    MMlong gain_two_lin;
    MMshort shift;

    if(gain_one > VOLCTRL_MINIMUM_GAIN)
    {
        gain_one_lin = get_gain_from_dB(gain_one, &shift);
        gain_one_lin = wL_msl(gain_one_lin,(shift - GUARD_BIT + 24));
    }
    else
    {
        gain_one_lin = 0;
    }

    if(gain_two > VOLCTRL_MINIMUM_GAIN)
    {
        gain_two_lin = get_gain_from_dB(gain_two, &shift);
        gain_two_lin = wL_msl(gain_two_lin,(shift - GUARD_BIT + 24));
    }
    else
    {
        gain_two_lin = 0;
    }

    gain_one_lin += gain_two_lin;

    return get_dB_from_gain(wextract_h(gain_one_lin), GUARD_BIT);
}

void volctrl_set_new_config(VOLCTRL_LOCAL_STRUCT_T *volctrl)
{
    int config_index;
    unsigned int init_current_gain;
    if(volctrl->new_config & VOLCTRL_IMMEDIAT_CMD_PENDING)
    {
        volctrl->new_config &= (~VOLCTRL_IMMEDIAT_CMD_PENDING);
		init_current_gain=1;
		config_index=1;
		volctrl_set_new_config_core(volctrl,config_index,init_current_gain); //program initial current gain
    }
    else
    {
        volctrl->new_config = 0;
/*		if (volctrl->duration==0) init_current_gain=1;
			else*/ init_current_gain=0;
		config_index=0;
		volctrl_set_new_config_core(volctrl,config_index,init_current_gain); //progrem end gain for ramp
		volctrl->configL &= ~1;volctrl->configR &= ~1; //bit0 for nmf bit1 for standalone
    }
}

void volctrl_set_new_config_core(VOLCTRL_LOCAL_STRUCT_T *volctrl,int config_index,unsigned int init_current_gain)
{
    int i;
#ifdef my_debug
	startcount=get_cycle_count();
#endif
    volctrl->gain_mask   = 0;
    if( volctrl->input_mode == VOLCTRL_INPUT_MONO)
    {
        /* Mono case, only apply gll */  
        volctrl_set_one_gain(volctrl, GLL, init_current_gain, volctrl->gain_cmd[config_index][GLL]);
        volctrl->gain_mask = GLL_MASK;
    }
    else if(volctrl->downmix)
    {
        if( volctrl->input_mode == VOLCTRL_INPUT_STEREO)
        {
            /* in this case we do : Left  = 0.707(gll+glr)Left + 0.707(grr+grl)Right
               Right = 0
               */
            MMshort gain_cmd;
            gain_cmd = volctrl_add_2_dB_gain(volctrl->gain_cmd[config_index][GLL],
                    volctrl->gain_cmd[config_index][GLR]);
            gain_cmd = gain_cmd - sixdB;

            volctrl_set_one_gain(volctrl, GLL, init_current_gain, gain_cmd);
            /* always activate GLL, even if 0 */
            volctrl->gain_mask = GLL_MASK;

            gain_cmd = volctrl_add_2_dB_gain(volctrl->gain_cmd[config_index][GRR],
                    volctrl->gain_cmd[config_index][GRL]);
            gain_cmd = gain_cmd - sixdB;
            volctrl_set_one_gain(volctrl, GRL, init_current_gain, gain_cmd);

            if(volctrl->gain_smoothed[GRL] || volctrl->gain[GRL])
                volctrl->gain_mask   |= GRL_MASK;
        }
        else 
        {
            /* mono to mono, only applied gll */  
            volctrl_set_one_gain(volctrl, GLL, init_current_gain, volctrl->gain_cmd[config_index][GLL]);
            volctrl->gain_mask = GLL_MASK;
        }
    }
    else  /* always output stereo ie always have GRR or GLR activated */
    {
#ifdef DUAL_MONO
        if(volctrl->input_mode == VOLCTRL_INPUT_DUAL_MONO)
        {
            MMshort gain_cmd;

            if(!(previous_gain_mask & GLR_MASK))
            {
                volctrl->gain_smoothed[GLR] = volctrl->gain_smoothed[GLL];
            }

            /* dual mono, there is no right input channel ==> add grX to glX */
            gain_cmd = volctrl_add_2_dB_gain(volctrl->gain_cmd[config_index][GLL],
                    volctrl->gain_cmd[config_index][GRL]);

            volctrl_set_one_gain(volctrl, GLL, init_current_gain, gain_cmd);
            volctrl->gain_mask = GLL_MASK;

            gain_cmd =  volctrl_add_2_dB_gain(volctrl->gain_cmd[config_index][GLR],
                    volctrl->gain_cmd[config_index][GRR]);

            volctrl_set_one_gain(volctrl, GLR, init_current_gain, gain_cmd);
            volctrl->gain_mask |= GLR_MASK;

            /* special test if gll == glr disactivate glr */
            if((volctrl->gain[GLL] == volctrl->gain[GLR]) &&
                    (volctrl->gain_smoothed[GLL] == volctrl->gain_smoothed[GLR]))
            {
                volctrl->gain_mask &= ~GLR_MASK;
                volctrl->nb_smooth_iter[GLR] = 0;
            }
        }
        else
#endif
        {
            /* generic case : Left  = (gll * Left) + (grl * Right)
               Right = (glr * Left) + (grr * Right)
               */
			for(i=0;i<4;i++)
			{
				//----------- only set gain on L or R channel if terminate was not configured or setramp was called for L or R channel resp
				if ((((i!=GLL)||(volctrl->configL!=0))&&(volctrl->terminate[GLL]==0))&&(((i!=GRR)||(volctrl->configR!=0))&&(volctrl->terminate[GLL]==0)))
				{
					if(volctrl->gain_cmd[config_index][i]>VOLCTRL_MINIMUM_GAIN || volctrl->gain_smoothed[i])
					{
						volctrl_set_one_gain(volctrl, i, init_current_gain, volctrl->gain_cmd[config_index][i]);
						volctrl->gain_mask |= (1<<i);
					}
				}
			}

            /* always activate gain on left and right */
            if(!(volctrl->gain_mask & LEFT_MASK))
            {
                volctrl_set_one_gain(volctrl, GLL, init_current_gain, volctrl->gain_cmd[config_index][GLL]);
                volctrl->gain_mask |= GLL_MASK;
            }

            if(!(volctrl->gain_mask & RIGHT_MASK))
            {
                volctrl_set_one_gain(volctrl, GRR, init_current_gain, volctrl->gain_cmd[config_index][GRR]);
                volctrl->gain_mask |= GRR_MASK;
            }
        }
    }
#ifdef my_debug
	stopcount=get_cycle_count();
	printf("cycles: %ld\n",stopcount-startcount);
#endif
}


MMshort EXTERN volctrl_freq[18]=
{
    8053064,  // Q23 representation of  ESAA_FREQ_UNKNOWNKHZ/100000
    8053064,  // Q23 representation of  ESAA_FREQ_192KHZ/100000
    8053064,  // Q23 representation of  ESAA_FREQ_176_4KHZ/100000
    8053064,  // Q23 representation of  ESAA_FREQ_128KHZ/100000
    8053064,  // Q23 representation of  ESAA_FREQ_96KHZ/100000
    7398752,  // Q23 representation of  ESAA_FREQ_88_2KHZ/100000
    5368709,  // Q23 representation of  ESAA_FREQ_64KHZ/100000
    4026532,  // Q23 representation of  ESAA_FREQ_48KHZ/100000
    3699376,  // Q23 representation of  ESAA_FREQ_44_1KHZ/100000
    2684355,  // Q23 representation of  ESAA_FREQ_32KHZ/100000
    2013266,  // Q23 representation of  ESAA_FREQ_24KHZ/100000
    1849688,  // Q23 representation of  ESAA_FREQ_22_05KHZ/100000
    1342177,  // Q23 representation of  ESAA_FREQ_16KHZ/100000
    1006633,  // Q23 representation of  ESAA_FREQ_12KHZ/100000
     924844,  // Q23 representation of  ESAA_FREQ_11_025KHZ/100000
     671089,  // Q23 representation of  ESAA_FREQ_8KHZ/100000
     603980,  // Q23 representation of  ESAA_FREQ_7_2KHZ/100000
     671089   // Q23 representation of  ESAA_FREQ_LAST_IN_LIST/100000
};

//--------- 100000/16/fs for calculation of duration from nb_iter ---
//	duration=nb_iter*100000/fs or (nb_iter*6250/fs)*16

#define coefunknown (int)((float)6250*(float)8388607/(float)96000)
#define coef192 	(int)((float)6250*(float)8388607/(float)96000)
#define coef176 	(int)((float)6250*(float)8388607/(float)96000)
#define coef128 	(int)((float)6250*(float)8388607/(float)96000)
#define coef96 		(int)((float)6250*(float)8388607/(float)96000)
#define coef88 		(int)((float)6250*(float)8388607/(float)88200)
#define coef64 		(int)((float)6250*(float)8388607/(float)64000)
#define coef48 		(int)((float)6250*(float)8388607/(float)48000)
#define coef44 		(int)((float)6250*(float)8388607/(float)44100)
#define coef32 		(int)((float)6250*(float)8388607/(float)32000)
#define coef24 		(int)((float)6250*(float)8388607/(float)24000)
#define coef22 		(int)((float)6250*(float)8388607/(float)22050)
#define coef16 		(int)((float)6250*(float)8388607/(float)16000)
#define coef12 		(int)((float)6250*(float)8388607/(float)12000)
#define coef11 		(int)((float)6250*(float)8388607/(float)11025)
#define coef8 		(int)((float)6250*(float)8388607/(float)8000)
#define coef7 		(int)((float)6250*(float)8388607/(float)7200)
#define coeflast 	(int)((float)6250*(float)8388607/(float)8000)

MMshort EXTERN volctrl_freqinv[18]=
{
	coefunknown, 
	coef192, 
	coef176, 
	coef128,
	coef96,
	coef88,
	coef64,
	coef48,
	coef44,
	coef32,
	coef24,
	coef22,
	coef16,
	coef12,
	coef11,
	coef8,
	coef7,
	coeflast
};


void volctrl_init(VOLCTRL_LOCAL_STRUCT_T *volctrl,int freq_enum)
{
    volctrl->sample_freq =  volctrl_freq[freq_enum];
	volctrl->sample_freqinv = volctrl_freqinv[freq_enum];
}

#ifdef REMOVEFLOAT
/*--- Division using Newton-Raphsen approximation for y=1/(2*x) -----*/
// 	          for each iteration, y=2*(y-x*y^2)
void volctrl_div(int *res, int num, int den) // res=num/(2*den)
{
	int temp,tabindex,y,i;
	const Float EXTERN * volctrl_invtab_ptr;
	volctrl_invtab_ptr = volctrl_invtab;
	/*---------- Calculate 1/(2*den) ------------------------------*/
	tabindex=((den-MAXVAL_PLUS_ONE_OVER2)>>SHIFTVAL_INV);
	y=volctrl_invtab_ptr[tabindex];
	for (i=0;i<numiter;i++)
	{
		temp=y;
		temp=wfmulr(temp,y);
		temp=wfmulr(temp,den);
		temp=y-temp;
		y=waddsat(temp,temp);
	}
	/*---------- Calculate num/(2*den) ----------------------------*/
	*res=wfmulr(y,num);
}
#endif //#ifndef REMOVEFLOAT
#endif //#ifndef MMDSP

