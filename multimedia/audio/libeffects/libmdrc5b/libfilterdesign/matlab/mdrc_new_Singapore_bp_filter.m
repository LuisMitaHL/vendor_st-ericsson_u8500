%% 

close all;
clear all;

N = 1024; % number of points of the spectrum 
fs = 48e3; % sampling rate
n_order = 4; % taps of the butterworth filter

% 5-band cutoff frequency
% cutoff_fq =  [0, 230, 740, 1630, 4780];
% cutoff_fq= [0 310 1e3 3e3 6e3];
cutoff_fq = [0 600 1e3 3e3 6e3];

nn_bands = length(cutoff_fq);

%% 
fq_band = zeros(N, nn_bands);
h_lp = zeros(N, nn_bands - 1);
h_hp = zeros(N, nn_bands - 1);

% first band lowpass
[b,a] = butter(n_order, cutoff_fq(2)*2/fs, 'low');
fq_band(:, 1) = freqz(b,a,N);

for ii = 1 : nn_bands - 1
    [b, a] = butter( 4, cutoff_fq(ii+1)*2/fs, 'low');
    [h_lp(:, ii) , w] = freqz( b, a, N);
    
    [b, a] = butter( 4, cutoff_fq(ii+1)*2/fs, 'high');
    [h_hp(:, ii) , w] = freqz( b, a, N);
    
end

% compute the responce of band pass filter
for ii = 2 : nn_bands-1
    fq_band( :, ii ) = h_lp(: , ii) .* h_hp(:, ii - 1);
end

% last band high pass 
[b,a] = butter( n_order, cutoff_fq(end)*2/fs, 'high' );
[fq_band(:, nn_bands), w] = freqz( b, a, N );

% dB figure for all the frequency response
figure, 
for ii = 1 : nn_bands
    plot(w / pi * fs /2, 20*log10(abs(fq_band(:, ii)))), hold on;
end

hold on, plot(w / pi * fs /2, 20*log10(abs(sum(fq_band, 2))), 'r');
stem(cutoff_fq(2:end), -100*ones(1, nn_bands-1),'fill','--'), hold on;
plot(w / pi * fs /2, -3 * ones(1,N), '--');hold on;
plot(w / pi * fs /2, 3 * ones(1,N), '--');
ylim([-40, 5]);ylabel('dB'); xlabel('Hz');
% title(['cut-off fq:' num2str(cutoff_fq)]);
title(' current bandpass filter ');

%% new bandpass filter
fq_band1 = zeros(N, nn_bands);
% first band lowpass
[b,a] = butter(n_order, cutoff_fq(2)*2/fs, 'low');
fq_band1(:, 1) = freqz(b,a,N);

% band pass filter
for ii = 2 : nn_bands-1
    [b, a] = butter(n_order, [cutoff_fq(ii), cutoff_fq(ii+1)]/fs*2);
    fq_band1( :, ii ) = freqz(b,a,N);
end

% last band high pass 
[b,a] = butter( n_order, cutoff_fq(end)*2/fs, 'high' );
[fq_band1(:, nn_bands), w] = freqz( b, a, N );

% dB figure for all the frequency response
figure, 
for ii = 1 : nn_bands
    plot(w / pi * fs /2, 20*log10(abs(fq_band1(:, ii)))), hold on;
end

hold on, plot(w / pi * fs /2, 20*log10(abs(sum(fq_band1, 2))), 'r');
stem(cutoff_fq(2:end), -100*ones(1, nn_bands-1),'fill','--'), hold on;
plot(w / pi * fs /2, -3 * ones(1,N), '--');hold on;
plot(w / pi * fs /2, 3 * ones(1,N), '--');
ylim([-40, 5]);ylabel('dB'); xlabel('Hz');
title(' new bandpass filter ');


