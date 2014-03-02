clear
close all
epsilon=1e-10;
NPTS=8192;
Fs=44100;
M1=7;
L1=5;
M2=7;
L2=9;
Ratio1=M1/L1;
Ratio2=M2/L2;
Ratio=Ratio1*Ratio2;

%--------------------- 1st stage ----------------------------------------------------
Fs1=Fs*M1;
Fstop1=(Fs1/2)/L1;
Fpass1=.9*Fs/2;
Rp1=.1;
Rs1=130;
[nstage1,fo,mo,w] = remezord( [Fpass1 Fstop1], [1 0], [10^(Rp1/20)-1 10^(-Rs1/20)], Fs1 );
coefstage1 = remez(nstage1-1,fo,mo,w);
[h1,f] = freqz(coefstage1,1,NPTS,Fs1);
figure
ampdB=20*log10(abs(h1)+epsilon);
plot(f,ampdB);
zoom on;
grid on;
title('1st stage frequency response');

%--------------------- 2nd stage -----------------------------------------------------
Fs2 = Fs*M2*M1/L1;
Fstop2 = Fs/2;
Fpass2 = Fpass1; 
Rp2=.1;
Rs2=130;
[nstage2,fo,mo,w] = remezord( [Fpass2 Fstop2], [1 0], [10^(Rp2/20)-1 10^(-Rs2/20)], Fs2 );
coefstage2 = remez(nstage2-1,fo,mo,w);
[h2,f] = freqz(coefstage2,1,NPTS,Fs2);
figure
ampdB=20*log10(abs(h2)+epsilon);
plot(f,ampdB);
zoom on;
grid on;
title('2nd stage frequency response');

ncoefs=nstage1+nstage2;
ncoefs

%-------------------- testing 44100->48000  --------------------------
Fsout=Fs*Ratio;
f1=1000;
f2=19800;
NSAMPin=12000;

T=1/Fs;
i=(0:NSAMPin-1);
t=i*T;
sig=cos(2*pi*f1*t)+cos(2*pi*f2*t);

sigin1=zeros(1,M1*NSAMPin);
sigin1(1:M1:M1*NSAMPin)=sig;
temp=M1*filter(coefstage1,1,sigin1);
sigout1=temp(1:L1:M1*NSAMPin);
[dummy siz]=size(sigout1);
sigin2=zeros(1,siz*M2);
sigin2(1:M2:siz*M2)=sigout1;
temp=M2*filter(coefstage2,1,sigin2);
[dummy siz]=size(temp);
sigout2=temp(1:L2:siz);
[dummy FFTSIZ]=size(sigout2);

y=(1/FFTSIZ)*fft(6.5*sigout2.*(chebwin(FFTSIZ,155)'),FFTSIZ);
spectrum=20*log10(abs(y)+epsilon);
i=(0:FFTSIZ-1);
freq=(i/FFTSIZ)*Fsout;
figure
plot(freq,spectrum);
grid on;
zoom on;
axis([0 max(freq)/2 -150 0]);
title(' 44->48 spectrum');

%-------------------- testing 48000->44100  --------------------------
Fs=48000;
Fsout=Fs/Ratio;
f1=1000;
f2=19800;
f3=22100;
NSAMPin=24000;

T=1/Fs;
i=(0:NSAMPin-1);
t=i*T;
sig=cos(2*pi*f1*t)+cos(2*pi*f2*t)+cos(2*pi*f3*t);

sigin1=zeros(1,L2*NSAMPin);
sigin1(1:L2:L2*NSAMPin)=sig;
temp=L2*filter(coefstage2,1,sigin1);
sigout1=temp(1:M2:L2*NSAMPin);
[dummy siz]=size(sigout1);
sigin2=zeros(1,siz*L1);
sigin2(1:L1:siz*L1)=sigout1;
temp=L1*filter(coefstage1,1,sigin2);
[dummy siz]=size(temp);
sigout2=temp(1:M1:siz);
[dummy FFTSIZ]=size(sigout2);

y=(1/FFTSIZ)*fft(6.25*sigout2.*(chebwin(FFTSIZ,155)'),FFTSIZ);
spectrum=20*log10(abs(y)+epsilon);
i=(0:FFTSIZ-1);
freq=(i/FFTSIZ)*Fsout;
figure
plot(freq,spectrum);
grid on;
zoom on;
axis([0 max(freq)/2 -150 0]);
title(' 48->44 spectrum');




