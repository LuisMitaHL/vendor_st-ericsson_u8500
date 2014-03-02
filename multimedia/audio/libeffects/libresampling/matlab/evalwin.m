function wval = evalwin(k,I,alpha);
%wval = evalwin(k,I,alpha);
%k:	window coefficients at integer values
%I:	index of desired tap from 1:size(k)
%alpha: delay between +.5T to -.5T from desired tap for interpolation of window value

NCOFS=7;	
delta=(NCOFS-1)/2;
pord=NCOFS-1;
[ntaps dummy]=size(k);
if (I<=((ntaps+1)/2))
	xmin=max(1,I-delta);
	xmax=xmin+NCOFS-1;
else
	xmax=min(ntaps,I+delta);
	xmin=xmax-NCOFS+1;
end
kval=k(xmin:xmax)';
xval=-delta:delta;
p=polyfit(xval,kval,pord);
x=I-xmin-delta+alpha;
wtemp=polyval(p,x);
wval=[wtemp>0].*wtemp;
