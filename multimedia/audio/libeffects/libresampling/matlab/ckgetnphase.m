clear;
close all;
coefperphase=100;%depends on desired template
%phasenum=[12 37];
phasenum=[12 7 9];
%phasenum=[12 7];
%phasenum=[7 9];
totalcoefs=sum(phasenum)*coefperphase;
maxperr=.1;
finlist=[8000,11025,12000,16000,22050,24000,32000,44100,48000,64000,88200,96000];
foutlist=[8000,11025,12000,16000,22050,24000,32000,44100,48000];
[dummy,numin]=size(finlist);
[dummy,numout]=size(foutlist);
Fin = fopen('srcratio.rtf','w');
[dummy,nbcoeftables]=size(phasenum);
fprintf(Fin,'SRC HIFI NCOEFS AND MIPS EVALUATION FOR POLYPHASE TABLES:');
for k=1:nbcoeftables
	fprintf(Fin,'%d,',phasenum(k));
end
fprintf(Fin,'\ntotalcoefs:%d (you can divide by 2 if coefs are symmetrical)\n',totalcoefs);
fprintf(Fin,'-----------------------------------------------------------------\n\n\n');

MIPS=[ ];
NUMSTAGE=[ ];
for i=1:numin
	for j=1:numout
		fin=finlist(i);
		fout=foutlist(j);
		[numerator,denominator]=getnphase(fin,fout,phasenum,maxperr);
		[dummy numstage]=size(numerator);
		NUMSTAGE=[NUMSTAGE,numstage];
		mips=0;
		nsmp=1;
		freqin=fin;
		for k=1:numstage
			freqout=freqin*numerator(k)/denominator(k);
			nphs=max(numerator(k),denominator(k));
			ncoefs=nphs*coefperphase;
			if (freqout>=freqin)
				mips=mips+(((fin*ncoefs/nphs)*nsmp*numerator(k)/denominator(k))/1e6);	
			else
				mips=mips+((fin*ncoefs/nphs)*nsmp/1e6);
			end
			nsmp=nsmp*numerator(k)/denominator(k);
			freqin=freqout;
		end
		fprintf(Fin,'%d->%d cascades:%d',fin,fout,numstage);
		for k=1:numstage
			fprintf(Fin,',%d/%d',numerator(k),denominator(k));
		end
		fprintf(Fin,',mipsfigure:%d\n',floor(mips));
		MIPS=[MIPS,mips];
	end
end
avgmips=mean(MIPS);
stdmips=std(MIPS);
avgnumstage=mean(NUMSTAGE);
stdnumstage=std(NUMSTAGE);
fprintf(Fin,'\navgmips: %d stdmips: %d avgnumstage: %d stdnumstage: %d\n',avgmips,stdmips,avgnumstage,stdnumstage);
fprintf(Fin,'---------------------------------------------------------------------------------------------------\n\n');
fclose(Fin);
