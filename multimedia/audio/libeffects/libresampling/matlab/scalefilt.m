function [shift,mult] = scalefilt(vector, nbPhases)
%[shift,mult] = scalefilt(vector, nbPhases)
%-----------------------------------------------------------------
%Scaling factors to correct filter output for unity gain.
%Takes into accout storage of coefs for max dynamic range.
%
%shift   =    exponent to be applied to filter output
%mult    =    fractional multiplier to be applied to filter output
%vector  =    generic coef table with maxval between .5 and 1
%nbPhases=    desired interpolation rate using zero insertion
%------------------------------------------------------------------
k=2^round(log2(1/sum(vector)));
% correct for decimation of coef table 
% correct for upsampling addition of zeroes
k=k*nbPhases;
shift=ceil(log2(k));
mult=k/(2^shift);



return;
