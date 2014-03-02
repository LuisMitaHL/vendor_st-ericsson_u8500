fc=1000;
fs=44100;
T=1/fs;
checkdisto=0;
status = who('maxDelayFast');
if ( size(status) == 0 )
  maxDelayFast = 0;
end

coefcalc44_48;

%************************** Standard Quality ************************

[shift_37_34 mult_37_34] = scalefilt(deci1_34_37, 37);
figure
freqz((2^shift_37_34)*(mult_37_34/37)*deci1_34_37);zoom on; 
title('frequency response after scaling');

% compute max delay
delay_37_34 = ceil(n_34_37 / 37);
delta=(delay_37_34*37)-n_34_37;

totalDelay = delay_37_34;

if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay
end

%Write the corresponding header file
fid = openfile('table44_48_fast.h');
fprintf(fid, '#define N_44_48_FAST %d\n',n_34_37);
fprintf(fid, '#define M_44_48_FAST %d\n',37);
fprintf(fid, '#define L_44_48_FAST %d\n',34);
fprintf(fid, '#define decimno_44_48_FAST %d\n',1);
fprintf(fid, '#define DELAY_44_48_FAST (%d)\n', delay_37_34);
fprintf(fid, '#define SHIFT_44_48_FAST (%d)\n' , -shift_37_34);
fprintf(fid, '#define SCALE_44_48_FAST FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_37_34);
if (commontable==1)
	fprintf(fid, '#define common_table\n');
	fprintf(fid, 'extern Float const YMEM resample_48_44[%d];\n',(nbcoef_48_44+1));
else
	fprintf(fid, 'extern Float const YMEM resample_44_48[%d];\n',(nbcoef_48_44+1));
end
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_37_34[%d];\n',incr_off_siz(37,34));
fprintf(fid, '#define NZCOEFS_44_48_FAST (%d)\n' , nzcoefs);
fclose(fid);
if (define_fidelity==HIFI)
	[shift_37_34H mult_37_34H] = scalefilt(deci1_34_37H, 37);
	figure
	freqz((2^shift_37_34H)*(mult_37_34H/37)*deci1_34_37H);zoom on; 
	title('frequency response after scaling');

	% compute max delay
	delay_37_34H = n_34_37H / 37;
	totalDelay = delay_37_34H;

	if( totalDelay > maxDelayFast )
		maxDelayFast = totalDelay
	end

	%Write the corresponding header file
	fid = openfile('table44_48_fastH.h');
	fprintf(fid, '#define N_44_48_FASTH %d \n ',n_34_37H);
	fprintf(fid, '#define M_44_48_FASTH %d \n ',37);
	fprintf(fid, '#define L_44_48_FASTH %d \n ',34);
	fprintf(fid, '#define decimno_44_48_FASTH %d \n ',1);
	fprintf(fid, '#define DELAY_44_48_FASTH (%d) \n ', delay_37_34H);
	fprintf(fid, '#define SHIFT_44_48_FASTH (%d) \n' , -shift_37_34H);
	fprintf(fid, '#define SCALE_44_48_FASTH FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_37_34H);
	if (commontable==1)
		fprintf(fid, '#define common_table\n');
		fprintf(fid, 'extern Float const YMEM resample_48_44H[%d];\n',(nbcoef_48_44H+1));
	else
		fprintf(fid, 'extern Float const YMEM resample_44_48H[%d];\n',(nbcoef_48_44H+1));
	end
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_37_34[%d];\n',incr_off_siz(37,34));
	fprintf(fid, '#define NZCOEFS_44_48_FASTH (%d) \n' , 0);
	fclose(fid);
end

if (checkdisto==0)
    N = 2048;
else
    N=44100;
end

NRES = floor(N*37/34);
		
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

res = upfirdn(input,deci1_34_37,37,34);
res1 = res * 2^(shift_37_34);
res1 = res1 * mult_37_34;
figure
epsilon=1e-10;
fftsiz=1024;
i=1:fftsiz;
freq=(i-1)*fs*(37/34)/fftsiz;
spect=(4/fftsiz)*fft(res(NRES-fftsiz+1:NRES).*chebwin(fftsiz,160)',fftsiz);
spectdb=20*log10(abs(spect)+epsilon);
plot(freq,spectdb);
grid on;
zoom on;
axis([0 fs*37/(34*2) -180 0]);
title('Spectrum of output signal');

if (checkdisto>0)
    fil1=fopen('simout.txt','wt');
    fprintf(fil1,'%f\n',res1);
    fclose('all');
end
%Write the corresponding test file
fid = fopen('testval_hamaca44_48_fast.c','w');

fprintf(fid, '#ifndef __flexcc2__\n');

%------------
fprintf(fid, '#define N_IN  %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in44_48[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
		
fprintf(fid,'const YMEM Float out44_48[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res1(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res1(k),k-1);
%--------
N=floor(N/16);
NRES=floor(NRES/16);
fprintf(fid, '#else\n');

%------------
fprintf(fid, '#define N_IN  %d \n ',N);
fprintf(fid, '#define N_OUT  %d \n ',NRES);
fprintf(fid,'Float in44_48[%d] = {\n',N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',input(k),k-1);
		
fprintf(fid,'const YMEM Float out44_48[%d] = {\n',NRES);
for k = 1:NRES-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',res1(k),k-1);
end
k = NRES;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) }; \t\t// %d\n',res1(k),k-1);
%--------

fprintf(fid, '#endif\n');




fclose(fid);		
