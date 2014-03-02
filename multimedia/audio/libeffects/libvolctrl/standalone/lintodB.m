function dB=lintodB(lin)
%function dB=lintodB(lin)

NUMITER=23;
val6dB=20*log10(2);

invalmant=lin;
valoffset=0;
while (invalmant<.5)
	valoffset=valoffset-val6dB;
	invalmant=invalmant*2;
end
while (invalmant>1)
	valoffset=valoffset+val6dB;
	invalmant=invalmant/2;
end
arg=(invalmant-1);
res=0;
sgn=1;
temp=1;
for (n=1:NUMITER)
	temp=temp*arg;
	res=res+sgn*temp/n;
	sgn=-sgn;
end
res=(20/log(10))*res;
res=res+valoffset;
dB=res;