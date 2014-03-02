function [lin,mant,exp,alphamant,alphaexp]=dBtolin(dB,dBmant,dBexp,NBITS);
%function [lin,mant,exp,alphamant,alphaexp]=dBtolin(dB,dBmant,dBexp,NBITS)
%
%lin=10^dB/20
%alpha=lin-1
%dB(-114<dB<114) and lin are in floating point format
%mant and exp are fixed point QNBITS format
% 
% float     mant      exp
% -------  --------   -----
% .35      0x59999A    1	
% .7       0x59999A    0
% 1.4      0x59999A   -1
i=0;
%***************************Floating point *********************
	%---------------- constants ----------------------
	kn=20*log10(2);
	kd=1/kn;
	ke=log(10)/20;
	ord=9; 
	for i=1:ord
		temp=1;
		for j=i:-1:1
			temp=temp*j;
		end
		ksf(i)=1/temp; %calculate factorial term
	
	end
	%-------------------------------------------------
	exp=floor(dB*kd); %find number of shifts
	mant=dB-exp*kn;   %get mantissa 
	x=mant*ke;	  %e^x = 10^mant
	y=1;
	for j=1:ord
		y=y+ksf(j)*x^j; %e^x using McClauren series
	end
	lin=y*2^exp; %shift result

%************************Fixed Point ******************************
	%------------------------ constants -----------------
	K1=round(4*2^NBITS/(20*log10(2))); %normalizd of 1/6.02dB
	K2=round(((20*log10(2))*2^(2*NBITS))/8); %normalized value of 6.02dB on Q47
	ke=round((8*log(10)/20)*2^NBITS); %10^x=e^(ke*x) ke is normalized
	ord=9;
	for i=1:ord
		temp=1;
		for j=i:-1:1
			temp=temp*j;
		end
		ksi(i)=round((1/temp)*2^(NBITS)); %calculate factorial terms
	end

	%--------------- get input ----------
	inval=dBmant;
	sgn=1;
	if (inval<0)
		sgn=-1;
		inval=-inval;
	end

	%------------------ exp=inval/6.02 dB--------------------
	exp=floor((K1*inval)*2^(-2*(NBITS+1)-dBexp));

	%----- find Q47 remainder mant=inval-exp*6.02dB ----------
	acc=0;
	for i=1:exp
		acc=acc+K2*2^(dBexp+3);
	end
	acc=inval*2^NBITS-acc;

	%--- normalize Q47 mant to Q23 format with n=exponent -------- 
	while ((acc<2^(2*NBITS))&(acc~=0))
		dBexp=dBexp+1;
		acc=acc*2;
	end
	x=round(acc*2^(-NBITS-1));
	x=round((sgn*x*ke)*2^(-NBITS)); %correct mant for power of e

	%----------------- find e^mant -----------------------------------
	acc1=0; 
	temp=2^NBITS;% 1 in Q23
	dBexp=dBexp+2;
	for i=1:ord
		temp=round(temp*x*2^(-NBITS));
		acc1=acc1+((temp*ksi(i))*2^(-i*dBexp));
	end
	acc=acc1+2^(2*NBITS);

	%---------------- final normalization for 10^dB/20 ---------------------
	while ((acc<2^(2*NBITS))&(acc~=0))
		exp=exp+1;
		acc=acc*2;
	end
		mant=round(acc*2^(-NBITS-1));
	exp=-sgn*exp-1;
	%--------------- final normalization for 10^dB/20-1 ---------------------
	alphaexp=-1;
	if (acc1>0)
		while (acc1<2^(2*NBITS))
			alphaexp=alphaexp+1;
			acc1=acc1*2;
		end
	else
		if (acc1~=0)
			while (acc1>-2^(2*NBITS))
				alphaexp=alphaexp+1;
				acc1=acc1*2;
			end
		end
	end
	alphamant=round(acc1*2^(-NBITS-1));

%*********************** End Fixed Point *****************************
