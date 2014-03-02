function [shift,mult] = scalefilt_low_latency(nbPhases)
%------------------------------------------------------------------
k=nbPhases;
shift=ceil(log2(k));
mult=k/(2^shift);
shift=-shift; %----- since the implemented function is wmsr(shift)
return;
