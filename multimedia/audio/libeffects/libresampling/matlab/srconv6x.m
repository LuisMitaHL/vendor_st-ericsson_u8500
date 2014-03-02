status = who('maxDelay');
if ( size(status) == 0 )
  maxDelay = 0;
end

% design fir filter for 12x resampling
coefcalc12;

%********************** Standard Quality ************************
[shift_6x mult_6x] = scalefilt(deci1_12x,12);
deci1_6x=deci1_12x(1:2:n12x);

figure
freqz((2^shift_6x)*(mult_6x/6)*deci1_6x);zoom on;
title('frequency response after scaling');

% compute max delay
delay_6x = n12x/12;
totalDelay = delay_6x;

if( totalDelay > maxDelay )
  maxDelay = totalDelay
end

%Write the corresponding header file
fid = openfile('table6xa.h');
fprintf(fid, '#define N_6xa %d \n ',n12x);
fprintf(fid, '#define M_6xa %d \n ',6);
fprintf(fid, '#define L_6xa %d \n ',1);
fprintf(fid, '#define decimno_6xa %d \n ',2);
fprintf(fid, '#define DELAY_6xa (%d)  \n ', delay_6x);
fprintf(fid, '#define SHIFT_6xa %d \n ',-shift_6x);
fprintf(fid, '#define SCALE_6x FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_6x);		
fprintf(fid, 'extern Float const YMEM resample_12x[%d];\n',(nbcoef)+1);		
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_6_1[%d];\n',incr_off_siz(6,1));
fprintf(fid, '#define NZCOEFS_6xa (%d)\n' , 0);

fclose(fid);
if ((define_fidelity==HIFI)|(define_fidelity==HIFI_LOCOEFS))
	[shift_6xH mult_6xH] = scalefilt(deci1_12xH,12);
	deci1_6xH=deci1_12xH(1:2:n12xH);

	figure
	freqz((2^shift_6xH)*(mult_6xH/6)*deci1_6xH);zoom on;
	title('frequency response after scaling');

	% compute max delay
	delay_6xH = n12xH/12;
	totalDelay = delay_6xH;

	if( totalDelay > maxDelay )
		maxDelay = totalDelay
	end

	%Write the corresponding header file
	fid = openfile('table6xaH.h');
	fprintf(fid, '#define N_6xaH %d \n ',n12xH);
	fprintf(fid, '#define M_6xaH %d \n ',6);
	fprintf(fid, '#define L_6xaH %d \n ',1);
	fprintf(fid, '#define decimno_6xaH %d \n ',2);
	fprintf(fid, '#define DELAY_6xaH (%d)  \n ', delay_6xH);
	fprintf(fid, '#define SHIFT_6xaH %d \n ',-shift_6xH);
	fprintf(fid, '#define SCALE_6xH FORMAT_FLOAT(%.15f,MAXVAL)\n',mult_6xH);		
	fprintf(fid, 'extern Float const YMEM resample_12xH[%d];\n',(nbcoefH)+1);		
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_6_1[%d];\n',incr_off_siz(6,1));
	fprintf(fid, '#define NZCOEFS_6xaH (%d)\n' , 0);
	fclose(fid);
end
