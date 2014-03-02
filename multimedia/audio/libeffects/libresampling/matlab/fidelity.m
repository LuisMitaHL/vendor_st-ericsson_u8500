%------------------- 3 possibilities for high fidelity -------
HIFI_OFF=0; %fidelity off
HIFI=1; %fidelity with low mips and high coef count
HIFI_LOCOEFS=2; %fidelity with higher mips and lower coef count







%------------------- current choice for compilation ---------
define_fidelity=HIFI;
%------------------------------------------------------------











fid = openfile('fidelity.h');
if (define_fidelity==HIFI)
	fprintf(fid, '#define hifi\n ');
else
	if (define_fidelity==HIFI_LOCOEFS)
		fprintf(fid, '#define hifi_locoefs\n');
	end
end
fprintf(fid, '\n');
fclose(fid);


