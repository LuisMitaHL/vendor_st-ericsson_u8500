clear
close all
NBITS=23;
maxdB=114;
Q=8;
onedB=2^Q;
%******************************** linear gain calculation ****************
val=-floor(maxdB*2^Q);
stopval=-val;
iter=1;
step=floor(onedB/32);
while (val<stopval)
	%------------ get reference value for testing ----
	dB=val*2^(-Q);
	ref=10^(dB/20);
	%---------- normalize input -----------------
	dBmant=val;
	dBexp=-NBITS+Q;
	if (dBmant>0)
		while (dBmant<2^(NBITS-1))
			dBmant=dBmant*2;
			dBexp=dBexp+1;
		end
	else
		if (dBmant~=0)
			while (dBmant>-2^(NBITS-1))
				dBmant=dBmant*2;
				dBexp=dBexp+1;
			end
		end
	end
	dBmant=round(dBmant);
	%------------- function call -------------------
	[res,mant,exp,alphamant,alphaexp]=dBtolin(dB,dBmant,dBexp,NBITS);
	%------------ test results ------------------
	DB(iter)=dB;
	if (ref==0)
		ref=1e-10;
	end
	PCTERR(iter)=100*(ref-res)/ref;
	RES(iter)=ref;
	res=(mant*2^(-exp))*2^(-NBITS);
	PCTERRbt(iter)=100*(ref-res)/ref;
	val=val+step;
	iter=iter+1;
end

figure
plot(DB,RES);
grid on;
zoom on;
title('dBtolin vs dB');
figure
plot(DB,PCTERR,'b');
hold on;
plot(DB,PCTERRbt,'r');
grid on;
zoom on;
title('%error (float(b),fixed(r)) for dBtolin function vs dB');


%******************************* ALPHA CALCULATION ***************
NTEST=10000;
val=floor(maxdB*2^Q);
fs=44100;
maxTime=30; % seconds

minIter=23;
maxIter=maxTime*fs;
if (maxIter>2^NBITS)
	maxIter=2^NBITS;
end
nbiter=minIter;
step=max((maxIter-minIter)/NTEST);
nn=1;
while (nbiter<maxIter)
	%----------------- reference values for testing -------------
	dB=val*2^(-Q); 
	delta_dbref=dB/nbiter;
	alpharef=10^(dB/(20*nbiter))-1;
	%---------- normalize input -----------------
	dBmant=val;
	dBexp=-NBITS+Q;
	if (dBmant>0)
		while (dBmant<2^(NBITS-1))
			dBmant=dBmant*2;
			dBexp=dBexp+1;
		end
	else
		if (dBmant~=0)
			while (dBmant>-2^(NBITS-1))
				dBmant=dBmant*2;
				dBexp=dBexp+1;
			end
		end
	end
	dBmant=round(dBmant);
	%-------- normalize nbiter -------------------
	nbiterexp=0;
	nbitermant=nbiter;
	while (nbitermant<2^(NBITS-1))
		nbitermant=nbitermant*2;
		nbiterexp=nbiterexp+1;
	end
	%--------- 1/2x --------------
	invnbiter_bt=inverse(nbitermant);
	%--------- delta_dbmant=dBmant/nbiter -------
	delta_dbmant=floor(invnbiter_bt*dBmant*2^(-NBITS));
	delta_dbexp=NBITS+dBexp-nbiterexp-1;
	%---------- calculate alpha ------------------------------------------------------------
	[res,mant,exp,alphamant,alphaexp]=dBtolin(delta_dbref,delta_dbmant,delta_dbexp,NBITS);
	%-------------------- test results ---------------------------
	alpha=(alphamant*2^(-alphaexp))*2^(-NBITS);
	if (alpharef==0)
		err=0;
	else
		err=100*(alpharef-alpha)/alpharef;
	end
	ALPHAREF(nn)=alpharef;
	ERR(nn)=err;
	nbiter=nbiter+step;
	nn=nn+1;
end
figure
plot(ALPHAREF,ERR);
grid on;
zoom on;
[maxerr idx]=max(abs(ERR));
alphamax=ALPHAREF(idx);
title(['max %error= ',num2str(maxerr), ' for alpha= ',num2str(alphamax)]);
xlabel('alpha');
ylabel('percent error of alpha fix vs alpha float');



