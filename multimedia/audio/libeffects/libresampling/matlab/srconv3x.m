status = who('maxDelay');
if ( size(status) == 0 )
  maxDelay = 0;
end

% design fir filter for 12x resampling
coefcalc12;

%************************* Standard Quality **********************
n3x=n12x/4;
[shift_3x mult_3x] = scalefilt(deci1_12x,12);
deci1_3x=deci1_12x(1:4:n12x);

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
fid = openfile('table3xa.h');
fprintf(fid, '#define N_3xa %d \n ',n12x);
fprintf(fid, '#define M_3xa %d \n ',3);
fprintf(fid, '#define L_3xa %d \n ',1);
fprintf(fid, '#define decimno_3xa %d \n ',4);
fprintf(fid, '#define DELAY_3xa (%d)  \n ', delay_3x);
fprintf(fid, '#define SHIFT_3xa %d \n ',-shift_3x);
fprintf(fid, '#define SCALE_3x FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_3x);		
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);		
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_3_1[%d];\n',incr_off_siz(3,1));
fprintf(fid, '#define NZCOEFS_3xa (%d)\n' , 0);

fclose(fid);

if ((define_fidelity==HIFI)|(define_fidelity==HIFI_LOCOEFS))
	n3xH=n12xH/4;
	[shift_3xH mult_3xH] = scalefilt(deci1_12xH,12);
	deci1_3xH=deci1_12xH(1:4:n12xH);

	figure
	freqz((2^shift_3xH)*(mult_3xH/3)*deci1_3xH);zoom on;
	title('frequency response after scaling');

	% compute max delay
	delay_3xH = n12xH/12;
	totalDelay = delay_3xH;

	if( totalDelay > maxDelay )
		maxDelay = totalDelay
	end

	%Write the corresponding header file
	fid = openfile('table3xaH.h');
	fprintf(fid, '#define N_3xaH %d \n ',n12xH);
	fprintf(fid, '#define M_3xaH %d \n ',3);
	fprintf(fid, '#define L_3xaH %d \n ',1);
	fprintf(fid, '#define decimno_3xaH %d \n ',4);
	fprintf(fid, '#define DELAY_3xaH (%d)  \n ', delay_3xH);
	fprintf(fid, '#define SHIFT_3xaH %d \n ',-shift_3xH);
	fprintf(fid, '#define SCALE_3xH FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_3xH);		
	fprintf(fid, 'extern Float const YMEM resample_12xH[%d];\n',(nbcoefH)+1);		
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_3_1[%d];\n',incr_off_siz(3,1));
	fprintf(fid, '#define NZCOEFS_3xaH (%d)\n' , 0);
	fclose(fid);
end
