#include "vcs_sine.h"

#define SINE_COUNT_MAX 13 

static const int16_t SineVector[SINE_COUNT_MAX] =
{ 0,  465,  823,  993,  935, 663,  239, -239, -663, -935,  -993, -823, -465 };

void vcs_sine_init(vcs_sine_t *seed) 
{
	seed->counter = 0;
}
void vcs_sine_fill(vcs_sine_t *seed, int16_t* const Buffer_p, const uint16_t NrOfSamples)
{
  int j;

  for(j = 0;j < NrOfSamples;j++)
  {
    Buffer_p[j] = SineVector[seed->counter];

    seed->counter++;
    if (SINE_COUNT_MAX == seed->counter)
    {
      seed->counter = 0;
    }
  }
}
