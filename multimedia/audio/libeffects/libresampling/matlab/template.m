function [pts] = template(f, val, N)
%function [pts] = template(f, val, N)
%---------------------------------------------------------------------------------
%create N pts for filter template using corner frequencies f and corner values val
%the last value of f corresponds to the Nth frequency value of the template
%---------------------------------------------------------------------------------
[dummy numf]=size(f);
lastfreq=f(numf);
xcorner=min(round(N*(f/lastfreq))+1,N);
[dummy ncorners]=size(xcorner);
pts=[ ];
for i=1:ncorners-1
	width=xcorner(i+1)-xcorner(i)+1;
	nextpts=linspace(val(i),val(i+1),width);
	pts=[pts nextpts(1:width-1)];
end
pts=[pts pts(N-1)];

