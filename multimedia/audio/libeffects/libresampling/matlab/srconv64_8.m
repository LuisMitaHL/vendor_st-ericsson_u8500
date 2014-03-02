clear;
close all;
srconv2x_down;
srconv4x_down;
fc=1000;
fs=64000;
T=1/fs;
checkdisto=0;
totalDelay = delay_4x + delay_2x;
maxDelayFast = totalDelay;
if (checkdisto==0)
    N = 2048;
else
    N = 64000;
end
NRES = floor(N/8);
		

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



res = upfirdn(input,deci1_2x,1,2);
res = res * 2^(shift_2x);
res = res * mult_2x;
figure
epsilon=1e-10;
fftsiz=128;
i=1:fftsiz;
freq=(i-1)*fs*(1/2)/fftsiz;
spect=(4/fftsiz)*fft(res(floor(N*1/2)-fftsiz+1:floor(N*1/2)).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (fs*1/(2*2)) -180 0]);
title('Spectrum of 1/2xinput signal');




res1 = upfirdn(res,deci1_4x,1,4);
res1 = res1 * 2^(shift_4x);
res1 = res1 * mult_4x;
figure;
i=1:fftsiz;
freq=(i-1)*fs*(1/8)/fftsiz;
spect=(4/fftsiz)*fft(res1(NRES-fftsiz+1:NRES).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (fs*(1/8)/2) -180 0]);
title('Spectrum of final output signal');




if (checkdisto>0)
	fil1=fopen('simout.txt','wt');
	fprintf(fil1,'%f\n',res1);
	fclose('all');
end
%Write the corresponding test file
fid = fopen('testval_hamaca64_8_fast.c','w');


fprintf(fid, '#ifndef __flexcc2__\n');

%-----------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in64_8[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
fprintf(fid,'const YMEM Float out64_8[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res1(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res1(k),k-1);
%--------
N=floor(8*N/16);
NRES=floor(8*NRES/16);
fprintf(fid, '#else\n');

%-----------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in64_8[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
fprintf(fid,'const YMEM Float out64_8[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res1(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res1(k),k-1);
%--------

fprintf(fid, '#endif\n');
fclose('all');






