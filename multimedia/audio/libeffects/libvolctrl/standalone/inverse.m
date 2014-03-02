function [y_bt]=inverse(den_bt)
%function [y_bt]=inverse(den_bt)
%y=1/2*den
tabsiz=4;
numiter=8;
NBITS=23;
MAXVAL=2^NBITS;
%---------- floating point ------------------
for i=1:tabsiz
	dd=.5+((i-1)/(2*tabsiz));
	tabval(i)=1/(2*dd);
end
den=den_bt/MAXVAL;
tabindex=floor(tabsiz*((den-.5)/.5));
y=tabval(tabindex+1);
for i=0:numiter-1
	temp=y;
	temp=temp*y;
	temp=temp*den;
	temp=y-temp;
	y=temp*2;
end
%---------- fixed point ------------------
for i=1:tabsiz
	dd=.5+((i-1)/(2*tabsiz));
	tabvali(i)=round(MAXVAL/(2*dd));
end
temp=den_bt-2^(NBITS-1);
tabindex=floor(temp*2^(-(NBITS-3)));
y_bt=tabvali(tabindex+1);
for i=0:numiter-1
	temp=y_bt;
	temp=round((temp*y_bt)*2^(-NBITS));
	temp=round((temp*den_bt)*2^(-NBITS));
	temp=y_bt-temp;
	y_bt=temp*2;
end

