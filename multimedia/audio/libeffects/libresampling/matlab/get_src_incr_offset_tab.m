function [src_incr_offset_tab]=get_src_incr_offset_tab(M,L);

%[src_incr_offset_tab]=get_src_incr_offset_tab(M,L) 
%24-bit word structure 00000000iiiiiiiioooooooo where i=binary incr value and o=binary offset value
%M is interpolation ratio and L is decimation ratio

for iter=0:M-1
	incrval=0;
	offsetval=iter;
	offsetval=offsetval+L;
	while (offsetval>=M)
		incrval=incrval+1;
		offsetval=offsetval-M;
	end
	offset_tab(iter+1)=offsetval;
	incr_tab(iter+1)=incrval;
end

for (iter=M:M+L-1)
	incrval=0;
	offsetval=iter;
	while (offsetval>=M)
		incrval=incrval+1;
		offsetval=offsetval-M;
	end
	offset_tab(iter+1)=offsetval;
	incr_tab(iter+1)=incrval;
end

[dummy sz]=size(offset_tab);
for iter=1:sz
	temp=(incr_tab(iter)*256) + (offset_tab(iter));
	src_incr_offset_tab(iter)=temp;
end





