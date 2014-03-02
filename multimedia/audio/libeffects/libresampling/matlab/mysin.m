N = 96;
N1 = N/2

for i=1:N
   input(i) = 0;
end

PI = atan(1)*4;
for i=1:N
input(i) = sin(2*PI*(N1+i-1)/N1)/2;
end


fid = fopen('sinTable.c','w');
fprintf(fid,'Float const YMEM sinTable[%d] = {\n', N);
for k = 1:N-1
     fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL),  \t\t// %d\n',input(k),k-1);
end
k = N;
fprintf(fid,'\tFORMAT_FLOAT(%.15f,MAXVAL) };  \t\t// %d\n',input(k),k-1);
