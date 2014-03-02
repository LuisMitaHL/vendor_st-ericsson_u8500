#include "cfuncts.h"
#define degree 7
#define NTAPS 89
#define MAXPHASE 16
#define NTAPSsinx 51
#define passratiopoly .455
#define MINDB 120.0
#define SRCBULKSIZ (2*NTAPS*(MAXPHASE+1))+NTAPSsinx+(((NTAPSsinx/2)+1)*(degree+1))
#define BLOCKSIZ 80
#define xmult 2147483647.0

