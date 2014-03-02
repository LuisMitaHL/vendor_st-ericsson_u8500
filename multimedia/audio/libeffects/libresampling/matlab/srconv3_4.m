status = who('maxDelayFast');
if ( size(status) == 0 )
  maxDelayFast = 0;
end

coefcalc12;

%**************************** Standard Quality ************************
n_3_4=n12x/3;
[shift_3_4 mult_3_4] = scalefilt(deci1_12x, 9);
deci1_3_4=deci1_12x(1:3:n12x);

figure
freqz((2^shift_3_4)*(mult_3_4/3)*deci1_3_4);zoom on; 
title('frequency response after scaling');

% compute max delay
delay_3_4=n12x/9;
totalDelay = delay_3_4;

if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay
end

%Write the corresponding header file
fid=openfile('table3_4.h');
fprintf(fid, '#define N_3_4 %d \n ',n12x);
fprintf(fid, '#define M_3_4 %d \n ',3);
fprintf(fid, '#define L_3_4 %d \n ',4);
fprintf(fid, '#define decimno_3_4 %d \n ',3);
fprintf(fid, '#define DELAY_3_4 (%d) \n ', delay_3_4);
fprintf(fid, '#define SHIFT_3_4 (%d) \n' , -shift_3_4);
fprintf(fid, '#define SCALE_3_4 FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_3_4);
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_3_4[%d];\n',incr_off_siz(3,4));
fprintf(fid, '#define NZCOEFS_3_4 (%d)\n' , 0);
fclose(fid);
if ((define_fidelity==HIFI)|(define_fidelity==HIFI_LOCOEFS))
	n_3_4H=n12xH/3;
	[shift_3_4H mult_3_4H] = scalefilt(deci1_12xH, 9);
	deci1_3_4H=deci1_12xH(1:3:n12xH);

	figure
	freqz((2^shift_3_4H)*(mult_3_4H/3)*deci1_3_4H);zoom on; 
	title('frequency response after scaling');

	% compute max delay
	delay_3_4H=n12xH/9;
	totalDelay = delay_3_4H;

	if( totalDelay > maxDelayFast )
		maxDelayFast = totalDelay
	end

	%Write the corresponding header file
	fid=openfile('table3_4H.h');
	fprintf(fid, '#define N_3_4H %d \n ',n12xH);
	fprintf(fid, '#define M_3_4H %d \n ',3);
	fprintf(fid, '#define L_3_4H %d \n ',4);
	fprintf(fid, '#define decimno_3_4H %d \n ',3);
	fprintf(fid, '#define DELAY_3_4H (%d) \n ', delay_3_4H);
	fprintf(fid, '#define SHIFT_3_4H (%d) \n' , -shift_3_4H);
	fprintf(fid, '#define SCALE_3_4H FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_3_4H);
	fprintf(fid, 'extern Float const YMEM resample_12xH[%d];\n',(nbcoefH)+1);
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_3_4[%d];\n',incr_off_siz(3,4));
	fprintf(fid, '#define NZCOEFS_3_4H (%d)\n' , 0);
	fclose(fid);
end


