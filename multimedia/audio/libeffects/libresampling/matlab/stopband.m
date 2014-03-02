sym_commontab_thresh=.58;

%--- THD = 90 dB MIPS +26% for 24->44kHz. 48->44;44->48 use common  half-sized symmetric table with 1258 coefs------
%Rs=116;
%allow2tables=0;

%--- THD = 84 dB MIPS +5% for 24->44kHz. 48->44;44->48 use 2 different half-sized symmetric tables with 1312 total coefs ------
%Rs=100;
%allow2tables=1;

%--- THD = 79 dB.  48->44;44->48 use common full-sized non-symmetric table with 1258 coefs ------
allow2tables=0;
commontable=1;
Rs=94;
improve_stdTHD=0;
if (improve_stdTHD==1)
	Rs=110;
end


