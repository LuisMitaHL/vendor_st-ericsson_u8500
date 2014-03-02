/*
  CFFTN header file
*/

#ifndef __cfftn_h
#define __cfftn_h

void inv_dit_fft_8pt(Float *y,   
                     Float *real,
                     Float *imag);

void forwardFFT( float *data, int length );

int cfftn(float Re[],
          float Im[],
          int  nTotal,
          int  nPass,
          int  nSpan,
          int  iSign);

int CFFTN(float *afftData,int len, int isign);
int CFFTNRI(float *afftDataReal,float *afftDataImag,int len, int isign);

int CFFTN_NI(float *InRealData,
              float *InImagData,
              float *OutRealData,
              float *OurImagData,
              int len, int isign);


#if (defined NEON )&&(defined ARM)
void cfftn_neon(float *afftData,int len, int isign);
#endif

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif


#endif
