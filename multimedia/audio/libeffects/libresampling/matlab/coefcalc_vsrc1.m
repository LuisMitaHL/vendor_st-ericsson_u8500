clear
close all;
% design fir filter for x2,x25 resampling for use in variable ppm frequency correction

%	ratio	nphases	divisor
%	-----	-------	-------	
%	x2			2		1	
%	x25			25		50

fsin=48000;
interpratio1=2;
interpratio2=25;
interpratio=interpratio1*interpratio2;

Rp=.4;
Rs=130;
Fin=fsin;
Fpass=Fin*.40;
Fstop1=Fin*.5;
Fstop2=interpratio1*Fstop1;
Fout1=Fin*interpratio1;
Fout2=Fin*interpratio1*interpratio2;
den=1;

%---------- x2 ---------
[nupx,fo,mo,w] = remezord( [Fpass Fstop1], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fout1);
cf=2*interpratio1; %make nb coefs an interger multiple of ntaps
nupx=cf*ceil(nupx/cf);
vsrc1num1 = remez(nupx-1,fo,mo,w);
[dummy,ncoef1]=size(vsrc1num1);
ntaps1=ncoef1/interpratio1;

%---------- x25 --------
[nupx,fo,mo,w] = remezord( [Fpass Fstop2], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fout2);
cf=2*interpratio2; %make nb coefs an interger multiple of ntaps
nupx=cf*ceil(nupx/cf);
vsrc1num2 = remez(nupx-1,fo,mo,w);
[dummy,ncoef2]=size(vsrc1num2);
ntaps2=ncoef2/interpratio2;

figure
freqz(vsrc1num1,den,1024,Fout1);
zoom on;
figure
freqz(vsrc1num2,den,1024,Fout2);
zoom on;
%-------------- Write the tables to a C-file ------
nbcoef=ncoef1/2;
[vsrc1num1,dummy]=normfilter1(vsrc1num1,2);
fid = openfile('vsrc1_table1.c');
fprintf(fid,'#include "resample_local.h"\n\n');
fprintf(fid,'Float const YMEM vsrc1_table1[%d] = {\n',nbcoef);
for k = 1:nbcoef-1
	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',vsrc1num1(k),k-1);
end
k = nbcoef;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL)   \t\t// %d\n',vsrc1num1(k),k-1);
fprintf(fid,'};  \n');
fclose(fid);

%-------------- Write the tables to a C-file ------
nbcoef=ncoef2/2;
[vsrc1num2,dummy]=normfilter1(vsrc1num2,25);
fid = openfile('vsrc1_table2.c');
fprintf(fid,'#include "resample_local.h"\n\n');
fprintf(fid,'Float const YMEM vsrc1_table2[%d] = {\n',nbcoef);
for k = 1:nbcoef-1
	fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',vsrc1num2(k),k-1);
end
k = nbcoef;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL)   \t\t// %d\n',vsrc1num2(k),k-1);
fprintf(fid,'};  \n');
fclose(fid);



