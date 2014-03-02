#ifndef _POST_FILTERS_H_
#define _POST_FILTERS_H_

#define mmax(a, b)      ((a) > (b) ? (a) : (b))
#define mmin(a, b)      ((a) < (b) ? (a) : (b))

#define sign_division(x,n,out) {t_sint32 t,t1; t=(x); t1=(-t); t=(t<t1) ? -(t1>>(n)):(t>>(n)); (out)=t;}


#define final_clip(x)  ((x) < -(x)) ? 0 : (((x)>255) ? 255 : (x))
#define clipd1(x,y,out) {t_sint32 z,t; z=abs((y)); t=(x); t=(t<(-z))?(-z):t; t=(t>z)?z:t; (out)=t;}
#define UpDownRamp(x,y,out) {t_sint32 z,t; t=(x); z=abs(t); (out)=mmax(0,z-mmax(0,(z-(y))<<1)); \
                                    (out)=(t < -t) ? (-(out)) : (out);}


void mp4e_deblock_H263(mp4_parameters*, unsigned char*);

#endif
