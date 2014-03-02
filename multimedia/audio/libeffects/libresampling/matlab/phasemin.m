function [coefs_phase_minimum] = phasemin(Fs,Fpass,Rp,Fstop,Rs)
%   [coefs_phase_minimum] = phasemin(Fs,Fpass,Rp,Fstop,Rs)
%   -----------------------------------------------------------------
%   This function will design a minimum (but not linear) phase 
%   low pass fir using the Remez algorithm and zero inversion.
%   Resulting normalized coefs are in the array coefs_phase_minimum.
%
%   Fs    =     sampling frequency in Hz
%   Fpass =     pass band maximum frequency in Hz 
%   Rp    =     pass band peak-to-peak ripple in dB
%   Fstop =     stop band minimum frequency in Hz
%   Rs    =     stop band attenuation in dB
%   -----------------------------------------------------------------
fftsiz=16384;
[ncoef,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
coefs_phaselinear = remez(ncoef,fo,mo,w);
%-------------------- Invert the zeros outside the unit circle ---------
Zeros=roots(coefs_phaselinear);
Mags=abs(Zeros);
[numZ,toto]=size(Mags);
for i=1:numZ
	if (Mags(i)>1)
		Zeros1(i)=1/Zeros(i);
	else
		Zeros1(i)=Zeros(i);
	end
end
%------------------- find the resulting polynomial --------------------
imp_resp=[1, -Zeros1(1)];
for i=2:numZ
	imp_resp=conv(imp_resp,[1, -Zeros1(i)]);
end
%-------------------- remove residual imag part (close to 0) ----
imp_resp=real(imp_resp);
%----------------- normalize the coefs -----------------------
[dummy numpts]=size(imp_resp);
respi=[imp_resp zeros(1,fftsiz-numpts)];
[maxr locr]=max(respi);
respi1=[respi(locr:fftsiz) respi(1:locr-1)];
respf=fft(respi1,fftsiz);
mxcf=max(abs(respf));
coefs_phase_minimum=imp_resp/mxcf;

