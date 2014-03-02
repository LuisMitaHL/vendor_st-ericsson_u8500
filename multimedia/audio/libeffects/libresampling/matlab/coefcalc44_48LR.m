fidelity

fsin=44100;
M=37;
Fpass=20000;
Fstop=26400; %-23 dB at f=24100 Hz 894 coefs
fs=M*fsin; %for oversampling
%Rp=.1;
Rp=.01;
Rs=94;
[nupx,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], fs);
nupx=2*round(nupx/2);
coefs_44_48LR = remez(nupx-1,fo,mo,w);

figure
freqz(coefs_44_48LR); zoom;
title('frequency response before normalizing');
%-------- Write the tables to a C-file -------------
[coefs_44_48LR,dummy]=normfilter1(coefs_44_48LR,1);
odd=rem(ceil(nupx/34),2);
temp1=(floor(ceil(nupx/(2*34)))+odd)*34;
odd=rem(ceil(nupx/37),2);
temp2=(floor(ceil(nupx/(2*37)))+odd)*37;
nbcoef_44_48LR=max(temp1,temp2);
nzcoefs=37;
Coefs_44_48LR=[zeros(1,nzcoefs) coefs_44_48LR];
nbcoef_44_48LR=nbcoef_44_48LR+nzcoefs;

fid = openfile('resample_table44_48LR.c');
fprintf(fid,'#include "resample_local.h"\n\n');
fprintf(fid,'#ifndef M4_LL_MSEC\n\n');
fprintf(fid,'Float const YMEM resample_44_48LR[%d] = {\n',(nbcoef_44_48LR+1));
for k = 1:nbcoef_44_48LR-1
		 fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',Coefs_44_48LR(k),k-1);
end
k=nbcoef_44_48LR;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',Coefs_44_48LR(k),k-1);
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',0,k);
fprintf(fid,'#endif //ifndef M4_LL_MSEC\n\n');
fclose(fid);


gen_src_incr_offset_tab;

