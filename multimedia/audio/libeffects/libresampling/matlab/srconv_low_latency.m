coefcalc_low_latency;

%***************** 46->16 **************************

%Write the corresponding header file
[shift,scale]=scalefilt_low_latency(1);
fid = openfile('table48_16_low_latency.h');
fprintf(fid, '#define N_48_16_low_latency %d \n ',nbcoef_48_16_low_latency);
fprintf(fid, '#define M_48_16_low_latency %d \n ',1);
fprintf(fid, '#define L_48_16_low_latency %d \n ',3);
fprintf(fid, '#define decimno_48_16_low_latency %d \n ',1);
fprintf(fid, '#define DELAY_48_16_low_latency (%d)  \n ',nbcoef_48_16_low_latency);
fprintf(fid, '#define SHIFT_48_16_low_latency %d \n ',shift);
fprintf(fid, '#define SCALE_48_16_low_latency FORMAT_FLOAT(%.15f,MAXVAL)\n',scale);		
fprintf(fid, 'extern Float const YMEM resample_48_16_low_latency[%d];\n',(nbcoef_48_16_low_latency)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_1_3[%d];\n',incr_off_siz(1,3));
fprintf(fid, '#define NZCOEFS_48_16_low_latency (%d)\n' , 0);

fclose(fid);




%***************** 16->8 **************************

%Write the corresponding header file
[shift,scale]=scalefilt_low_latency(1);
fid = openfile('table16_8_low_latency.h');
fprintf(fid, '#define N_16_8_low_latency %d \n ',nbcoef_16_8_low_latency);
fprintf(fid, '#define M_16_8_low_latency %d \n ',1);
fprintf(fid, '#define L_16_8_low_latency %d \n ',2);
fprintf(fid, '#define decimno_16_8_low_latency %d \n ',1);
fprintf(fid, '#define DELAY_16_8_low_latency (%d)  \n ',nbcoef_16_8_low_latency);
fprintf(fid, '#define SHIFT_16_8_low_latency %d \n ',shift);
fprintf(fid, '#define SCALE_16_8_low_latency FORMAT_FLOAT(%.15f,MAXVAL)\n',scale);		
fprintf(fid, 'extern Float const YMEM resample_16_8_low_latency[%d];\n',(nbcoef_16_8_low_latency)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_1_2[%d];\n',incr_off_siz(1,2));
fprintf(fid, '#define NZCOEFS_16_8_low_latency (%d)\n' , 0);


fclose(fid);




%***************** 8->16 **************************

%Write the corresponding header file
[shift,scale]=scalefilt_low_latency(2);
fid = openfile('table8_16_low_latency.h');
fprintf(fid, '#define N_8_16_low_latency %d \n ',nbcoef_8_16_low_latency);
fprintf(fid, '#define M_8_16_low_latency %d \n ',2);
fprintf(fid, '#define L_8_16_low_latency %d \n ',1);
fprintf(fid, '#define decimno_8_16_low_latency %d \n ',1);
fprintf(fid, '#define DELAY_8_16_low_latency (%d)  \n ',nbcoef_8_16_low_latency/2);
fprintf(fid, '#define SHIFT_8_16_low_latency %d \n ',shift);
fprintf(fid, '#define SCALE_8_16_low_latency FORMAT_FLOAT(%.15f,MAXVAL)\n',scale);		
fprintf(fid, 'extern Float const YMEM resample_8_16_low_latency[%d];\n',(nbcoef_8_16_low_latency)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_2_1[%d];\n',incr_off_siz(2,1));
fprintf(fid, '#define NZCOEFS_8_16_low_latency (%d)\n' , 0);


fclose(fid);




%***************** 16->48 **************************

%Write the corresponding header file
[shift,scale]=scalefilt_low_latency(3);
fid = openfile('table16_48_low_latency.h');
fprintf(fid, '#define N_16_48_low_latency %d \n ',nbcoef_16_48_low_latency);
fprintf(fid, '#define M_16_48_low_latency %d \n ',3);
fprintf(fid, '#define L_16_48_low_latency %d \n ',1);
fprintf(fid, '#define decimno_16_48_low_latency %d \n ',1);
fprintf(fid, '#define DELAY_16_48_low_latency (%d)  \n ',nbcoef_16_48_low_latency/3);
fprintf(fid, '#define SHIFT_16_48_low_latency %d \n ',shift);
fprintf(fid, '#define SCALE_16_48_low_latency FORMAT_FLOAT(%.15f,MAXVAL)\n',scale);		
fprintf(fid, 'extern Float const YMEM resample_16_48_low_latency[%d];\n',(nbcoef_16_48_low_latency)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_3_1[%d];\n',incr_off_siz(3,1));
fprintf(fid, '#define NZCOEFS_16_48_low_latency (%d)\n' , 0);


fclose(fid);







