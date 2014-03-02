function [dummy] = polezero(B,A,Title)
%------------------------------------------------------------------------------
% function = polezero(B,A,Title);
% displays poles(red) and zeros(blue) of the z-domain polynomial B/A with Title 
%------------------------------------------------------------------------------
Zeros=roots(B);
Poles=roots(A);
figure
plot(real(Zeros),imag(Zeros),'b*');
hold on;
plot(real(Poles),imag(Poles),'r*');
th=(0:.1:2*pi+.1);
line=cos(th)+j*sin(th);
plot(real(line),imag(line),'g');
r=1.5;
axis([-r r -r r]);
hold off;
grid on;
zoom on;
title([Title , ',Zeros in blue and Poles in red ']);

