function [f,shift] = normfilter(vector, nbPhases)

% make sure sum of coefficients is exactly one
sum = 0;
for i=1:length(vector)
  sum = sum + vector(i);
end
sum
vector = vector / sum;

% correct for upsampling addition of zeroes
vector = vector * nbPhases;

% find maximum dynamics for filter
shift = 0;
n1 = max(vector);
shift = 0;
while( n1 > 1)
  shift = shift-1;
  n1 = n1/2;
end
while( n1 < 0.5 )
  shift = shift+1;
  n1 = n1*2;
end

% limit scaling to 8 bits to avoid wrap-around in the
% accumulation (MMDSP has 8 bit extension)
while ( shift > 8 )
  print "Shift > 8 wrap-around may occur\n";
  shift = shift - 1;
end

f = vector*(2^shift);

return;