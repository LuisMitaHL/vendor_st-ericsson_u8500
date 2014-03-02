clear;
close all;
NBITER=1000;
M=34;
M=input('Enter value M: ');
L=37;
L=input('Enter value L: ');
offset=M;
NBIN=64;
%NBIN=input('Number of input samples: ');
maxoffsetout=0;
minoffsetout=1000;
maxoffsetin=0;
minoffsetin=1000;
error=0;
%------------ Test calcul for nbin and nbout ----
for iter=1:NBITER  
    %------------------------------------------
    nb=0;j=0;
    nbpred=ceil(((NBIN*M)-offset)/L);
    while (j<NBIN)
        if (offset<M) 
            %---- calc out ---
            offset=offset+L;
            if (offset>maxoffsetout)
                maxoffsetout=offset;
            end
            if (offset<minoffsetout)
                minoffsetout=offset;
            end
            nb=nb+1;
        else
            %--- get input ---
            offset=offset-M;
            if (offset>maxoffsetin)
                maxoffsetin=offset;
            end
            if (offset<minoffsetin)
                minoffsetin=offset;
            end
            j=j+1;
        end
    end
    if (nb~=nbpred)
        error=1;
    end
end
error
%maxoffsetout
%minoffsetout
%maxoffsetin
%minoffsetin