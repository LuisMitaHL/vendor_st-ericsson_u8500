function y=poly3(ypts,x);
%function y=poly3(ypts,x);
%3rd order polynomial calculation approximation for y=f(x) with x input in range 0..3
%ypts is the vector containing 4 known values of y at abcisseses 0:3 respectively 

y3=ypts(3+1);
y2=ypts(2+1);
y1=ypts(1+1);
y0=ypts(0+1);

b3=y3-3*y2+3*y1-y0;
b2=-3*y3+12*y2-15*y1+6*y0;
b1=2*y3-9*y2+18*y1-11*y0;
b0=6*y0;
y=(b3*x^3+b2*x^2+b1*x+b0)*.166666666667;






