status = who('maxDelay');
if ( size(status) == 0 )
  maxDelay = 0;
end

% design fir filter for 12x resampling
coefcalc12g;
n2x=n12x/6;
[shift_2x mult_2x] = scalefilt(deci1_12xg,12);
deci1_2x=deci1_12xg(1:6:n12x);

figure
freqz((2^shift_2x)*(mult_2x/2)*deci1_2x);zoom on;
title('frequency response after scaling');

% compute max delay
delay_2x = n12x/12;
totalDelay = delay_2x;

if( totalDelay > maxDelay )
  maxDelay = totalDelay
end



%Write the corresponding header file
fid = openfile('table2xag.h');
fprintf(fid, '#define N_2xag %d \n ',n12x);
fprintf(fid, '#define M_2xag %d \n ',2);
fprintf(fid, '#define L_2xag %d \n ',1);
fprintf(fid, '#define decimno_2xag %d \n ',6);
fprintf(fid, '#define DELAY_2xag (%d)  \n ', delay_2x);
fprintf(fid, '#define SHIFT_2xag %d \n ',-shift_2x);
fprintf(fid, '#define SCALE_2xg FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_2x);		
fprintf(fid, 'extern Float const YMEM resample_12xg[%d];\n',(nbcoef)+1);		
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_12_6[%d];\n',incr_off_siz(2,1));
fprintf(fid, '#define NZCOEFS_2xag (%d)\n' , 0);

fclose(fid);
