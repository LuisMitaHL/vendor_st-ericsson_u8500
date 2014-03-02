clear
close all

choice_comment=[
'1:  2nd order iir notch at 1000Hz                                                    ';
'2:  10th order iir lpf at fc=1100                                                    ';
'3:  Remez filter design lpf at fc=2100                                               ';
'4:  Arbitrary xfer function using fft for lpf fir filter design                      ';
'5:  Arbitrary xfer function using fft for bpf fir filter design                      ';
'6:  2nd order iir allpass 1000Hz                                                     ';
'7:  Arbitrary xfer function using fft for amplitude and group delay fir filter design';
'8:  low latency nonlinear phase filter design                                        ';
'9:  sinx/x fir filter design                                                         ';
'10: Actual Remez up/down samplerate 44->48 conversion filter design                  ';
'11: 8th order iir lpf samplerate 44->48                                              ';
'12: 1 msec group delay test on music signals                                         ';
'13: nonlinear low latency up/down samplerate 44->48 conversion filter design         ';
'14: 2stage stage 1 Remez filter design lpf                                           ';
'15: 2stage stage 2 Remez filter design lpf                                           ';
'16: New S Remez up/down samplerate 44->48 conversion filter design                   ';
'17: New S Remez up/down samplerate 48->44 conversion filter design                   ';
'18: Actual Remez up/down samplerate 12x conversion filter design                     ';

];
[numchoice dummy]=size(choice_comment);
[dummy choicelen]=size(choice_comment(1,:));
choice_comment
choice=input(['enter choice from 1 to ',num2str(numchoice),': ']);
if (choice>numchoice)
	'bad choice try again'
	break;
end
comment=choice_comment(choice,5:choicelen)

THDTARGET=.1;
Semilogx=0;
fs=44100; %default value unless specified
M=1;
L=1;
axislabels=[0 fs/2 0 7]; %default axis for group delay display in msec
MAXCOEFS=2000;
NPTS=4096;
usesos=0;
system('../../tools/bin/Linux/functgen sig.pcm');
system('../../tools/bin/Linux/functgen sigout.pcm');
%------------------ 2nd order iir notch at 1000Hz -----
if (choice==1)
	fc=1000;
	theta=2*pi*fc/fs;
	Rz=1;
	Rp=.99;
	b=[1 -2*Rz*cos(theta) Rz^2];
	a=[1 -2*Rp*cos(theta) Rp^2];
	[h f]=freqz(b,a,NPTS);
	maxamp=max(abs(h));
	b=b/maxamp;
	system('../../tools/bin/Linux/functgen --fc=1000 --fstop=2000  --numfreq=2 sig.pcm');
	axislabels=[0 fs/2 0 2.5];%in msec
end
%------------------ 10th order iir lpf at fc=1100 -----
if (choice==2)
	N=8;
	if ((N/2)*3>MAXCOEFS)
		'Too Many Coefficients'
		return;
	end
	Rp=.3;
	Rs=100;
	fc=1100;
	wn=fc/(fs/2);
	[b,a] = ellip(N,Rp,Rs,wn);
	system('../../tools/bin/Linux/functgen --fc=1100 --fstop=1800  --numfreq=2 sig.pcm');
	axislabels=[0 fs/2 0 5];%in msec
end
%----------------- Remez filter design lpf at fc=2100----------------
if (choice==3)
	Fpass=2100;
	Fstop=2900;
	Rp=.3;
	Rs=100;
	[nupx,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], fs);
	nupx=2*floor(nupx/2);
	if (nupx>MAXCOEFS)
		'Too Many Coefficients'
		return;
	end
	b = remez(nupx-1,fo,mo,w);
	a=1;
	system('../../tools/bin/Linux/functgen --fc=2000 --fstop=4000  --numfreq=2 sig.pcm');
	axislabels=[0 fs/2 1.5 2.5];%in msec
end
%----- Arbitrary xfer function using fft for lpf fir filter design ---
if (choice==4)
	%----------------------------------- create template  -----------------------
	f=		[0    1000  3000 fs/2]; % amplitude template 
	amp=	[0    0     -12  -12];
	%---------------------------------- calculate coefs ------------------------
	b=coefcalc(f,amp,600); 
	a=1;
	system('../../tools/bin/Linux/functgen --fc=1000 --fstop=3400  --numfreq=2 sig.pcm');
	axislabels=[0 fs/2 6 8];%in msec
end
%----- Arbitrary xfer function using fft for bpf fir filter design ---
if (choice==5)
	%----------------------------------- create template  -----------------------
	f=		[0    1100  1900 2100 2900 fs/2]; % amplitude template 
	amp=	[-40  -40   0    0    -40  -40];
	%---------------------------------- calculate coefs ------------------------
	b=coefcalc(f,amp,600); 
	a=1;
	system('../../tools/bin/Linux/functgen --fc=2000 --fstop=5000  --numfreq=2 sig.pcm');
	axislabels=[0 fs/2 6 8];%in msec
end
%------------------ 2nd order iir allpass 1000Hz -----
if (choice==6)
	fc=1000;
	theta=2*pi*fc/fs;
	Rp=.99;
	Rz=1/Rp;
	b=[1 -2*Rz*cos(theta) Rz^2];
	a=[1 -2*Rp*cos(theta) Rp^2];
	[h f]=freqz(b,a,NPTS);
	maxamp=max(abs(h));
	b=b/maxamp;
	system('../../tools/bin/Linux/functgen --fc=1000 --fstop=2000  --numfreq=2 sig.pcm');
	axislabels=[0 fs/2 0 5];%in msec
end
%------ Arbitrary xfer function using fft for amplitude and group delay fir filter design ----
if (choice==7)
	%----------------------------------- create template  -----------------------
	ncoefs=510;
	%-------------------------- create amplitude and grpdelay template ---
	famp=	[0    2240  3273  8270  12400  15680 16540  17570  fs/2]; % amplitude template 
	amp=	[-60  -60   0     0     6      6     0      -60    -60  ];
	fdel= 	[0    10853 12575  14298 fs/2]; %group delay template
	del=	[0    0     .36    0     0    ];
	%-------------------------- design filter coefs -------------------
	b=coefcalcg(famp,amp,fdel,del,ncoefs);
	a=1;
	system('../../tools/bin/Linux/functgen --fc=5000 --fstop=15000  --numfreq=2 sig.pcm');
	axislabels=[0 fs/2 5.7 6.15];%in msec
end
%----------------- low latency nonlinear phase filter design ----------------
if (choice==8)
	fs=16000;
	Fpass=3600;
	Fstop=4400;
	Rp=.3;
	Rs=58.5;
	b=phasemin1(fs,Fpass,Rp,Fstop,Rs+4);
	a=1;
	system('../../tools/bin/Linux/functgen --fc=3000 --fstop=5000  --fs=16000 --numfreq=2 sig.pcm');
	axislabels=[0 fs/4 0 1.2]; %in msec
end
%----------------- sinx/x fir filter design ----------------
if (choice==9)
	fc=19500;
	ratio=fc/fs;
	NTAPS=151;
	order=7;
	mindB=90;
	p=pfit(ratio,NTAPS,order,mindB);
	alpha=0;
	b=sinxcalc(p,alpha);
	a=1;
	system('../../tools/bin/Linux/functgen --fc=10000 --fstop=20520  --fs=44100 --numfreq=2 sig.pcm');
	axislabels=[0 fc 1.5 2]; %in msec
end
%----------------- Remez up/down samplerate 44->48 conversion filter design ----------------
if (choice==10)
	M=37;
	L=34;
	fsout=48000;
	Fpass=18000;
	Fstop=22100;
	fs=M*fs; %for oversampling
	Rp=.3;
%	Rp=input(['enter +- ripple value (.3,.1,.05,.02) in dB: ' ]);
	Semilogx=input('Semilogx (1 or 0): '); 
	Rs=94;
	[nupx,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], fs);
	nupx = round(nupx/(M*L))*(M*L);
	if (nupx>MAXCOEFS)
		'Too Many Coefficients'
		return;
	end
	b = remez(nupx-1,fo,mo,w);
	a=1;
	system('../../tools/bin/Linux/functgen --numfreq=2 --fstop=17000 --amp=1.9 sig.pcm');
	axislabels=[0 fsout/2 12 20]; %in msec

end
%------------------ 8th order iir lpf samplerate 44->48  -----
if (choice==11)
	M=37;
	L=34;
	N=8;
	if ((N/2)*3>MAXCOEFS)
		'Too Many Coefficients'
		return;
	end
	Rp=.6;
	Rs=70;
	fc=17000;
	fs=M*fs;
	wn=fc/(fs/2);
	[b,a] = ellip(N,Rp,Rs,wn);
	b=b*1.035;
	system('../../tools/bin/Linux/functgen --amp=1.9 sig.pcm');
	axislabels=[0 (fs/L)/2 0 17];%in msec
end
%--------------------- 1 msec group delay test on music signals ----
if (choice==12)
	%----------------------------------- create template  -----------------------
	ncoefs=1800;
	%-------------------------- create amplitude and grpdelay template ---
	famp=	[0    2240  3273  8270  12400  15680 16540  18000  22050  fs/2]; % amplitude template 
	amp=	[0       0     0     0      0      0     0      0   -100  -100];
	fdel= 	[0     300   500   800   1000  fs/2]; %group delay template
	del=	[0       0     1     1      0    0 ];
	%-------------------------- design filter coefs -------------------
	b=coefcalcg(famp,amp,fdel,del,ncoefs);
	a=1;
	system('../../tools/bin/Linux/convrt BassISO.wav sig.pcm -it w -ot b -oend b -outputgain .8');
	axislabels=[0 1500 20 22];%in msec
end
%----------------- nonlinear low latency up/down samplerate 44->48 conversion filter design ----------------
if (choice==13)
	M=37;
	L=34;
	fsout=48000;
	Fpass=18000;
	Fstop=22100;
	fs=M*fs; %for oversampling
	Rp=.1;
	Rs=94;
	b=phasemin1(fs,Fpass,Rp,Fstop,Rs);
	[dummy,ncoefs]=size(b);
	if (ncoefs>MAXCOEFS)
		'Too Many Coefficients'
		return;
	end
	a=1;
	system('../../tools/bin/Linux/functgen --amp=1.9 sig.pcm');
	axislabels=[0 fsout/2 0 14]; %in msec
end
%----------------- 2stage stage 1 Remez filter design lpf  ----------------
if (choice==14)
	Rp=.01;
	Rs=100;

	interpratio1=7;
	decimratio1=5;
	Fs1=fs*interpratio1;

	interpratio2=7;
	decimratio2=9;
	Fs2=(Fs1/decimratio1)*interpratio2;

	Fsout=fs*interpratio1*interpratio2/(decimratio1*decimratio2);

	Fpass1=18000;
	Fstop1=Fs1/(2*interpratio1);
	Fpass2=18000;
	Fstop2=Fs2/(2*decimratio2);

	[ncoef1,fo,mo,w] = remezord( [Fpass1 Fstop1], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs1 );
	ncoef1 = round(ncoef1/2)*2;
	coef1 = remez(ncoef1-1,fo,mo,w);

	[ncoef2,fo,mo,w] = remezord( [Fpass2 Fstop2], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs2 );
	ncoef2 = round(ncoef2/2)*2;
	coef2 = remez(ncoef2-1,fo,mo,w);

	fs=Fs1;
	M=interpratio1;
	L=decimratio1;
	b=coef1;
	a=1;

	system(['../../tools/bin/Linux/functgen --amp=1.9  sig.pcm']);
	axislabels=[0 (fs/L)/2 2 4];%in msec

end
%----------------- 2stage stage 2 Remez filter design lpf  ----------------
if (choice==15)
	Rp=.01;
	Rs=100;

	interpratio1=7;
	decimratio1=5;
	Fs1=fs*interpratio1;

	interpratio2=7;
	decimratio2=9;
	Fs2=(Fs1/decimratio1)*interpratio2;

	Fsout=fs*interpratio1*interpratio2/(decimratio1*decimratio2);

	Fpass1=18000;
	Fstop1=Fs1/(2*interpratio1);
	Fpass2=18000;
	Fstop2=Fs2/(2*decimratio2);

	[ncoef1,fo,mo,w] = remezord( [Fpass1 Fstop1], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs1 );
	ncoef1 = round(ncoef1/2)*2;
	coef1 = remez(ncoef1-1,fo,mo,w);

	[ncoef2,fo,mo,w] = remezord( [Fpass2 Fstop2], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs2 );
	ncoef2 = round(ncoef2/2)*2;
	coef2 = remez(ncoef2-1,fo,mo,w);
	
	fs=Fs2;
	M=interpratio2;
	L=decimratio2;
	b=coef2;
	a=1;

	system(['../../tools/bin/Linux/functgen --amp=1.9 --fs=61740  --fc=1000 --numfreq=2 --fstop=24000 sig.pcm']);
	axislabels=[0 (fs/L)/2 2 4];%in msec

end
%----------------- New S Remez up/down samplerate 44->48 conversion filter design ----------------
if (choice==16)
	M=37;
	L=34;
	fsout=48000;
	Fpass=20000;
	Fstop=26400; %-23 dB at f=24100 Hz 894 coefs
%	Fstop=24650; %-52 dB at f=24100 Hz 1230 coefs
%	Fstop=24300; %-70 dB at f=24100 Hz 1330 coefs
	fs=M*fs; %for oversampling
	Rp=.1;
	Semilogx=input('Semilogx (1 or 0): ');
%	Semilogx=0;
	Rs=94;
	[nupx,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], fs);
	nupx=2*round(nupx/2);
	if (nupx>MAXCOEFS)
		'Too Many Coefficients'
		return;
	end
	b = remez(nupx-1,fo,mo,w);
	a=1;
	THDTARGET=.01;
	system('../../tools/bin/Linux/functgen --fs=44100 --numfreq=2 --amp=1.0 --fstop=20000 sig.pcm');
	axislabels=[0 fsout/2 8 12]; %in msec
end
%----------------- New S Remez up/down samplerate 48->44 conversion filter design ----------------
if (choice==17)
	fs=48000;
	M=34;
	L=37;
	fsout=44100;
	Fpass=20000;
	Fstop=26400; %-23 dB at f=24100 Hz 894 coefs
%	Fstop=24650; %-52 dB at f=24100 Hz 1230 coefs
%	Fstop=24300; %-70 dB at f=24100 Hz 1330 coefs
	fs=M*fs; %for oversampling
	Rp=.1;
	Semilogx=input('Semilogx (1 or 0): ');
%	Semilogx=0;
	Rs=94;
	[nupx,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], fs);
	nupx=2*round(nupx/2);
	if (nupx>MAXCOEFS)
		'Too Many Coefficients'
		return;
	end
	b = remez(nupx-1,fo,mo,w);
	a=1;
	THDTARGET=.0005;
	system('../../tools/bin/Linux/functgen --fs=48000 --numfreq=2 --amp=1.0 --fc=20000 --fstop=22000 sig.pcm');
	axislabels=[0 fsout/2 8 12]; %in msec
end
%----------------- Actual Remez up/down samplerate 12x conversion filter design ----------------
if (choice==18)
	M=12;
	L=2;
	fs=M*fs; %for oversampling
	fsout=44100;
	Fpass=19845;
	Fstop=22050; 
	Rp=.3;
	Semilogx=input('Semilogx (1 or 0): ');
%	Semilogx=0;
	Rs=90;
	[n12x,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], fs);
	n12x=(floor(n12x/(360*2))+1 ) *360*2;
	if (n12x>MAXCOEFS)
		'Too Many Coefficients'
		return;
	end
	b = remez(n12x-1,fo,mo,w);
	a=1;
	system('../../tools/bin/Linux/functgen --fs=44100 --numfreq=2 --amp=1.0 --fstop=19845 sig.pcm');
	axislabels=[0 fsout/2  0 2]; %in msec
end



%----------------- display template ------------------------------
[dummy,ncoefs]=size(b);
[h,f] = freqz(b,a,NPTS,fs);
ampdB=20*log10(abs(h)+1e-10);
if (Semilogx==1)
	figure
	semilogx(f,ampdB);
	grid on;
	zoom on;
	title(['filter amplitude response in dB vs frequency ncoefs: ',num2str(ncoefs)]);
	xlabel(comment);
else
	figure
	plot(f,ampdB);
	grid on;
	zoom on;
	title(['filter amplitude response in dB vs frequency ncoefs: ',num2str(ncoefs)]);
	xlabel(comment);
end

[Gd,F] = grpdelay(b,a,NPTS,fs);
Gd=1000*Gd/(fs/L); %convert nsamples to msec
figure
plot(F,Gd);
grid on;
zoom on;
axis(axislabels);
title(['filter group delay response in msec vs frequency for fsout=',num2str(fs/L)]);
xlabel(comment);
%---------------- calculate biquad coefficients if iir is needed ---
[dummy,sz]=size(a);
if (sz>1)
	iir=1;
	if (usesos==0)	
		[KN,KD]=findbiq(b,a);
		[numcells dummy]=size(KD);
		b1=[ ];
		a1=[ ];
		sos=[ ];
		for (i=1:numcells)
			b1=[b1 KN(i,:)];
			a1=[a1 KD(i,:)];
			sos=[sos ; KN(i,:) KD(i,:)];
		end
	else
		dir_flag='up';
		[sos,gn] = tf2sos(b,a,dir_flag);
		[numcells,dummy]=size(sos);
		a1=[ ];
		b1=[ ];
		gain=1.0;
		for (i=1:numcells)
			if (i==numcells)
				gain=gn;
			end
			b1=[b1 gain*sos(i,1:3)];
			a1=[a1 sos(i,4:6)];
		end
	end
else
	iir=0;
	b1=b;
	a1=a;
end
%---------------- store coefs for ext. C program ----
foutn = fopen('num.txt','w');
foutd = fopen('den.txt','w');
[dummy sz]=size(b1);

for i=1:sz
	fprintf(foutn,' %16.10e \n',b1(i));
end
[dummy sz]=size(a1);
for i=1:sz
	fprintf(foutd,' %16.10e \n',a1(i));
end
fclose('all');

%----------------------------- verify Matlab and C results ---------------
if (choice==10)
	system(['../../tools/bin/Linux/filteri sig.pcm sigout.pcm b 2 2 2 ',num2str(M),' ', num2str(L), ' num.txt ']);
	system(['evdist --fs=',num2str(fs/L),' --thdtarget=',num2str(THDTARGET),' sigout.pcm']);
end
system(['../../tools/bin/Linux/filter sig.pcm sigout.pcm b 2 ',num2str(M),' ', num2str(L), ' num.txt den.txt']);
system(['evdist --fs=',num2str(fs/L),' --thdtarget=',num2str(THDTARGET),' sigout.pcm']);
system('../../tools/bin/Linux/convrt sig.pcm sig.txt       -it b -ot f -iend b ');
system('../../tools/bin/Linux/convrt sigout.pcm sigout.txt -it b -ot f -iend b ');
load sig.txt;
load sigout.txt;
[szout dummy]=size(sigout);
%ssigoutm=M*upfirdn(sig,b,M,L); %another possibility for fir only
[szin dummy]=size(sig);
ssig=zeros(1,M*szin); %upsampling by M 
ssig(1:M:M*szin)=sig(1:szin);
if (iir==0)
	sigoutm=M*filter(b,a,ssig);
else
	sigoutm=M*sosfilt(sos,ssig);
end
ssigoutm=(sigoutm(1:L:L*szout))'; %downsampling by L
diff=max(abs(sigout-ssigoutm(1:szout)));
bitdiff=round(32768*diff);
bitdiff

display=1;
if (display==1)
	figure
	plot(sig,'b');
	hold on;
	plot(sigout,'r');
	hold off;
	grid on;
	zoom on;
	title(['input(b) and output(r) signals with : ',num2str(bitdiff),' bits difference between Matlab and C simulations']);
	xlabel(comment);
end


