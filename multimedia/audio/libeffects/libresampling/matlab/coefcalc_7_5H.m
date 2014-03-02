epsilon=1e-10;
NPTS=8192;
fidelity;
fid = openfile('resample_table7_5H.c');
fprintf(fid,'#include "resample.h"\n');
fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
if (define_fidelity==HIFI_LOCOEFS)
	M1=7;
	L1=5;
	Ratio1=M1/L1;
	Fs=44100;
	Fs1=Fs*M1;
	Rs1=130;
	Rp1=.1;
	Fpass1=.9*Fs/2;
	Fstop1=(Fs1/2)/L1;
	[remezncoefs,fo,mo,w] = remezord( [Fpass1 Fstop1], [1 0], [10^(Rp1/20)-1 10^(-Rs1/20)], Fs1 );
	remezncoefs
	lcd=M1*L1*2;
	n_7_5H=lcd*ceil(remezncoefs/lcd);
	n_7_5H
	coef_7_5H=remez(n_7_5H-1,fo,mo,w);
	[h1,f] = freqz(coef_7_5H,1,NPTS,Fs1);
	figure
	ampdB=20*log10(abs(h1)+epsilon);
	plot(f,ampdB);
	zoom on;
	grid on;
	title('coef-7-5 frequency response');
	%-------- Write the tables to a C-file -------------
	[coef_7_5H,dummy]=normfilter1(coef_7_5H,1);
	nbcoef_7_5H=n_7_5H/2;

	fprintf(fid,'#include "resample_local.h"\n\n');
	fprintf(fid,'Float const YMEM resample_7_5H[%d] = {\n',(nbcoef_7_5H+1));
	for k = 1:nbcoef_7_5H-1
    	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',coef_7_5H(k),k-1);
	end
	k=nbcoef_7_5H;
	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',coef_7_5H(k),k-1);
	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',0,k);
	%--------------------------------------------------
end
fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
fclose(fid);
gen_src_incr_offset_tab;

