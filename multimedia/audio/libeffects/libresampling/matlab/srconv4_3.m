status = who('maxDelayFast');
if ( size(status) == 0 )
  maxDelayFast = 0;
end
coefcalc12;

%********************** Standard Quality **********************************
n_4_3=n12x/3;
[shift_4_3 mult_4_3] = scalefilt(deci1_12x, 12);
deci1_4_3=deci1_12x(1:3:n12x);

figure
freqz((2^shift_4_3)*(mult_4_3/4)*deci1_4_3);zoom on; 
title('frequency response after scaling');

% compute max delay
delay_4_3=n12x/12;
totalDelay = delay_4_3;

if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay
end

%Write the corresponding header file
fid = openfile('table4_3.h');
fprintf(fid, '#define N_4_3 %d \n ',n12x);
fprintf(fid, '#define M_4_3 %d \n ',4);
fprintf(fid, '#define L_4_3 %d \n ',3);
fprintf(fid, '#define decimno_4_3 %d \n ',3);
fprintf(fid, '#define DELAY_4_3 (%d) \n ', delay_4_3);
fprintf(fid, '#define SHIFT_4_3 (%d) \n' , -shift_4_3);
fprintf(fid, '#define SCALE_4_3 FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_4_3);
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_4_3[%d];\n',incr_off_siz(4,3));
fprintf(fid, '#define NZCOEFS_4_3 (%d)\n' , 0);

fclose(fid);
if ((define_fidelity==HIFI)|(define_fidelity==HIFI_LOCOEFS))
	n_4_3H=n12xH/3;
	[shift_4_3H mult_4_3H] = scalefilt(deci1_12xH, 12);
	deci1_4_3H=deci1_12xH(1:3:n12xH);

	figure
	freqz((2^shift_4_3H)*(mult_4_3H/4)*deci1_4_3H);zoom on; 
	title('frequency response after scaling');

	% compute max delay
	delay_4_3H=n12xH/12;
	totalDelay = delay_4_3H;

	if( totalDelay > maxDelayFast )
		maxDelayFast = totalDelay
	end

	%Write the corresponding header file
	fid = openfile('table4_3H.h');
	fprintf(fid, '#define N_4_3H %d \n ',n12xH);
	fprintf(fid, '#define M_4_3H %d \n ',4);
	fprintf(fid, '#define L_4_3H %d \n ',3);
	fprintf(fid, '#define decimno_4_3H %d \n ',3);
	fprintf(fid, '#define DELAY_4_3H (%d) \n ', delay_4_3H);
	fprintf(fid, '#define SHIFT_4_3H (%d) \n' , -shift_4_3H);
	fprintf(fid, '#define SCALE_4_3H FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_4_3H);
	fprintf(fid, 'extern Float const YMEM resample_12xH[%d];\n',(nbcoefH)+1);
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_4_3[%d];\n',incr_off_siz(4,3));
	fprintf(fid, '#define NZCOEFS_4_3H (%d)\n' , 0);
	fclose(fid);
end
