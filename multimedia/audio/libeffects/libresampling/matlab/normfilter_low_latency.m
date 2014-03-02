function [f] = normfilter_low_latency(vector, nbPhases)

% make sure sum of coefficients is exactly one
sum = 0;
for i=1:length(vector)
  sum = sum + vector(i);
end
sum
f = vector / sum;
return;
