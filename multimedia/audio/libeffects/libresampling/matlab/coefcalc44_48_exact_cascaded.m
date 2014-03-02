clear
close all
epsilon=1e-10;
startfreq=44100;
numcoefs=0;
%---------------------------- update conversion x4/3 ------------
Fin=startfreq;
Fout=44100*4/3;
Fs=Fin*4;
Rs=130;
Fstop=Fin/2;%Fstop=29400;
Fpass=Fin*.9/2;
Rp=.1;
[n_4_3,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
n_4_3_toround=n_4_3;
n_4_3_toround
lcd=4*3;
n_4_3= ceil(n_4_3/lcd)*lcd
numcoefs=numcoefs+n_4_3/2;
coef_4_3 = remez(n_4_3-1,fo,mo,w);
figure
Npts=4096;
[h,f] = freqz(coef_4_3,1,Npts,Fs);
hdB=20*log10(abs(h)+epsilon);
plot(f,hdB);
grid on;
zoom on;
title('frequency response for 4_3');
%---------------------------- update conversion x8/7 ------------
Fin=Fout;
Fout=Fin*8/7;
Fs=Fin*8;
Rs=130;
Fpass=startfreq*.9/2;
Fstop=Fin/2;%Fstop=33600;
Rp=.1;
[n_8_7,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
n_8_7_toround=n_8_7;
n_8_7_toround
lcd=8*7;
n_8_7= ceil(n_8_7/lcd)*lcd
numcoefs=numcoefs+n_8_7/2;
coef_8_7 = remez(n_8_7-1,fo,mo,w);
figure
Npts=4096;
[h,f] = freqz(coef_8_7,1,Npts,Fs);
hdB=20*log10(abs(h)+epsilon);
plot(f,hdB);
grid on;
zoom on;
title('frequency response for 8_7');
%---------------------------- update conversion x5/7 ------------
Fin=Fout;
Fout=Fin*5/7;
Fs=Fin*5;
Rs=130;
Fpass=startfreq*.9/2;
Fstop=Fout/2;
Rp=.1;
[n_5_7,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
n_5_7_toround=n_5_7;
n_5_7_toround
lcd=5*7*2;
n_5_7= ceil(n_5_7/lcd)*lcd
numcoefs=numcoefs+n_5_7/2;
coef_5_7 = remez(n_5_7-1,fo,mo,w);
figure
Npts=4096;
[h,f] = freqz(coef_5_7,1,Npts,Fs);
hdB=20*log10(abs(h)+epsilon);
plot(f,hdB);
grid on;
zoom on;
title('frequency response for 5_7');
numcoefs

%*********************** UP SAMPLING 44100->48000 **************************************
%------------------------------- generate input test signal ---------------------------
Nsamples=4096;
fs=44100;
f1=1000;
f2=10000;
f3=44100*.9/2;
T=1/fs;
i=0:Nsamples-1;
t=i*T;
sigin=cos(2*pi*f1*t)+cos(2*pi*f2*t)+cos(2*pi*f3*t);
%-------------------------- test the filters ----------------------------------------

sigtemp=zeros(1,4*Nsamples);
sigtemp(1:4:4*Nsamples)=sigin;
sigtemp1=4*filter(coef_4_3,1,sigtemp);
sigout=sigtemp1(1:3:4*Nsamples);
clear sigtemp;
clear sigtemp1;
[dummy Nsamples]=size(sigout);


sigtemp=zeros(1,8*Nsamples);
sigtemp(1:8:8*Nsamples)=sigout;
sigtemp1=8*filter(coef_8_7,1,sigtemp);
sigout=sigtemp1(1:7:8*Nsamples);
clear sigtemp;
clear sigtemp1;
[dummy Nsamples]=size(sigout);


sigtemp=zeros(1,5*Nsamples);
sigtemp(1:5:5*Nsamples)=sigout;
sigtemp1=5*filter(coef_5_7,1,sigtemp);
sigout=sigtemp1(1:7:5*Nsamples);
clear sigtemp;
clear sigtemp1;
[dummy Nsamples]=size(sigout);

%-------------------------- display results ----------------------------------------
if (rem(Nsamples,2)==0)
	Nsamples=Nsamples-1;
end
i=1:Nsamples;
freq=(i-1)*Fout/Nsamples;
winval=chebwin(Nsamples,140);
amp=4/Nsamples*fft(winval'.*(sigout(1:Nsamples)));
dbamp=20*log10(abs(amp)+epsilon);
figure
plot(freq,dbamp);
grid on;
zoom on;
axis([0,Fout/2,-160,0]);
title('total chain for upsampling 44100->48000 ratios are: 4/3x8/7x5/7');

%*********************** DOWN SAMPLING 48000->44100 **************************************
%------------------------------- generate input test signal ---------------------------
Nsamples=4096;
fs=48000;
f1=1000;
f2=10000;
f3=44100*.9/2;
f4=fs/2;
T=1/fs;
i=0:Nsamples-1;
t=i*T;
sigin=cos(2*pi*f1*t)+cos(2*pi*f2*t)+cos(2*pi*f3*t)+cos(2*pi*f4*t);
%-------------------------- test the filters ----------------------------------------

sigtemp=zeros(1,7*Nsamples);
sigtemp(1:7:7*Nsamples)=sigin;
sigtemp1=7*filter(coef_5_7,1,sigtemp);
sigout=sigtemp1(1:5:7*Nsamples);
clear sigtemp;
clear sigtemp1;
[dummy Nsamples]=size(sigout);

sigtemp=zeros(1,7*Nsamples);
sigtemp(1:7:7*Nsamples)=sigout;
sigtemp1=7*filter(coef_8_7,1,sigtemp);
sigout=sigtemp1(1:8:7*Nsamples);
clear sigtemp;
clear sigtemp1;
[dummy Nsamples]=size(sigout);


sigtemp=zeros(1,3*Nsamples);
sigtemp(1:3:3*Nsamples)=sigout;
sigtemp1=3*filter(coef_4_3,1,sigtemp);
sigout=sigtemp1(1:4:3*Nsamples);
clear sigtemp;
clear sigtemp1;
[dummy Nsamples]=size(sigout);

%-------------------------- display results ----------------------------------------
Fout=44100;
if (rem(Nsamples,2)==0)
	Nsamples=Nsamples-1;
end
i=1:Nsamples;
freq=(i-1)*Fout/Nsamples;
winval=chebwin(Nsamples,140);
amp=4/Nsamples*fft(winval'.*(sigout(1:Nsamples)));
dbamp=20*log10(abs(amp)+epsilon);
figure
plot(freq,dbamp);
grid on;
zoom on;
axis([0,Fout/2,-160,0]);
title('total chain for downsampling 48000->44100 ratios are: 7/5x7/8x3/4');

