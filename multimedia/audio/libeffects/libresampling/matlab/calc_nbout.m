function [nbout,offset]=calc_nbout(nbin,M,L,offset);
%[nbout,offset] = calc_nbout(nbin,M,L,offset)
nbout=0;
if ((nbin*M)>offset)
	summ=nbin*M-offset-1;
	nbout=floor(summ/L)+1;
end
offset=offset-nbin*M+nbout*L;





