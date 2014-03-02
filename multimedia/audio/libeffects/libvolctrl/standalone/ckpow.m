clear;
close all;
fid = fopen('../libvolctrl/volctrl_powtab.c','w');
NUMBITS=23;
MAXVAL=(2^NUMBITS)-1;
%***************************** calculations for alpha approximation ******************
e=2.71828182;
NTEST=10000;
%----------------- input -------------
fs=44100;
delta_gain_dB=-114;
maxTime=10000; % in msec
%maxTime=input('enter time in msec(250): ');
for iter=1:NTEST
	Time(iter)=maxTime*(iter/NTEST);
	Time(iter)=max(Time(iter),1);
	nb_iter=round(fs*Time(iter)/1000);
	nb_iter=max(nb_iter,1);
	d_dB=delta_gain_dB/(20*nb_iter);
	%----------------- total formula, alpha = 10^delta_dB -1 ------------------
	alpharef(iter)=10^d_dB-1;
	%------------------------------------------------------------------------
	% find 10^d_dB - 1 = e^x - 1
	% so that x=d_dB/log10(e)
	% e^x = 1 + x + x^2/2! + x^3/3! + ..... 
	% then alpha = x + x^2/2! + x^3/3! + ..... 
	%------------------------------------------------------------------------
	delta_gain_dB_code=delta_gain_dB*256; % in the C code
	coef=1/(256*20.*log10(e));
	x=delta_gain_dB_code*coef/nb_iter;  
	alpha(iter)=x+x^2/2+x^3/6+x^4/24; % 4 terms of the series is enough for the approximation
	%------------------------------------------------------------------------
	err(iter)=100*(alpharef(iter)-alpha(iter))/(alpharef(iter)-1e-12);
end
ex=0;
while (coef<.5)
	coef=coef*2;
	ex=ex+1;
end
fprintf(fid,'Float const EXTERN coef_volctrl=FORMAT_FLOAT(%.15f,MAXVAL); %d\n',coef);
fprintf(fid,'\n');
fprintf(fid,'#define Exp_coef %d\n',ex);

figure
plot(alpha,err);
hold on;
plot(alpha,err,'r*');
grid on;
zoom on;
title(['alpha %error vs alpha(min= ',num2str(alpha(NTEST)),' with corresponding %error= ',num2str(err(NTEST)),')']); 

%------------------ formula  ---------------------------------------------
% Approximation for y=1/(2*x) using Newton iterations
% If y=1/(2*x) then x=1/(2*y)
% Find zero of f(y)=1/(2*y)-x
% Given a starting value of y the next iteration y=y-f(y)/f(y)'
% But f(y)'=-1/(2*y^2)
% So substituting y=2*(y-x*y^2)




%***************************** calculations for alpha approximation ******************
tabsiz=4; % Look up table size 
numiter=8; % only 2 if tabsiz=64
numtest=10000;
%--------------- Generate the tables ---------
for i=1:tabsiz
	dd=.5+((i-1)/(2*tabsiz));
	tabvali(i)=round(MAXVAL/(2*dd));
	tabval(i)=1/(2*dd);
end
fprintf(fid,'#define MAXVAL_PLUS_ONE_OVER2 0x400000\n');
shiftval=22-log2(tabsiz);
fprintf(fid,'#define SHIFTVAL_INV %d\n',shiftval);
fprintf(fid,'#define numiter %d\n',numiter);
fprintf(fid,'Float const EXTERN volctrl_invtab[%d] = {\n',(tabsiz));
for k = 1:tabsiz-1
   fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',tabvali(k)/MAXVAL,k-1);
end
k = tabsiz;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',tabvali(k)/MAXVAL,k-1);
fclose(fid);
%--------------------------------------------
for iter=0:numtest-1
	den=.5+iter/(2*numtest);
	tabindex=floor(tabsiz*((den-.5)/.5));
	y=tabval(tabindex+1);
	for i=0:numiter-1
		temp=y;
		temp=temp*y;
		temp=temp*den;
		temp=y-temp;
		y=temp*2;
	end
	yref=1/(2*den);
	Den(iter+1)=den;
	err(iter+1)=floor((y-yref)*MAXVAL);
end
figure;
plot(Den,err);
grid on;
zoom on;
title(['num of error bits for approximation of 1/(2*x) vs x in Q',num2str(NUMBITS),' format']);



