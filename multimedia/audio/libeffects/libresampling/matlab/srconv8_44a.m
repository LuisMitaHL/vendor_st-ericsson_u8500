clear;
close all;
srconv6x
srconv48_44
fc=1000;
fs=8000;
T=1/fs;
checkdisto=0;
totalDelay = delay_34_37 + delay_6x ;
if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay;
end
if (checkdisto==0)
    N = 2048;
else
    N = 8000;
end
NRES = floor(N*44/8);
		

for i=1:N
	input(i) = 0;
end

PI = atan(1) * 4;
rand('seed',0);

if (checkdisto==0)
    for i=1:N
		input(i) = (rand(1) - 0.5);
    end
else
    for i=1:N
		input(i) = sin(2*pi*fc*T*(i-1))/2;
    end    
end


res = upfirdn(input,deci1_6x,6,1);
res = res * 2^(shift_6x);
res = res * mult_6x;
figure;
epsilon=1e-10;
fftsiz=2048;
i=1:fftsiz;
freq=(i-1)*fs*(6)/fftsiz;
spect=(4/fftsiz)*fft(res(6*N-fftsiz+1:6*N).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (fs*2/2) -180 0]);
title('Spectrum of x6 output signal');



res1 = upfirdn(res,deci1_34_37,34,37);
res1 = res1 * 2^(shift_34_37);
res1 = res1 * mult_34_37;
figure
i=1:fftsiz;
freq=(i-1)*fs*(6*34/37)/fftsiz;
spect=(4/fftsiz)*fft(res1(floor(5.5*N)-fftsiz+1:floor(5.5*N)).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (6*fs*34/(37*2)) -180 0]);
title('Spectrum of 2nd stage output');


if (checkdisto>0)
	fil1=fopen('simout.txt','wt');
	fprintf(fil1,'%f\n',res1);
	fclose('all');
end
%Write the corresponding test file
fid = fopen('testval_hamaca8_44_fasta.c','w');

fprintf(fid, '#ifndef __flexcc2__\n');
%---------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in8_44[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
fprintf(fid,'const YMEM Float out8_44[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res1(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res1(k),k-1);
%--------
N=floor(N/16);
NRES=floor(NRES/16);
fprintf(fid, '#else\n');
%---------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in8_44[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
fprintf(fid,'const YMEM Float out8_44[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res1(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res1(k),k-1);
%--------
fprintf(fid, '#endif\n');
fclose('all');





