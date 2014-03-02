status = who('maxDelayFast');
if ( size(status) == 0 )
  maxDelayFast = 0;
end

coefcalc12;

[shift_10_12 mult_10_12] = scalefilt(deci1_12x, 10);

figure
freqz(2^shift_10_12*mult_10_12*deci1_12x/10);zoom on; 
title('frequency response after scaling');

% compute max delay
delay_10_12=n12x/10;
totalDelay = delay_10_12;

if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay
end


%Write the corresponding header file
fid = openfile('table10_12.h');
fprintf(fid, '#define N_10_12 %d \n ',n12x);
fprintf(fid, '#define M_10_12 %d \n ',5);
fprintf(fid, '#define L_10_12 %d \n ',6);
fprintf(fid, '#define decimno_10_12 %d \n ',2);
fprintf(fid, '#define DELAY_10_12 (%d) \n ', delay_10_12);
fprintf(fid, '#define SHIFT_10_12 (%d) \n' , -shift_10_12);
fprintf(fid, '#define SCALE_10_12 FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_10_12);
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_5_6[%d];\n',incr_off_siz(5,6));
fprintf(fid, '#define NZCOEFS_10_12 (%d)\n' , 0);
fclose(fid);

