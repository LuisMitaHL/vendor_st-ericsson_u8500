clear;
close all;
%------------- Constants ---------------------------------------
epsilon=1e-10;
shortmax=32767;
%----------------------- Input information -------------------------------
out_name='sigref.bin';
%out_name=input('Enter file name for binary samples: ','s');
fso=48000;
%fso=input('Enter sampling frequency: ');
stereo=1;
%stereo=input('Enter stereo(1) or mono(0): '); 
fl=1000;
%fl=input('Enter left or mono channel frequency: ');
if (stereo==1)
    fr=2000;
    %fr=input('Enter right channel frequency: ');
end
duration=1;
%duration=input('Enter signal duration in seconds: ');
ampl=.707;
%ampl=input('Enter signal amplitude 0 to 1.0: ');
%----------------------- Generate signal samples ------------------------------
nsamples=ceil(duration*fso);
i=0:nsamples-1;
t=(1/fso)*i;
sigl=ampl*sin(2*pi*fl*t);
if (stereo==1)
    sigr=ampl*sin(2*pi*fr*t);
    sig(1:2:2*nsamples)=sigl(1:nsamples);
    sig(2:2:2*nsamples)=sigr(1:nsamples);
else
    sig=sigl;
end
sigi=round(sig*shortmax);
%--------------------- write to binary file --------------------------------------
fout=fopen(out_name,'wb');
fwrite(fout,sigi,'int16');
fclose('all');
