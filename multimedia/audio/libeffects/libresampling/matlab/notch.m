function out=notch(in,fstop,df,fs,fftsiz,numcoef,showresp);
%----------------------------------------------------------
%out     =    notch(in,fstop,df,fs,fftsiz,numcoef,showresp)
%in      =    input signal
%fstop   =    notch filter center frequency(s)
%df      =    template for notch response
%fs      =    sampling frequency
%fftsiz  =    size of fft used to calculate the coefs
%numcoef =    number of coefs for notch fir
%showresp=   (1)display frequency response of filter
%----------------------------------------------------------

epsilon=1e-10;
stopdb=160;
val=ones(1,fftsiz/2);

[dummy numfreq]=size(fstop);

%------------- remove the fstop frequencies ---
for i=1:numfreq
    f1=fstop(i)-df;
    f2=fstop(i)+df;
    indx1=round((f1/(fs/2))*(fftsiz/2))+1;
    indx2=round((f2/(fs/2))*(fftsiz/2))+1;
    val(indx1:indx2)=zeros(1,indx2-indx1+1);
end
%------------- limit the band between 20 and 22kHz ---
flo=20;fhi=22000;
indx1=1;
indx2=round((flo/(fs/2))*(fftsiz/2))+1;
val(indx1:indx2)=zeros(1,indx2-indx1+1);
indx1=round((fhi/(fs/2))*(fftsiz/2))+1;
indx2=fftsiz/2;
val(indx1:indx2)=zeros(1,indx2-indx1+1);
%-----------------------------------------------------
ifftin=[val 0 val(fftsiz/2:-1:2)];
temp=ifft(ifftin,fftsiz);
ktemp=real(temp);
mid=fftsiz/2;
ktemp1(1:mid-1)=ktemp(2+mid:fftsiz);
ktemp1(mid:fftsiz-1)=ktemp(1:mid);
ktemp2(1:numcoef)=ktemp1(mid-floor(numcoef/2):mid+floor(numcoef/2));
num=ktemp2.*chebwin(numcoef,stopdb)';
den=1;
out=filter(num,den,in);

if (showresp>0)
	npts=fftsiz;
	resp=freqz(num,den,npts);
	respdb=20*log10(abs(resp)+epsilon);
	i=1:npts;
	freq=(fs/2)*(i-1)/npts;
	figure
	plot(freq,respdb);
	grid on;
	zoom on;
	title('Notch filter response');
end
