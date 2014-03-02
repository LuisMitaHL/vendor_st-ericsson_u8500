clear
close all;
% design fir filter for x25 resampling for use in variable ppm frequency correction

%	ratio	nphases	divisor
%	-----	-------	-------	
%	x25			25	25	

fsin=48000;
interpratio=25;

Rp=.4;
Rs=130;
Fin=fsin;
Fpass=Fin*.40;
Fstop=Fin*.5;
Fout=Fin*interpratio;
den=1;


%---------- x25 --------
[nupx,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fout);
cf=2*interpratio; %make nb coefs an interger multiple of ntaps
nupx=cf*ceil(nupx/cf);
vsrc1num = remez(nupx-1,fo,mo,w);
[dummy,ncoef]=size(vsrc1num);
ntaps=ncoef/interpratio;

figure
freqz(vsrc1num,den,1024,Fout);
zoom on;
%-------------- Write the tables to a C-file ------
nbcoef=ncoef/2;
[vsrc1num,dummy]=normfilter1(vsrc1num,25);
fid = openfile('vsrc1_table.c');
fprintf(fid,'#include "resample_local.h"\n\n');
fprintf(fid,'Float const YMEM vsrc1_table[%d] = {\n',nbcoef);
for k = 1:nbcoef-1
	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',vsrc1num(k),k-1);
end
k = nbcoef;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL)   \t\t// %d\n',vsrc1num(k),k-1);
fprintf(fid,'};  \n');
fclose(fid);



