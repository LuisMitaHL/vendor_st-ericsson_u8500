% design fir filter for 44.1->32 decimation
status = who('deci1_29_40');
if ( size(status) == 0 )
  % this filter takes forever, only compute it if needed
  
  Fs = 1278900
  Fpass = 13772;
  Fstop = 15986;
  Rp = 0.1;
%  Rs = 96;
%  Rs = 116;
  stopband;
 [n_29_40,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );

%------------------ Calcul for integer number of coefs per phase ---------------------
%       ratio   nphases decimno divisor
%       -----   ------- ------- -------
%       29/40   29      1       29 
%       40/29   40      1       40 
%
%
%       all divisors:   29*2    40*2   	
%                       29*2   	5*2*2*2*2  
%
%       common num:    	5*2*2*2*2*29=2320 



  n_29_40 = round(n_29_40/2320)*2320

  % remez returns n+1 order, force to even value
  deci1_29_40 = remez(n_29_40-1,fo,mo,w);
  figure
  freqz(deci1_29_40); zoom;
  title('frequency response before normalizing');
  %-------- Write the tables to a C-file -------------
  [deci1_29_40,dummy]=normfilter1(deci1_29_40,1);
  fid = openfile('resample_table44_32.c');
%  fprintf(fid,'#include "resample.h"\n');
  fprintf(fid,'#include "resample_local.h"\n\n');
  fprintf(fid,'#ifndef USE_LOWMEM\n');
  fprintf(fid,'Float const YMEM resample_44_32[%d] = {\n',(n_29_40/2)+1);
  for k = 1:n_29_40/2-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',deci1_29_40(k),k-1);
  end
  k = n_29_40/2;
  fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) ,  \t\t// %d\n',deci1_29_40(k),k-1);
  fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',1,k);
  fprintf(fid,'#endif /* USE_LOWMEM */\n');
  fclose(fid);
  %--------------------------------------------------

end
