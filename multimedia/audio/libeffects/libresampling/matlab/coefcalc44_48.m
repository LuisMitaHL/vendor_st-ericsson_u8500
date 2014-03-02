fidelity
Fs = 1632000;
Fpass = 18000;
Fstop = 22100;
Rp = .3;
stopband;
%****************************** Standard quality ********************************
[n_34_37,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
n_34_37_before_rounding=n_34_37;
n_34_37_before_rounding
if (commontable==1)
	lcd=34*37;
	if (improve_stdTHD==1)
		lcd=2;
	end
else
	lcd=37*2;
end
n_34_37 = round(n_34_37/lcd)*lcd
deci1_34_37 = remez(n_34_37-1,fo,mo,w);
figure
freqz(deci1_34_37); zoom;
title('frequency response before normalizing');
%-------- Write the tables to a C-file -------------
[deci1_34_37,dummy]=normfilter1(deci1_34_37,1);
if (commontable==1)
	odd=rem(ceil(n_34_37/34),2);
	temp1=(floor(ceil(n_34_37/(2*34)))+odd)*34;
	odd=rem(ceil(n_34_37/37),2);
	temp2=(floor(ceil(n_34_37/(2*37)))+odd)*37;
	nbcoef_48_44=max(temp1,temp2);
else
	nbcoef_48_44=n_34_37/2;
end

nzcoefs=0;
if (improve_stdTHD==1)
	nzcoefs=37;
end
coefs_48_44=[zeros(1,nzcoefs) deci1_34_37];
nbcoef_48_44=nbcoef_48_44+nzcoefs;




if (commontable==1)
  	fid = openfile('resample_table48_44.c');
  	fprintf(fid,'#include "resample_local.h"\n\n');
	fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
  	fprintf(fid,'Float const YMEM resample_48_44[%d] = {\n',(nbcoef_48_44+1));
  	for k = 1:nbcoef_48_44-1
    	 fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',coefs_48_44(k),k-1);
  	end
	k=nbcoef_48_44;
	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',coefs_48_44(k),k-1);
	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',0,k);
	fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
  	fclose(fid);
	fid = openfile('resample_table44_48.c');
	fprintf(fid,'#include "resample_local.h"\n\n');
	fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
	fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
	fclose(fid);
else 
	fid = fopen('resample_table44_48.c','w');
	fprintf(fid,'#include "resample_local.h"\n\n');
	fprintf(fid,'Float const YMEM resample_44_48[%d] = {\n',(nbcoef_48_44+1));
	for k = 1:nbcoef_48_44-1
		fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',deci1_34_37(k),k-1);
	end
	k=nbcoef_48_44;
	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',deci1_34_37(k),k-1);
	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',0,k);
	fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
	fclose(fid);
end
if (commontable==1)
  	fid = openfile('resample_table48_44H.c');
  	fprintf(fid,'#include "resample_local.h"\n\n');
	fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
else

end
if (define_fidelity==HIFI)
	Rs=130;
	Rp=.1;
	Fpass=19845; 
	Fstop=22800;%allow aliasing above 18kHz for lower SRC load
	[n_34_37H,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
	n_34_37_before_rounding=n_34_37H;
	n_34_37_before_rounding
	if (commontable==1)
		lcd=34*37;
	else
		lcd=37*2;
	end
	n_34_37H = round(n_34_37H/lcd)*lcd
	deci1_34_37H = remez(n_34_37H-1,fo,mo,w);
	figure
	freqz(deci1_34_37H); zoom;
	title('frequency response before normalizing');
	%-------- Write the tables to a C-file -------------
	[deci1_34_37H,dummy]=normfilter1(deci1_34_37H,1);
	if (commontable==1)
		odd=rem(n_34_37H/34,2);
		temp1=(floor(n_34_37H/(2*34))+odd)*34;
		odd=rem(n_34_37H/37,2);
		temp2=(floor(n_34_37H/(2*37))+odd)*37;
		nbcoef_48_44H=max(temp1,temp2);
	else
		nbcoef_48_44H=n_34_37H/2;
	end
	if (commontable==1)
  		fid = openfile('resample_table48_44H.c');
  		fprintf(fid,'#include "resample_local.h"\n\n');
		fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
  		fprintf(fid,'Float const YMEM resample_48_44H[%d] = {\n',(nbcoef_48_44H+1));
  		for k = 1:nbcoef_48_44H-1
    	 	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',deci1_34_37H(k),k-1);
  		end
		k=nbcoef_48_44H;
		fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',deci1_34_37H(k),k-1);
		fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',0,k);
		fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
  		fclose(fid);
		fid = openfile('resample_table44_48H.c');
		fprintf(fid,'#include "resample_local.h"\n\n');
		fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
		fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
		fclose(fid);
	else 
		fid = openfile('resample_table44_48H.c');
		fprintf(fid,'#include "resample_local.h"\n\n');
		fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
		fprintf(fid,'Float const YMEM resample_44_48H[%d] = {\n',(nbcoef_48_44H+1));
		for k = 1:nbcoef_48_44H-1
			fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',deci1_34_37H(k),k-1);
		end
		k=nbcoef_48_44H;
		fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',deci1_34_37H(k),k-1);
		fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',0,k);
		fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
		fclose(fid);
	end
	%--------------------------------------------------
else
	if (commontable==1)
  		fid = openfile('resample_table48_44H.c');
  		fprintf(fid,'#include "resample_local.h"\n\n');
		fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
		fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
		fclose(fid);
	end
	fid = openfile('resample_table44_48H.c');
	fprintf(fid,'#include "resample_local.h"\n\n');
	fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
	fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
	fclose(fid);
end
gen_src_incr_offset_tab;
