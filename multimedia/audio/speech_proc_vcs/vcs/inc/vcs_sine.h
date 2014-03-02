#ifndef VCS_SINE_H
#define VCS_SINE_H

#include <stdint.h>


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct vcs_sine_s {
	unsigned int counter;
} vcs_sine_t;

void vcs_sine_init(vcs_sine_t *seed);
void vcs_sine_fill(vcs_sine_t *seed, int16_t* const Buffer_p, const uint16_t NrOfSamples);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif