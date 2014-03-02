#ifndef _SBR_H_
#define _SBR_H_

extern CODEC_INIT_T
sbr_open(CODEC_INTERFACE_T *eaacPlus_Interface);

extern SBR_ERROR
sbr_decode(CODEC_INTERFACE_T *interface_fe, int *numchannels, int first_time);

extern void
sbr_close(CODEC_INTERFACE_T *interface_fe);

#endif
