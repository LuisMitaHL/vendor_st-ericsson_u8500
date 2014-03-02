function [incr,offset]=src_state(M,L,offset);
%[incr,offset] = src_state(M,L,offset)


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

incr=incr_tab(offset+1);
offset=offset_tab(offset+1);







