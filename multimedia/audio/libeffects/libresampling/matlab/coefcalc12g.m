    % design fir filter for 12x resampling
    Fs = 1;
    Fstop = 1/24;
    Rp = 0.3;
	stopband;

%   For standard SRC applicatioon
%	Fpass = .9*Fstop;
%	Rs=90;

%   For gaming application
    Fpass = .8*Fstop;
	Rs=80;

    [n12x,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
	n12x
%------------------ Calcul for integer number of coefs per phase ---------------------
%	ratio	nphases	decimno	divisor
%	-----	-------	-------	-------
%	x1			1		12		12
%	x2			2		6		12
%	x3			3		4		12
%	x4			4		3		12
%	x6			6		2		12
%	x12			12		1		12
%	1/2			1		6		6
%	1/3			1		4		4
%	1/4			1		3		3
%	1/6			1		2		2
%	1/12		1		1		1		
%	2/3			2		4		8
%	3/2			3		4		12
%	3/4			3		3		9
%  	4/3     	4       3		12
%
%	12/10		12		1
%	9/12		9		1
%	12/9		12		1
%	10/12		10		1
%
%	all divisors: 	12 			6 		4 		3 		2 		8 		9		10
%					2*3*2		2*3		2*2		3		2		2*2*2	3*3		2*5
%	common num:	2*2*2*3*3*5=360
%
%	common_num=360;
%	all divisors for conversion towards 48 only
%								6		4		3		2		8
%								2*3		2*2		3		2		2*2*2
%	common num: 2*2*2*3=24
%								
	common_num=24;
	
%	symmetry=1; %requires loading the whole table
	symmetry=2; %to allow loading only half the table
    n12x = ( floor(n12x/(common_num*symmetry))+1 ) *common_num*symmetry

    % remez returns n+1 order, force to even value
    deci1_12xg = remez(n12x-1,fo,mo,w);
    figure
    freqz(deci1_12xg); zoom;
    title('frequency response before normalizing');
    %-------------- Write the tables to a C-file ------
	nbcoef=n12x/symmetry;
    [deci1_12xg,dummy]=normfilter1(deci1_12xg,1);
    fid = openfile('resample_table12xg.c');
%    fprintf(fid,'#include "resample.h"\n');
    fprintf(fid,'#include "resample_local.h"\n\n');
%	fprintf(fid,'#ifdef USE_LOW_MIPS\n');
%    fprintf(fid,'Float const YMEM resample_4x[N_4x+1] = {\n');
    fprintf(fid,'Float const YMEM resample_12xg[%d] = {\n',(nbcoef)+1);
    for k = 1:nbcoef-1
       fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',deci1_12xg(k),k-1);
    end
    k = nbcoef;
    fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',deci1_12xg(k),k-1);
    fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',symmetry-1,k);
%	fprintf(fid,'#endif\n');
    fclose(fid);
    %--------------------------------------------------
	gen_src_incr_offset_tab;

