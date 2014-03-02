status = who('maxDelay');
if ( size(status) == 0 )
  maxDelay = 0;
end

% design fir filter for 12x resampling
coefcalc12;

%****************************** Standard Quality ******************
n2x=n12x/6;
[shift_2x mult_2x] = scalefilt(deci1_12x,6);
deci1_2x=deci1_12x(1:6:n12x);

figure
freqz((2^shift_2x)*(mult_2x)*deci1_2x);zoom on;
title('frequency response after scaling');

% compute max delay
delay_2x = n12x/6;
totalDelay = delay_2x;

if( totalDelay > maxDelay )
  maxDelay = totalDelay
end

%Write the corresponding header file
fid = openfile('table2d.h');
fprintf(fid, '#define N_2d %d \n ',n12x);
fprintf(fid, '#define M_2d %d \n ',1);
fprintf(fid, '#define L_2d %d \n ',2);
fprintf(fid, '#define decimno_2d %d \n ',6);
fprintf(fid, '#define DELAY_2d (%d)  \n ', delay_2x);
fprintf(fid, '#define SHIFT_2d %d \n ',-shift_2x);
fprintf(fid, '#define SCALE_2d FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_2x);		
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);		
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_1_2[%d];\n',incr_off_siz(1,2));
fprintf(fid, '#define NZCOEFS_2d (%d)\n' , 0);
fclose(fid);

if ((define_fidelity==HIFI)|(define_fidelity==HIFI_LOCOEFS))
	n2xH=n12xH/6;
	[shift_2xH mult_2xH] = scalefilt(deci1_12xH,6);
	deci1_2xH=deci1_12xH(1:6:n12xH);

	figure
	freqz((2^shift_2xH)*(mult_2xH)*deci1_2xH);zoom on;
	title('frequency response after scaling');

	% compute max delay
	delay_2xH = n12xH/6;
	totalDelay = delay_2xH;

	if( totalDelay > maxDelay )
		maxDelay = totalDelay
	end

	%Write the corresponding header file
	fid = openfile('table2dH.h');
	fprintf(fid, '#define N_2dH %d \n ',n12xH);
	fprintf(fid, '#define M_2dH %d \n ',1);
	fprintf(fid, '#define L_2dH %d \n ',2);
	fprintf(fid, '#define decimno_2dH %d \n ',6);
	fprintf(fid, '#define DELAY_2dH (%d)  \n ', delay_2xH);
	fprintf(fid, '#define SHIFT_2dH %d \n ',-shift_2xH);
	fprintf(fid, '#define SCALE_2dH FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_2xH);		
	fprintf(fid, 'extern Float const YMEM resample_12xH[%d];\n',(nbcoefH)+1);		
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_1_2[%d];\n',incr_off_siz(1,2));
	fprintf(fid, '#define NZCOEFS_2dH (%d)\n' , 0);
	fclose(fid);
end
