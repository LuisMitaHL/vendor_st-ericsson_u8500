function [p] = pfit(ratio,NTAPS,order,mindB);
%p = 		pfit(ratio,NTAPS,order,mindB) for approx. of sinx/x variable delay fir,length NTAPS
%----------------------------------------------------------------------------------------------------
%p: 		array containing polynomial coefs of dimension ((NTAPS+1)/2,degree+1).
%       	p(NTAPS-1:-1:(NTAPS+1)/2) are symmetrically related to  p(0:(NTAPS-3)/2).
%       	even terms p0,p2,...p(degree-1) are equal and odd terms p1,p3,..pdegree are negated.
%ratio: 	-6dB cutoff freq to sampling frequency ratio (fc/fs)
%NTAPS:  	odd number of filter taps to be calculated using p
%order:     order of approximation polynomial   
%mindB:     attenuation floor of chebwin
%-----------------------------------------------------------------------------------------------------
epsilon=1e-10;
LTAPS=(NTAPS-1)/2;
TMAX=LTAPS+0.5;
NUM1=101;
DALPHA = 1./(NUM1 - 1);
iter=1;
i=1:NUM1;
alpha=-.5+DALPHA*(i-1);
k=chebwin(NTAPS,mindB);
for L=-LTAPS:0;
	tnorm=L+alpha;
	[mval indx]=min(abs(tnorm));
	if (mval==0)
		tnorm(indx)=epsilon;
	end

	%blackman window
%	win=(.42-.5*cos(pi+(pi*tnorm/TMAX))+.08*cos(2*(pi+(pi*tnorm/TMAX))));

	%cheby window
	win = evalwin(k,L+LTAPS+1,alpha);
    
    	h=((2.*ratio)*sin(2.*pi*ratio*(tnorm)))./(2.*pi*ratio*(tnorm));
	H=h.*win;
	pval=polyfit(alpha,H,order);
	p(iter,1:order+1)=pval(order+2-(1:order+1));
	iter=iter+1;
end



