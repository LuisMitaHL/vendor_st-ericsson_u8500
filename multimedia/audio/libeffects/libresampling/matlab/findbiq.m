function [KN,KD]=findbiq(b,a);
%function [KN,KD]=findbiq(b,a);
%b and a are numerator and denominator iir coefs and KN and KD are the biquad coefficients
NPTS=4096;
SIGSIZ=256;
%----------------------- find biquad coefs -------------------------
Zeros=roots(b);
poles=roots(a);
[numzeros dummy]=size(Zeros);
numbiq=numzeros/2;
for i=1:numbiq
	kkn(i,1:3)=conv([1 -Zeros(2*i-1)],[1 -Zeros(2*i)]);
	kkd(i,1:3)=conv([1 -poles(2*i-1)],[1 -poles(2*i)]);
end
%---- put denominator coefs in ascending order of radii -----------
for i=1:numbiq
	rd(i)=sqrt(kkd(i,3));
end
[dummy order]=sort(rd);
for i=1:numbiq
	kd(i,:)=kkd(order(i),:);
end
%------------- check all numerator sequences for best precision ----------------
seq=permorder(numbiq);
[numcas dummy]=size(seq);
rand('seed',0);
sigin=2*rand(1,SIGSIZ)-1;
minerr=1e12;
for (iter=1:numcas)
	seqi=seq(iter,:);
	for i=1:numbiq
		kn(i,:)=kkn(seqi(i),:);
	end
	%--------- normalize the gain to 1.0 -------------
	hf=ones(1,NPTS);
	for i=1:numbiq
		h=(abs(freqz(kn(i,:),kd(i,:),NPTS)))';
		hf=h.*hf;
		maxval=max(hf);
		hf=hf/maxval;
		kn(i,:)=kn(i,:)/maxval;
	end
%	num=1;
%	den=1;
%	for i=1:numbiq
%		num=conv(num,kn(i,:));
%		den=conv(den,kd(i,:));
%	end
%	absval=max(abs(freqz(num,den,NPTS)));
%	kn(1,:)=kn(1,:)/absval;
	%---------- filter the signal in fixed and floating point ---------------
	sig=sigin;
	sig1=sigin;
	for i=1:numbiq
		sig=filter(kn(i,:),kd(i,:),sig);
		sig1=biq_fix(kn(i,:),kd(i,:),sig1);
	end
	err=max(abs(sig-sig1));
	%-------------- minimize the fixed point error -------------------------
	if (err<minerr)
		minerr=err;
		for i=1:numbiq
			KN(i,:)=kn(i,:);
			KD(i,:)=kd(i,:);
		end
	end
end
