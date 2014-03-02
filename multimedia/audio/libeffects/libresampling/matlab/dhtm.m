%function to return the modified 1-d discrete hilbert transform
%The code uses N pt. Fast Fourier Transforms
%Niranjan Damera-Venkata,
%Brian L. Evans and Shawn R. McCaslin
%% Takes arguements (Magnitude Spectrum,Truncation length)   

function y=dhtm(mag,N,s)



sig(1:(N/2))=sign(linspace(1,(N/2),(N/2)));
sig((N/2)+1)=0;
sig((N/2)+2:N)=sign(linspace(-1,-(N/2)-1,(N/2)-1));
sig(1)=0;


logmag=log(abs(mag));

in=ifft(logmag);

ph=-j*fft(sig'.*in);

rec=mag.*exp(j*ph);

recu=ifft(rec);

y=recu(1:s);