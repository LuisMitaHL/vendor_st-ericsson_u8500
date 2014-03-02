status = who('maxDelayFast');
if ( size(status) == 0 )
  maxDelayFast = 0;
end

coefcalc12;

[shift_12_10 mult_12_10] = scalefilt(deci1_12x, 12);

figure
freqz(2^shift_12_10*mult_12_10*deci1_12x/12);zoom on; 
title('frequency response after scaling');

% compute max delay
delay_12_10=n12x/12;
totalDelay = delay_12_10;

if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay
end


%Write the corresponding header file
fid = openfile('table12_10.h');
fprintf(fid, '#define N_12_10 %d \n ',n12x);
fprintf(fid, '#define M_12_10 %d \n ',6);
fprintf(fid, '#define L_12_10 %d \n ',5);
fprintf(fid, '#define decimno_12_10 %d \n ',2);
fprintf(fid, '#define DELAY_12_10 (%d) \n ', delay_12_10);
fprintf(fid, '#define SHIFT_12_10 (%d) \n' , -shift_12_10);
fprintf(fid, '#define SCALE_12_10 FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_12_10);
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_6_5[%d];\n',incr_off_siz(6,5));
fprintf(fid, '#define NZCOEFS_12_10 (%d)\n' , 0);

fclose(fid);

