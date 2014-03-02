function [nbin,offset]=calc_nbin(nbout,M,L,offset);
%[nbin,offset] = calc_nbin(nbout,M,L,offset)
summ=offset+nbout*L-M-L;
if (summ<0)
	nbin=0;
else
	nbin=1+floor(summ/M);
end
offset=offset+nbout*L-nbin*M;

