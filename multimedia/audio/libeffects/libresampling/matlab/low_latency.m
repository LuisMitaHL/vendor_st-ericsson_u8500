clear;
close all;
NPTS=1024;
epsilon=1e-10;
fftsiz=16384;

method=input('Enter Minimum phase function to be used (1=zero inversion,2=Hilbert transform): ');



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


figure;
plot (b_ear_8to16_low_latency);
grid on;
zoom on;
title 'coefs b-ear-8to16';

polezero(b_ear_8to16_low_latency,1,'original filter');

freqin=8000;
freqout=16000;
imp_resp=b_ear_8to16_low_latency;
numpts=size(imp_resp,1);
respi=[imp_resp' zeros(1,fftsiz-numpts)];
[maxr locr]=max(respi);
respi1=[respi(locr:fftsiz) respi(1:locr-1)];
respf=fft(respi1,fftsiz);

figure
i=1:fftsiz;
freq=((i-1)/fftsiz)*freqout;
fftresp=20*log10(abs(respf)+epsilon);
maxresp=max(fftresp);
plot(freq,fftresp-maxresp,'b');
grid on;
zoom on;
title(['coefs b-ear-8to16 frequency response for fin= ',num2str(freqin),' fout= ',num2str(freqout)]);
axis([0 freqout/2 maxresp-100 maxresp]);

figure
[gd,w]=grpdelay(respi,1,fftsiz);
f=freqout*(w/(2*pi));
GD=1000*gd/freqout;
plot(f,GD,'b');
title(['coefs b-ear-8to16 group delay response in msec for fin= ',num2str(freqin),' fout= ',num2str(freqout)]);
grid on;
zoom on;
axis([0 min(freqin/2,freqout/2) 0 2]);

%---- Get template from original transfer function and design minimum phase filter -----------
Fs = 16000;
Fstop = 4400;
Rp = 0.3;
Fpass = 3600;
Rs=58.5;
if (method==1)
	'call function phasemin for calculating min phase filter coefs using zero inversion'
	b_ear_8to16_low_latencym = phasemin(Fs,Fpass,Rp,Fstop,Rs);
else
	'call function phasemin1 for calculating min phase filter coefs using Hibert Transform'
	b_ear_8to16_low_latencym = phasemin1(Fs,Fpass,Rp,Fstop,Rs+4);
end
%------------------------- plot the results -------------------------------------
figure;
plot (b_ear_8to16_low_latencym);
grid on;
zoom on;
title 'redesigned coefs b-ear-8to16';
polezero(b_ear_8to16_low_latencym,1,'redesigned filter');
clear imp_resp;
imp_resp=b_ear_8to16_low_latencym;
numpts=size(imp_resp,2);
clear respi;
respi=[imp_resp zeros(1,fftsiz-numpts)];
[maxr locr]=max(respi);
clear respi1;
respi1=[respi(locr:fftsiz) respi(1:locr-1)];
clear respf;
respf=fft(respi1,fftsiz);

figure
i=1:fftsiz;
freq=((i-1)/fftsiz)*freqout;
fftresp=20*log10(abs(respf)+epsilon);
maxresp=max(fftresp)+5;
plot(freq,fftresp,'b');
grid on;
zoom on;
title(['redesigned coefs b-ear-8to16 frequency response for fin= ',num2str(freqin),' fout= ',num2str(freqout)]);
axis([0 freqout/2 maxresp-100 maxresp]);

figure
[gd,w]=grpdelay(respi,1,fftsiz);
f=freqout*(w/(2*pi));
GD=1000*gd/freqout;
plot(f,GD,'b');
title(['redesigned coefs b-ear-8to16 group delay response in msec for fin= ',num2str(freqin),' fout= ',num2str(freqout)]);
grid on;
zoom on;
axis([0 min(freqin/2,freqout/2) 0 2]);


