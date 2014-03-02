function [K] = sinxcalc(p,alpha);
%K = sinxcalc(p,alpha) for coef calc. sinx/x variable delay fir
%p: 	array containing polynomial coefs of dimension ((ntaps+1)/2,degree+1) 
%alpha:	desired sample delay from -0.5T to +0.5T
%pp:    intermediate values pp(0:(ntaps-1)/2)=p(0:(ntaps-1)/2)
%       reverse copy p(0:(ntaps-3)/2) to pp(ntaps-1:-1:(ntaps+1)/2)) with negation of odd terms
%K:     fir coef K(j)=sumi(pp(j,i)*alpha^i) where: j=0:ntaps-1; i=0:degree;
%cycles:(ntaps/2)*(degree+3)
[numcof,numiter]=size(p);
ntaps=2*numcof-1;
degree=numiter-1;
xval(1)=1.0;
for i=1:degree
	xval(i+1)=xval(i)*alpha;
end
for j=1:(ntaps-1)/2
	evensum(j)=0;
	for i=0:2:degree-1
		evensum(j)=evensum(j)+p(j,i+1)*xval(i+1);	
	end
	oddsum(j)=0;
	for i=1:2:degree
		oddsum(j)=oddsum(j)+p(j,i+1)*xval(i+1);	
	end
end
K((ntaps+1)/2)=0;
for i=0:degree-1;
	K((ntaps+1)/2)=K((ntaps+1)/2)+p((ntaps+1)/2,i+1)*alpha^i;
end
for j=1:(ntaps-1)/2
	K(j)        =evensum(j)+oddsum(j);
	K(ntaps-j+1)=evensum(j)-oddsum(j);
end 
	