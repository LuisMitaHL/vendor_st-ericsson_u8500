close all;
srconv4x
fc=1000;
fs=11025;
T=1/fs;
checkdisto=0;
if (checkdisto==0)
	N = 2048;
else
	N = 11000;
end

NRES = N*4;
		
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

res = upfirdn(input,deci1_4x,4,1);
res = res * 2^(shift_4x);
res = res * mult_4x;
figure;
epsilon=1e-10;
fftsiz=4096;
i=1:fftsiz;
freq=(i-1)*fs*4/fftsiz;
spect=(4/fftsiz)*fft(res(NRES-fftsiz+1:NRES).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 fs*4/2 -180 0]);
title('Spectrum of output signal');


%freqz(res)
if (checkdisto>0)
	fil1=fopen('simout.txt','wt');
	fprintf(fil1,'%f\n',res);
	fclose('all');
end
%Write the corresponding test file
fid = fopen('testval_hamaca11_44a.c','w');
fprintf(fid, '#ifndef __flexcc2__\n');
	
%--------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
		
fprintf(fid,'Float in11_44[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
fprintf(fid,'const YMEM Float out11_44[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res(k),k-1);
%-------
N=floor(N/16);
NRES=floor(NRES/16);
fprintf(fid, '#else\n');
%--------
fprintf(fid, '#define N_IN   %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
		
fprintf(fid,'Float in11_44[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
fprintf(fid,'const YMEM Float out11_44[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res(k),k-1);
%-------
fprintf(fid, '#endif\n');
fclose('all');



