status = who('maxDelayFast');
if ( size(status) == 0 )
  maxDelayFast = 0;
end

coefcalc12;

[shift_9_12 mult_9_12] = scalefilt(deci1_12x, 9);

figure
freqz(2^shift_9_12*mult_9_12*deci1_12x/9);zoom on; 
title('frequency response after scaling');

% compute max delay
delay_9_12=n12x/9;
totalDelay = delay_9_12;

if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay
end


%Write the corresponding header file
fid = openfile('table9_12.h');
fprintf(fid, '#define N_9_12 %d \n ',n12x);
fprintf(fid, '#define M_9_12 %d \n ',3);
fprintf(fid, '#define L_9_12 %d \n ',4);
fprintf(fid, '#define decimno_9_12 %d \n ',3);
fprintf(fid, '#define DELAY_9_12 (%d) \n ', delay_9_12);
fprintf(fid, '#define SHIFT_9_12 (%d) \n' , -shift_9_12);
fprintf(fid, '#define SCALE_9_12 FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_9_12);
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);
fprintf(fid, '#define NZCOEFS_9_12 (%d)\n' , 0);


fclose(fid);

