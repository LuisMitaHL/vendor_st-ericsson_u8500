clear;
close all;

%-- init -- 
interpratio1=2;
interpratio2=25;
%interpratio1=5;
%interpratio2=10;
interpratio=interpratio1*interpratio2;
interpratio
NSAMPLES=48000;
ORD=3;

ppm=1000;
epsilon=1e-10;
fsin=48000;

fsout=fsin+fsin*ppm/1e6;
dalpha=-interpratio*ppm/1e6;
alpha=0;


%-- calculate fixed 2-stage interpolation filters ---  
den=1;
Rp=.4;
Rs=130;
Fin=fsin;
Fpass=Fin*.40;
Fstop1=Fin*.5;
Fstop2=interpratio1*Fstop1;
Fout1=Fin*interpratio1;
Fout2=Fin*interpratio1*interpratio2;

%---------- x5 ---------
[nupx,fo,mo,w] = remezord( [Fpass Fstop1], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fout1);
cf=2*interpratio1; %make nb coefs an interger multiple of ntaps
nupx=cf*ceil(nupx/cf);
num1 = remez(nupx-1,fo,mo,w);
[dummy,ncoef1]=size(num1);
ntaps1=ncoef1/interpratio1;

%---------- x10 --------
[nupx,fo,mo,w] = remezord( [Fpass Fstop2], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fout2);
cf=2*interpratio2; %make nb coefs an interger multiple of ntaps
nupx=cf*ceil(nupx/cf);
num2 = remez(nupx-1,fo,mo,w);
[dummy,ncoef2]=size(num2);
ntaps2=ncoef2/interpratio2;

figure
freqz(num1,den,1024,Fout1);
zoom on;
figure
freqz(num2,den,1024,Fout2);
zoom on;

%----- total coefficients and predicted MIPS for filters ---
ncoefs=ncoef1+ncoef2;
ncoefs
MIPS=fsin*(ncoef1+ntaps2*4)/1e6;
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
offset1=0;
M1=interpratio1;
L1=1;
D1(1:ntaps1)=zeros(1,ntaps1);

offset2=0;
M2=interpratio2;
L2=interpratio;
D2(1:ntaps2)=zeros(1,ntaps2);

done1=0;
done2=0;
y=zeros(1,ORD+1);
convcnt=0;
insmpl=1;
outsmpl=1;

while (insmpl<NSAMPLES-ntaps1-ntaps2)
	alpha=alpha+dalpha;
	if (alpha>.5)
		alpha=alpha-1;
		offset2=offset2+1;
	else
		if (alpha<-.5)
			alpha=alpha+1;
			offset2=offset2-1;
		end
	end
	done2=0;
	while (done2==0) %--- 2nd stage call -------
		if (offset2<M2)
			%------- convolution for 4 samples --
			temp=sum(D2(1:ntaps2).*(num2(offset2+1:interpratio2:offset2+1+(interpratio2*ntaps2)-1)));
			y(1:ORD+1)=[temp y(1:ORD)];
			offset2=offset2+1;
			convcnt=convcnt+1;
			if (convcnt==4)
				convcnt=0;
				done2=1;
				offset2=offset2+L2-4;
			end
		else
			%------- update 2nd stage delay line with one 1st stage output sample--
			done1=0;
			while (done1==0) %--- 1st stage call -----
				if (offset1<M1)
					nxtsamp=sum(D1(1:ntaps1).*(num1(offset1+1:interpratio1:offset1+1+(interpratio1*ntaps1)-1)));
					offset1=offset1+L1;
					done1=1;
				else     %---- update 1st stage delay line with one input sample ---
					nxtsamp=insig(insmpl);
					D1=[nxtsamp D1(1:ntaps1-1)];
					insmpl=insmpl+1;
					offset1=offset1-M1;
				end	
			end		
			D2=[nxtsamp D2(1:ntaps2-1)];
			offset2=offset2-M2;
		end
	end % while done2==0
	xinp=alpha+.5;
	if (ORD==3)
		outsig(outsmpl)=poly3(y(ORD+1 :-1 :1),xinp);
	else
		outsig(outsmpl)=poly2a(y(ORD+1 :-1 :1),xinp);
	end
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

