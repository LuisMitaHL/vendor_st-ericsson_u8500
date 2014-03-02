status = who('maxDelayFast');
if ( size(status) == 0 )
  maxDelayFast = 0;
end

coefcalc12;

[shift_12_9 mult_12_9] = scalefilt(deci1_12x, 12);

figure
freqz(2^shift_12_9*mult_12_9*deci1_12x/12);zoom on; 
title('frequency response after scaling');

% compute max delay
delay_12_9=n12x/12;
totalDelay = delay_12_9;

if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay
end


%Write the corresponding header file
fid = openfile('table12_9.h');
fprintf(fid, '#define N_12_9 %d \n ',n12x);
fprintf(fid, '#define M_12_9 %d \n ',4);
fprintf(fid, '#define L_12_9 %d \n ',3);
fprintf(fid, '#define decimno_12_9 %d \n ',3);
fprintf(fid, '#define DELAY_12_9 (%d) \n ', delay_12_9);
fprintf(fid, '#define SHIFT_12_9 (%d) \n' , -shift_12_9);
fprintf(fid, '#define SCALE_12_9 FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_12_9);
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);
fprintf(fid, '#define NZCOEFS_12_9 (%d)\n' , 0);

fclose(fid);

