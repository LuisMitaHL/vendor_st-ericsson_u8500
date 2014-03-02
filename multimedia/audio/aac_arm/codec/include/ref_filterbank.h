
#ifndef _REF_FILTERBANK_H_
#define _REF_FILTERBANK_H_

#define MAX_NUM_QMF_BANDS              128

struct SAC_POLYPHASE_ANA_FILTERBANK
{
	double *X;
	double XData[10*MAX_NUM_QMF_BANDS];
};

struct SAC_POLYPHASE_SYN_FILTERBANK
{
	double *V;
	double VData[20*MAX_NUM_QMF_BANDS];
};

typedef struct SAC_POLYPHASE_SYN_FILTERBANK SAC_POLYPHASE_SYN_FILTERBANK;
typedef struct SAC_POLYPHASE_ANA_FILTERBANK SAC_POLYPHASE_ANA_FILTERBANK;

void SacInitSynFilterbank(long resolution, long bDownSampleSBR, long low_power_mode);

void SacCalculateSynFilterbank(SAC_POLYPHASE_SYN_FILTERBANK *self,
							   float * Sr,
							   float * Si,
							   float * timeSig,
							   long low_power_mode);

void SacInitAnaFilterbank(long resolution, long low_power_mode);


void SacCalculateAnaFilterbank(SAC_POLYPHASE_ANA_FILTERBANK *self,
							   float * timeSig,
							   float * Sr,
							   float * Si,
							   long low_power_mode);
#endif
