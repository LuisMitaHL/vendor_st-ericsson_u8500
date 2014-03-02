/*
//---------------------------------------------------------------------------\ 
//
//               (C) copyright Fraunhofer - IIS (2001)
//                        All Rights Reserved
//
//   filename: PnsConformanceMain.h
//             
//   author  : Martin Weishart martin.weishart@iis.fhg.de
//   date    : 2001-10-26
//   contents/description: provides a struct  
//                         to calculate PNS conformance criteria
//
//--------------------------------------------------------------------------\
*/

#define Float double

#define SFB_1 51
#define SFB_2 15
#define FRAME_SIZE_LONG 2048
#define FRAME_SIZE_SHORT 256
#define FRAME_SIZE_LD 1024
#define BLOCK_LENGTH 64

typedef struct {
  Float meanRatio;
  Float standardDeviationRatio;
} critical_ratio;


int CalculateTemporalConformance( char * Pcmfile,
                                  char * refPcmfile,
								  long frequency,
                                  unsigned int frameSize,
                                  int delay);

int CalculateSpectralConformance( char * Pcmfile,
                                  char * refPcmfile,
								  long frequency,
                                  unsigned int frameSize,
                                  int delay);







