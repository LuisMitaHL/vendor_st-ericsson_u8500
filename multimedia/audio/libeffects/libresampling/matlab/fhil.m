% [B,A] = fhil(N) where B is the set of N odd coefficents
% which can be used as a fir on a real input signal to calculate
% its imaginary output and A is the set of N odd coefficients
% which can be used as a fir on a real input signal to calculate
% its real output

function [B,A] = fhil(N)
C=(N-1)/2;
K=1:2:(C-1);
H(C-K+1)=-2./(K*pi);
H(C+K+1)=2./(K*pi);
H(N)=-H(1);
KH=hamming(N);
B=H.*KH';
for I=1:N
	A(I)=0;
end
A((N+1)/2)=1;


