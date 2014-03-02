status = who('maxDelay');
if ( size(status) == 0 )
  maxDelay = 0;
end

% design fir filter for 12x resampling
coefcalc12g;
n3x=n12x/4;
[shift_3x mult_3x] = scalefilt(deci1_12xg,12);
deci1_3x=deci1_12xg(1:4:n12x);

figure
freqz((2^shift_3x)*(mult_3x/3)*deci1_3x);zoom on;
title('frequency response after scaling');

% compute max delay
delay_3x = n12x/12;
totalDelay = delay_3x;

if( totalDelay > maxDelay )
  maxDelay = totalDelay
end



%Write the corresponding header file
fid = openfile('table3xag.h');
fprintf(fid, '#define N_3xag %d \n ',n12x);
fprintf(fid, '#define M_3xag %d \n ',3);
fprintf(fid, '#define L_3xag %d \n ',1);
fprintf(fid, '#define decimno_3xag %d \n ',4);
fprintf(fid, '#define DELAY_3xag (%d)  \n ', delay_3x);
fprintf(fid, '#define SHIFT_3xag %d \n ',-shift_3x);
fprintf(fid, '#define SCALE_3xg FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_3x);		
fprintf(fid, 'extern Float const YMEM resample_12xg[%d];\n',(nbcoef)+1);		
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_12_4[%d];\n',incr_off_siz(3,1));
fprintf(fid, '#define NZCOEFS_3xag (%d)\n' , 0);

fclose(fid);
