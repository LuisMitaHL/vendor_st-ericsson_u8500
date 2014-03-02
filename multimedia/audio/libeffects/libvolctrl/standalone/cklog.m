clear
close all
NBITS=23;
NTEST=10000;
Q=8;
dBmax=114;
dBmin=-114;
invalmax=10^(dBmax/20);
invalmin=10^(dBmin/20);
incr=(dBmax-dBmin)/(NTEST-1);
incr=(10^(incr/20))-1;
inval=invalmin;

%******************************* FLOATING POINT ***************
i=0;
while (i<NTEST)
	INVAL(i+1)=inval;
	lin=inval;
	ref=20*log10(inval);
	REF(i+1)=ref;
	res=lintodB(lin);
	RES(i+1)=res;
	err=res-ref;
	ERR(i+1)=err;
	inval=inval*(1+incr);
	i=i+1;
end

%------------------ find floating point algorithm error --------------
figure
plot(INVAL,REF);
grid on;
zoom on;
title('20*log10(input) vs input');
figure
plot(INVAL,ERR);
grid on;
zoom on;
title(' floating point 20*log10(input) error vs input');

%*************************** FIXED POINT ****************
i=0;
inval=invalmin;
while (i<NTEST)
	invalmant=inval;
	%--------------- find invalmant_bt and invalexp_bt ----
	invalexp_bt=0;
	while (invalmant<.5)
		invalmant=invalmant*2;
		invalexp_bt=invalexp_bt+1;
	end
	while (invalmant>1+1e-10)
		invalmant=invalmant/2;
		invalexp_bt=invalexp_bt-1;
	end
	invalmant_bt=round(invalmant*2^NBITS);

	%--------- find ref and res_bt ----------------------
	ref=20*log10(invalmant_bt*2^(-NBITS-invalexp_bt));
	REF(i+1)=ref;
	[dBmant,dBexp]=lintodBb(invalmant_bt,invalexp_bt,NBITS);
	res_bt=(dBmant*2^(-NBITS-dBexp));

	%--------- find ERR_bt ------------------------
	RESbt(i+1)=round(res_bt*2^Q); %store dB value in Q8 format
	INVALbt(i+1)=inval;
	err_bt=res_bt-ref;
	ERR_bt(i+1)=err_bt;

	%---------------------------------------
	i=i+1;
	inval=inval*(1+incr);
end
figure
plot(INVALbt,ERR_bt);
grid on;
zoom on;
title('fixed point 20*log10(input) error vs input');

