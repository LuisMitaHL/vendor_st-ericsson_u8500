clear;
close all;
Fs = 1632000;
Fstop = 26400;
lcd=2;
Rp = .3;
Fpass = 20000;
Rs = 94;


NUMVALS=5;
Rpval=[.3 .1 .05 .02 .01];
iter=2;
%---------------------- Various values of Rp ------------------------------
figure
ncoefs=[ ];
RP=[ ];
	Rp=Rpval(iter);
	[n_34_37,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
	n_34_37 = round(n_34_37/lcd)*lcd;
	deci1_34_37 = remez(n_34_37-1,fo,mo,w);
	[h,freq]=freqz(deci1_34_37,1,4096,Fs); 
	ampdB=20*log10(abs(h)+1e-10);
	semilogx(freq,ampdB);
	ncoefs=[ncoefs n_34_37];
	RP=[RP Rp];
grid on;
zoom on;
hold off;
axis([0 26400 -120 10]);
title('frequency response of src response vs different passband ripple amplitudes');
%xlabel(['various Rp templates: ',num2str(RP),' and corresponding ncoefs: ',num2str(ncoefs)]);
xlabel(['Rp template: ',num2str(RP)]);

figure
ncoefs=[ ];
RP=[ ];
	Rp=Rpval(iter);
	[n_34_37,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
	n_34_37 = round(n_34_37/lcd)*lcd;
	deci1_34_37 = remez(n_34_37-1,fo,mo,w);
	[h,freq]=freqz(deci1_34_37,1,4096,Fs); 
	ampdB=20*log10(abs(h)+1e-10);
	semilogx(freq,ampdB);
	ncoefs=[ncoefs n_34_37];
	RP=[RP Rp];
grid on;
zoom on;
hold off;
axis([0 24000 -8 8]);
title('zoom on frequency response of src response vs different passband ripple amplitudes');
xlabel(['Rp template: ',num2str(RP)]);
Fpass
Fstop
Rp
Rs
ncoefs
