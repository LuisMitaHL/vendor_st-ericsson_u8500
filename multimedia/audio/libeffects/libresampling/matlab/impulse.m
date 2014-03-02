% ratio 21/20 
Fs = 48000*21
Fpass = 19000;
Fstop = 24000; % keep original bw
Rp = 0.1;
Rs = 96;
[n1,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [Rp/20 10^(-Rs/20)], Fs );
n_21_20 = (floor(n1/21/2)+1)*21*2

% remez returns n+1 order, force to even value
deci1_21_20 = remez(n_21_20-1,fo,mo,w);
[deci_21_20 shift_21_20] = normfilter(deci1_21_20,21);
freqz(deci1_21_20); zoom;

% compute max delay
delay_48_21_20= n_21_20 / 21

% ratio 7/8:   
Fs = 48000*21/20*7
Fpass = 19000;
Fstop = 22050; % avoid aliasing
Rp = 0.1;
Rs = 96;

[n3,fo,mo,w] = remezord( [Fpass Fstop], [1 0], [Rp/20 10^(-Rs/20)], Fs );
n_7_8 = (floor(n3/7/2)+1)*7*2

% remez returns n+1 order, force to even value
deci1_7_8 = remez(n_7_8-1,fo,mo,w);
[deci_7_8 shift_7_8] = normfilter(deci1_7_8,7);
freqz(deci1_7_8); zoom;

input(1) = 1;
res1 = upfirdn(input,deci1_21_20,21,20);
freqz(res1*20);
figure;
res2 = upfirdn(res1,deci1_7_8,7,8);
freqz(res2*20*8);

