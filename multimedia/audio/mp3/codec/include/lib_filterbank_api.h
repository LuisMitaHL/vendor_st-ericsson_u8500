
#ifndef _DSPLIB_FILTERBANK_API_
#define _DSPLIB_FILTERBANK_API_

#include "lib_macros.h"

#ifdef ARM
struct DSPLIB_POLYPHASE_ANA_FILTERBANK
{
	long resolution;
	int *X;	
	double gain;
	void (*AnaNptAFB)(int * Sr, int * Si, int * Yin, long resolution);
	void (*libNptfdctIII)(int *out, int *in);		    
	int offset;
	const int *pWindowAna;
};

struct DSPLIB_POLYPHASE_SYN_FILTERBANK
{
	long resolution;
	int *V;
	double gain;	
	long bDownSampleSBR;
	void (*SynNptSFB)(int * V, int * Sr, int * Si, long resolution);
	void (*libNptfdctII)(int *out, int *in);	
	int offset;
	const int *pWindowSyn;
};
#else

struct DSPLIB_POLYPHASE_ANA_FILTERBANK
{
	long resolution;
	Float *X;	
	double gain;
	void (*AnaNptAFB)(Float * Sr, Float * Si, Float * Yin, long resolution);
	void (*libNptfdctIII)(Float *out, Float *in);		    
	int offset;
	const Float *pWindowAna;
};

struct DSPLIB_POLYPHASE_SYN_FILTERBANK
{
	long resolution;
	Float *V;
	double gain;	
	long bDownSampleSBR;
	void (*SynNptSFB)(Float * V, Float * Sr, Float * Si, long resolution);
	void (*libNptfdctII)(Float *out, Float *in);	
	int offset;
	const Float *pWindowSyn;
};
#endif

typedef struct DSPLIB_POLYPHASE_SYN_FILTERBANK DSPLIB_POLYPHASE_SYN_FILTERBANK;
typedef struct DSPLIB_POLYPHASE_ANA_FILTERBANK DSPLIB_POLYPHASE_ANA_FILTERBANK;


/*

Interface for High Quality Synthesis Filterbank initialization

self          : i/p : pointer to a variable of type DSPLIB_POLYPHASE_SYN_FILTERBANK
resolution    : i/p : Number of Bands, options are [32 64 128]
bDownSampleSBR: i/p : Flag for 32 band Downsample filterbank = 1
                      Flag for Normal filterbank usage = 0

*/

void DspLibInitHQSynFilterbank(DSPLIB_POLYPHASE_SYN_FILTERBANK *self, 
							   long resolution,
							   long bDownSampleSBR);


/*

Interface for High Quality Synthesis Filterbank.

self          : i/p : pointer to a variable of type DSPLIB_POLYPHASE_SYN_FILTERBANK
Sr            : o/p : 32 Bit pointer to Real output data, Buffer length
Si            : o/p : 32 Bit pointer to Imaginary output data
timeSig       : i/p : 32 Bit pointer to PCM input data  

*/

#ifdef ARM
void DspLibHighQualitySynFilterbank(DSPLIB_POLYPHASE_SYN_FILTERBANK *self,
									int * Sr,
									int * Si,
									int * timeSig);
#else
void DspLibHighQualitySynFilterbank(DSPLIB_POLYPHASE_SYN_FILTERBANK *self,
									Float * Sr,
									Float * Si,
									Float * timeSig);
#endif
/*

Interface for High Quality Analysis Filterbank initialization

self          : i/p : pointer to a variable of type DSPLIB_POLYPHASE_ANA_FILTERBANK
resolution    : i/p : Number of Bands, options are [32 64 128]

*/

void DspLibInitHQAnaFilterbank(DSPLIB_POLYPHASE_ANA_FILTERBANK *self,
							   long resolution);

/*

Interface for High Quality Analysis Filterbank.

self          : i/p : pointer to a variable of type DSPLIB_POLYPHASE_ANA_FILTERBANK
timeSig       : i/p : 32 Bit pointer to PCM input data
Sr            : o/p : 32 Bit pointer to Real output data, Buffer length
Si            : o/p : 32 Bit pointer to Imaginary output data


*/

#ifdef ARM
void DspLibHighQualityAnaFilterbank(DSPLIB_POLYPHASE_ANA_FILTERBANK *self,
									int * timeSig,
									int * Sr,
									int * Si);
#else
void DspLibHighQualityAnaFilterbank(DSPLIB_POLYPHASE_ANA_FILTERBANK *self,
									Float * timeSig,
									Float * Sr,
									Float * Si);
#endif

/*

Interface for Low Power Synthesis Filterbank initialization

self          : i/p : pointer to a variable of type DSPLIB_POLYPHASE_SYN_FILTERBANK
resolution    : i/p : Number of Bands, options are [32 64 128]

*/

void DspLibInitLPSynFilterbank(DSPLIB_POLYPHASE_SYN_FILTERBANK *self, 
							   long resolution);

/*

Interface for Low Power Synthesis Filterbank.

self          : i/p : pointer to a variable of type DSPLIB_POLYPHASE_SYN_FILTERBANK
Sr            : o/p : 32 Bit pointer to Real output data, Buffer length
Si            : o/p : 32 Bit pointer to Imaginary output data
timeSig       : i/p : 32 Bit pointer to PCM input data  

*/

#ifdef ARM
void DspLibLowPowerSynFilterbank(DSPLIB_POLYPHASE_SYN_FILTERBANK *self,
								 int * Sr,
								 int * Si,
								 int * timeSig);
#else
void DspLibLowPowerSynFilterbank(DSPLIB_POLYPHASE_SYN_FILTERBANK *self,
								 Float * Sr,
								 Float * Si,
								 Float * timeSig);
#endif
/*

Interface for Low Power Analysis Filterbank initialization

self          : i/p : pointer to a variable of type DSPLIB_POLYPHASE_ANA_FILTERBANK
resolution    : i/p : Number of Bands, options are [32 64 128]

*/

void DspLibInitLPAnaFilterbank(DSPLIB_POLYPHASE_ANA_FILTERBANK *self,
							   long resolution);

/*

Interface for Low Power Analysis Filterbank.

self          : i/p : pointer to a variable of type DSPLIB_POLYPHASE_ANA_FILTERBANK
timeSig       : i/p : 32 Bit pointer to PCM input data
Sr            : o/p : 32 Bit pointer to Real output data, Buffer length
Si            : o/p : 32 Bit pointer to Imaginary output data

*/
#ifdef ARM
void DspLibLowPowerAnaFilterbank(DSPLIB_POLYPHASE_ANA_FILTERBANK *self,
								 int * timeSig,
								 int * Sr,
								 int * Si);
#else

void DspLibLowPowerAnaFilterbank(DSPLIB_POLYPHASE_ANA_FILTERBANK *self,
								 Float * timeSig,
								 Float * Sr,
								 Float * Si);

#endif


#endif /* _DSPLIB_FILTERBANK_API_ */
