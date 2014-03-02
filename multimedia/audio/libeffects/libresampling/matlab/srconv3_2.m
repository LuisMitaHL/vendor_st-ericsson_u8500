status = who('maxDelayFast');
if ( size(status) == 0 )
  maxDelayFast = 0;
end

coefcalc12;

%****************************** Standard Quality ************************
n_3_2=n12x/4;
[shift_3_2 mult_3_2] = scalefilt(deci1_12x, 12);
deci1_3_2=deci1_12x(1:4:n12x);

figure
freqz((2^shift_3_2)*(mult_3_2/3)*deci1_3_2);zoom on; 
title('frequency response after scaling');

% compute max delay
delay_3_2=n12x/12;
totalDelay = delay_3_2;

if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay
end

%Write the corresponding header file
fid = openfile('table3_2.h');
fprintf(fid, '#define N_3_2 %d \n ',n12x);
fprintf(fid, '#define M_3_2 %d \n ',3);
fprintf(fid, '#define L_3_2 %d \n ',2);
fprintf(fid, '#define decimno_3_2 %d \n ',4);
fprintf(fid, '#define DELAY_3_2 (%d) \n ', delay_3_2);
fprintf(fid, '#define SHIFT_3_2 (%d) \n' , -shift_3_2);
fprintf(fid, '#define SCALE_3_2 FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_3_2);
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_3_2[%d];\n',incr_off_siz(3,2));
fprintf(fid, '#define NZCOEFS_3_2 (%d)\n' , 0);

fclose(fid);
if ((define_fidelity==HIFI)|(define_fidelity==HIFI_LOCOEFS))
	n_3_2H=n12xH/4;
	[shift_3_2H mult_3_2H] = scalefilt(deci1_12xH, 12);
	deci1_3_2H=deci1_12xH(1:4:n12xH);

	figure
	freqz((2^shift_3_2H)*(mult_3_2H/3)*deci1_3_2H);zoom on; 
	title('frequency response after scaling');

	% compute max delay
	delay_3_2H=n12xH/12;
	totalDelay = delay_3_2H;

	if( totalDelay > maxDelayFast )
		maxDelayFast = totalDelay
	end
	%Write the corresponding header file
	fid = openfile('table3_2H.h');
	fprintf(fid, '#define N_3_2H %d \n ',n12xH);
	fprintf(fid, '#define M_3_2H %d \n ',3);
	fprintf(fid, '#define L_3_2H %d \n ',2);
	fprintf(fid, '#define decimno_3_2H %d \n ',4);
	fprintf(fid, '#define DELAY_3_2H (%d) \n ', delay_3_2H);
	fprintf(fid, '#define SHIFT_3_2H (%d) \n' , -shift_3_2H);
	fprintf(fid, '#define SCALE_3_2H FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_3_2H);
	fprintf(fid, 'extern Float const YMEM resample_12xH[%d];\n',(nbcoefH)+1);
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_3_2[%d];\n',incr_off_siz(3,2));
	fprintf(fid, '#define NZCOEFS_3_2H (%d)\n' , 0);
	fclose(fid);
end

