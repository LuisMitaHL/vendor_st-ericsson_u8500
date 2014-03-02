coefcalc_7_9H;

if (define_fidelity==HIFI_LOCOEFS)
	[shift mult] = scalefilt(coef_7_9H, L2);
	figure
	freqz((2^shift)*(mult/L2)*coef_7_9H);zoom on;
	title('frequency response after scaling');
	delay_9_7H = n_7_9H / L2;
	%Write the corresponding header file
	fid = openfile('table9_7H.h');
	fprintf(fid, '#define N_9_7H %d \n ',n_7_9H);
	fprintf(fid, '#define M_9_7H %d \n ',L2);
	fprintf(fid, '#define L_9_7H %d \n ',M2);
	fprintf(fid, '#define decimno_9_7H %d \n ',1);
	fprintf(fid, '#define DELAY_9_7H (%d) \n ', delay_9_7H);
	fprintf(fid, '#define SHIFT_9_7H (%d) \n' , -shift);
	fprintf(fid, '#define SCALE_9_7H FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult);
	fprintf(fid, 'extern Float const YMEM resample_7_9H[%d];\n',(nbcoef_7_9H+1));
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_9_7[%d];\n',incr_off_siz(L2,M2));
	fprintf(fid, '#define NZCOEFS_9_7H (%d)\n' , 0);
	fclose(fid);
end


