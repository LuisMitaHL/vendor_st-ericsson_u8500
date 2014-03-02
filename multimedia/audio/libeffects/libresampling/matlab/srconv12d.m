status = who('maxDelay');
if ( size(status) == 0 )
  maxDelay = 0;
end

% design fir filter for 12x resampling
coefcalc12;

%************************** Standard Quality ******************
[shift_12x mult_12x] = scalefilt(deci1_12x,1);
deci1_12x=deci1_12x(1:1:n12x);

figure
freqz((2^shift_12x)*(mult_12x)*deci1_12x);zoom on;
title('frequency response after scaling');

% compute max delay
delay_12x = n12x;
totalDelay = delay_12x;

if( totalDelay > maxDelay )
  maxDelay = totalDelay
end

%Write the corresponding header file
fid = openfile('table12d.h');
fprintf(fid, '#define N_12d %d \n ',n12x);
fprintf(fid, '#define M_12d %d \n ',1);
fprintf(fid, '#define L_12d %d \n ',12);
fprintf(fid, '#define decimno_12d %d \n ',1);
fprintf(fid, '#define DELAY_12d (%d)  \n ', delay_12x);
fprintf(fid, '#define SHIFT_12d %d \n ',-shift_12x);
fprintf(fid, '#define SCALE_12d FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_12x);		
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);		
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_1_12[%d];\n',incr_off_siz(1,12));
fprintf(fid, '#define NZCOEFS_12d (%d) \n' , 0);
fclose(fid);
if ((define_fidelity==HIFI)|(define_fidelity==HIFI_LOCOEFS))
	[shift_12xH mult_12xH] = scalefilt(deci1_12xH,1);
	deci1_12xH=deci1_12xH(1:1:n12xH);

	figure
	freqz((2^shift_12xH)*(mult_12xH)*deci1_12xH);zoom on;
	title('frequency response after scaling');

	% compute max delay
	delay_12xH = n12xH;
	totalDelay = delay_12xH;

	if( totalDelay > maxDelay )
		maxDelay = totalDelay
	end

	%Write the corresponding header file
	fid = openfile('table12dH.h');
	fprintf(fid, '#define N_12dH %d \n ',n12xH);
	fprintf(fid, '#define M_12dH %d \n ',1);
	fprintf(fid, '#define L_12dH %d \n ',12);
	fprintf(fid, '#define decimno_12dH %d \n ',1);
	fprintf(fid, '#define DELAY_12dH (%d)  \n ', delay_12xH);
	fprintf(fid, '#define SHIFT_12dH %d \n ',-shift_12xH);
	fprintf(fid, '#define SCALE_12dH FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_12xH);		
	fprintf(fid, 'extern Float const YMEM resample_12xH[%d];\n',(nbcoefH)+1);		
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_1_12[%d];\n',incr_off_siz(1,12));
	fprintf(fid, '#define NZCOEFS_12dH (%d) \n' , 0);
	fclose(fid);
end
