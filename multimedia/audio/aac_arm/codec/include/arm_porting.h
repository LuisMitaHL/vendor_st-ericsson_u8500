
#ifndef _ARMPORT_H
#define _ARMPORT_H

#ifdef ARM

#define mpeg_calc_crc                      mpeg_calc_crc_arm 
#define buffer_reset_simd                  buffer_reset_arm
#define buffer_reset                       buffer_reset_arm
#define buffer_copy                        buffer_copy_arm
#define vector_copy                        vector_copy_arm
#define vector_copy_simd                   vector_copy_arm_simd
#define SbrSynFilterbank_opt               SbrSynFilterbank_opt_arm
#define SbrSynFilterbank_opt_lp            SbrSynFilterbank_opt_lp_arm

#if defined(HWLOOP)
#undef  HWLOOP

#define __lcount(a) __lcount ## a
#define __liter(a) __liter ## a

#define HWLOOP(a) { int __lcount(__LINE__); int __liter(__LINE__);\
for((__liter(__LINE__) = (a)), (__lcount(__LINE__)) = 0; \
  (__lcount(__LINE__)) < (__liter(__LINE__)); (__liter(__LINE__))--) 

#endif

#endif/* ARM */


#endif /* Do not edit below this line */

















