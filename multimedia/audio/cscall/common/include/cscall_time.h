#ifdef __cplusplus
//extern C {
#endif
#include <time.h>

static unsigned long long getTime() {
	struct timespec ts;
	unsigned long long result;
	
	if(clock_gettime(CLOCK_MONOTONIC, &ts))
	{
	  return 0;
	}
	else
	{
	  result = (unsigned long long)ts.tv_sec * (unsigned long long)1000000 + (unsigned long long)(ts.tv_nsec/1000);
	  return result;
	}
}

#ifdef __cplusplus
//}
#endif
