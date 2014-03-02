clear;
close all;
epsilon=1e-10;
fftsiz=16384;
freqin=48000;
freqin=input('Enter src input sampling frequency: ');
freqout=44100;
freqout=input('Enter src output sampling frequency: ');


tempin=round(freqin/1000);
tempout=round(freqout/1000);
filename=['ir',num2str(tempin),'_',num2str(tempout),'.flt'];
%filename=input('Enter file name containing src impulse response: ','s');
f1=fopen(filename,'rt');
imp_resp=fscanf(f1,'%e');
fclose('all');



numpts=size(imp_resp,1);
respi=[imp_resp' zeros(1,fftsiz-numpts)];
[maxr locr]=max(respi);
respi1=[respi(locr:fftsiz) respi(1:locr-1)];
respf=fft(respi1,fftsiz);
figure
plot(respi,'b');
grid on;
zoom on;
title(['src impulse response for fin= ' num2str(freqin) ' fout= ' num2str(freqout)]);
figure
i=1:fftsiz;
freq=((i-1)/fftsiz)*freqout;
fftresp=20*log10(abs(respf)+epsilon);
maxresp=max(fftresp)+5;
plot(freq,fftresp,'b');
grid on;
zoom on;
title(['src frequency response for fin= ' num2str(freqin) ' fout= ' num2str(freqout)]);
axis([0 freqout/2 maxresp-100 maxresp]);

figure
[gd,w]=grpdelay(respi,1,fftsiz);
f=freqout*(w/(2*pi));
GD=1000*gd/freqout;
plot(f,GD,'b');
title(['src group delay response in msec for fin= ' num2str(freqin) ' fout= ' num2str(freqout)]);
grid on;
zoom on;
axis([0 min(freqin/2,freqout/2) 0 10]);

  
