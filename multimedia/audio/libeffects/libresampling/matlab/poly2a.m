function y=poly2a(ypts,x);
%function y=poly2a(ypts,x);
%2nd order polynomial calculation approximation for y=f(x) with x input in range 0..2
%ypts is the vector containing 3 known values of y at abcisseses 0:2 respectively 

%------------------------------- 2nd order Lagrangian Interpolation --------------------------------------------
%l0=  (x-x1)*(x-x2)
%    ---------------
%    (x0-x1)*(x0-x2)

%l1=  (x-x0)*(x-x2)
%    ---------------
%    (x1-x0)*(x1-x2)

%l2=  (x-x0)*(x-x1)
%    ---------------
%    (x2-x0)*(x2-x1)

%where the 3 given points are (x0,y0); (x1,y1); (x2,y2) and x0=0,x1=1,x2=2 and x is the value for interpolating y
%so y=y0*l0 + y1*l1 + y2*l2
%------------------------------------------------------------------------------------------------------------------


y2=ypts(2+1);
y1=ypts(1+1);
y0=ypts(0+1);


B=     x  -  1;
C=  .5*x  -  1;


l0=        B*C;
l1=      2*x*C;
l2=      4*x*B;


y = y0*l0 - y1*l1 + y2*l2/8;








































%------------ Polyval ---------------------
%b3=y3-3*y2+3*y1-y0;
%b2=-3*y3+12*y2-15*y1+6*y0;
%b1=2*y3-9*y2+18*y1-11*y0;
%b0=6*y0;
%y=(b3*x^3+b2*x^2+b1*x+b0)*.166666666667;

%----------- Develped Lagrange -------------
%x1=x;
%x2=x^2;
%x3=x^3;
%l0=-y0*  (x3  -6*x2  +11*x1  -6);
%l1=y1* (3*x3 -15*x2  +18*x1    );
%l2=y2*(-3*x3 +12*x2   -9*x1    );
%l3=y3*   (x3  -3*x2   +2*x1    );
%y=(l0+l1+l2+l3)*.16666666666667;
