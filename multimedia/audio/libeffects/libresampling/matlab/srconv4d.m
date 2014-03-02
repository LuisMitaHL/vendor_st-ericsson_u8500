status = who('maxDelay');
if ( size(status) == 0 )
  maxDelay = 0;
end

% design fir filter for 12x resampling
coefcalc12;

%********************** Standard Quality ********************************
n4x=n12x/3;
[shift_4x mult_4x] = scalefilt(deci1_12x,3);
deci1_4x=deci1_12x(1:3:n12x);

figure
freqz((2^shift_4x)*(mult_4x)*deci1_4x);zoom on;
title('frequency response after scaling');

% compute max delay
delay_4x = n12x/3;
totalDelay = delay_4x;

if( totalDelay > maxDelay )
  maxDelay = totalDelay
end

%Write the corresponding header file
fid = openfile('table4d.h');
fprintf(fid, '#define N_4d %d \n ',n12x);
fprintf(fid, '#define M_4d %d \n ',1);
fprintf(fid, '#define L_4d %d \n ',4);
fprintf(fid, '#define decimno_4d %d \n ',3);
fprintf(fid, '#define DELAY_4d (%d)  \n ', delay_4x);
fprintf(fid, '#define SHIFT_4d %d \n ',-shift_4x);
fprintf(fid, '#define SCALE_4d FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_4x);		
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_1_4[%d];\n',incr_off_siz(1,4));
fprintf(fid, '#define NZCOEFS_4d (%d)\n' , 0);

fclose(fid);

if ((define_fidelity==HIFI)|(define_fidelity==HIFI_LOCOEFS))
	n4xH=n12xH/3;
	[shift_4xH mult_4xH] = scalefilt(deci1_12xH,3);
	deci1_4xH=deci1_12xH(1:3:n12xH);

	figure
	freqz((2^shift_4xH)*(mult_4xH)*deci1_4xH);zoom on;
	title('frequency response after scaling');

	% compute max delay
	delay_4xH = n12xH/3;
	totalDelay = delay_4xH;

	if( totalDelay > maxDelay )
		maxDelay = totalDelay
	end

	%Write the corresponding header file
	fid = openfile('table4dH.h');
	fprintf(fid, '#define N_4dH %d \n ',n12xH);
	fprintf(fid, '#define M_4dH %d \n ',1);
	fprintf(fid, '#define L_4dH %d \n ',4);
	fprintf(fid, '#define decimno_4dH %d \n ',3);
	fprintf(fid, '#define DELAY_4dH (%d)  \n ', delay_4xH);
	fprintf(fid, '#define SHIFT_4dH %d \n ',-shift_4xH);
	fprintf(fid, '#define SCALE_4dH FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_4xH);		
	fprintf(fid, 'extern Float const YMEM resample_12xH[%d];\n',(nbcoefH)+1);
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_1_4[%d];\n',incr_off_siz(1,4));
	fprintf(fid, '#define NZCOEFS_4dH (%d)\n' , 0);
	fclose(fid);
end



