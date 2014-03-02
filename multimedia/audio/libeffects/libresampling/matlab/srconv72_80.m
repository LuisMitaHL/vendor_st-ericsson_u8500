srconv12_9;
srconv10_12;
fc=1000;
fs=7200;
T=1/fs;

N = 2048;

NRES1 = floor(N*12/9);
NRES = floor(N*10/9);
		
for i=1:N
	input(i) = 0;
end

PI = atan(1) * 4;
rand('seed',0);

for i=1:N
	input(i) = (rand(1) - 0.5);
end

epsilon=1e-10;
fftsiz=2048;

res = upfirdn(input,deci1_12x,12,9);
res1 = res * 2^(shift_12_9);
res1 = res1 * mult_12_9;
figure
i=1:fftsiz;
freq=(i-1)*fs*(12/9)/fftsiz;
spect=(4/fftsiz)*fft(res1(NRES1-fftsiz+1:NRES1).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 fs*12/(9*2) -180 0]);
title('Spectrum of output of first stage');


res2 = upfirdn(res1,deci1_12x,10,12);
res3 = res2 * 2^(shift_10_12);
res3 = res3 * mult_10_12;
figure
i=1:fftsiz;
freq=(i-1)*fs*(10/9)/fftsiz;
spect=(4/fftsiz)*fft(res3(NRES-fftsiz+1:NRES).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 fs*10/(9*2) -180 0]);
title('Spectrum of final output ');



%Write the corresponding test file
fid = fopen('testval_hamaca72_80_fast.c','w');


fprintf(fid, '#ifndef __flexcc2__\n');
%-------------------------------------
fprintf(fid, '#define N_IN  %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in72_80[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
		
fprintf(fid,'const YMEM Float out72_80[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res3(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res3(k),k-1);
%------------------------------------
N=floor(N/16);
NRES=floor(NRES/16);
fprintf(fid, '#else\n');
%-------------------------------------
fprintf(fid, '#define N_IN  %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in72_80[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
		
fprintf(fid,'const YMEM Float out72_80[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res3(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res3(k),k-1);
%------------------------------------
fprintf(fid, '#endif\n');


fclose(fid);		
