function [dBmant,dBexp]=lintodBb(linmant,linexp,NBITS)
%function [dBmant,dBexp]=lintodBb(linmant,linexp,NBITS);

%----------------- defines and constants -----------------------------
NUMITER=NBITS;
for (i=1:NUMITER)
	coef_bt(i)=round(1.0*(2^NBITS)/i);
end
K=round((20/(1.0*16*log(10)))*2^NBITS);
val6dB=20*log10(2);
val6dBmant=round(val6dB*2^(2*NBITS+1-3)); %Q47
invalmant_bt=linmant;
invalexp_bt=linexp;

%-------------- finite series ---------------------------------
arg_bt=invalmant_bt-2^NBITS;
exp=0;
if (arg_bt~=0)
	while(arg_bt>-2^(NBITS-1)) %normalize arg_bt
		exp=exp+1;
		arg_bt=2*arg_bt;
	end
end

%---------------- x-x^2/2+x^3/3+... ---------------------------
logmant=-2^NBITS;
acc=0;
for (n=1:NUMITER)
	logmant=round(-logmant*arg_bt*2^(-NBITS));
	acc=acc+(logmant*coef_bt(n))*2^(2-n*exp);
end

%------------- normalize and multiply results by 20/log(10) ----
logexp=0;
if (acc~=0)
	while (acc>(-2^(2*NBITS)))
		acc=acc*2;
		logexp=logexp+1;
	end
	while (acc<(-2^(2*NBITS+1)))
		acc=round(acc/2);
		logexp=logexp-1;
	end
end 
logmant=round(acc*2^(-NBITS-1));
acc=logmant*K;
if (acc~=0)
	while (acc>(-2^(2*NBITS)))
		acc=acc*2;
		logexp=logexp+1;
	end
end
acc=round(acc*2^(-NBITS+1));

%------------------ add or subtract n*6dB ---------------- 
acc1=0; 
if (invalexp_bt>0)
	for j=1:invalexp_bt
		acc1=acc1-val6dBmant;
	end
else
	for j=-1:-1:invalexp_bt
		acc1=acc1+val6dBmant;
	end
end
acc1=round(acc1*(2^(-NBITS+logexp)));	
acc=acc+acc1;

%---------------- final normalization ----------------------------
neg=1;
if (acc<0)
	neg=-1;
	acc=-acc;
end
if (acc~=0)
	while (acc>2^(2*NBITS+1))
		acc=round(acc/2);
		logexp=logexp-1;
	end
	while (acc<2^(2*NBITS))
		acc=acc*2;
		logexp=logexp+1;
	end
end
acc=neg*acc;
dBmant=round(acc*2^(-NBITS-1));
dBexp=logexp-NBITS-3;
