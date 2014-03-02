clear;
close all;
srconv64_48
srconv48_44
fc=1000;
fs=64000;
T=1/fs;
checkdisto=0;
totalDelay = delay_34_37 + delay_3_4;
if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay;
end
if (checkdisto==0)
    N = 2048;
else
    N = 64000;
end
NRES = floor(N*34*3/(37*4));
		

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



res = upfirdn(input,deci1_3_4,3,4);
res = res * 2^(shift_3_4);
res = res * mult_3_4;
figure;
fftsiz=1024;
epsilon=1e-10;
i=1:fftsiz;
freq=(i-1)*fs*(3/4)/fftsiz;
spect=(4/fftsiz)*fft(res(floor(N*3/4)-fftsiz+1:floor(N*3/4)).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (fs*(3/4)/2) -180 0]);
title('Spectrum of input*3/4 signal');






res1 = upfirdn(res,deci1_34_37,34,37);
res1 = res1 * 2^(shift_34_37);
res1 = res1 * mult_34_37;
figure
i=1:fftsiz;
freq=(i-1)*fs*(102/148)/fftsiz;
spect=(4/fftsiz)*fft(res1(NRES-fftsiz+1:NRES).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (fs*102/(148*2)) -180 0]);
title('Spectrum of output signal');







if (checkdisto>0)
	fil1=fopen('simout.txt','wt');
	fprintf(fil1,'%f\n',res1);
	fclose('all');
end
%Write the corresponding test file
fid = fopen('testval_hamaca64_44_fast.c','w');


fprintf(fid, '#ifndef __flexcc2__\n');

%-----------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in64_44[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
fprintf(fid,'const YMEM Float out64_44[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res1(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res1(k),k-1);
%---------
N=floor(N/16);
NRES=floor(NRES/16);
fprintf(fid, '#else\n');
%-----------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in64_44[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
fprintf(fid,'const YMEM Float out64_44[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res1(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res1(k),k-1);
%---------
fprintf(fid, '#endif\n');
fclose('all');





