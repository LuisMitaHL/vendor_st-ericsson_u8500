status = who('maxDelay');
if ( size(status) == 0 )
  maxDelay = 0;
end

% design fir filter for 12x resampling
coefcalc12g;
n4x=n12x/3;
[shift_4x mult_4x] = scalefilt(deci1_12xg,12);
deci1_4x=deci1_12xg(1:3:n12x);

figure
freqz((2^shift_4x)*(mult_4x/4)*deci1_4x);zoom on;
title('frequency response after scaling');

% compute max delay
delay_4x = n12x/12;
totalDelay = delay_4x;

if( totalDelay > maxDelay )
  maxDelay = totalDelay
end



%Write the corresponding header file
fid = openfile('table4xag.h');
fprintf(fid, '#define N_4xag %d \n ',n12x);
fprintf(fid, '#define M_4xag %d \n ',4);
fprintf(fid, '#define L_4xag %d \n ',1);
fprintf(fid, '#define decimno_4xag %d \n ',3);
fprintf(fid, '#define DELAY_4xag (%d)  \n ', delay_4x);
fprintf(fid, '#define SHIFT_4xag %d \n ',-shift_4x);
fprintf(fid, '#define SCALE_4xg FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_4x);		
fprintf(fid, 'extern Float const YMEM resample_12xg[%d];\n',(nbcoef)+1);		
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_12_3[%d];\n',incr_off_siz(4,1));
fprintf(fid, '#define NZCOEFS_4xag (%d)\n' , 0);
fclose(fid);
