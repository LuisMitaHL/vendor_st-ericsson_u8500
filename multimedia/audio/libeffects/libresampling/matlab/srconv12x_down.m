clear;
close all;
srconv12d
fc=1000;
fs=96000;
T=1/fs;
checkdisto=0;
if (checkdisto==0)
	N = 2048;
else
	N = 96000;
end

NRES = floor(N/12);
		
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

res = upfirdn(input,deci1_12x,1,12);		
res = res * 2^(shift_12x);
res = res * mult_12x;
figure;
epsilon=1e-10;
fftsiz=128;
i=1:fftsiz;
freq=(i-1)*fs/(12*fftsiz);
spect=(4/fftsiz)*fft(res(NRES-fftsiz+1:NRES).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 fs/(12*2) -180 0]);
title('Spectrum of output signal');
if (checkdisto>0)
        fil1=fopen('simout.txt','wt');
        fprintf(fil1,'%f\n',res);
        fclose('all');
end


%Write the corresponding test file
fid = fopen('testval_hamacadown_12xa.c','w');
		
fprintf(fid, '#ifndef __flexcc2__\n');

%---------------------------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float indown_12x[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
fprintf(fid,'const YMEM Float outdown_12x[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res(k),k-1);
%---------------------------		
N=floor(12*N/16);
NRES=floor(12*NRES/16);
fprintf(fid, '#else\n');
%---------------------------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float indown_12x[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
fprintf(fid,'const YMEM Float outdown_12x[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res(k),k-1);
%---------------------------		
fprintf(fid, '#endif\n');
fclose('all');


