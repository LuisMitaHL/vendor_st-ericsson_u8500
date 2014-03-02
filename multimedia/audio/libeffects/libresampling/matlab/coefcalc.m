function b=coefcalc(f,ampdB,ncoefs);
%function b=coefcalc(f,ampdB,ncoefs)
%---------------------------------------------------------------------
%Calculates normalized real phase-linear FIR coefs b,(ncoefs=odd) 
%template ampdB values for each corner frequency f 
%f is specified from 0 to fs/2 included  
%---------------------------------------------------------------------

NPTS=4096;
[hdB] = template(f,ampdB,NPTS);
h=10.^(hdB/20);
fftin(1)=h(1);
%------------------------------ put template in fftin in Hermitien symmetry -----
fftin(2:NPTS)=h(1:NPTS-1);
fftin(NPTS+1)=h(NPTS);
fftin(NPTS+2:2*NPTS)=h(NPTS-1:-1:1);
%------------------------------ calculate fft -----------------------------
fftout=fft(fftin,2*NPTS);
temp=real(fftout);
%- fftout in Hermitien symmetry imag part is 0, k= rearranged real part of fftout ---
ncoefs=2*floor((ncoefs-1)/2)+1; %make ncoefs an odd number
num=ceil(ncoefs/2);
k1=temp(2*NPTS-(num-2):2*NPTS);
k2=temp(1:num);
k=[k1 k2];
%------------------------------ window k -------------------------------------
k=k.*hanning(ncoefs)';
%------------------------------ normalize k -------------------------------------
[h,w]=freqz(k,1,NPTS);
k=k/max(abs(h));
b=k;


