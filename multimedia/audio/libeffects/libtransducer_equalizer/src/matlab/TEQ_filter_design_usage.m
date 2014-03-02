%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% This matlab script is an example of transducer equalizer filter design usage with 3 different methods :
% 1 - biquad IIR (Infinite Impulse Response) filter design with "TEQ_filter_design(0, ...)" matlab command
% 2 - linear phase FIR (Finite Impulse Response) filter design with "TEQ_filter_design(1, ...)" matlab command
% 3 - minimum phase FIR filter design with "TEQ_filter_design(2, ...)" matlab command
%
%
% TEQ_filter_design matlab script has the following interface :
% - filter_type     : 0 for "biquad", 1 for "linear phase FIR", 2 for "minimum phase FIR"
% - name            : name of the filter to design
% - n               : number of points for the filter response computation and display
% - order_l         : left filter order
% - gain_table_l    : table of left linear gains
% - freq_table_l    : table of left frequencies (hertz)
% - order_r         : right filter order
% - gain_table_r    : table of right linear gains
% - freq_table_r    : table of right frequencies (hertz)
% - sampling_freq   : sampling frequency (hertz)
% - stereo          : 0 for mono filter, 1 for stereo filter
% - same_filter_l_r : 0 for different left & right filters, 1 for same left & right filters
%
% Remarks :
% - gain_table_l and freq_table_l must have the same size
% - gain_table_r and freq_table_r must have the same size
% - gain_table_l and gain_table_r must contain positive values
% - freq_table_l and freq_table_r must be positive, sorted in ascending order and must be < sampling_freq / 2
% - if stereo = 0 : order_r, gain_table_r, freq_table_r and same_filter_l_r are ignored
% - if stereo = 1 and same_filter_l_r = 1 : order_r, gain_table_r and freq_table_r are ignored
% - for "biquad"; order_l and order_r are the number of cascaded left and right biquads cells
% - for "linear phase FIR" and "minimum phase FIR"; order_l and order_r are the number of left and right FIR filter coefficients
%   (the actual number of coefficients will be the first odd number greater or equal to the asked number)
%
%
% Filter design algorithm will do its best to generate a filter as close as possible to the filter response constraints
% but there is no guaranty that it will match exactly the constraints.
%
% The constraint is expressed by order_x and the couple of tables gain_table_x and freq_table_x (with x = l or r) :
%     gain = gain_table_x(i) for frequency = freq_table_x(i) with i=1...size(gain_table_x)
%
%
% Each of these 3 filter design methods generates 5 different text files :
% - <name>_filter.txt file     : raw 32 bits fixed-point filter parameters
% - param_aep_<name>.txt file  : AEP  like formated (for eil standalone tests)   24 bits fixed-point filter parameters
% - param_ate_<name>.txt file  : ATE  like formated (for hamaca tests)           24 bits fixed-point filter parameters
% - param_nmf_<name>.txt file  : NMF  like formated (for nmfil standalone tests) 32 bits fixed-point filter parameters
% - param_mmte_<name>.txt file : MMTE like formated (for board tests)            32 bits fixed-point filter parameters
%
% it also display 4 different curves for both 24 bits (for MMDSP) & 32 bits (for ARM NEON) fixed-point filter design parameters
% depending on signal frequency (hertz) :
% - linear filter response in red with constraints in blue (top left)
% - decibel filter response in green with constraints in blue (top right)
% - phase response in blue (bottom left)
% - group delay in cyan (bottom right)
%
%
% biquad filter equation :
%                               order-1  [b0(i) + b1(i).z^-1 + b2(i).z^-2].2^b_exp(i)
% H(z) = gain_mant.2^gain_exp . Product ----------------------------------------------
%                                i=0        1 + 2.a1(i).z^-1 + a2(i).z^-2
%
% FIR filter equation :
%                              order-1
% H(z) = gain_mant.2^gain_exp .  Sum  coef(i).z^-i
%                                i=0
%
%
% Each kind of filter has its own advantages & disadvantages :
% - biquad (i.e. TEQ_filter_design(0, ...) :
%     - good matching to response constraints with relatively low order => processing cycles efficiency
%     - low algorithmic delay (good for delay telephony constraints) except for band transitions
%     - phase distorsion (no problem for voice signal, may be an issue for music signal)
%     - small risk of filter divergence (due to recursive aspect of IIR filtering combined with fixed-point arithmetic errors propagation)
% - linear phase FIR (i.e. TEQ_filter_design(1, ...) :
%     - need higher filter order than biquad to match response constraints => lower processing cycles efficiency
%     - higher algorithmic delay (order/2 samples), may be an issue for telephony delay constraints
%     - no phase distorsion (good for both voice signal and music signal)
%     - no risk of filter divergence
% - minimum phase FIR (i.e. TEQ_filter_design(2, ...) :
%     - need higher filter order than biquad to match response constraints => lower processing cycles efficiency
%     - low algorithmic delay (good for delay telephony constraints) except for band transitions
%     - phase distorsion (no problem for voice signal, may be an issue for music signal)
%     - complex filter design (risk of divergence during filter design, thus filter design is difficult to embed)
%       but if design is done offline, convergence is checked offline, thus if it is OK there is no risk
%       of filter divergence during samples filtering
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



% Examples of usage of biquad, linear_phase_FIR and minimum_phase_FIR filter design for
% mono, stereo with same left & right filters and stereo with different left & right filters.

%                    name                             n     order_l gain_table_l                       freq_table_l                                     order_r gain_table_r                         freq_table_r                                     sampling_freq stereo same_filter_l_r

% biquad mono
TEQ_filter_design(0, 'biquad_8k_mono',                4096, 10,     [0 0 0.6 1 1 0.8 0.7 0.5 0.7],     [0 200 300 500 700 1000 2000 3000 4000],         10,     [0 0 0.6 1 1 0.8 0.7 0.5 0.7],       [0 200 300 500 700 1000 2000 3000 4000],         8000,         0,      1);
fprintf(1, 'next ?');
pause;
fprintf(1, '\n');
% biquad stereo with same left & right filters
TEQ_filter_design(0, 'biquad_44k_stereo1',            4096, 10,     [1 1 1 1 0.5 0.3 0.2 0.1 0.1 0.1], [0 200 500 700 1000 2000 4000 8000 16000 22050], 10,     [1 1 1 1 0.5 0.3 0.2 0.1 0.1 0.1],   [0 200 500 700 1000 2000 4000 8000 16000 22050], 44100,        1,      1);
fprintf(1, 'next ?');
pause;
fprintf(1, '\n');
% biquad stereo with different left & right filters
TEQ_filter_design(0, 'biquad_44k_stereo2',            4096, 10,     [1 1 1 1 0.5 0.3 0.2 0.1 0.1 0.1], [0 200 500 700 1000 2000 4000 8000 16000 22050], 10,     [0.1 0.1 0.1 0.1 0.2 0.3 0.5 1 1 1], [0 200 500 700 1000 2000 4000 8000 16000 22050], 44100,        1,      0);
fprintf(1, 'next ?');
pause;
fprintf(1, '\n');

% linear phase FIR mono
TEQ_filter_design(1, 'linear_phase_FIR_8k_mono',      4096, 50,     [0 0 0.6 1 1 0.8 0.7 0.5 0.7],     [0 200 300 500 700 1000 2000 3000 4000],         50,     [0 0 0.6 1 1 0.8 0.7 0.5 0.7],       [0 200 300 500 700 1000 2000 3000 4000],         8000,         0,     1);
fprintf(1, 'next ?');
pause;
fprintf(1, '\n');
% linear phase FIR stereo with same left & right filters
TEQ_filter_design(1, 'linear_phase_FIR_44k_stereo1',  4096, 50,     [1 1 1 1 0.5 0.3 0.2 0.1 0.1 0.1], [0 200 500 700 1000 2000 4000 8000 16000 22050], 50,     [1 1 1 1 0.5 0.3 0.2 0.1 0.1 0.1],   [0 200 500 700 1000 2000 4000 8000 16000 22050], 44100,        1,     1);
fprintf(1, 'next ?');
pause;
fprintf(1, '\n');
% linear phase FIR stereo with different left & right filters
TEQ_filter_design(1, 'linear_phase_FIR_44k_stereo2',  4096, 50,     [1 1 1 1 0.5 0.3 0.2 0.1 0.1 0.1], [0 200 500 700 1000 2000 4000 8000 16000 22050], 50,     [0.1 0.1 0.1 0.1 0.2 0.3 0.5 1 1 1], [0 200 500 700 1000 2000 4000 8000 16000 22050], 44100,        1,     0);
fprintf(1, 'next ?');
pause;
fprintf(1, '\n');

% minimum phase FIR mono
TEQ_filter_design(2, 'minimum_phase_FIR_8k_mono',     4096, 50,     [0 0 0.6 1 1 0.8 0.7 0.5 0.7],     [0 200 300 500 700 1000 2000 3000 4000],         50,     [0 0 0.6 1 1 0.8 0.7 0.5 0.7],       [0 200 300 500 700 1000 2000 3000 4000],         8000,         0,     1);
fprintf(1, 'next ?');
pause;
fprintf(1, '\n');
% minimum phase FIR stereo with same left & right filters
TEQ_filter_design(2, 'minimum_phase_FIR_44k_stereo1', 4096, 50,     [1 1 1 1 0.5 0.3 0.2 0.1 0.1 0.1], [0 200 500 700 1000 2000 4000 8000 16000 22050], 50,     [1 1 1 1 0.5 0.3 0.2 0.1 0.1 0.1],   [0 200 500 700 1000 2000 4000 8000 16000 22050], 44100,        1,     1);
fprintf(1, 'next ?');
pause;
fprintf(1, '\n');
% minimum phase FIR stereo with different left & right filters
TEQ_filter_design(2, 'minimum_phase_FIR_44k_stereo2', 4096, 50,     [1 1 1 1 0.5 0.3 0.2 0.1 0.1 0.1], [0 200 500 700 1000 2000 4000 8000 16000 22050], 50,     [0.1 0.1 0.1 0.1 0.2 0.3 0.5 1 1 1], [0 200 500 700 1000 2000 4000 8000 16000 22050], 44100,        1,     0);
