epsilon=1e-10;
NPTS=8192;
fidelity;
fid = openfile('resample_table7_9H.c');
fprintf(fid,'#include "resample.h"\n');
fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
if (define_fidelity==HIFI_LOCOEFS)
	M1=7;
	L1=5;
	M2=7;
	L2=9;
	Fs=44100;
	Fpass1=.9*Fs/2;
	Fs2 = Fs*M2*M1/L1;
	Fstop2 = 1.03*Fs/2; %allow aliasing above 18kHz for lower SRC load
	Fpass2 = Fpass1; 
	Rp2=.15;
	Rs2=122;
	[remezncoefs,fo,mo,w] = remezord( [Fpass2 Fstop2], [1 0], [10^(Rp2/20)-1 10^(-Rs2/20)], Fs2 );
	remezncoefs
	lcd=M2*L2*2;
	n_7_9H=lcd*ceil(remezncoefs/lcd);
	n_7_9H
	coef_7_9H=remez(n_7_9H-1,fo,mo,w);
	[h2,f] = freqz(coef_7_9H,1,NPTS,Fs2);
	figure
	ampdB=20*log10(abs(h2)+epsilon);
	plot(f,ampdB);
	zoom on;
	grid on;
	title('coef-7-9 frequency response');
	%-------- Write the tables to a C-file -------------
	[coef_7_9H,dummy]=normfilter1(coef_7_9H,1);
	nbcoef_7_9H=n_7_9H/2;

	fprintf(fid,'#include "resample_local.h"\n\n');
	fprintf(fid,'Float const YMEM resample_7_9H[%d] = {\n',(nbcoef_7_9H+1));
	for k = 1:nbcoef_7_9H-1
    	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',coef_7_9H(k),k-1);
	end
	k=nbcoef_7_9H;
	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',coef_7_9H(k),k-1);
	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',0,k);
	%--------------------------------------------------
end
fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
fclose(fid);
gen_src_incr_offset_tab;

