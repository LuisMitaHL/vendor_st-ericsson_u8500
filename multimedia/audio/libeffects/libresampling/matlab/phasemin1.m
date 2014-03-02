function [coefs_phase_minimum1] = phasemin1(Fs,Fpass,Rp,Fstop,Rs)
%   [coefs_phase_minimum1] = phasemin1(Fs,Fpass,Rp,Fstop,Rs)
%   ------------------------------------------------------------------------------
%   This function will design a minimum (but not linear) phase 
%   low pass fir using the Remez algorithm and Hilbert Transform.
%   Resulting normalized coefs are in the array coefs_phase_minimum1.
%
%   Fs    =     sampling frequency in Hz
%   Fpass =     pass band maximum frequency in Hz 
%   Rp    =     pass band peak-to-peak ripple in dB
%   Fstop =     stop band minimum frequency in Hz
%   Rs    =     stop band attenuation in dB
%
%   Reference: 
%   Optimal Design of Real and Complex Minimum Phase Digital FIR Filters
%   Niranjan Damera-Venkata and Brian L. Evans 
%   Department of Electrical and Computer Engineering, Engineering Science Building, 
%   The University of Texas at Austin, Austin, TX 78712-1084 bevans@ece.utexas.edu 
%   --------------------------------------------------------------------------------


epsilon=1e-10;
NPTS=1024;
N=2^19;
%------------- design for H^2 -----------------------------------------------------
Rs=Rs*2;
[ncoef,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
ncoef=2*ceil(ncoef/2); %must be even
b = remez(ncoef,fo,mo,w);
%-------------- post processing using Hilbert Transform ---------------------------
H=fft(b,N); 
w1=linspace(0,2*pi,N);
H1=real(H.*exp(j*w1*(ncoef/2)));
clear H;
d1=max(H1)-1;
d2=0-min(H1);
S=4/(sqrt(1+d1+d2)+sqrt(1-d1+d2))^2;
H2=H1+d2;
clear H1;
H3=H2*S;
HR=sqrt(H3)+epsilon;
clear H3;
y=dhtm(HR',N,(ncoef/2)+1);
clear HR;
coefs_phase_minimum1=(real(y))';

