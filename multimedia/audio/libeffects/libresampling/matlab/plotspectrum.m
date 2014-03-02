clear;
%close all;
epsilon=1e-10;

filename='outsig.txt';

value=input(['inputfilename(',num2str(filename),'): '],'s');
[dummy sizvalue]=size(value);
if (sizvalue~=0)
	filename=value;	
end

signal=load(filename);
fsout=48048;
value=input(['sample frequency in Hz(',num2str(fsout),'): '],'s');
[dummy sizvalue]=size(value);
if (sizvalue~=0)
	fsout=str2num(value)
end


figure

[nsmpls dummy]=size(signal);
if (rem(nsmpls,2)==0)
	winval=chebwin(nsmpls-1,140)';
	winval(nsmpls)=0;
else
	winval=chebwin(nsmpls,140)';
end
fftin=winval.*((1/nsmpls)*signal(1:nsmpls)');
amp=abs(fft(fftin));
ampdb=20*log10(amp+epsilon);
ampdb= ampdb+(-max(ampdb));
i=1:nsmpls;
freq=((i-1)/nsmpls)*fsout;
plot(freq,ampdb);
grid on;
zoom on;
axis([0 fsout/2 -150 0]);
txt=['spectrum for file ',filename, ' with fsout=',num2str(fsout)];
title(txt);


