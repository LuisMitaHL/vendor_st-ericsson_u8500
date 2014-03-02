coefcalc_7_9H;

if (define_fidelity==HIFI_LOCOEFS)
	[shift mult] = scalefilt(coef_7_9H, M2);
	figure
	freqz((2^shift)*(mult/M2)*coef_7_9H);zoom on;
	title('frequency response after scaling');
	delay_7_9H = n_7_9H / M2;
	%Write the corresponding header file
	fid = openfile('table7_9H.h');
	fprintf(fid, '#define N_7_9H %d \n ',n_7_9H);
	fprintf(fid, '#define M_7_9H %d \n ',M2);
	fprintf(fid, '#define L_7_9H %d \n ',L2);
	fprintf(fid, '#define decimno_7_9H %d \n ',1);
	fprintf(fid, '#define DELAY_7_9H (%d) \n ', delay_7_9H);
	fprintf(fid, '#define SHIFT_7_9H (%d) \n' , -shift);
	fprintf(fid, '#define SCALE_7_9H FORMAT_FLOAT(%.15f,MAXVAL) \n' , mult);
	fprintf(fid, 'extern Float const YMEM resample_7_9H[%d];\n',(nbcoef_7_9H+1));
	fprintf(fid, 'extern int const RESAMPLE_MEM src_incr_offset_7_9[%d];\n',incr_off_siz(M2,L2));
	fprintf(fid, '#define NZCOEFS_7_9H (%d)\n' , 0);
	fclose(fid);
end


