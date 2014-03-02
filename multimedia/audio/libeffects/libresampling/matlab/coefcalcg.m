function b=coefcalcg(famp,ampdB,fdel,grpdelay,ncoefs);
%function  b=coefcalcg(famp,ampdB,fdel,grpdelay,ncoefs)
%-------------------------------------------------------------------------------
%Calculates normalized real non-phase-linear FIR coefs b,(ncoefs divisable by 4) 
%template ampdB values for each corner frequency famp 
%template grpdel values in msec for each corner frequency fdel
%famp,fdel are specified from 0 to fs/2 included but 
%need not have the same number of corner frequencies
%--------------------------------------------------------------------------------
NPTS=4096;

[dummy nb_template_pts]=size(famp);
fs=2*famp(nb_template_pts);
lamp=template(famp,ampdB,NPTS/2);
grpdel=template(fdel,grpdelay,NPTS/2);

i=1:NPTS;
freq=fs*((i-1)/NPTS);

%--------------------- calculate absolute amplitude ---
for i=1:NPTS/2
	ampa(i)=10^(lamp(i)/20);
end
ampa=[ampa ampa(NPTS/2)*ones(1,NPTS/2)];
%--------------------- calculate phase ----------------
grpdel=[grpdel grpdel(NPTS/2)*ones(1,NPTS/2)];

phas(1)=0;
for i=1:NPTS-1
	dw=2*pi*(freq(i+1)-freq(i));
	dpho=dw*grpdel(i)/1000;
	phas(i+1)=phas(i)-dpho;
end

%------------------ calculate the complex frequency response ----
for i=1:NPTS
	resp(i)=ampa(i)*cos(phas(i))+(j*ampa(i)*sin(phas(i)));
end

%------------------ inverse fft to get coefficients ----------
K=ifft(resp,NPTS);

%------------------ truncate and center the coefficients ------------------
nb_pts=(ncoefs+2)/2;
%----- make nb_pts an even integer number ---
nb_pts=2*floor(nb_pts/2);

for i=1:(nb_pts/2)
	coef(i+(nb_pts/2)-1)=K(i);
end
k=NPTS-nb_pts/2+2;
for i=1:(nb_pts/2)-1
	coef(i)=K(k);
	k=k+1;
end
%---------------- window the coefficients ---------------------

coef=coef.*hamming(nb_pts-1)';
coef(nb_pts)=0;
%----------------- find hilbert coefs -------------------------
[hili,hilr]=fhil(nb_pts-1);
hilcoef=hilr+j*hili;

%----------------- convolve coefs with hilbert coefs ----------
coef=real(conv(coef,hilcoef));
b=coef;
a=1;
[h f]=freqz(b,a,4096);
maxamp=max(abs(h));
b=b/maxamp;








