fidelity;
fid = openfile('resample_table_incr_offset.c');
%fprintf(fid,'#include "resample.h"\n');
fprintf(fid,'#include "resample_local.h"\n\n');

%------------------------------- All use cases including low latency -------------------------
if (define_fidelity==HIFI_LOCOEFS)
	M_LIST=[34 37 37 6 4 3 2 3 4 6  1  1  1  1  1  2  3  5 7 5 7 9];
	L_LIST=[37 34 17 1 1 1 1 2 3 5 12  6  4  3  2  3  4  6 5 7 9 7];
else
	M_LIST=[34 37 37 6 4 3 2 3 4 6  1  1  1  1  1  2  3  5 ];
	L_LIST=[37 34 17 1 1 1 1 2 3 5 12  6  4  3  2  3  4  6 ];
end
%------------------------------- low latency use case ---------------------------------------
%M_LIST=[3  1  2  1];
%L_LIST=[1  3  1  2];

[dummy NUMCAS]=size(M_LIST);
totalcoef=0;
for iter=1:NUMCAS
	M=M_LIST(iter);
	L=L_LIST(iter);
	[src_incr_offset_tab]=get_src_incr_offset_tab(M,L);
	[dummy,Nbcoef]=size(src_incr_offset_tab);
	if (((M~=3)|(L~=1))&((M~=1)|(L~=3))) %check for M4 config
			fprintf(fid, '#ifndef M4_LL_MSEC\n');
	end
	fprintf(fid,'int const RESAMPLE_MEM src_incr_offset_%d_%d[%d] = {\n',M,L,Nbcoef);

	for k = 1:Nbcoef-1
    	fprintf(fid,'%d,\n',src_incr_offset_tab(k));
	end
	k = Nbcoef;
	fprintf(fid,'%d\n',src_incr_offset_tab(k));
	fprintf(fid,'};\n');
	if (((M~=3)|(L~=1))&((M~=1)|(L~=3))) %check for M4 config
		fprintf(fid,'#endif\n');
	end
	incr_off_siz(M,L)=Nbcoef;

	totalcoef=totalcoef+Nbcoef;
end
res=['total number of coefficients needed for all use cases is: ',num2str(totalcoef)];
fprintf(1,'%s\n',res);
fclose(fid);

