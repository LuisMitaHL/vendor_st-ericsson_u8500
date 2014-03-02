status = who('maxDelay');
if ( size(status) == 0 )
  maxDelay = 0;
end

% design fir filter for 12x resampling
coefcalc12g;
%n6xg=n12x/2;
[shift_6x mult_6x] = scalefilt(deci1_12xg,12);
deci1_6x=deci1_12xg(1:2:n12x);

figure
freqz((2^shift_6x)*(mult_6x/6)*deci1_6x);zoom on;
title('frequency response after scaling');

% compute max delay
delay_6x = n12x/12;
totalDelay = delay_6x;

if( totalDelay > maxDelay )
  maxDelay = totalDelay
end



%Write the corresponding header file
fid = openfile('table6xag.h');
fprintf(fid, '#define N_6xag %d \n ',n12x);
fprintf(fid, '#define M_6xag %d \n ',6);
fprintf(fid, '#define L_6xag %d \n ',1);
fprintf(fid, '#define decimno_6xag %d \n ',2);
fprintf(fid, '#define DELAY_6xag (%d)  \n ', delay_6x);
fprintf(fid, '#define SHIFT_6xag %d \n ',-shift_6x);
fprintf(fid, '#define SCALE_6xg FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_6x);		
fprintf(fid, 'extern Float const YMEM resample_12xg[%d];\n',(nbcoef)+1);		
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_6_1[%d];\n',incr_off_siz(6,1));
fprintf(fid, '#define NZCOEFS_6xag (%d)\n' , 0);

fclose(fid);
