clear;
close all;

%-- init -- 
interpratio=25;
interpratio
NSAMPLES=48000;


ppm=1000;
%ppm=-10000;
epsilon=1e-10;
fsin=48000;

fsout=fsin+fsin*ppm/1e6;
dalpha=-interpratio*ppm/1e6;
alpha=0;


%-- calculate interpolation filters ---  
den=1;
Rp=.4;
Rs=130;
%Rs=110;
Fin=fsin;
Fpass=Fin*.40;
Fstop=Fin*.5;
Fout=Fin*interpratio;

%---------- x25 ---------
[nupx,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fout);
cf=2*interpratio; %make nb coefs an interger multiple of ntaps
nupx=cf*ceil(nupx/cf);
num = remez(nupx-1,fo,mo,w);
[dummy,ncoef]=size(num);
ntaps=ncoef/interpratio;


figure
freqz(num,den,1024,Fout);
zoom on;



%----- total coefficients and predicted MIPS for filters ---
ncoef
MIPS=fsin*(ntaps*4)/1e6;
MIPS



%----- prepare input signal -----
T1=1/fsin;
f1=1000;
f2=10000;
f3=19000;
f4=23800;

%----- generate input samples ----
i=0:NSAMPLES-1;
t=i*T1;
insig=cos(2*pi*f1*t)+cos(2*pi*f2*t)+cos(2*pi*f3*t)+cos(2*pi*f4*t);

%---- calculate output signal ----
offset=0;
M=interpratio;
L=interpratio;
D(1:ntaps)=zeros(1,ntaps);

done=0;
y=zeros(1,4);
convcnt=0;
insmpl=1;
outsmpl=1;

while (insmpl<NSAMPLES-ntaps-ntaps)
	alpha=alpha+dalpha;
	if (alpha>.5)
		alpha=alpha-1;
		offset=offset+1;
	else
		if (alpha<-.5)
			alpha=alpha+1;
			offset=offset-1;
		end
	end
	done=0;
	while (done==0) 
		if (offset<M)
			%------- convolution for 4 samples --
			temp=sum(D(1:ntaps).*(num(offset+1:interpratio:offset+1+(interpratio*ntaps)-1)));
			y(1:4)=[temp y(1) y(2) y(3)];
			offset=offset+1;
			convcnt=convcnt+1;
			if (convcnt==4)
				convcnt=0;
				done=1;
				offset=offset+L-4;
			end
		else
			%------- update 2nd stage delay line with one 1st stage output sample--
			nxtsamp=insig(insmpl);
			insmpl=insmpl+1;
			D=[nxtsamp D(1:ntaps-1)];
			offset=offset-M;
		end
	end % while done==0
	xinp=alpha+.5;
	outsig(outsmpl)=poly3(y(4 :-1 :1),xinp);
	outsmpl=outsmpl+1;
end


%---- plot results ------

figure

[dummy nsmpls]=size(outsig);
if (rem(nsmpls,2)==0)
	winval=chebwin(nsmpls-1,140)';
	winval(nsmpls)=0;
else
	winval=chebwin(nsmpls,140)';
end
mult=6*interpratio;
fftin=winval.*((mult/nsmpls)*outsig(1:nsmpls));
amp=abs(fft(fftin));
ampdb=20*log10(amp+epsilon);
i=1:nsmpls;
freq=((i-1)/nsmpls)*fsout;
plot(freq,ampdb);
grid on;
zoom on;
axis([0 fsin/2 -150 0]);
title(['Results for ppm=',num2str(ppm),'']);

