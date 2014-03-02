function [order]=permorder(N)
% function [order]=permorder(N)
% find all possible orders of N elements
maxnum=N^N;
order=[ ];
val=0;
for i=1:maxnum
	num=val;
	for j=1:N
		digits(j)=rem(num,N)+1;
		num=floor(num/N);
	end
	for i=1:N
		numoccur=0;
		for (j=1:N)
			if (digits(j)==i)
				numoccur=numoccur+1;
			end
		end
		if ((numoccur>1)|(numoccur<1))
			break
		end
	end
	if (numoccur==1)
		order=[order; digits];
	end
	val=val+1;
end
