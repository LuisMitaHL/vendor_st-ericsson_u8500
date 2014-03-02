clear;
close all;
srconv3x_down;
srconv2x_down;
srconv44_48;
fc=1000;
fs=44100;
T=1/fs;
checkdisto=0;
totalDelay = delay_37_34 + delay_3x + delay_2x;
maxDelayFast = totalDelay;
if (checkdisto==0)
    N = 2048;
else
    N = 44100;
end
NRES = floor(N*37/(34*3*2));
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



res = upfirdn(input,deci1_3x,1,3);
res = res * 2^(shift_3x);
res = res * mult_3x;
figure
epsilon=1e-10;
fftsiz=256;
i=1:fftsiz;
freq=(i-1)*fs*(1/3)/fftsiz;
spect=(4/fftsiz)*fft(res(floor(N*1/3)-fftsiz+1:floor(N*1/3)).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (fs*1/(3*2)) -180 0]);
title('Spectrum of (1/3)xinput signal');




res1 = upfirdn(res,deci1_34_37,37,34);
res1 = res1 * 2^(shift_37_34);
res1 = res1 * mult_37_34;
figure;
i=1:fftsiz;
freq=(i-1)*fs*(37/(34*3))/fftsiz;
spect=(4/fftsiz)*fft(res1(floor(N*37/(34*3))-fftsiz+1:floor(N*37/(34*3))).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (fs*(37/(34*3*2))) -180 0]);
title('Spectrum of (1/3)*(37/34) output signal');


res2 = upfirdn(res1,deci1_2x,1,2);
res2 = res2 * 2^(shift_2x);
res2 = res2 * mult_2x;
figure;
i=1:fftsiz;
freq=(i-1)*fs*(37/(34*3*2))/fftsiz;
spect=(4/fftsiz)*fft(res2((NRES-fftsiz+1):NRES).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 (fs*(37/(34*3*2*2))) -180 0]);
title('Spectrum of output signal');




if (checkdisto>0)
	fil1=fopen('simout.txt','wt');
	fprintf(fil1,'%f\n',res2);
	fclose('all');
end
%Write the corresponding test file
fid = fopen('testval_hamaca44_8_fasta.c','w');

fprintf(fid, '#ifndef __flexcc2__\n');

%-------------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in44_8[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
		
fprintf(fid,'const YMEM Float out44_8[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res2(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res2(k),k-1);
%---------
N=floor(5*N/16);
NRES=floor(5*NRES/16);
fprintf(fid, '#else\n');

%-------------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in44_8[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
		
fprintf(fid,'const YMEM Float out44_8[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res2(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res2(k),k-1);
%---------
fprintf(fid, '#endif\n');
fclose('all');





