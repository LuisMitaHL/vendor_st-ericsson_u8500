function y=poly2(ypts,x);
%function y=poly2(ypts,x);
%2nd order polynomial calculation approximation for y=f(x) with x input in range 0..2
%ypts is the vector containing 3 known values of y at abcisseses 0:2 respectively 

y2=ypts(2+1);
y1=ypts(1+1);
y0=ypts(0+1);

b2=y2-2*y1+y0;
b1=-y2+4*y1-3*y0;
b0=2*y0;
y=(b2*x^2+b1*x+b0)*.5;







