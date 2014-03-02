clear;
close all;
marge=.1;
marge=0;
BLOCKSIZ=64;
SPILLBUFSIZ=floor(BLOCKSIZ/8);
interpratio=24;
interpratio=37;
interpratio=input(' enter interpratio (37 or 24): ');
order=2;
order=input(' enter order of interpolator (1 or 2): ');
if (order==1)
	convmax=2;
	interpratio=interpratio*2;
else
	convmax=3;
end
%--------------------------- input parameters -----------------------------------------
fs1=44100;
fs1=input(' enter input sampling frequency: ');
fs2=48000;
fs2=input(' enter ouput sampling frequency: ');
if (fs2>fs1)
	L=round((fs1/fs2)*interpratio);
	M=interpratio;
else
	M=round((fs2/fs1)*interpratio);
	L=interpratio;
end
dalpha=((fs1*M/L)-fs2)/fs2;
M
L
dalpha
%--------------------------------------------------------------------------------------
NPTS=2048;
Epsilon=1e-10;
%--------------------------- calculate fixed  interpolation filter ------------------
Fs = 1;
Fstop = .98/(2*interpratio);
Rp = 0.3;
Fpass = .889*Fstop;
Rs=100;
[nupx,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [10^(Rp/20)-1 10^(-Rs/20)], Fs );
nupx=2*floor(nupx/2);
deci1_upx = remez(nupx-1,fo,mo,w);
figure
plot(deci1_upx);
grid on;
zoom on;
title(' interpolation filter coefs');
figure
freqz(deci1_upx,8192); zoom;
title('frequency response of fixed interpolation filter');
grid on;
zoom on;
%############################### TESTING #######################################
%----------------------------- generate sine waves  -----------
amp=.33;
T1=1/fs1;
f1=1000;
if (M>L)
	f2=.889*.98*(1-marge)*fs1/2;
	f3=.49875*fs1;
else
	f2=.889*.98*(1-marge)*fs2/2;
	f3=.49875*fs2;
end
NTEST=1024*interpratio*L/M;
i=(1:NTEST);
t1=(i-1)*T1;
sigin=amp*sin(2*pi*f1*t1)+amp*sin(2*pi*f2*t1)+amp*sin(2*pi*f3*t1);
%-------------------------- prepare filter coefs ------------------------------
ntaps=floor(nupx/M)+1;
ntaps
%------ make nb coefs a multiple of M with padded zeros ----------------
ncoefs=nupx+M;
coef=zeros(1,ncoefs);
coef(1:nupx)=deci1_upx;
%------- run the src ---------------------------------------------------
iter=0;
offset=0;
inptr=1;
outptr=1;
D=zeros(1,ntaps);
convcnt=0;
alpha=0;
dalpha1=L*dalpha/(L-convmax);
y=zeros(1,3);
nbconv=0;
nbupdate=0;
remainbuf=zeros(1,BLOCKSIZ);
nbremain=0;
remainptr=1;
nbspill=0;
spillbuf=zeros(1,SPILLBUFSIZ);
if (fs2<fs1)
	while (iter<NTEST-BLOCKSIZ)
		nbin=0;
		nbout=0;
		while (nbin<BLOCKSIZ)
			if (offset<M)
				if (nbremain==BLOCKSIZ)
					for (i=1:BLOCKSIZ)
						sigout(outptr)=remainbuf(i);
						outptr=outptr+1;
					end
					nbremain=0;
					remainptr=1;
					nbout=BLOCKSIZ;
				end
				%-------convolution--------
				temp=M*sum(D(1:ntaps).*(coef(offset+1:M:offset+1+(M*ntaps)-1)));
				y(1:3)=[temp y(1:2) ];
				offset=offset+1;
				convcnt=convcnt+1;
				if (convcnt>=convmax)
					nbconv=nbconv+1;
					if (order==1)
%						m=y(1)-y(2);
%						remainbuf(remainptr)=y(2)+m*(alpha+.5);
						remainbuf(remainptr)=(alpha+.5)*y(1)-(alpha-.5)*y(2);
					else
%						a1=y(3);
%						a2=y(2)-a1;
%						a3=(y(1)-a1-2*a2)/2;
%						remainbuf(remainptr)=a1+a2*(alpha+1)+a3*(alpha+1)*alpha;
						remainbuf(remainptr)=(alpha/2)*(alpha+1)*y(1)-(alpha+1)*(alpha-1)*y(2)+(alpha/2)*(alpha-1)*y(3);
					end
					nbremain=nbremain+1;
					remainptr=remainptr+1;
					convcnt=0;
					for k=1:L-convmax
						offset=offset+1;
						alpha=alpha+dalpha1;
						if (alpha>.5)
							%skip an output sample next time
							alpha=alpha-1;
							offset=offset+1;
						else
							%use same output sample next time
							if (alpha<-.5)
								alpha=alpha+1;
								offset=offset-1;
							end
						end
					end
				end
			else
				%-------update-------------
				nbupdate=nbupdate+1;
				D=[sigin(inptr) D(1:ntaps-1)];
				inptr=inptr+1;	
				offset=offset-M;
				iter=iter+1;
				nbin=nbin+1;
			end
		end
	end
else
	%------------- UP SAMPLING ---------------
	while (iter<NTEST-BLOCKSIZ)
		nbin=0;
		nbout=0;
		nbconsumed=0;
		nbin_needed=round((BLOCKSIZ+((SPILLBUFSIZ/2)-nbspill))*L/M);
		i=1;
		while (nbspill~=0)
			sigout(outptr)=spillbuf(i);
			outptr=outptr+1;
			nbout=nbout+1;
			i=i+1;
			nbspill=nbspill-1;
		end
		while (nbin<nbin_needed)
			if (offset<M)
				%-------convolution--------
				temp=M*sum(D(1:ntaps).*(coef(offset+1:M:offset+1+(M*ntaps)-1)));
				y(1:3)=[temp y(1:2)];
				offset=offset+1;
				convcnt=convcnt+1;
				if (convcnt>=convmax)
					if (order==1)
%						m=y(1)-y(2);
%						sigtemp=y(2)+m*(alpha+.5);
						sigtemp=(alpha+.5)*y(1)-(alpha-.5)*y(2);						
					else
%						a1=y(3);
%						a2=y(2)-a1;
%						a3=(y(1)-a1-2*a2)/2;
%						sigtemp=a1+a2*(alpha+1)+a3*(alpha+1)*alpha;
						sigtemp=(alpha/2)*(alpha+1)*y(1)-(alpha+1)*(alpha-1)*y(2)+(alpha/2)*(alpha-1)*y(3);
					end
					nbconv=nbconv+1;
					if (nbout==BLOCKSIZ)
						spillbuf(nbspill+1)=sigtemp;
						nbspill=nbspill+1;
					else
						sigout(outptr)=sigtemp;
						outptr=outptr+1;
						nbout=nbout+1;
					end
					convcnt=0;
					for k=1:L-convmax
						offset=offset+1;
						alpha=alpha+dalpha1;
						if (alpha>.5)
							%skip an output sample next time
							alpha=alpha-1;
							offset=offset+1;
						else
							%use same output sample next time
							if (alpha<-.5)
								alpha=alpha+1;
								offset=offset-1;
							end
						end
					end
				end
			else
				%-------update-------------
				nbupdate=nbupdate+1;
				if (nbremain==0)
					i=1;
					while (nbremain<BLOCKSIZ)
						remainbuf(i)=sigin(inptr);
						inptr=inptr+1;
						nbremain=nbremain+1;
						i=i+1;
						iter=iter+1;
					end
					remainptr=1;
					nbconsumed=BLOCKSIZ;
				end
				D=[remainbuf(remainptr) D(1:ntaps-1)];
				remainptr=remainptr+1;
				nbin=nbin+1;
				nbremain=nbremain-1;
				offset=offset-M;
			end
		end
	end
end
%---------------- check results --------------------------------------
[dummy sizout]=size(sigout);
exactratio=nbconv/nbupdate;
exactfreq=exactratio*fs1;
nonadjfreq=M*fs1/L;
dalphaexact=(nonadjfreq-exactfreq)/exactfreq;
%dalpha
%dalphaexact
fftsiz=2^(floor(log2(sizout)));
fftin=(chebwin(fftsiz-1,120))';
fftin=[fftin,0];
fftin=fftin.*sigout(1:fftsiz);
mag=fft(fftin,fftsiz);
mag=abs(mag);
mag=20*log10(mag+1e-10);
magmax=max(mag);
mag=mag-magmax;
i=(1:fftsiz);
freq=(exactfreq/fftsiz)*(i-1);
figure
plot(freq,mag);
grid on;
zoom on;
axis([0 exactfreq/2 -150 0]);
title(['spectrum of output signal with fs= ',num2str(exactfreq)]);
%-------------  output signal to be analyzed with evaldist c program  ---------------------------
f1=fopen('outsig.bin','wb');
fwrite(f1,sigout*(2^31),'integer*4');
%--------------------------  output signal to be analyzed with cool edit ----------------------
f1=fopen('outsig.pcm','wb');
fwrite(f1,sigout*(2^15),'integer*2');
fclose('all');
system([ '../../tools/bin/Linux/evaldist outsig.bin ',  num2str(exactfreq), ' .1' ]);

