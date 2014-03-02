NPTS=4096;
epsilon=1e-10;
%------------------ design filter ---------------------
if mode=='I'
	interp=1;
	fs=8000;
else
	interp=0;
	fs=48000;
end
if (interp==1)
	fso=ratio*fs;
else
	fso=fs/ratio;
end
N=10;
numcells=N/2;
Rp=.4;
Rs=84;
fpratio=.43;
wn=2*fpratio/ratio;
[B,A] = ellip(N,Rp,Rs,wn);
[hf,w]=freqz(B,A,NPTS);
hdb=20*log10(abs(hf)+epsilon);
figure(1);
if (interp>0)
	freq=fso*(w/(2*pi));
else
	freq=fs*(w/(2*pi));
end
plot(freq,hdb);
grid on;
zoom on;
if (interp>0)
	axis([0 fs/2 -100 5]);
	title(['frequency response of interpolation filter with ratio of ',num2str(ratio)]);
else
	axis([0 fso/2 -100 5]);
	title(['frequency response of decimation filter with ratio of ',num2str(ratio)]);
end

phase=unwrap(angle(hf));
if (interp>0)
	dw=(w(2)-w(1))*fso;
else
	dw=(w(2)-w(1))*fs;
end
for i=1:NPTS-1
	gd(i)=-1000*(phase(i+1)-phase(i))/dw;
end
gd(NPTS)=gd(NPTS-1);
figure(2);
plot(freq,gd);
grid on;
zoom on;
if (interp>0)
	axis([0 fs/2 0 10]);
	title(['grp delay in msec of interpolation filter with ratio of ',num2str(ratio)]);
else
	axis([0 fso/2 0 10]);
	title(['grp delqy in msec of decimation filter with ratio of ',num2str(ratio)]);
end
%------------------ Find biquad coefs -----------------------
[kn kd]=findbiq(B,A);
%----------- adjust the 1st stage coefs for interpolation gain loss ----
kn(1,1:3)=kn(1,1:3)*ratio;
if mode=='D'
	coefatt=1/ratio;
else
	coefatt=1;
end
maxval=max(kn(1,1:3));
if (maxval>1.0)
	kn(1,1:3)=kn(1,1:3)/maxval;
	outgain=maxval;
else
	outgain=1;
end
%-------------------- calculate the output gain and shift coeficients -------------------
shiftout=ceil(log2(outgain));
kout=outgain/(2^shiftout);
%------ Reduce maxabsval of numerator and den coefs to 1.0 ---
kn(1:N/2,2)=kn(1:N/2,2)/2;
kd(1:N/2,2)=kd(1:N/2,2)/2;
%------------------ output the coefs ---------------------------
coeffile=['resample_iircoef_',num2str(ratio),'.c'];
f1=fopen(coeffile,'wb');
fprintf(f1,'#include "resample.h"\n');
fprintf(f1,'#include "resample_local.h"\n\n');
nbcoef=(5*N/2)+2;
tabname=['Float const YMEM resample_iircoef_',num2str(ratio),'[',num2str(nbcoef),'] = {'];
fprintf(f1,'%s\n',tabname);
for i=1:N/2
	for j=1:3
     	fprintf(f1,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t\n',kn(i,j));
	end
	for j=2:3
     	fprintf(f1,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t\n',kd(i,j));
	end
end
fprintf(f1,'\t%d,  \t\t\n',shiftout);
fprintf(f1,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t\n',kout);
fclose(f1);
%-------------------- create an input signal ----------------------------
if (interp>0)
	ninsamples=2048;
	ninsamplesdsp=32;
else
	ninsamples=12288;
	ninsamplesdsp=192;
end
fc=1000;
amp=.8;
i=1:ninsamples;
tin=(i-1)*1/fs;
sigin=amp*cos(2*pi*fc*tin);
if (interp>0)
	noutsamples=ninsamples*ratio;
	noutsamplesdsp=ninsamplesdsp*ratio;
else
	noutsamples=ninsamples/ratio;
	noutsamplesdsp=ninsamplesdsp/ratio;
end
%----------------- filter the signal ------------------------------------------------------
if (interp==1)
	sigin1(1:noutsamples)=zeros(1,noutsamples);
	sigin1(1:ratio:noutsamples)=sigin(1:ninsamples);
else
	sigin1=sigin;
end

for i=1:numcells
	b=[kn(i,1) 2*kn(i,2) kn(i,3)];
	a=[kd(i,1) 2*kd(i,2) kd(i,3)];
	sigin1=filter(b,a,sigin1);
end
sigout1=sigin1;

if (interp==0)
	sigout(1:noutsamples)=coefatt*sigout1(1:ratio:ninsamples)*outgain;
else
	sigout=coefatt*sigout1*outgain;
end
%------------------ display input and output signals ------
figure(3)
plot(tin,sigin);
grid on;
zoom on;
title('Input signal');
figure(4)
i=1:noutsamples;
tout=(i-1)*1/fso;
plot(tout,sigout);
grid on;
zoom on;
if (interp>0)
	title(['Interpolated Output signal with ratio of ',num2str(ratio)]);
else
	title(['Decimated Output signal with ratio of ',num2str(ratio)]);
end
%----------- Write the corresponding test file --------------------
sigfile=['testval_iircoef_',mode,'_',num2str(ratio),'.c'];
f1 = fopen(sigfile,'w');
fprintf(f1, '#ifndef MMDSP\n');

fprintf(f1, '#define N_IN  %d \n ',ninsamples);
fprintf(f1, '#define N_OUT  %d \n ',noutsamples);
tabname=['Float in_iir_',mode,'_',num2str(ratio),'[',num2str(ninsamples),'] = {'];
fprintf(f1,'%s\n',tabname);
for k = 1:ninsamples-1
     fprintf(f1,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',sigin(k),k-1);
end
k = ninsamples;
fprintf(f1,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',sigin(k),k-1);
tabname=['const YMEM Float out_iir_',mode,'_',num2str(ratio),'[',num2str(noutsamples),'] = {'];
fprintf(f1,'%s\n',tabname);
for k = 1:noutsamples-1
     fprintf(f1,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',sigout(k),k-1);
end
k = noutsamples;
fprintf(f1,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',sigout(k),k-1);

fprintf(f1, '#else\n');

fprintf(f1, '#define N_IN  %d \n ',ninsamplesdsp);
fprintf(f1, '#define N_OUT  %d \n ',noutsamplesdsp);
tabname=['Float in_iir_',mode,'_',num2str(ratio),'[',num2str(ninsamplesdsp),'] = {'];
fprintf(f1,'%s\n',tabname);
for k = 1:ninsamplesdsp-1
     fprintf(f1,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',sigin(k),k-1);
end
k = ninsamplesdsp;
fprintf(f1,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',sigin(k),k-1);
tabname=['const YMEM Float out_iir_',mode,'_',num2str(ratio),'[',num2str(noutsamplesdsp),'] = {'];
fprintf(f1,'%s\n',tabname);
for k = 1:noutsamplesdsp-1
     fprintf(f1,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',sigout(k),k-1);
end
k = noutsamplesdsp;
fprintf(f1,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',sigout(k),k-1);

fprintf(f1, '#endif\n');
fclose(f1);

