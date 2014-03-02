status = who('maxDelayFast');
if ( size(status) == 0 )
  maxDelayFast = 0;
end

coefcalc44_48LR;



%--------- find shift and multiplier to scale coefficient table values --------------------------
%    this operation replaces scalefilt which doesn't check for corrected coef values <= 1 
%
	epsilon=1e-6;
	targetgain=37; %take care of zero insertion
	actualgain=sum(Coefs_44_48LR); %actual coefficient table gain
	k=targetgain/actualgain; % find linear gain correction for actual gain to obtain targetgain
	shift=ceil(log2(k)); % decompose to shift and multipler
	Mult=k/(2^shift);
	Maxval=(Mult*2^shift)*max(abs(Coefs_44_48LR)); % find max value of corrected coefficients
	if (Maxval>1)
		mult=Mult/(Maxval+epsilon); % limit max to 1.0
	end
	correction_error_in_percent=100*((mult-Mult)/Mult);
%---------------------------------------------------------
shift_44_48LR=shift; 
mult_44_48LR=mult;
%[shift_44_48LR mult_44_48LR] = scalefilt(Coefs_44_48LR, 37);

figure
freqz((2^shift_44_48LR)*(mult_44_48LR/37)*Coefs_44_48LR);zoom on; 
title('frequency response after scaling');

% compute max delay
delay_44_48LR = ceil(nupx / 37);
delta=(delay_44_48LR*37)-nupx;

totalDelay = delay_44_48LR;

if( totalDelay > maxDelayFast )
  maxDelayFast = totalDelay
end

%Write the corresponding header file
fid = openfile('table44_48LR.h');
fprintf(fid, '#define N_44_48LR %d\n',nupx);
fprintf(fid, '#define M_44_48LR %d\n',37);
fprintf(fid, '#define L_44_48LR %d\n',34);
fprintf(fid, '#define decimno_44_48LR %d\n',1);
fprintf(fid, '#define DELAY_44_48LR (%d)\n', delay_44_48LR);
fprintf(fid, '#define SHIFT_44_48LR (%d)\n' , -shift_44_48LR);
fprintf(fid, '#define SCALE_44_48LR FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult_44_48LR);
fprintf(fid, '#define common_table\n');
fprintf(fid, 'extern Float const YMEM resample_44_48LR[%d];\n',(nbcoef_44_48LR+1));
fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_37_34[%d];\n',incr_off_siz(37,34));
fprintf(fid, '#define NZCOEFS_44_48LR (%d)\n' , nzcoefs);
fclose(fid);

