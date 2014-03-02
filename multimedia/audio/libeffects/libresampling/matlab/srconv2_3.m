status = who('maxDelayFast');
if ( size(status) == 0 )
  maxDelayFast = 0;
end

coefcalc12;

%****************************** Standard quality *****************************
n_2_3=n12x/4;
[shift_2_3 mult_2_3] = scalefilt(deci1_12x, 8);
deci1_2_3=deci1_12x(1:4:n12x);

figure
freqz((2^shift_2_3)*(mult_2_3/2)*deci1_2_3);zoom on; 
title('frequency response after scaling');

% compute max delay
delay_2_3=n12x/8;
totalDelay = delay_2_3;

if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay
end

%Write the corresponding header file
fid = openfile('table2_3.h');
fprintf(fid, '#define N_2_3 %d \n ',n12x);
fprintf(fid, '#define M_2_3 %d \n ',2);
fprintf(fid, '#define L_2_3 %d \n ',3);
fprintf(fid, '#define decimno_2_3 %d \n ',4);
fprintf(fid, '#define DELAY_2_3 (%d) \n ', delay_2_3);
fprintf(fid, '#define SHIFT_2_3 (%d) \n' , -shift_2_3);
fprintf(fid, '#define SCALE_2_3 FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_2_3);
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_2_3[%d];\n',incr_off_siz(2,3));
fprintf(fid, '#define NZCOEFS_2_3 (%d)\n' , 0);
fclose(fid);
if ((define_fidelity==HIFI)|(define_fidelity==HIFI_LOCOEFS))
	n_2_3H=n12xH/4;
	[shift_2_3H mult_2_3H] = scalefilt(deci1_12xH, 8);
	deci1_2_3H=deci1_12xH(1:4:n12xH);

	figure
	freqz((2^shift_2_3H)*(mult_2_3H/2)*deci1_2_3H);zoom on; 
	title('frequency response after scaling');

	% compute max delay
	delay_2_3H=n12xH/8;
	totalDelay = delay_2_3H;

	if( totalDelay > maxDelayFast )
		maxDelayFast = totalDelay
	end

	%Write the corresponding header file
	fid = openfile('table2_3H.h');
	fprintf(fid, '#define N_2_3H %d \n ',n12xH);
	fprintf(fid, '#define M_2_3H %d \n ',2);
	fprintf(fid, '#define L_2_3H %d \n ',3);
	fprintf(fid, '#define decimno_2_3H %d \n ',4);
	fprintf(fid, '#define DELAY_2_3H (%d) \n ', delay_2_3H);
	fprintf(fid, '#define SHIFT_2_3H (%d) \n' , -shift_2_3H);
	fprintf(fid, '#define SCALE_2_3H FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_2_3H);
	fprintf(fid, 'extern Float const YMEM resample_12xH[%d];\n',(nbcoefH)+1);
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_2_3[%d];\n',incr_off_siz(2,3));
	fprintf(fid, '#define NZCOEFS_2_3H (%d)\n' , 0);
	fclose(fid);
end



