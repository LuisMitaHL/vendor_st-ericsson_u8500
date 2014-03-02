coefcalc_7_5H;

if (define_fidelity==HIFI_LOCOEFS)
	[shift mult] = scalefilt(coef_7_5H, L1);
	figure
	freqz((2^shift)*(mult/L1)*coef_7_5H);zoom on;
	title('frequency response after scaling');
	delay_5_7H = n_7_5H / L1;
	%Write the corresponding header file
	fid = openfile('table5_7H.h');
	fprintf(fid, '#define N_5_7H %d \n ',n_7_5H);
	fprintf(fid, '#define M_5_7H %d \n ',L1);
	fprintf(fid, '#define L_5_7H %d \n ',M1);
	fprintf(fid, '#define decimno_5_7H %d \n ',1);
	fprintf(fid, '#define DELAY_5_7H (%d) \n ', delay_5_7H);
	fprintf(fid, '#define SHIFT_5_7H (%d) \n' , -shift);
	fprintf(fid, '#define SCALE_5_7H FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult);
	fprintf(fid, 'extern Float const YMEM resample_7_5H[%d];\n',(nbcoef_7_5H+1));
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_5_7[%d];\n',incr_off_siz(L1,M1));
	fprintf(fid, '#define NZCOEFS_5_7H (%d)\n' , 0);

	fclose(fid);
end


