function TEQ_filter_design(filter_type, name, n, order_l, gain_table_l, freq_table_l, order_r, gain_table_r, freq_table_r, sampling_freq, stereo, same_filter_l_r)

    if (stereo ~= 0) && (same_filter_l_r == 0)
        fprintf(1, 'left ');
        compute_right = 1;
    else
        compute_right = 0;
    end

    % left FIR filter computation
    switch filter_type
        case 0
            fprintf(1, 'biquad %s', name);
            [nb_biquad_cells_l sos_filter_l_23 g_sos_filter_l_23 exp_g_sos_filter_l_23 b_filter_l_23 a_filter_l_23 sos_filter_l_31 g_sos_filter_l_31 exp_g_sos_filter_l_31 b_filter_l_31 a_filter_l_31] = compute_biquad(order_l, gain_table_l, freq_table_l, sampling_freq, n);
        case 1
            fprintf(1, 'linear phase FIR %s', name);
            [nb_FIR_coefs_l g_filter_l_23 exp_g_filter_l_23 b_filter_l_23 g_filter_l_31 exp_g_filter_l_31 b_filter_l_31] = compute_linear_phase_FIR(order_l, gain_table_l, freq_table_l, sampling_freq, n);
        case 2
            fprintf(1, 'minimum phase FIR %s', name);
            [nb_FIR_coefs_l g_filter_l_23 exp_g_filter_l_23 b_filter_l_23 g_filter_l_31 exp_g_filter_l_31 b_filter_l_31] = compute_minimum_phase_FIR(order_l, gain_table_l, freq_table_l, sampling_freq, n);
        otherwise
            fprintf(1, 'unknown filter type\n');
            return;
    end

    % left filter response display
    if filter_type == 0
        display_biquad(0, n, gain_table_l, freq_table_l, b_filter_l_23, a_filter_l_23, b_filter_l_31, a_filter_l_31, sampling_freq);
    else
        display_FIR(0, n, gain_table_l, freq_table_l, g_filter_l_23 * pow2(exp_g_filter_l_23) * b_filter_l_23, g_filter_l_31 * pow2(exp_g_filter_l_31) * b_filter_l_31, sampling_freq);
    end
    fprintf(1, ' done\n');

    if compute_right == 1
        % right filter computation
        fprintf(1, 'right ');
        switch filter_type
            case 0
                fprintf(1, 'biquad %s', name);
                [nb_biquad_cells_r sos_filter_r_23 g_sos_filter_r_23 exp_g_sos_filter_r_23 b_filter_r_23 a_filter_r_23 sos_filter_r_31 g_sos_filter_r_31 exp_g_sos_filter_r_31 b_filter_r_31 a_filter_r_31] = compute_biquad(order_r, gain_table_r, freq_table_r, sampling_freq, n);
            case 1
                fprintf(1, 'linear phase FIR %s', name);
                [nb_FIR_coefs_r g_filter_r_23 exp_g_filter_r_23 b_filter_r_23 g_filter_r_31 exp_g_filter_r_31 b_filter_r_31] = compute_linear_phase_FIR(order_r, gain_table_r, freq_table_r, sampling_freq, n);
            case 2
                fprintf(1, 'minimum phase FIR %s', name);
                [nb_FIR_coefs_r g_filter_r_23 exp_g_filter_r_23 b_filter_r_23 g_filter_r_31 exp_g_filter_r_31 b_filter_r_31] = compute_minimum_phase_FIR(order_r, gain_table_r, freq_table_r, sampling_freq, n);
        end

        % right filter response display
        if filter_type == 0
            display_biquad(2, n, gain_table_r, freq_table_r, b_filter_r_23, a_filter_r_23, b_filter_r_31, a_filter_r_31, sampling_freq);
        else
            display_FIR(2, n, gain_table_r, freq_table_r, g_filter_r_23 * pow2(exp_g_filter_r_23) * b_filter_r_23, g_filter_r_31 * pow2(exp_g_filter_r_31) * b_filter_r_31, sampling_freq);
        end
        fprintf(1, ' done\n');
    else
        if filter_type == 0
            nb_biquad_cells_r     = nb_biquad_cells_l;
            sos_filter_r_23       = sos_filter_l_23;
            g_sos_filter_r_23     = g_sos_filter_l_23;
            exp_g_sos_filter_r_23 = exp_g_sos_filter_l_23;
            b_filter_r_23         = b_filter_l_23;
            a_filter_r_23         = a_filter_l_23;
            sos_filter_r_31       = sos_filter_l_31;
            g_sos_filter_r_31     = g_sos_filter_l_31;
            exp_g_sos_filter_r_31 = exp_g_sos_filter_l_31;
            b_filter_r_31         = b_filter_l_31;
            a_filter_r_31         = a_filter_l_31;
        else
            nb_FIR_coefs_r        = nb_FIR_coefs_l;
            g_filter_r_23         = g_filter_l_23;
            exp_g_filter_r_23     = exp_g_filter_l_23;
            b_filter_r_23         = b_filter_l_23;
            g_filter_r_31         = g_filter_l_31;
            exp_g_filter_r_31     = exp_g_filter_l_31;
            b_filter_r_31         = b_filter_l_31;
        end
    end

    % filter coefficients save
    if filter_type == 0
        save_biquad(name, nb_biquad_cells_l, sos_filter_l_23, g_sos_filter_l_23, exp_g_sos_filter_l_23, b_filter_l_23, a_filter_l_23, sos_filter_l_31, g_sos_filter_l_31, exp_g_sos_filter_l_31, b_filter_l_31, a_filter_l_31, nb_biquad_cells_r, sos_filter_r_23, g_sos_filter_r_23, exp_g_sos_filter_r_23, b_filter_r_23, a_filter_r_23, sos_filter_r_31, g_sos_filter_r_31, exp_g_sos_filter_r_31, b_filter_r_31, a_filter_r_31, stereo, same_filter_l_r);
    else
        save_FIR(name, nb_FIR_coefs_l, g_filter_l_23, exp_g_filter_l_23, b_filter_l_23, g_filter_l_31, exp_g_filter_l_31, b_filter_l_31, nb_FIR_coefs_r, g_filter_r_23, exp_g_filter_r_23, b_filter_r_23, g_filter_r_31, exp_g_filter_r_31, b_filter_r_31, stereo, same_filter_l_r);
    end




% biquad filter computation
% -------------------------

function [nb_biquad_cells sos_filter23 g_sos_filter23 exp_g_sos_filter23 b_filter23 a_filter23 sos_filter31 g_sos_filter31 exp_g_sos_filter31 b_filter31 a_filter31] = compute_biquad(biquad_order, gain_table, freq_table, sampling_freq, n)

    % floating point computation

    filter_order                    = max(2, 2 * biquad_order);
    [b a]                           = yulewalk(filter_order, freq_table / sampling_freq * 2, gain_table);
    [sos_filter_up g_sos_filter_up] = tf2sos(b, a, 'up');
    nb_biquad_cells                 = size(sos_filter_up, 1);

    % 24 bits fixed-point
    % -------------------
    h_sos_filter                    = ones(n, 1);
    sos_filter23                    = sos_filter_up;
    g_sos_filter23                  = g_sos_filter_up;
    for i = 1 : nb_biquad_cells
        [h_sos_filter_tmp w_sos_filter_tmp] = freqz(sos_filter23(i, 1:3), sos_filter23(i, 4:6), n);
        h_sos_filter                        = h_sos_filter .* h_sos_filter_tmp;
        g                                   = max(abs(h_sos_filter));
        h_sos_filter                        = h_sos_filter / g;
        sos_filter23(i, 1:3)                = sos_filter23(i, 1:3) / g;
        g_sos_filter23                      = g_sos_filter23 * g;
    end

    exp_g_sos_filter23 = ceil(log2(g_sos_filter23));
    g_sos_filter23     = round_n(23, g_sos_filter23 * pow2(23 - exp_g_sos_filter23)) / pow2(23);
    b_filter23         = [g_sos_filter23 * pow2(exp_g_sos_filter23)];
    a_filter23         = [1];
    for i = 1 : nb_biquad_cells
        tmp                = ceil(log2(max(abs(sos_filter23(i, 1:3)))));
        sos_filter23(i, 1) = round_n(23, sos_filter23(i, 1) * pow2(23 - tmp)) / pow2(23 - tmp);
        sos_filter23(i, 2) = round_n(23, sos_filter23(i, 2) * pow2(23 - tmp)) / pow2(23 - tmp);
        sos_filter23(i, 3) = round_n(23, sos_filter23(i, 3) * pow2(23 - tmp)) / pow2(23 - tmp);
        sos_filter23(i, 5) = round_n(23, sos_filter23(i, 5) * pow2(22))       / pow2(22);
        sos_filter23(i, 6) = round_n(23, sos_filter23(i, 6) * pow2(23))       / pow2(23);
        b_filter23         = conv(b_filter23, sos_filter23(i, 1:3));
        a_filter23         = conv(a_filter23, sos_filter23(i, 4:6));
    end

    % 32 bits fixed-point
    % -------------------
    h_sos_filter                    = ones(n, 1);
    sos_filter31                    = sos_filter_up;
    g_sos_filter31                  = g_sos_filter_up;
    for i = 1 : nb_biquad_cells
        [h_sos_filter_tmp w_sos_filter_tmp] = freqz(sos_filter31(i, 1:3), sos_filter31(i, 4:6), n);
        h_sos_filter                        = h_sos_filter .* h_sos_filter_tmp;
        g                                   = max(abs(h_sos_filter));
        h_sos_filter                        = h_sos_filter / g;
        sos_filter31(i, 1:3)                = sos_filter31(i, 1:3) / g;
        g_sos_filter31                      = g_sos_filter31 * g;
    end

    exp_g_sos_filter31 = ceil(log2(g_sos_filter31));
    g_sos_filter31     = round_n(31, g_sos_filter31 * pow2(31 - exp_g_sos_filter31)) / pow2(31);
    b_filter31         = [g_sos_filter31 * pow2(exp_g_sos_filter31)];
    a_filter31         = [1];
    for i = 1 : nb_biquad_cells
        tmp                = ceil(log2(max(abs(sos_filter31(i, 1:3)))));
        sos_filter31(i, 1) = round_n(31, sos_filter31(i, 1) * pow2(31 - tmp)) / pow2(31 - tmp);
        sos_filter31(i, 2) = round_n(31, sos_filter31(i, 2) * pow2(31 - tmp)) / pow2(31 - tmp);
        sos_filter31(i, 3) = round_n(31, sos_filter31(i, 3) * pow2(31 - tmp)) / pow2(31 - tmp);
        sos_filter31(i, 5) = round_n(31, sos_filter31(i, 5) * pow2(30))       / pow2(30);
        sos_filter31(i, 6) = round_n(31, sos_filter31(i, 6) * pow2(31))       / pow2(31);
        b_filter31         = conv(b_filter31, sos_filter31(i, 1:3));
        a_filter31         = conv(a_filter31, sos_filter31(i, 4:6));
    end



% linear phase FIR filter computation
% -----------------------------------

function [nb_FIR_coefs g_filter23 exp_g_filter23 b_filter23 g_filter31 exp_g_filter31 b_filter31] = compute_linear_phase_FIR(FIR_order, gain_table, freq_table, sampling_freq, n)

    % floating point computation
    % --------------------------
    FIR_order           = max(2, 2 * floor(FIR_order / 2));
    b                   = fir2(FIR_order, freq_table / sampling_freq * 2, gain_table);
    nb_FIR_coefs        = size(b, 2);
    [h_filter w_filter] = freqz(b, [1], n);
    gain                = max(abs(h_filter));
    b                   = b / gain;
    max_b               = max(1, max(abs(b)));

    % 24 & 32 bits fixed-point
    % ------------------------
    g_filter23     = (pow2(23) - 1) / pow2(23) * max_b;
    g_filter31     = (pow2(31) - 1) / pow2(31) * max_b;
    exp_g_filter23 = ceil(log2(g_filter23));
    exp_g_filter31 = ceil(log2(g_filter31));
    g_filter23     = round_n(23, g_filter23 * pow2(23 - exp_g_filter23)) / pow2(23);
    g_filter31     = round_n(31, g_filter31 * pow2(31 - exp_g_filter31)) / pow2(31);
    b_filter23     = b / (g_filter23 * pow2(exp_g_filter23));
    b_filter31     = b / (g_filter31 * pow2(exp_g_filter31));
    for i = 1 : nb_FIR_coefs
        b_filter23(i) = round_n(23, b_filter23(i) * pow2(23)) / pow2(23);
        b_filter31(i) = round_n(31, b_filter31(i) * pow2(31)) / pow2(31);
    end
    g_filter23     = g_filter23 * gain * pow2(exp_g_filter23);
    g_filter31     = g_filter31 * gain * pow2(exp_g_filter31);
    exp_g_filter23 = ceil(log2(g_filter23));
    exp_g_filter31 = ceil(log2(g_filter31));
    g_filter23     = round_n(23, g_filter23 * pow2(23 - exp_g_filter23)) / pow2(23);
    g_filter31     = round_n(31, g_filter31 * pow2(31 - exp_g_filter31)) / pow2(31);




% minimum phase FIR filter computation
% ------------------------------------

function [nb_FIR_coefs g_filter23 exp_g_filter23 b_filter23 g_filter31 exp_g_filter31 b_filter31] = compute_minimum_phase_FIR(FIR_order, gain_table, freq_table, sampling_freq, n)

    % floating point computation
    % --------------------------
    FIR_order           = max(2, 2 * floor(FIR_order / 2));
    % gain_table is squared because linear_phase2minimum_phase computes the square root
    lin_ph_b            = fir2(2 * FIR_order, freq_table / sampling_freq * 2, gain_table .* gain_table);
    nb_FIR_coefs        = size(lin_ph_b, 2);
    b                   = linear_phase2minimum_phase(lin_ph_b(floor((nb_FIR_coefs + 1) / 2) : nb_FIR_coefs));
    nb_FIR_coefs        = size(b, 2);
    [h_filter w_filter] = freqz(b, [1], n);
    gain                = max(abs(h_filter));
    b                   = b / gain;
    max_b               = max(1, max(abs(b)));

    % 24 & 32 bits fixed-point
    % ------------------------
    g_filter23     = (pow2(23) - 1) / pow2(23) * max_b;
    g_filter31     = (pow2(31) - 1) / pow2(31) * max_b;
    exp_g_filter23 = ceil(log2(g_filter23));
    exp_g_filter31 = ceil(log2(g_filter31));
    g_filter23     = round_n(23, g_filter23 * pow2(23 - exp_g_filter23)) / pow2(23);
    g_filter31     = round_n(31, g_filter31 * pow2(31 - exp_g_filter31)) / pow2(31);
    b_filter23     = b / (g_filter23 * pow2(exp_g_filter23));
    b_filter31     = b / (g_filter31 * pow2(exp_g_filter31));
    for i = 1 : nb_FIR_coefs
        b_filter23(i) = round_n(23, b_filter23(i) * pow2(23)) / pow2(23);
        b_filter31(i) = round_n(31, b_filter31(i) * pow2(31)) / pow2(31);
    end
    g_filter23     = g_filter23 * gain * pow2(exp_g_filter23);
    g_filter31     = g_filter31 * gain * pow2(exp_g_filter31);
    exp_g_filter23 = ceil(log2(g_filter23));
    exp_g_filter31 = ceil(log2(g_filter31));
    g_filter23     = round_n(23, g_filter23 * pow2(23 - exp_g_filter23)) / pow2(23);
    g_filter31     = round_n(31, g_filter31 * pow2(31 - exp_g_filter31)) / pow2(31);



function [y, ssp, iter] = linear_phase2minimum_phase(h, iter_max, x)
    %% function [y, ssp, iter] = linear_phase2minimum_phase(h)
    %%
    %% Function for extracting the minimum-phase factor from
    %% the linear-phase filter h.
    %% Input:
    %%     h = [h(0), h(1), ..., h(N)]   row vector
    %% where the h vector is the right half of a linear-phase FIR filter.
    %% It is presumed that any unit-circle zeros of h are of even
    %% multiplicity.
    %%
    %% Example 1:
    %%   f(z) = 2 + z^{11} + z^{-11}   -> minphase_f(z) = 1 + z^{-11}
    %%
    %%   b  = zeros(1,23); l=length(b)+1; b(1)=b(l-1)=1; b(l/2)=2;
    %%   [bm, ssp, it] = linear_phase2minimum_phase(b((l/2):(l-1)));
    %%   bm_ref = zeros(1,12); bm_ref(1)=bm_ref(12)=1;
    %%   assert((norm(bm-bm_ref)<1e-8) && (abs(it-27)==0));
    %%
    %% Example 2:
    %%   Br(z) : random polynomial of degree 15
    %%   B(z) = Br(z) * Br(1/z) * z**15
    %%   Bm   = minphase_B(z)                (Bm and Br should be identical)
    %%
    %%   br = rand(1,16); b = conv(br,fliplr(br));
    %%   l = length(b)+1; bm = linear_phase2minimum_phase(b((l/2):(l-1)));
    %%   [Bm,w] = freqz(bm,1,512); [Br,w] = freqz(br,1,512);
    %%   assert((norm(abs(Br)-abs(Bm))<1e-12) && (abs(roots(bm))<=1));
    %%
    %%
    %% Copyright (c) January 2002, by H. J. Orchard ad A. N. Willson Jr.

    if nargin < 3
        y = [1 zeros(1, length(h)-1)];  % initialize y
    else
        y = x;
    end
    if nargin < 2
        iter_max = 10;
    end
    ssp = realmax;                      % a large number
    ss = ssp/2;                         % smaller large number
    iter = 0; d = 0;                    % init. iter. counter and correction vector
    while (ss < ssp) && (iter < iter_max)
        y = y + d'; ssp = ss;           % update y and move old norm(d) value
        iter = iter + 1;                % incr. the iteration count
        Ar = toeplitz([y(1), zeros(1,length(h)-1)], y);
        Al = fliplr(toeplitz([y(length(h)), zeros(1,length(h)-1)], fliplr(y)));
        A = Al + Ar;                    % Create the A matrix
        b = h' - Al*y';                 % and create the b vector
        d = A\b;                        % solve Ad = b for the correction vector d
        ss = norm(d);                   % get norm to see if we're still decreasing
    end



function [out]=round_n(n, in)
    out=round(in);
    max_round=round(pow2(n)-1);
    min_round=round(-pow2(n));
    if out>max_round
        out=max_round
    elseif out<min_round
        out=min_round;
    end



% biquad filter response display

function display_biquad(figure_offset, n, gain_table, freq_table, b_filter23, a_filter23, b_filter31, a_filter31, sampling_freq)

    [gd_filter f_filter] = grpdelay(b_filter23, a_filter23, n, sampling_freq);
    [h_filter  f_filter] = freqz(b_filter23, a_filter23, n, sampling_freq);
    mod_filter           = max(abs(h_filter), 0.0001 * ones(size(h_filter)));
    mod_filter_dB        = 20 * log10(mod_filter);
    gain_table_dB        = 20 * log10(max(gain_table, 0.0001 * ones(size(gain_table))));
    figure(figure_offset + 1);
    clf(figure_offset + 1);
    subplot(2, 2, 1);
    hold on;
    grid;
    plot(freq_table, gain_table, '-bo');
    plot(f_filter, mod_filter, '-r');
    title('24 bits transducer biquad response (linear)');
    hold off;
    subplot(2, 2, 2);
    hold on;
    grid;
    plot(freq_table, gain_table_dB, '-bo');
    plot(f_filter, mod_filter_dB, '-g');
    title('24 bits transducer biquad response (dB)');
    hold off;
    subplot(2, 2, 3);
    hold on;
    grid;
    plot(f_filter, phase(h_filter) * 180 / pi, '-b');
    title('24 bits transducer biquad phase (degrees)');
    hold off;
    subplot(2, 2, 4);
    hold on;
    grid;
    plot(f_filter, gd_filter, '-c');
    title('24 bits transducer biquad group delay');
    hold off;

    [gd_filter f_filter] = grpdelay(b_filter31, a_filter31, n, sampling_freq);
    [h_filter  f_filter] = freqz(b_filter31, a_filter31, n, sampling_freq);
    mod_filter           = max(abs(h_filter), 0.0001 * ones(size(h_filter)));
    mod_filter_dB        = 20 * log10(mod_filter);
    figure(figure_offset + 2);
    clf(figure_offset + 2);
    subplot(2, 2, 1);
    hold on;
    grid;
    plot(freq_table, gain_table, '-bo');
    plot(f_filter, mod_filter, '-r');
    title('32 bits transducer biquad response (linear)');
    hold off;
    subplot(2, 2, 2);
    hold on;
    grid;
    plot(freq_table, gain_table_dB, '-bo');
    plot(f_filter, mod_filter_dB, '-g');
    title('32 bits transducer biquad response (dB)');
    hold off;
    subplot(2, 2, 3);
    hold on;
    grid;
    plot(f_filter, phase(h_filter) * 180 / pi, '-b');
    title('32 bits transducer biquad phase (degrees)');
    hold off;
    subplot(2, 2, 4);
    hold on;
    grid;
    plot(f_filter, gd_filter, '-c');
    title('32 bits transducer biquad group delay');
    hold off;



% FIR filter response display

function display_FIR(figure_offset, n, gain_table, freq_table, b_filter23, b_filter31, sampling_freq)

    [gd_filter f_filter] = grpdelay(b_filter23, 1, n, sampling_freq);
    [h_filter  f_filter] = freqz(b_filter23, 1, n, sampling_freq);
    mod_filter           = max(abs(h_filter), 0.0001 * ones(size(h_filter)));
    mod_filter_dB        = 20 * log10(mod_filter);
    gain_table_dB        = 20 * log10(max(gain_table, 0.0001 * ones(size(gain_table))));
    figure(figure_offset + 1);
    clf(figure_offset + 1);
    subplot(2, 2, 1);
    hold on;
    grid;
    plot(freq_table, gain_table, '-bo');
    plot(f_filter, mod_filter, '-r');
    title('24 bits transducer FIR response (linear)');
    hold off;
    subplot(2, 2, 2);
    hold on;
    grid;
    plot(f_filter, mod_filter_dB, '-g');
    plot(freq_table, gain_table_dB, '-bo');
    title('24 bits transducer FIR response (dB)');
    hold off;
    subplot(2, 2, 3);
    hold on;
    grid;
    plot(f_filter, phase(h_filter) * 180 / pi, '-b');
    title('24 bits transducer FIR phase (degrees)');
    hold off;
    subplot(2, 2, 4);
    hold on;
    grid;
    plot(f_filter, gd_filter, '-c');
    title('24 bits transducer FIR group delay');
    hold off;

    [gd_filter f_filter] = grpdelay(b_filter31, 1, n, sampling_freq);
    [h_filter  f_filter] = freqz(b_filter31, 1, n, sampling_freq);
    mod_filter           = max(abs(h_filter), 0.0001 * ones(size(h_filter)));
    mod_filter_dB        = 20 * log10(mod_filter);
    figure(figure_offset + 2);
    clf(figure_offset + 2);
    subplot(2, 2, 1);
    hold on;
    grid;
    plot(freq_table, gain_table, '-bo');
    plot(f_filter, mod_filter, '-r');
    title('32 bits transducer FIR response (linear)');
    hold off;
    subplot(2, 2, 2);
    hold on;
    grid;
    plot(freq_table, gain_table_dB, '-bo');
    plot(f_filter, mod_filter_dB, '-g');
    title('32 bits transducer FIR response (dB)');
    hold off;
    subplot(2, 2, 3);
    hold on;
    grid;
    plot(f_filter, phase(h_filter) * 180 / pi, '-b');
    title('32 bits transducer FIR phase (degrees)');
    hold off;
    subplot(2, 2, 4);
    hold on;
    grid;
    plot(f_filter, gd_filter, '-c');
    title('32 bits transducer FIR group delay');
    hold off;



% biquad filter coefficients save

function save_biquad(name, nb_biquad_cells_l, sos_filter_l_23, g_sos_filter_l_23, exp_g_sos_filter_l_23, b_filter_l_23, a_filter_l_23, sos_filter_l_31, g_sos_filter_l_31, exp_g_sos_filter_l_31, b_filter_l_31, a_filter_l_31, nb_biquad_cells_r, sos_filter_r_23, g_sos_filter_r_23, exp_g_sos_filter_r_23, b_filter_r_23, a_filter_r_23, sos_filter_r_31, g_sos_filter_r_31, exp_g_sos_filter_r_31, b_filter_r_31, a_filter_r_31, stereo, same_biquad_l_r)

    if stereo ~= 0
        if same_biquad_l_r == 0
            nb_alloc_biquad_cells_per_channel = max(nb_biquad_cells_l, nb_biquad_cells_r);
            save_right                        = 1;
        else
            nb_alloc_biquad_cells_per_channel = nb_biquad_cells_l;
            save_right                        = 0;
        end
    else
        nb_alloc_biquad_cells_per_channel = nb_biquad_cells_l;
        save_right                        = 0;
    end
    nb_biquad_cells_per_channel = nb_alloc_biquad_cells_per_channel;

    [file err] = fopen(sprintf('%s_filter.txt', name), 'w');
    fprintf(file, 'nb_biquad_cells_l   = %11d\n', nb_biquad_cells_l);
    fprintf(file, 'biquad_gain_exp_l   = %11d\n', exp_g_sos_filter_l_31);
    fprintf(file, 'biquad_gain_mant_l  = %11d\n', round_n(31, g_sos_filter_l_31 * pow2(31)));
    if save_right == 0
        for i = 1:nb_biquad_cells_l
            tmp = ceil(log2(max(abs(sos_filter_l_31(i, 1:3)))));
            fprintf(file, 'b_exp[%2d]           = %11d\n', i - 1, tmp);
            fprintf(file, 'b0   [%2d]           = %11d\n', i - 1, round_n(31, sos_filter_l_31(i, 1) * pow2(31 - tmp)));
            fprintf(file, 'b1   [%2d]           = %11d\n', i - 1, round_n(31, sos_filter_l_31(i, 2) * pow2(31 - tmp)));
            fprintf(file, 'b2   [%2d]           = %11d\n', i - 1, round_n(31, sos_filter_l_31(i, 3) * pow2(31 - tmp)));
            fprintf(file, 'a1   [%2d]           = %11d\n', i - 1, round_n(31, sos_filter_l_31(i, 5) * pow2(30)));
            fprintf(file, 'a2   [%2d]           = %11d\n', i - 1, round_n(31, sos_filter_l_31(i, 6) * pow2(31)));
        end
    else
        fprintf(file, 'nb_biquad_cells_r   = %11d\n', nb_biquad_cells_r);
        fprintf(file, 'biquad_gain_exp_r   = %11d\n', exp_g_sos_filter_r_31);
        fprintf(file, 'biquad_gain_mant_r  = %11d\n', round_n(31, g_sos_filter_r_31 * pow2(31)));
        for i = 1:nb_biquad_cells_l
            tmp = ceil(log2(max(abs(sos_filter_l_31(i, 1:3)))));
            fprintf(file, 'b_exp_l[%2d]         = %11d\n', i - 1, tmp);
            fprintf(file, 'b0_l   [%2d]         = %11d\n', i - 1, round_n(31, sos_filter_l_31(i, 1) * pow2(31 - tmp)));
            fprintf(file, 'b1_l   [%2d]         = %11d\n', i - 1, round_n(31, sos_filter_l_31(i, 2) * pow2(31 - tmp)));
            fprintf(file, 'b2_l   [%2d]         = %11d\n', i - 1, round_n(31, sos_filter_l_31(i, 3) * pow2(31 - tmp)));
            fprintf(file, 'a1_l   [%2d]         = %11d\n', i - 1, round_n(31, sos_filter_l_31(i, 5) * pow2(30)));
            fprintf(file, 'a2_l   [%2d]         = %11d\n', i - 1, round_n(31, sos_filter_l_31(i, 6) * pow2(31)));
        end
        for i = 1:nb_biquad_cells_r
            tmp = ceil(log2(max(abs(sos_filter_r_31(i, 1:3)))));
            fprintf(file, 'b_exp_r[%2d]         = %11d\n', i - 1, tmp);
            fprintf(file, 'b0_r   [%2d]         = %11d\n', i - 1, round_n(31, sos_filter_r_31(i, 1) * pow2(31 - tmp)));
            fprintf(file, 'b1_r   [%2d]         = %11d\n', i - 1, round_n(31, sos_filter_r_31(i, 2) * pow2(31 - tmp)));
            fprintf(file, 'b2_r   [%2d]         = %11d\n', i - 1, round_n(31, sos_filter_r_31(i, 3) * pow2(31 - tmp)));
            fprintf(file, 'a1_r   [%2d]         = %11d\n', i - 1, round_n(31, sos_filter_r_31(i, 5) * pow2(30)));
            fprintf(file, 'a2_r   [%2d]         = %11d\n', i - 1, round_n(31, sos_filter_r_31(i, 6) * pow2(31)));
        end
    end
    fclose(file);

    [file err] = fopen(sprintf('param_mmte_%s.txt', name), 'w');
    fprintf(file, 'OMX_SetParameter $1 AFM_IndexParamTransducerEqualizer stereo=%d nb_alloc_biquad_cells_per_channel=%d nb_alloc_FIR_coefs_per_channel=0\n', stereo, nb_alloc_biquad_cells_per_channel);
    tmp3 = round_n(31, g_sos_filter_l_31 * pow2(31));
    fprintf(file, 'OMX_SetConfig    $1 AFM_IndexConfigTransducerEqualizer same_biquad_l_r=%d same_FIR_l_r=1 biquad_first=1 nb_biquad_cells_per_channel=%d nb_FIR_coefs_per_channel=0 biquad_gain_exp_l=%d biquad_gain_mant_l=0x%08X FIR_gain_exp_l=0 FIR_gain_mant_l=0x7FFFFFFF', same_biquad_l_r, nb_biquad_cells_per_channel, exp_g_sos_filter_l_31, tmp3);
    if save_right == 1
        tmp3 = round_n(31, g_sos_filter_r_31 * pow2(31));
        fprintf(file, ' biquad_gain_exp_r=%d biquad_gain_mant_r=0x%08X', exp_g_sos_filter_r_31, tmp3);
    end
    fprintf(file, '\n');
    j = 0;
    for i = 1:nb_biquad_cells_per_channel
        if i <= nb_biquad_cells_l
            tmp1 = ceil(log2(max(abs(sos_filter_l_31(i, 1:3)))));
            fprintf(file, 'OMX_SetConfig    $1 AFM_IndexConfigTransducerEqualizer biquad_cell[%d].b_exp=%d', j, tmp1);
            tmp2 = round_n(31, sos_filter_l_31(i, 1) * pow2(31 - tmp1));
            if tmp2 < 0
                tmp3 = tmp2 + 4294967296;
            else
                tmp3 = tmp2;
            end
            fprintf(file, ' biquad_cell[%d].b0=0x%08X', j, tmp3);
            tmp2 = round_n(31, sos_filter_l_31(i, 2) * pow2(31 - tmp1));
            if tmp2 < 0
                tmp3 = tmp2 + 4294967296;
            else
                tmp3 = tmp2;
            end
            fprintf(file, ' biquad_cell[%d].b1=0x%08X', j, tmp3);
            tmp2 = round_n(31, sos_filter_l_31(i, 3) * pow2(31 - tmp1));
            if tmp2 < 0
                tmp3 = tmp2 + 4294967296;
            else
                tmp3 = tmp2;
            end
            fprintf(file, ' biquad_cell[%d].b2=0x%08X', j, tmp3);
            tmp2 = round_n(31, sos_filter_l_31(i, 5) * pow2(30));
            if tmp2 < 0
                tmp3 = tmp2 + 4294967296;
            else
                tmp3 = tmp2;
            end
            fprintf(file, ' biquad_cell[%d].a1=0x%08X', j, tmp3);
            tmp2 = round_n(31, sos_filter_l_31(i, 6) * pow2(31));
            if tmp2 < 0
                tmp3 = tmp2 + 4294967296;
            else
                tmp3 = tmp2;
            end
            fprintf(file, ' biquad_cell[%d].a2=0x%08X\n', j, tmp3);
        else
            fprintf(file, 'OMX_SetConfig    $1 AFM_IndexConfigTransducerEqualizer biquad_cell[%d].b_exp=0', j);
            fprintf(file, ' biquad_cell[%d].b0=0x7FFFFFFF', j);
            fprintf(file, ' biquad_cell[%d].b1=0x00000000', j);
            fprintf(file, ' biquad_cell[%d].b2=0x00000000', j);
            fprintf(file, ' biquad_cell[%d].a1=0x00000000', j);
            fprintf(file, ' biquad_cell[%d].a2=0x00000000\n', j);
        end
        j = j + 1;
        if save_right == 1
            if i <= nb_biquad_cells_r
                tmp1 = ceil(log2(max(abs(sos_filter_r_31(i, 1:3)))));
                fprintf(file, 'OMX_SetConfig    $1 AFM_IndexConfigTransducerEqualizer biquad_cell[%d].b_exp=%d', j, tmp1);
                tmp2 = round_n(31, sos_filter_r_31(i, 1) * pow2(31 - tmp1));
                if tmp2 < 0
                    tmp3 = tmp2 + 4294967296;
                else
                    tmp3 = tmp2;
                end
                fprintf(file, ' biquad_cell[%d].b0=0x%08X', j, tmp3);
                tmp2 = round_n(31, sos_filter_r_31(i, 2) * pow2(31 - tmp1));
                if tmp2 < 0
                    tmp3 = tmp2 + 4294967296;
                else
                    tmp3 = tmp2;
                end
                fprintf(file, ' biquad_cell[%d].b1=0x%08X', j, tmp3);
                tmp2 = round_n(31, sos_filter_r_31(i, 3) * pow2(31 - tmp1));
                if tmp2 < 0
                    tmp3 = tmp2 + 4294967296;
                else
                    tmp3 = tmp2;
                end
                fprintf(file, ' biquad_cell[%d].b2=0x%08X', j, tmp3);
                tmp2 = round_n(31, sos_filter_r_31(i, 5) * pow2(30));
                if tmp2 < 0
                    tmp3 = tmp2 + 4294967296;
                else
                    tmp3 = tmp2;
                end
                fprintf(file, ' biquad_cell[%d].a1=0x%08X', j, tmp3);
                tmp2 = round_n(31, sos_filter_r_31(i, 6) * pow2(31));
                if tmp2 < 0
                    tmp3 = tmp2 + 4294967296;
                else
                    tmp3 = tmp2;
                end
                fprintf(file, ' biquad_cell[%d].a2=0x%08X\n', j, tmp3);
            else
                fprintf(file, 'OMX_SetConfig    $1 AFM_IndexConfigTransducerEqualizer biquad_cell[%d].b_exp=0', j);
                fprintf(file, ' biquad_cell[%d].b0=0x7FFFFFFF', j);
                fprintf(file, ' biquad_cell[%d].b1=0x00000000', j);
                fprintf(file, ' biquad_cell[%d].b2=0x00000000', j);
                fprintf(file, ' biquad_cell[%d].a1=0x00000000', j);
                fprintf(file, ' biquad_cell[%d].a2=0x00000000\n', j);
            end
            j = j + 1;
        end
    end
    fclose(file);



% FIR filter coefficients save

function save_FIR(name, nb_FIR_coefs_l, g_filter_l_23, exp_g_filter_l_23, b_filter_l_23, g_filter_l_31, exp_g_filter_l_31, b_filter_l_31, nb_FIR_coefs_r, g_filter_r_23, exp_g_filter_r_23, b_filter_r_23, g_filter_r_31, exp_g_filter_r_31, b_filter_r_31, stereo, same_FIR_l_r)

    if stereo ~= 0
        if same_FIR_l_r == 0
            nb_alloc_FIR_coefs_per_channel = max(nb_FIR_coefs_l, nb_FIR_coefs_r);
            save_right                     = 1;
        else
            nb_alloc_FIR_coefs_per_channel = nb_FIR_coefs_l;
            save_right                     = 0;
        end
    else
        nb_alloc_FIR_coefs_per_channel = nb_FIR_coefs_l;
        save_right                     = 0;
    end
    nb_FIR_coefs_per_channel = nb_alloc_FIR_coefs_per_channel;

    [file err] = fopen(sprintf('%s_filter.txt', name), 'w');
    fprintf(file, 'nb_FIR_coefs_l   = %11d\n', nb_FIR_coefs_l);
    fprintf(file, 'FIR_gain_exp_l   = %11d\n', exp_g_filter_l_31);
    fprintf(file, 'FIR_gain_mant_l  = %11d\n', round_n(31, g_filter_l_31 * pow2(31)));
    if save_right == 0
        for i = 1:nb_FIR_coefs_l
            fprintf(file, 'coef[%2d]         = %11d\n', i - 1, round_n(31, b_filter_l_31(i) * pow2(31)));
        end
    else
        fprintf(file, 'nb_FIR_coefs_r   = %11d\n', nb_FIR_coefs_r);
        fprintf(file, 'FIR_gain_exp_r   = %11d\n', exp_g_filter_r_31);
        fprintf(file, 'FIR_gain_mant_r  = %11d\n', round_n(31, g_filter_r_31 * pow2(31)));
        for i = 1:nb_FIR_coefs_l
            fprintf(file, 'coef_l[%2d]       = %11d\n', i - 1, round_n(31, b_filter_l_31(i) * pow2(31)));
        end
        for i = 1:nb_FIR_coefs_r
            fprintf(file, 'coef_r[%2d]       = %11d\n', i - 1, round_n(31, b_filter_r_31(i) * pow2(31)));
        end
    end
    fclose(file);

    [file err] = fopen(sprintf('param_mmte_%s.txt', name), 'w');
    fprintf(file, 'OMX_SetParameter $1 AFM_IndexParamTransducerEqualizer stereo=%d nb_alloc_biquad_cells_per_channel=0 nb_alloc_FIR_coefs_per_channel=%d\n', stereo, nb_alloc_FIR_coefs_per_channel);
    tmp3 = round_n(31, g_filter_l_31 * pow2(31));
    fprintf(file, 'OMX_SetConfig    $1 AFM_IndexConfigTransducerEqualizer same_biquad_l_r=1 same_FIR_l_r=%d biquad_first=1 nb_biquad_cells_per_channel=0 nb_FIR_coefs_per_channel=%d biquad_gain_exp_l=0 biquad_gain_mant_l=0x7FFFFFFF FIR_gain_exp_l=%d FIR_gain_mant_l=0x%08X', same_FIR_l_r, nb_FIR_coefs_per_channel, exp_g_filter_l_31, tmp3);
    if save_right == 1
        tmp3 = round_n(31, g_filter_r_31 * pow2(31));
        fprintf(file, ' FIR_gain_exp_r=%d FIR_gain_mant_r=0x%08X', exp_g_filter_r_31, tmp3);
    end
    fprintf(file, '\n');
    j = 0;
    for i = 1:nb_FIR_coefs_per_channel
        if mod(j, 8) == 0
            fprintf(file, 'OMX_SetConfig    $1 AFM_IndexConfigTransducerEqualizer ');
        else
            fprintf(file, ' ');
        end
        if i <= nb_FIR_coefs_l
            tmp2 = round_n(31, b_filter_l_31(i) * pow2(31));
            if tmp2 < 0
                tmp3 = tmp2 + 4294967296;
            else
                tmp3 = tmp2;
            end
            fprintf(file, 'FIR_coef[%d]=0x%08X', j, tmp3);
        else
            fprintf(file, 'FIR_coef[%d]=0x00000000', j);
        end
        j = j + 1;
        if save_right == 1
            if i <= nb_FIR_coefs_r
                tmp2 = round_n(31, b_filter_r_31(i) * pow2(31));
                if tmp2 < 0
                    tmp3 = tmp2 + 4294967296;
                else
                    tmp3 = tmp2;
                end
                fprintf(file, ' FIR_coef[%d]=0x%08X', j, tmp3);
            else
                fprintf(file, ' FIR_coef[%d]=0x00000000', j);
            end
            j = j + 1;
        end
        if mod(j, 8) == 0
            fprintf(file, '\n');
        end
    end
    if mod(j, 8) ~= 0
        fprintf(file, '\n');
    end
    fclose(file);
