clear;
close all;
fc=1000;
fs=11025;
T=1/fs;
checkdisto=0;
status = who('maxDelayFast');
if ( size(status) == 0 )
  maxDelayFast = 0;
end

coefcalc40_29;
[shift_29_40 mult_29_40] = scalefilt(deci1_29_40, 29);
figure
freqz((2^shift_29_40)*(mult_29_40/29)*deci1_29_40);zoom on; 
title('frequency response after scaling');

% compute max delay
if (rem(n_29_40,29)~=0)
    delay_29_40=floor(((floor(n_29_40/29)+1)/2)+1)*2
else
    delay_29_40=n_29_40/29
end
totalDelay = delay_29_40;

if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay
end


%Write the corresponding header file
fid = openfile('table11_8_fast.h');
fprintf(fid, '#define N_11_8_FAST %d \n ',n_29_40/2);
fprintf(fid, '#define M_11_8_FAST %d \n ',29);
fprintf(fid, '#define L_11_8_FAST %d \n ',40);
fprintf(fid, '#define decimno_11_8_FAST %d \n ',1);
fprintf(fid, '#define DELAY_11_8_FAST (%d) \n ', delay_29_40);
fprintf(fid, '#define SHIFT_11_8_FAST (%d) \n' , -shift_29_40);
fprintf(fid, '#define SCALE_11_8_FAST FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_29_40);

fprintf(fid, 'extern Float const YMEM resample_44_32[%d];\n',(n_29_40/2)+1);

fclose(fid);

if (checkdisto==0)
    N = 2048;
else
    N=11025;
end

NRES = floor(N*29/40);
		
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

res = upfirdn(input,deci1_29_40,29,40);
res1 = res * 2^(shift_29_40);
res1 = res1 * mult_29_40;
figure
epsilon=1e-10;
fftsiz=1024;
i=1:fftsiz;
freq=(i-1)*fs*(29/40)/fftsiz;
spect=(4/fftsiz)*fft(res(NRES-fftsiz+1:NRES).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 fs*29/(40*2) -180 0]);
title('Spectrum of output signal');

if (checkdisto>0)
    fil1=fopen('simout.txt','wt');
    fprintf(fil1,'%f\n',res1);
    fclose('all');
end
%Write the corresponding test file
fid = fopen('testval_hamaca11_8_fast.c','w');



fprintf(fid, '#ifndef __flexcc2__\n');
%-----------------------------------------------
fprintf(fid, '#define N_IN  %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in11_8[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
		
fprintf(fid,'const YMEM Float out11_8[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res1(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res1(k),k-1);
%-------------------------------------------------
N=floor(N/16);
NRES=floor(NRES/16);
fprintf(fid, '#else\n');
%-----------------------------------------------
fprintf(fid, '#define N_IN  %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in11_8[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
		
fprintf(fid,'const YMEM Float out11_8[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res1(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res1(k),k-1);
%-------------------------------------------------
fprintf(fid, '#endif\n');


fclose(fid);		
