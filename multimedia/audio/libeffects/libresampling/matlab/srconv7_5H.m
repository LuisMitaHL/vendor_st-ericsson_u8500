coefcalc_7_5H;

if (define_fidelity==HIFI_LOCOEFS)
	[shift mult] = scalefilt(coef_7_5H, M1);
	figure
	freqz((2^shift)*(mult/M1)*coef_7_5H);zoom on;
	title('frequency response after scaling');
	delay_7_5H = n_7_5H / M1;
	%Write the corresponding header file
	fid = openfile('table7_5H.h');
	fprintf(fid, '#define N_7_5H %d \n ',n_7_5H);
	fprintf(fid, '#define M_7_5H %d \n ',M1);
	fprintf(fid, '#define L_7_5H %d \n ',L1);
	fprintf(fid, '#define decimno_7_5H %d \n ',1);
	fprintf(fid, '#define DELAY_7_5H (%d) \n ', delay_7_5H);
	fprintf(fid, '#define SHIFT_7_5H (%d) \n' , -shift);
	fprintf(fid, '#define SCALE_7_5H FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult);
	fprintf(fid, 'extern Float const YMEM resample_7_5H[%d];\n',(nbcoef_7_5H+1));
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_7_5[%d];\n',incr_off_siz(M1,L1));
	fprintf(fid, '#define NZCOEFS_7_5H (%d)\n' , 0);
	fclose(fid);
end


