status = who('maxDelay');
if ( size(status) == 0 )
  maxDelay = 0;
end
%*************************** Standard quality ********************
% design fir filter for 12x resampling
coefcalc12;
n4x=n12x/3;
[shift_4x mult_4x] = scalefilt(deci1_12x,12);
deci1_4x=deci1_12x(1:3:n12x);

figure
freqz((2^shift_4x)*(mult_4x/4)*deci1_4x);zoom on;
title('frequency response after scaling');

% compute max delay
delay_4x = n12x/12;
totalDelay = delay_4x;

if( totalDelay > maxDelay )
  maxDelay = totalDelay
end
%Write the corresponding header file
fid = fopen('table11_44a.h','w');
fprintf(fid, '#define N_11_44a %d \n ',n12x);
fprintf(fid, '#define M_11_44a %d \n ',4);
fprintf(fid, '#define L_11_44a %d \n ',1);
fprintf(fid, '#define decimno_11_44a %d \n ',3);
fprintf(fid, '#define DELAY_11_44a (%d)  \n ', delay_4x);
fprintf(fid, '#define SHIFT_11_44a %d \n ',-shift_4x);
fprintf(fid, '#define SCALE_11_44 FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_4x);		
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_4_1[%d];\n',incr_off_siz(4,1));
fprintf(fid, '#define NZCOEFS_11_44a (%d) \n' , 0);
fclose(fid);
if ((define_fidelity==HIFI)|(define_fidelity==HIFI_LOCOEFS))
	% design fir filter for 12x resampling
	n4xH=n12xH/3;
	[shift_4xH mult_4xH] = scalefilt(deci1_12xH,12);
	deci1_4xH=deci1_12xH(1:3:n12xH);

	figure
	freqz((2^shift_4xH)*(mult_4xH/4)*deci1_4xH);zoom on;
	title('frequency response after scaling');

	% compute max delay
	delay_4xH = n12xH/12;
	totalDelay = delay_4xH;

	if( totalDelay > maxDelay )
  		maxDelay = totalDelay
	end
	%Write the corresponding header file
	fid = fopen('table11_44aH.h','w');
	fprintf(fid, '#define N_11_44aH %d \n ',n12xH);
	fprintf(fid, '#define M_11_44aH %d \n ',4);
	fprintf(fid, '#define L_11_44aH %d \n ',1);
	fprintf(fid, '#define decimno_11_44aH %d \n ',3);
	fprintf(fid, '#define DELAY_11_44aH (%d)  \n ', delay_4xH);
	fprintf(fid, '#define SHIFT_11_44aH %d \n ',-shift_4xH);
	fprintf(fid, '#define SCALE_11_44H FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_4xH);		
	fprintf(fid, 'extern Float const YMEM resample_12xH[%d];\n',(nbcoefH)+1);
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_4_1[%d];\n',incr_off_siz(4,1));
	fprintf(fid, '#define NZCOEFS_11_44aH (%d) \n' , 0);
	fclose(fid);
end




