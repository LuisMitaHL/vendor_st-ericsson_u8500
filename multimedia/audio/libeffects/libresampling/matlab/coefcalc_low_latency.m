clear;
close all;

b_mic_48to16_low_latency = [
 7.6662190749e-003 
 3.2033348044e-002  
 8.1502335054e-002  
 1.5237875903e-001 
 2.2285303618e-001 
 2.5814236916e-001  
 2.2891439568e-001  
 1.3334983083e-001 
 6.4435766910e-003 
 -9.4527836079e-002 
 -1.2303308191e-001 
 -7.3677499079e-002 
 1.2799744790e-002  
 7.6110222350e-002  
 7.7148298742e-002  
 2.3456889137e-002 
-3.9552049113e-002 
-6.4597356425e-002 
-3.7718430120e-002  
1.4236969383e-002 
 4.8091538730e-002  
 3.9548814102e-002  
 4.5926862538e-004 
 -3.4307909410e-002 
-3.7289735635e-002 
-9.9054880792e-003  
2.1818262413e-002  
3.1326168323e-002 
 1.3509739522e-002 
 -1.3512712472e-002 
 -2.6182082648e-002 
 -1.5814030375e-002 
 5.9423672808e-003  
 1.9409544569e-002  
 1.4438981673e-002 
 -2.2998022575e-003 
-1.5180418781e-002 
-1.4010162635e-002 
-1.8494216745e-003  
9.4782066569e-003 
 1.0632553365e-002  
 2.2278533878e-003 
 -7.2386710643e-003 
 -9.7688274021e-003 
-4.4685901081e-003  
2.9098009564e-003  
5.8916552047e-003  
2.7570540533e-003 
-2.9029944672e-003 
-6.0764082037e-003 
-4.5789021068e-003 
-1.5759487624e-004 
 3.7325760568e-003  
 4.8956133546e-003  
 3.5908948378e-003  
 1.6215235182e-003
 0];


b_mic_16to8_low_latency = [
 1.8753042940e-002  
 9.3836464842e-002  
 2.3278193274e-001  
 3.5081626087e-001 
 3.1284331366e-001  
 9.4568556279e-002 
 -1.3115897319e-001 
 -1.5734089421e-001 
 3.0782206452e-003  
 1.2278388428e-001  
 5.6348615529e-002 
 -7.4580832107e-002 
-7.6018825565e-002  
3.3939095437e-002  
7.5578177621e-002 
-4.4108598305e-003 
-6.5899786608e-002 
-1.5169642508e-002  
5.2750228053e-002  
2.6927323424e-002 
-3.9134448804e-002 
-3.2900585499e-002  
2.6534129956e-002  
3.4736400498e-002 
-1.5643830192e-002 
-3.3724734414e-002  
6.7261463406e-003  
3.0872309457e-002 
 2.2245108777e-004 
 -2.6929399062e-002 
 -5.3341969690e-003  
 2.2451839027e-002 
 8.7991742474e-003 
 -1.7865350288e-002 
 -1.0853589162e-002  
 1.3472241482e-002 
 1.1750651642e-002 
 -9.4763324057e-003 
 -1.1726220831e-002  
 6.0128663312e-003 
 1.1023041758e-002 
 -3.1634166238e-003 
 -9.8580342867e-003  
 9.3826922365e-004 
 8.4612828444e-003  
 6.7898023428e-004 
 -7.0041787384e-003 
 -1.8029835212e-003 
 5.7452945339e-003  
 2.5842615263e-003 
 -4.8947894753e-003 
 -3.8537461970e-003 
 4.7574250760e-003  
 9.6143641297e-003  
 6.9886878968e-003  
 2.0391227092e-003]; 



b_ear_8to16_low_latency = [
 6.1364365803e-002  
 2.8099842208e-001  
 6.2077314184e-001  
 7.8941848110e-001 
 5.0510106202e-001 
 -6.6584449623e-002 
 -3.7554009826e-001 
 -1.5403105336e-001 
 2.0639416596e-001  
 1.9827450543e-001 
 -9.2365442678e-002 
 -1.8553441459e-001 
 2.3299085634e-002  
 1.5655101505e-001  
 1.6517189592e-002 
 -1.2544091320e-001 
-3.7988695684e-002  
9.7045864311e-002  
4.8019520821e-002 
-7.2755249896e-002 
-5.0905066657e-002  
5.2703620059e-002  
4.9372261955e-002 
-3.6595834773e-002 
-4.5229550244e-002  
2.3961513110e-002  
3.9661714831e-002 
-1.4332913667e-002 
-3.3491136819e-002  
7.2859010872e-003  
2.7338881553e-002 
-2.4381742985e-003 
-2.1737701604e-002 
-4.9708919411e-004  
1.7614879796e-002  
1.7314325771e-003 
-2.0843167830e-002 
-2.0403064583e-002 
-6.4557598308e-003  
1.0795638540e-003]; 



b_ear_16to48_low_latency = [
 4.7295169659e-002  
 1.6836786687e-001  
 3.7590002458e-001  
 6.1794476403e-001 
 7.8508701264e-001  
 7.6370746534e-001  
 5.1570819397e-001  
 1.2688555447e-001 
-2.2393436764e-001 
-3.6858404504e-001 
-2.6007110280e-001 
-5.8732574022e-003 
 2.0532266706e-001  
 2.3594056277e-001  
 9.1075096066e-002 
 -9.8194085807e-002 
-1.8402690100e-001 
-1.1533058246e-001  
3.3368872983e-002  
1.3435175375e-001 
 1.1350165481e-001  
 4.1285299090e-003 
 -9.3297477607e-002 
 -1.0074006576e-001 
-2.4293084675e-002  
6.1339050087e-002  
8.3985940026e-002  
3.3468996906e-002 
-3.7495237248e-002 
-6.6711058260e-002 
-3.5855684545e-002  
2.0115732880e-002 
 5.0304293786e-002  
 3.3380672661e-002 
 -8.9533530199e-003 
 -3.7240121243e-002 
-3.0273458569e-002 
-4.2714977267e-004  
2.2879802362e-002  
2.0359847295e-002 
-2.6222896181e-003 
-2.5118829373e-002 
-3.0516284298e-002 
-1.8614744569e-002 
-1.7608199017e-003  
7.7799731120e-003  
9.7397200932e-003
0];

NPTS=1024;
epsilon=1e-8;

% 1 ----------------------------------
figure;
subplot(3,1,1),plot (b_mic_48to16_low_latency);
grid on;
zoom on;
title 'coefs b-mic-48to16_low_latency';

[h,w] = freqz(b_mic_48to16_low_latency,1,NPTS);
amp=20*log10(abs(h)+epsilon);
f=48000*(w/(2*pi));
subplot(3,1,2),plot(f,amp);
grid on;
zoom on;
title 'freq resp of b-mic-48to16 mips est. for mac only is 16000*57/1e6=.90';
axis([0 24000 -80 5]);

freqin=48000;
freqout=16000;
imp_resp=b_mic_48to16_low_latency';
numpts=size(imp_resp,2);
respi=[imp_resp zeros(1,NPTS-numpts)];
[gd,w]=grpdelay(respi,1,NPTS);
GD=1000*gd/max(freqin,freqout);
subplot(3,1,3),plot(f,GD,'b');
title('group delay response in msec');
grid on;
zoom on;
axis([0 min(freqin,freqout)/2 0 2]);
Fs = max(freqin,freqout);
Fstop = 1.1*min(freqin/2,freqout/2);
Rp = 0.3;
Fpass = .875*min(freqin/2,freqout/2);
Rs=60;
coef_redesign=phasemin1(Fs,Fpass,Rp,Fstop,Rs);
figure;
subplot(3,1,1),plot (coef_redesign);
grid on;
zoom on;
title 'redesigned coefficients';
[h,w] = freqz(coef_redesign,1,NPTS);
amp=20*log10(abs(h)+epsilon);
subplot(3,1,2),plot(f,amp);
grid on;
zoom on;
title 'freq resp of redesigned coefficients';
axis([0 24000 -80 5]);
imp_resp=coef_redesign;
numpts=size(imp_resp,2);
respi=[imp_resp zeros(1,NPTS-numpts)];
[gd,w]=grpdelay(respi,1,NPTS);
GD=1000*gd/max(freqin,freqout);
subplot(3,1,3),plot(f,GD,'b');
title('group delay response in msec of redesigned coefficients');
grid on;
zoom on;
axis([0 min(freqin,freqout)/2 0 2]);
% 2 ---------------------------------
figure;
subplot(3,1,1),plot (b_mic_16to8_low_latency);
grid on;
zoom on;
title 'coefs b-mic-16to8';

[h,w] = freqz(b_mic_16to8_low_latency,1,NPTS);
amp=20*log10(abs(h)+epsilon);
f=16000*(w/(2*pi));
subplot(3,1,2),plot(f,amp);
grid on;
zoom on;
title 'freq resp of b-mic-16to8 mips est. for mac only is 8000*56/1e6=.45';
axis([0 8000 -80 5]);

freqin=16000;
freqout=8000;
imp_resp=b_mic_16to8_low_latency';
numpts=size(imp_resp,2);
respi=[imp_resp zeros(1,NPTS-numpts)];
[gd,w]=grpdelay(respi,1,NPTS);
GD=1000*gd/max(freqin,freqout);
subplot(3,1,3),plot(f,GD,'b');
title('group delay response in msec');
grid on;
zoom on;
axis([0 min(freqin,freqout)/2 0 2]);
Fs = max(freqin,freqout);
Fstop = 1.05*min(freqin/2,freqout/2);
Rp = 0.3;
Fpass = .9*min(freqin/2,freqout/2);
Rs=65;
coef_redesign=phasemin1(Fs,Fpass,Rp,Fstop,Rs);
figure;
subplot(3,1,1),plot (coef_redesign);
grid on;
zoom on;
title 'redesigned coefficients';
[h,w] = freqz(coef_redesign,1,NPTS);
amp=20*log10(abs(h)+epsilon);
subplot(3,1,2),plot(f,amp);
grid on;
zoom on;
title 'freq resp of redesigned coefficients';
axis([0 8000 -80 5]);
imp_resp=coef_redesign;
numpts=size(imp_resp,2);
respi=[imp_resp zeros(1,NPTS-numpts)];
[gd,w]=grpdelay(respi,1,NPTS);
GD=1000*gd/max(freqin,freqout);
subplot(3,1,3),plot(f,GD,'b');
title('group delay response in msec of redesigned coefficients');
grid on;
zoom on;
axis([0 min(freqin,freqout)/2 0 2]);
% 3 ---------------------------------
figure;
subplot(3,1,1),plot (b_ear_8to16_low_latency);
grid on;
zoom on;
title 'coefs b-ear-8to16';

[h,w] = freqz(b_ear_8to16_low_latency,1,NPTS);
amp=20*log10(abs(h)+epsilon);
f=16000*(w/(2*pi));
subplot(3,1,2),plot(f,amp);
grid on;
zoom on;
title 'freq resp of b-ear-8to16 mips est for mac only is 8000*40/1e6=.32';
axis([0 8000 -80 10]);

freqin=8000;
freqout=16000;
imp_resp=b_ear_8to16_low_latency';
numpts=size(imp_resp,2);
respi=[imp_resp zeros(1,NPTS-numpts)];
[gd,w]=grpdelay(respi,1,NPTS);
GD=1000*gd/max(freqin,freqout);
subplot(3,1,3),plot(f,GD,'b');
title('group delay response in msec');
grid on;
zoom on;
axis([0 min(freqin,freqout)/2 0 2]);
Fs = max(freqin,freqout);
Fstop = 1.1*min(freqin/2,freqout/2);
Rp = 0.3;
Fpass = .9*min(freqin/2,freqout/2);
Rs=60;
coef_redesign=phasemin1(Fs,Fpass,Rp,Fstop,Rs);
figure;
subplot(3,1,1),plot (coef_redesign);
grid on;
zoom on;
title 'redesigned coefficients';
[h,w] = freqz(coef_redesign,1,NPTS);
amp=20*log10(abs(h)+epsilon);
subplot(3,1,2),plot(f,amp);
grid on;
zoom on;
title 'freq resp of redesigned coefficients';
axis([0 8000 -80 5]);
imp_resp=coef_redesign;
numpts=size(imp_resp,2);
respi=[imp_resp zeros(1,NPTS-numpts)];
[gd,w]=grpdelay(respi,1,NPTS);
GD=1000*gd/max(freqin,freqout);
subplot(3,1,3),plot(f,GD,'b');
title('group delay response in msec of redesigned coefficients');
grid on;
zoom on;
axis([0 min(freqin,freqout)/2 0 2]);
% 4 --------------------------------
figure;
subplot(3,1,1),plot (b_ear_16to48_low_latency);
grid on;
zoom on;
title 'coefs b-ear-16to48';

[h,w] = freqz(b_ear_16to48_low_latency,1,NPTS);
amp=20*log10(abs(h)+epsilon);
f=48000*(w/(2*pi));
subplot(3,1,2),plot(f,amp);
grid on;
zoom on;
title 'freq resp of b-ear-16to48 mips est for mac only is 16000*47/1e6=.75';
axis([0 24000 -75 15]);

freqin=16000;
freqout=48000;
imp_resp=b_ear_16to48_low_latency';
numpts=size(imp_resp,2);
respi=[imp_resp zeros(1,NPTS-numpts)];
[gd,w]=grpdelay(respi,1,NPTS);
GD=1000*gd/max(freqin,freqout);
subplot(3,1,3),plot(f,GD,'b');
title('group delay response in msec');
grid on;
zoom on;
axis([0 min(freqin,freqout)/2 0 2]);
Fs = max(freqin,freqout);
Fstop = 8800;
Rp = 0.3;
Fpass = 7200;
Rs=50;
coef_redesign=phasemin1(Fs,Fpass,Rp,Fstop,Rs);
figure;
subplot(3,1,1),plot (coef_redesign);
grid on;
zoom on;
title 'redesigned coefficients';
[h,w] = freqz(coef_redesign,1,NPTS);
amp=20*log10(abs(h)+epsilon);
subplot(3,1,2),plot(f,amp);
grid on;
zoom on;
title 'freq resp of redesigned coefficients';
axis([0 24000 -80 5]);
imp_resp=coef_redesign;
numpts=size(imp_resp,2);
respi=[imp_resp zeros(1,NPTS-numpts)];
[gd,w]=grpdelay(respi,1,NPTS);
GD=1000*gd/max(freqin,freqout);
subplot(3,1,3),plot(f,GD,'b');
title('group delay response in msec of redesigned coefficients');
grid on;
zoom on;
axis([0 min(freqin,freqout)/2 0 2]);
%------------------- Write the tables to separate C-files --------------------------------
[b_mic_48to16_low_latency]=normfilter_low_latency(b_mic_48to16_low_latency,1);
[nbcoef_48_16_low_latency,dummy]=size(b_mic_48to16_low_latency);
fid = openfile('resample_table48_16_low_latency.c');
%fprintf(fid,'#include "resample.h"\n');
fprintf(fid,'#include "resample_local.h"\n\n');
%fprintf(fid,'#ifdef USE_LOW_LATENCY\n');
fprintf(fid,'Float const YMEM resample_48_16_low_latency[%d] = {\n',(nbcoef_48_16_low_latency+1));
for k = 1:nbcoef_48_16_low_latency-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',b_mic_48to16_low_latency(k),k-1);
end
k=nbcoef_48_16_low_latency;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',b_mic_48to16_low_latency(k),k-1);
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',0,k);
%fprintf(fid,'#endif\n');
fclose(fid);
%--------------------------------------------------------------------------------------------
[b_mic_16to8_low_latency]=normfilter_low_latency(b_mic_16to8_low_latency,1);
[nbcoef_16_8_low_latency,dummy]=size(b_mic_16to8_low_latency);
fid = openfile('resample_table16_8_low_latency.c');
%fprintf(fid,'#include "resample.h"\n');
fprintf(fid,'#include "resample_local.h"\n\n');
fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
%fprintf(fid,'#ifdef USE_LOW_LATENCY\n');
fprintf(fid,'Float const YMEM resample_16_8_low_latency[%d] = {\n',(nbcoef_16_8_low_latency+1));
for k = 1:nbcoef_16_8_low_latency-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',b_mic_16to8_low_latency(k),k-1);
end
k=nbcoef_16_8_low_latency;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',b_mic_16to8_low_latency(k),k-1);
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',0,k);
%fprintf(fid,'#endif\n');
fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
fclose(fid);
%--------------------------------------------------------------------------------------------
[b_ear_16to48_low_latency]=normfilter_low_latency(b_ear_16to48_low_latency,3);
[nbcoef_16_48_low_latency,dummy]=size(b_ear_16to48_low_latency);
fid = openfile('resample_table16_48_low_latency.c');
%fprintf(fid,'#include "resample.h"\n');
fprintf(fid,'#include "resample_local.h"\n\n');
%fprintf(fid,'#ifdef USE_LOW_LATENCY\n');
fprintf(fid,'Float const YMEM resample_16_48_low_latency[%d] = {\n',(nbcoef_16_48_low_latency+1));
for k = 1:nbcoef_16_48_low_latency-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',b_ear_16to48_low_latency(k),k-1);
end
k=nbcoef_16_48_low_latency;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',b_ear_16to48_low_latency(k),k-1);
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',0,k);
%fprintf(fid,'#endif\n');
fclose(fid);
%--------------------------------------------------------------------------------------------
[b_ear_8to16_low_latency]=normfilter_low_latency(b_ear_8to16_low_latency,2);
[nbcoef_8_16_low_latency,dummy]=size(b_ear_8to16_low_latency);
fid = openfile('resample_table8_16_low_latency.c');
%fprintf(fid,'#include "resample.h"\n');
fprintf(fid,'#include "resample_local.h"\n\n');
fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
%fprintf(fid,'#ifdef USE_LOW_LATENCY\n');
fprintf(fid,'Float const YMEM resample_8_16_low_latency[%d] = {\n',(nbcoef_8_16_low_latency+1));
for k = 1:nbcoef_8_16_low_latency-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',b_ear_8to16_low_latency(k),k-1);
end
k=nbcoef_8_16_low_latency;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',b_ear_8to16_low_latency(k),k-1);
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',0,k);
%fprintf(fid,'#endif\n');
fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
fclose(fid);


gen_src_incr_offset_tab;







