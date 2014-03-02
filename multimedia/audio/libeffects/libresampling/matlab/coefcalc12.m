    % design fir filter for 12x resampling

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
	fidelity;
	common_num=360;
	symmetry=2; %to allow loading only half the table
    Fs = 1;
    Fstop = 1/24;
	stopband;
	if (improve_stdTHD==1)
		commun_num=72;
	end
	Fpass = .9*Fstop;

	%************************ standard quality ***********************************
    Rp = 0.3;
	Rs=90;
	if (improve_stdTHD==1)
		Rs=100;
	end
    [n12x,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
	n12x
	if (improve_stdTHD==0)
    	n12x = ( floor(n12x/(common_num*symmetry))+1 ) *common_num*symmetry
	else
		n12x = (floor(n12x/144)+1)*144
	end
    deci1_12x = remez(n12x-1,fo,mo,w);
    figure
    freqz(deci1_12x); zoom;
    title('frequency response before normalizing');
    %-------------- Write the tables to a C-file ------
	nbcoef=n12x/symmetry;
    [deci1_12x,dummy]=normfilter1(deci1_12x,1);
    fid = openfile('resample_table12x.c');
    fprintf(fid,'#include "resample_local.h"\n\n');
	fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
    fprintf(fid,'Float const YMEM resample_12x[%d] = {\n',(nbcoef)+1);
    for k = 1:nbcoef-1
       fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',deci1_12x(k),k-1);
    end
    k = nbcoef;
    fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',deci1_12x(k),k-1);
    fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',symmetry-1,k);
	fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
    fclose(fid);

	fid = openfile('resample_table12xH.c');
	fprintf(fid,'#include "resample_local.h"\n\n');
	fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
	if ((define_fidelity==HIFI)|(define_fidelity==HIFI_LOCOEFS))
		Rp=.1;
		Rs=130;
		[n12xH,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
		n12xH
		n12xH = ( floor(n12xH/( 2*2*2*3*3 ))+1 ) *(2*2*2*3*3)
		deci1_12xH = remez(n12xH-1,fo,mo,w);
		figure
		freqz(deci1_12xH); zoom;
		title('frequency response before normalizing');
		%-------------- Write the tables to a C-file ------
		nbcoefH=n12xH/symmetry;
		[deci1_12xH,dummy]=normfilter1(deci1_12xH,1);
%		fid = openfile('resample_table12xH.c');
%		fprintf(fid,'#include "resample_local.h"\n\n');
		fprintf(fid,'Float const YMEM resample_12xH[%d] = {\n',(nbcoefH)+1);
		for k = 1:nbcoefH-1
			fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',deci1_12xH(k),k-1);
		end
		k = nbcoefH;
		fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',deci1_12xH(k),k-1);
		fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',symmetry-1,k);
		%--------------------------------------------------
	end
	fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
	fclose(fid);
	gen_src_incr_offset_tab;
