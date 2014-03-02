function [output]=biq_fix(kn,kd,sig);
NBITS=24;
mult=2^(NBITS-1);
[dummy npts]=size(sig);
for i=1:3
	KN(i)=round(kn(i)*mult);
	KD(i)=round(kd(i)*mult);
end
KN(2)=2*round(KN(2)/2);
KD(2)=2*round(KD(2)/2);
sigin(3:npts+2)=round(sig*mult);
sigin(1:2)=zeros(1,2);
intnode=sigin(3:npts+2)*KN(1)+sigin(2:npts+1)*KN(2)+sigin(1:npts)*KN(3);
Output(1:npts+2)=zeros(1,npts+2);
for iter=1:npts;
	Output(iter+2)=round((-Output(iter+1)*KD(2)-Output(iter)*KD(3)+intnode(iter))/mult);
end
output=Output(3:npts+2)/mult;
	
