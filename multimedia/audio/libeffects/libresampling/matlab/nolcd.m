clear;
close all;
%-------------------- init ----------------
NSMPLS=4096;
NPTS=1024;
epsilon=1e-10;
fs=48000;
M=34;
L=37;
float=0;
%--------------------- filter design -----
Rp=.4;
Rs=120;
fsout=fs*M;
Fstop=(fs*M/L)*.5;
Fpass=Fstop*.8;
%lcd=M*L;
lcd=2;
[ncfs,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], fsout);
ncfs=lcd*ceil(ncfs/lcd);
ncfs
coef = remez(ncfs-1,fo,mo,w);
%------------------ plot filter design ----
[h,f] = freqz(coef,1,NPTS,fsout);
respb=20*log10(abs(h)+epsilon);
figure
plot(f,respb);
grid on;
zoom on;
axis([0 fs/2 -150 0]);
%------------------------ coefcalc parameters -------
nzcoefs=max(M,L);
coef=[zeros(1,nzcoefs) coef];
ncoefs=nzcoefs+ncfs
%------------------------ srconv paramaters ---------
ntaps=ncfs/M
ntaps=ceil(ntaps); 	%to be stored in include file
ncfs;			%to be stored in include file
nzcoefs;		%to be stored in include file
%--------------------- testing ----------------------
D(1:ntaps)=zeros(1,ntaps);
offset=0;
nsmpls=0;
outsig=[ ];
i=0:NSMPLS-1;
fc=1000;
T=1/fs;
t=i*T;
forwardlen=ceil(ntaps/2)
revlen=ntaps-forwardlen
if (float==1)
	dfindx=nzcoefs-ntaps*M+ncfs;
	drindx=ncfs+2*nzcoefs;
	while (nsmpls<NSMPLS)
		if (offset<M)
			%------------ forward convolution -----------------------
			indx=offset+dfindx;
			temp=coef(indx+1:M:ncoefs);
			coeff=temp(1:forwardlen);
			out=sum(D(1:forwardlen).*coeff);
			indx=indx+M*forwardlen;
			%----------- reverse convolution ------------------------
			indx=drindx-indx-1;
			temp=coef(indx+1:-M:1);
			coefr=temp(1:revlen);
			out=out+sum(D(forwardlen+1:ntaps).*coefr);
			%---------- store results --------------------------------
			outsig=[outsig out];
			%-----------------------------------------------------------
			offset=offset+L;
		else
			%------------ update delay line ---------------------------
			D(1:ntaps)=[cos(2*pi*fc*t(nsmpls+1)) D(1:ntaps-1)];
			offset=offset-M;
			nsmpls=nsmpls+1;
		end
	end
else
	dfindx=nzcoefs-ntaps*M+ncfs;
	while (nsmpls<NSMPLS)
		if (offset<M)
			%------------ forward convolution -----------------------
			indx=offset+dfindx;
			temp=coef(indx+1:M:ncoefs);
			coeff=temp(1:forwardlen);
			out=sum(D(1:forwardlen).*coeff);
			indx=indx+M*forwardlen;
			%----------- reverse convolution ------------------------
			indx=nzcoefs+M-offset-1;
			temp=coef(indx+1:M:ncoefs);		
			coefr=temp(1:revlen);
			out=out+sum(D(ntaps:-1:forwardlen+1).*coefr);
			%---------- store results --------------------------------
			outsig=[outsig out];
			%-----------------------------------------------------------
			offset=offset+L;
		else
			%------------ update delay line ---------------------------
			D(1:ntaps)=[cos(2*pi*fc*t(nsmpls+1)) D(1:ntaps-1)];
			offset=offset-M;
			nsmpls=nsmpls+1;
		end
	end
end
%------------ plot test results -------------
winval=chebwin(NPTS-1,140);
winval=[winval' 0];
fftin=outsig(1+100:NPTS+100).*winval;
fftout=fft(fftin,NPTS);
amp=20*log10(abs(fftout)+epsilon);
amp=amp-max(amp);
i=0:NPTS-1;
fsout=44100;
freq=fsout*i/NPTS;
figure
plot(freq,amp);
grid on;
zoom on;
axis([0 fsout/2 -150 0]);

