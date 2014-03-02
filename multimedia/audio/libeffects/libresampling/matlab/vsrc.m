%************************ EVALUATE FOR 7th order INTERPOLATION OF COEFFICIENTS *************
clear
close all;
%----------------------------- init ----------------------------------
outptr=1;
fsin=48000;
interpratio=32;
'Calculate Filter Coefs'
%--------------------------- calculate fixed  interpolation filter ------------------
Fs = 1;
Fstop = 1/(2*interpratio);
Rp = 0.1;
Fpass = .9*Fstop;
Rs=130;
[nupx,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
nupx=interpratio*(floor(nupx/interpratio)+1);
deci1_upx = remez(nupx-1,fo,mo,w);
%-------------------------- interpolation filter coefs ------------------------------
ord=7;
numtaps=nupx/interpratio;
temp=[-.5 :1/interpratio: .5];
alphaval=temp(1:interpratio);
for i=0:numtaps-1
	p(i+1,:)=polyfit(alphaval, deci1_upx(i*interpratio+(1:interpratio)),ord);
end
%-------------------------- prepare input signal -------------------------------
T1=1/fsin;
f1=18000;
t=0;
smpl=1;
alpha=0;
dalpha=.005;
D=zeros(1,numtaps);
nbconv=8192;
nbupd=0;
'Start test'
for iter=1:nbconv
	%------------------ generate  sample --------------------
	for i=0:numtaps-1
		coef(i+1)=polyval(p(i+1,:),alpha);
	end
	outsig(smpl)=interpratio*sum(D(1:numtaps).*(coef(1:numtaps)));
	smpl=smpl+1;
	alpha=alpha+dalpha;
	nup=1;
	if (alpha>=.5)
		alpha=alpha-1;
		nup=2;
	else
		if (alpha<-.5)
			alpha=alpha+1;
			nup=0;
		end
	end
	%------- update delay line with one input sample  -------------
	nbupd=nbupd+nup;
	while (nup>0)
		nxtsamp=cos(2*pi*f1*t);
		t=t+T1;
		D=[nxtsamp D(1:numtaps-1)];
		nup=nup-1;
	end
end % test loop
figure(1)
epsilon=1e-10;
if (rem(nbconv,2)==0)
	winval=chebwin(nbconv-1,140)';
	winval(nbconv)=0;
else
	winval=chebwin(nbconv,140)';
end
fftin=winval.*((6/nbconv)*outsig(1:nbconv));
amp=abs(fft(fftin));
ampdb=20*log10(amp+epsilon);
fsout=fsin*nbconv/nbupd;
nbupd
nbconv
fsout
i=1:nbconv;
freq=((i-1)/nbconv)*fsout;
plot(freq,ampdb);
grid on;
zoom on;
axis([0 fsin/2 -160 0]);
title('Results for 7th order interpolator of coefficients');
%************************ EVALUATE VARIOUS ORDERS FOR INTERPOLATION OF OUTPUT SIGNAL *************
clear
%----------------------------- init ----------------------------------
outptr=1;
fsin=48000;
interpratio=32;
M=interpratio;
L=interpratio;
'Calculate Filter Coefs'
%--------------------------- calculate fixed  interpolation filter ------------------
Fs = 1;
Fstop = 1/(2*interpratio);
Rp = 0.1;
Fpass = .9*Fstop;
Rs=130;
[nupx,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
cf=2*interpratio; %make nb coefs an interger multiple of ntaps
nupx=cf*ceil(nupx/cf);
deci1_upx = remez(nupx-1,fo,mo,w);
%-------------------------- prepare filter coefs ------------------------------
ntaps=nupx/interpratio;
D=zeros(1,ntaps);
ncoefs=nupx;
coef(1:nupx)=deci1_upx;
DISP=['NCOEFS: ',num2str(ncoefs)];
DISP
%-------------------------- prepare input signal -------------------------------
T1=1/fsin;
f1=18000;
offset=0;
convcnt=0;
t=0;
smpl=1;
alpha=0;
dalpha=-.005;
'Start test'
for iter=1:2048
	alpha=alpha+dalpha;
	if alpha>.5
		alpha=alpha-1;
		offset=offset+1;
	else
		if alpha<-.5
			alpha=alpha+1;
			offset=offset-1;
		end
	end
	%------------------ generate one output sample --------------------
	done=0;
	y=zeros(1,4);
	while (done==0)
		if (offset<M)
			%------- convolution for 4 samples on contiguous oversampling phases --------
			temp=interpratio*sum(D(1:ntaps).*(coef(offset+1:interpratio:offset+1+(interpratio*ntaps)-1)));
			y(1:4)=[temp y(1) y(2) y(3)];
			offset=offset+1;
			convcnt=convcnt+1;
			if (convcnt==4)
				convcnt=0;
				done=1;
				offset=offset+L-4;
			end
		else
			%------- update delay line with one input sample  -------------
			nxtsamp=cos(2*pi*f1*t);
			t=t+T1;
			D=[nxtsamp D(1:ntaps-1)];
			offset=offset-M;
		end
	end % while done==0
%------------------------ 3rd order interpolation ---------
	xinp=alpha+2;	
%	p=polyfit([0 1 2 3],y(4:-1:1),3);
%	outval3(smpl)=polyval(p,xinp);
	outval3(smpl)=poly3(y(4:-1:1),xinp);
%------------------------ 2nd order interpolation ---------
	xinp=alpha+1;	
%	p=polyfit([0 1 2],y(3:-1:1),2);
%	outval2(smpl)=polyval(p,alpha+.5);
	outval2(smpl)=poly2(y(3:-1:1),xinp);
%------------------------ 1st order interpolation ---------
	outval1(smpl)=(alpha+.5)*y(1)-(alpha-.5)*y(2);
%------------------------- no interpolation ----------------
	outval(smpl)=y(1);
%-----------------------------------------------------------
	smpl=smpl+1; 
end % test loop

epsilon=1e-10;
figure(2)
[dummy nsmpls]=size(outval);
if (rem(nsmpls,2)==0)
	winval=chebwin(nsmpls-1,140)';
	winval(nsmpls)=0;
else
	winval=chebwin(nsmpls,140)';
end
mult=6;
%----------- plot 0, 1, 2, and 3 interp orders ------
fftin=winval.*((mult/nsmpls)*outval(1:nsmpls));
amp=abs(fft(fftin));
ampdb=20*log10(amp+epsilon);
i=1:nsmpls;
freq=((i-1)/nsmpls)*fsin;
plot(freq,ampdb,'r');
hold on;
fftin=winval.*((mult/nsmpls)*outval1(1:nsmpls));
amp=abs(fft(fftin));
ampdb=20*log10(amp+epsilon);
i=1:nsmpls;
freq=((i-1)/nsmpls)*fsin;
plot(freq,ampdb,'y');
fftin=winval.*((mult/nsmpls)*outval2(1:nsmpls));
amp=abs(fft(fftin));
ampdb=20*log10(amp+epsilon);
i=1:nsmpls;
freq=((i-1)/nsmpls)*fsin;
plot(freq,ampdb,'g');
fftin=winval.*((mult/nsmpls)*outval3(1:nsmpls));
amp=abs(fft(fftin));
ampdb=20*log10(amp+epsilon);
i=1:nsmpls;
freq=((i-1)/nsmpls)*fsin;
plot(freq,ampdb,'b');
hold off;
%------------------------------------------------
grid on;
zoom on;
axis([0 fsin/2 -150 0]);
title('Results for x32 oversampling and interpolator orders of 0,1,2,3 in red,yellow,green,blue');
clear
%************* COMPLETE SIMULATION WITH DPLL JITTER BUFFER AND INTERPOLATOR ************
%----------------------------- init ----------------------------------
NTEST=5e6;
gear_shift_time=floor(.75*NTEST);
MAX_DISP=8192;
n_ignore=NTEST-MAX_DISP+1;
FRAMESIZ=1024;
BLOCKSIZ=32;
TEMPBUF_SIZ=4*FRAMESIZ;
offst=200; %  PPM frequency offset
offsetmult=1+(offst/1e6);
tempbuf(1:TEMPBUF_SIZ)=zeros(1,TEMPBUF_SIZ);
tempbuf_nbsamples=TEMPBUF_SIZ/2;
tempbuf_inptr=1;
tempbuf_outptr=TEMPBUF_SIZ/2+1;
outptr=1;
fsin=48000;
rho=24; 
pll_freq=0;
pll_phase=0;
TIN=FRAMESIZ/fsin;
delta_tin=TIN;
delta_tin1=2*TIN;
TOUT=(BLOCKSIZ/fsin)*offsetmult;%fsin>fsout
delta_tout=TIN/2;
[k1,k2]=pllcalc(1,400e-6,rho);
k1=abs(k1*8);
k2=abs(k2/2);
interpratio=32;
M=interpratio;
L=interpratio;
nbconsumed=0;
nbgen=0;
iter=1;
frameiter=1;
disp_iter=1;
disp_stride=floor(NTEST/MAX_DISP);
disp_stride_cnt=disp_stride;
offset=floor(M/2);
convcnt=0;
in_smplno=1;
out_smplno=1;
delta_phase=0;
nframes=0;
latency=0;
%latency=.3;
'Calculate Filter Coefs'
%--------------------------- calculate fixed  interpolation filter ------------------
Fs = 1;
Fstop = 1/(2*interpratio);
Rp = 0.1;
Fpass = .9*Fstop;
Rs=130;
[nupx,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
cf=2*interpratio; %make nb coefs an interger multiple of ntaps
nupx=cf*ceil(nupx/cf);
deci1_upx = remez(nupx-1,fo,mo,w);
%-------------------------- prepare filter coefs ------------------------------
ntaps=nupx/interpratio;
D=zeros(1,ntaps);
ncoefs=nupx;
coef=zeros(1,ncoefs);
coef(1:nupx)=deci1_upx;
DISP=['NCOEFS: ',num2str(ncoefs)];
DISP
%-------------------------- prepare input signal -------------------------------
amp=.25;
T1=1/fsin;
f1=1000;
f2=18000;
%@@@@@@@@@@@@@@@@@@@@@@@ TEST @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
'Start test'
while (iter<NTEST) %NTEST samples will be generated
	if ((delta_tin<delta_tout)|(delta_tin1<delta_tout))
		if (delta_tin<delta_tout) 			% Main frame clock with 0 latency
			delta_tout=delta_tout-delta_tin;
			delta_tin=TIN;
			delta_tin1=TIN*latency*rand; % Decoder IT is delayed by a certain amount from main frame clock
		else
			delta_tout=delta_tout-delta_tin1; % Decoder IT with a certain latency with respect to main frame clock
			delta_tin=delta_tin-delta_tin1;
			delta_tin1=2*TIN; % Disable Decoder IT until at least next Main frame clock cycle
		%----------- generate one frame of new samples and put in tempbuf -----------
			for j=1:FRAMESIZ 
				t1=(in_smplno-1)*T1;
				in_smplno=in_smplno+1;
				sigin=amp*sin(2*pi*f1*t1)+amp*sin(2*pi*f2*t1); % mix 2 sine waves at 1000 and 18000 Hz
				tempbuf(tempbuf_inptr)=sigin;
				tempbuf_inptr=tempbuf_inptr+1;
				if (tempbuf_inptr>TEMPBUF_SIZ)
					tempbuf_inptr=1;
				end
			end
			%----------- sample the phase derived from the fullness of the tempbuf ------------------
			delta_phase=(tempbuf_nbsamples-(TEMPBUF_SIZ/2))/TEMPBUF_SIZ;
			tempbuf_nbsamples=tempbuf_nbsamples+FRAMESIZ;
			%------------- display of fullness of tempbuf and frameno -----------------------------
			TEMPBUF_NBSAMPLES(frameiter)=tempbuf_nbsamples;
			frameiter=frameiter+1;
			nframes=nframes+1;
			if ((rem(nframes,10))==0)
				DISP=['frameno: ',num2str(nframes),'/',num2str(floor(NTEST/FRAMESIZ))];
				DISP
			end
		end %------------------ end of decoder IT -------------
	else  
	%--------------- DAC interrupt -----------------------------
		delta_tin=delta_tin-delta_tout;
		delta_tin1=delta_tin1-delta_tout;
		delta_tout=TOUT;
		%------------ run the src to generate BLOCKSIZ output samples ----------	
		for k=1:BLOCKSIZ
			%------------- update DPLL --------------------------
			pll_freq=pll_freq+k2*delta_phase;
			pll_phase=pll_phase+delta_phase*k1;
			pll_phase=pll_phase+pll_freq;
			%--------- -1 < pll_phase < +1 ----------
			if (pll_phase>1)
				pll_phase=pll_phase-2; 
				offset=offset+1; %---- go to next oversampling phase
			else
				if (pll_phase<-1)
					pll_phase=pll_phase+2;
					offset=offset-1; %---- go to previous oversampling phase
				end
			end
			alpha=pll_phase/2; %----------- -.5 < alpha < .5 --------
			%------------------ generate one output sample --------------------
			done=0;
			y=zeros(1,4);
			while (done==0)
				if (offset<M)
					%------- convolution for 2 samples on contiguous oversampling phases --------
					temp=interpratio*sum(D(1:ntaps).*(coef(offset+1:interpratio:offset+1+(interpratio*ntaps)-1)));
					y(1:4)=[temp y(1) y(2) y(3)];
					offset=offset+1;
					convcnt=convcnt+1;
					if (convcnt==4)
						convcnt=0;
						done=1;
						offset=offset+L-4;
					end
				else
					%------- update delay line with one sample from tempbuf -------------
					D=[tempbuf(tempbuf_outptr) D(1:ntaps-1)];
					tempbuf_outptr=tempbuf_outptr+1;
					if (tempbuf_outptr>TEMPBUF_SIZ)
						tempbuf_outptr=1;
					end
					tempbuf_nbsamples=tempbuf_nbsamples-1;
					offset=offset-M;
					nbconsumed=nbconsumed+1;
				end
			end % while done==0
			%------------------------ 1st order interpolation ---------
%			outval=(alpha+.5)*y(1)-(alpha-.5)*y(2);
			%------------------------ 2nd order interpolation ---------
%			xinp=alpha+1;	
%			p=polyfit([0 1 2],y(3:-1:1),2);
%			outval=polyval(p,xinp);
%			outval(smpl)=poly3(y(4:-1:1),xinp);
			%------------------------ 3rd order interpolation ---------
			xinp=alpha+2;	
%			p=polyfit([0 1 2 3],y(4:-1:1),3);
%			outval=polyval(p,xinp);
			outval=poly3(y(4:-1:1),xinp);
		 	%----- reduce PLL BW by 32 after generation of a certain number of outsamples -----
			iter=iter+1;
			if (iter==gear_shift_time)
				k1=k1/32;
				k2=0;
			end
			%--------- output signal,alpha to vectors only after a certain time to reduce memory requirements ------
			out_smplno=out_smplno+1;
			if (out_smplno>n_ignore)		
				sigout(outptr)=outval;
				ALPHA(outptr)=alpha;
				outptr=outptr+1;
			end
			%------------ output pll_freq only every few samples to reduce memory requirements ------------------
			disp_stride_cnt=disp_stride_cnt-1;
			if (disp_stride_cnt==0)
				disp_stride_cnt=disp_stride;
				PLL_FREQ(disp_iter)=pll_freq;
				disp_iter=disp_iter+1;
			end
			nbgen=nbgen+1;
		end % for k=1:BLOCKSIZ
	end %DAC interrupt
end % test loop


%---------------- check results --------------------------------------
figure(3)
plot(TEMPBUF_NBSAMPLES);
grid on;
zoom on;
title('TEMPBUFNBSAMPLES');

fsout=fsin*nbgen/nbconsumed;
figure(4)
plot(PLL_FREQ);
grid on;
zoom on;
title(['Evolution of PLLFREQ during generation of ',num2str(NTEST),' output samples at fsout= ',num2str(round(fsout))]);

figure(5)
[dummy sizalpha]=size(ALPHA);
plot(ALPHA(sizalpha-511:sizalpha));
grid on;
axis([0 512 -.75 .75]);
title('last 512 values of ALPHA');


[dummy sizout]=size(sigout);
fftsiz=2^(floor(log2(sizout)));
fftin=(chebwin(fftsiz-1,120))';
fftin=[fftin,0];
sigout_analysis=sigout(1:fftsiz);
fftin=fftin.*sigout_analysis;
mag=fft(fftin,fftsiz);
mag=abs(mag);
mag=20*log10(mag+1e-10);
magmax=max(mag);
mag=mag-magmax;
i=(1:fftsiz);
freq=(fsout/fftsiz)*(i-1);
figure(6)
plot(freq,mag);
grid on;
zoom on;
axis([0 fsout/2 -150 0]);
title(['spectrum of output signal with fsout= ',num2str(round(fsout)),' Hz']);
%-------------  output signal to be analyzed with evaldist c program  ---------------------------
f1=fopen('outsig.bin','wb');
fwrite(f1,sigout*(2^31),'integer*4');
%--------------------------  output signal to be analyzed with cool edit ----------------------
f1=fopen('outsig.pcm','wb');
fwrite(f1,sigout*(2^15),'integer*2');
fclose('all');
system([ 'evaldist outsig.bin ',  num2str(fsout), ' .1' ]);

