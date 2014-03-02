/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mips.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <pcmprocessings/mips.nmf>
#include <assert.h>
#include <stdio.h>

#ifdef __flexcc2__
unsigned long mips_cycles = 0;
#endif /* __flexcc2__ */


//FIXME to be remove after bug fix TEMA0000161
#define get_cycle_count() 0xDEAD


////////////////////////////////////////////////////////////
//			Global Variables
////////////////////////////////////////////////////////////
#ifdef __flexcc2__
#define CYCLES_OVERHEAD 60
#define BUFFER_SIZE     (19200 * 2)                        // 0.1 second maxi for 192 kHz stereo
//#define BUFFER_SIZE     (192000 * 2)                       // 1 second maxi for 192 kHz stereo
static EXTMEM volatile t_sword buffer_sample[BUFFER_SIZE]; // volatile to force memory read & write in waiting loop of processBuffer routine
#endif /* __flexcc2__ */
static int  read_index         = 0;
static int  write_index        = 0;
static int  mInputBitSize      = 0;
static int  nb_channels        = 0;
static int  sample_freq        = 0;
static long samples_per_second = 0;
static long cycles_per_second  = 0;
static int  delay_us           = 0;
static int  delay_sample       = 0;

////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////


static int get_sample_freq(t_sample_freq freq)
{
    switch(freq)
    {
        case FREQ_192KHZ:
	    return 192000;
        case FREQ_176_4KHZ:
	    return 176400;
        case FREQ_128KHZ:
	    return 128000;
        case FREQ_96KHZ:
	    return 96000;
        case FREQ_88_2KHZ:
	    return 88200;
        case FREQ_64KHZ:
	    return 64000;
        case FREQ_48KHZ:
	    return 48000;
        case FREQ_44_1KHZ:
	    return 44100;
        case FREQ_32KHZ:
	    return 32000;
        case FREQ_24KHZ:
	    return 24000;
        case FREQ_22_05KHZ:
	    return 22050;
        case FREQ_16KHZ:
	    return 16000;
        case FREQ_12KHZ:
	    return 12000;
        case FREQ_11_025KHZ:
	    return 11025;
        case FREQ_8KHZ:
	    return 8000;
        case FREQ_7_2KHZ:
	    return 7200;
        default:
	    return 0;
    }
}


static void update_values(void)
{
    samples_per_second = ((long) nb_channels) * ((long) sample_freq);
    if(samples_per_second == 0)
    {
	delay_sample = 0;
	read_index   = write_index;
    }
    else
    {
        delay_sample = (int) (((long) delay_us) * samples_per_second / 1000000);
	if(delay_sample > BUFFER_SIZE - 1)
	{
	    delay_sample = BUFFER_SIZE - 1;
	}
	if(write_index < delay_sample)
	{
	    read_index = write_index - delay_sample + BUFFER_SIZE;
	}
	else
	{
	    read_index = write_index - delay_sample;
	}
    }
}


void METH(open)()
{
}


void METH(reset)()
{
    int i;

    for(i = 0; i < BUFFER_SIZE; i++)
    {
	buffer_sample[i] = 0;
    }
    read_index  = 0;
    write_index = delay_sample;
}


void METH(setConfig)(Mips_configure_t Mips_configure)
{
    delay_us          = (int)  Mips_configure.delay_us;
    cycles_per_second = (long) Mips_configure.cycles_per_second;
    
    update_values();
}


void METH(newFormat)(t_sample_freq *freq, t_uint16 *chans_nb, t_uint16 *sample_bitsize)
{
    nb_channels = (int) *chans_nb;
    sample_freq = get_sample_freq(*freq);

    update_values();
}


void METH(processBuffer)(t_sword *p_inbuf, t_sword *p_outbuf, t_uint16 size)
{
    int i;
#ifdef __flexcc2__
    unsigned long begin_cycles;
    unsigned long end_cycles;
    unsigned long current_cycles;
    
    begin_cycles = get_cycle_count();
    if(samples_per_second != 0)
    {
        end_cycles = begin_cycles + cycles_per_second * ((unsigned long) size) / ((unsigned long) samples_per_second) - CYCLES_OVERHEAD;
    }
    else
    {
        end_cycles = begin_cycles;
    }
#endif /* __flexcc2__ */
    
    for(i = 0; i < size; i++)
    {
        buffer_sample[write_index++] = p_inbuf[i];
	p_outbuf[i]                  = buffer_sample[read_index++];
	if(write_index == BUFFER_SIZE)
	{
	    write_index = 0;
	}
	if(read_index == BUFFER_SIZE)
	{
	    read_index = 0;
	}
    }

#ifdef __flexcc2__
    for(i = write_index; (current_cycles = get_cycle_count()) < end_cycles; i++)
    {
        t_sword tmp;
	
	if(i == BUFFER_SIZE)
	{
	    i = 0;
	}
        tmp              = buffer_sample[i];
	buffer_sample[i] = tmp;
    }
    mips_cycles += current_cycles - begin_cycles + CYCLES_OVERHEAD;
#endif /* __flexcc2__ */
}

