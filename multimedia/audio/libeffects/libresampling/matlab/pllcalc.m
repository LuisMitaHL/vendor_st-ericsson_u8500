function [k1,k2]=pllcalc(fs,fc,rho)
%------------------------------------------------------------
% function [k1,k2]=pllcalc(fs,fc,rho)
%------------------------------------------------------------
%    fs=sampling frequency
%    fc=bandwidth
%    rho=damping factor normally .707 for critical damping
%    k1=phase coefficient
%    k2=frequency coefficient
%------------------------------------------------------------
T = 1/fs;
wn = 2.*pi*fc;
alfa = (exp(-rho*wn*T))*cos(wn*T*sqrt(1-rho^2));
beta = (exp(-rho*wn*T))*sin(wn*T*sqrt(1-rho^2));
k1= 1.0 - alfa^2 - beta^2;
k2 = (1.0 - alfa)^2 + beta^2;



