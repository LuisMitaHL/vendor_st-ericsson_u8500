clear;
close all;
NBITER=1000;
M=34;
M=input('Enter value M: ');
L=37;
L=input('Enter value L: ');
NBOUT=32;
NBOUT=input('Target number of output samples: ');
offset=M;
%------------ Test calcul for of nbconsumed vs NBOUT ----
for iter=1:NBITER
    %------ This is the magic formula ---------
    nbconspred=ceil((offset+(NBOUT*L)-L-M+1)/M);
    %------------------------------------------
    nbOut=0;j=0;
    while (nbOut<NBOUT)
        if (offset>=M)
            %--- get input ---
            offset=offset-M;
            j=j+1;
        else
            %---- calc out ---
            offset=offset+L;
            nbOut=nbOut+1;
        end
    end
    nbconsumed=j;
    NUMOUT(iter)=nbOut;
    PREDERR(iter)=nbconspred-j;
    NUMCONSUMED(iter)=nbconsumed;
end
avgnumout=mean(NUMOUT);
avgnumout
devnumout=max(NUMOUT)-min(NUMOUT);
devnumout
maxprederr=max(abs(PREDERR));
maxprederr
avgnumconsumed=mean(NUMCONSUMED);
avgnumconsumed
