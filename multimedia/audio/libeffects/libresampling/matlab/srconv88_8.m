clear;
close all;
srconv6x_down;
srconv44_48;
srconv2x_down;
fc=1000;
fs=88200;
T=1/fs;
checkdisto=0;
totalDelay = delay_37_34 + delay_2x + delay_6x;
maxDelayFast = totalDelay;
if (checkdisto==0)
    N = 2048;
else
    N = 88200;
end
NRES = floor(N*37/408);
		

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



res = upfirdn(input,deci1_6x,1,6);
res = res * 2^(shift_6x);
res = res * mult_6x;
figure
epsilon=1e-10;
fftsiz=128;
i=1:fftsiz;
freq=(i-1)*fs*(1/6)/fftsiz;
spect=(4/fftsiz)*fft(res(floor(N*1/6)-fftsiz+1:floor(N*1/6)).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (fs*1/(6*2)) -180 0]);
title('Spectrum of 1/6xinput signal');




res1 = upfirdn(res,deci1_34_37,37,34);
res1 = res1 * 2^(shift_37_34);
res1 = res1 * mult_37_34;
figure;
i=1:fftsiz;
freq=(i-1)*fs*(37/204)/fftsiz;
spect=(4/fftsiz)*fft(res1(floor(N*37/204)-fftsiz+1:floor(N*37/204)).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (fs*(37/204)/2) -180 0]);
title('Spectrum of 2x output signal');


res2 = upfirdn(res1,deci1_2x,1,2);
res2 = res2 * 2^(shift_2x);
res2 = res2 * mult_2x;
figure;
i=1:fftsiz;
freq=(i-1)*fs*(37/408)/fftsiz;
spect=(4/fftsiz)*fft(res2(NRES-fftsiz+1:NRES).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (fs*(37/408)/2) -180 0]);
title('Spectrum of final output signal');



if (checkdisto>0)
	fil1=fopen('simout.txt','wt');
	fprintf(fil1,'%f\n',res2);
	fclose('all');
end
%Write the corresponding test file
fid = fopen('testval_hamaca88_8_fast.c','w');


fprintf(fid, '#ifndef __flexcc2__\n');

%----------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in88_8[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
		
fprintf(fid,'const YMEM Float out88_8[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res2(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res2(k),k-1);
%-------
N=floor(11*N/16);
NRES=floor(11*NRES/16);
fprintf(fid, '#else\n');
%----------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in88_8[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
		
fprintf(fid,'const YMEM Float out88_8[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res2(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res2(k),k-1);
%-------
fprintf(fid, '#endif\n');
fclose('all');





