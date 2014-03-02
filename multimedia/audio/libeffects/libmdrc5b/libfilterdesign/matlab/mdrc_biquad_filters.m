function mdrc_biquad_filters(CutFreq, gains_dB, sampling_freq, biquad_size)
    biquad_order = 2;
    fs           = 48000;

    if nargin < 1
        fprintf(1, 'need at leat 1 parameter : cut frequencies table !\n');
        return;
    end;
    assert(size(CutFreq, 1) == 1);
    if nargin > 1
        assert(size(gains_dB, 1) == 1);
        assert(size(gains_dB, 2) == size(CutFreq, 2) + 1);
    else
        gains_dB = zeros(1, size(CutFreq, 2) + 1);
    end;
    gains = power(10, gains_dB / 20);
    if nargin > 2
        fs = sampling_freq;
    end;
    if nargin > 3
        biquad_order = biquad_size;
    end;

    file = fopen('mdrc_biquad_filters.txt', 'w');

    n = size(CutFreq, 2);
    if n >= 1
        subplot(2, 2, 1);
        hold on;
        title('Butterworth biquads (linear)');
        subplot(2, 2, 2);
        hold on;
        title('Butterworth sub-bands (linear)');
        subplot(2, 2, 3);
        hold on;
        title('Butterworth biquads (dB)');
        subplot(2, 2, 4);
        hold on;
        title('Butterworth sub-bands (dB)');

        min_dB = -30.0;

        % biquads
        f = fs;
        for sub_fig = 1 : 2
            % linear scale
            subplot(2, 2, sub_fig);
            hold on;
            for i = 1 : n
                plot([CutFreq(i), CutFreq(i)], [0.0, 1.0], ':k');       % Cut frequencies
            end;
            plot([0.0, f / 2.0], [1.0,       1.0],       ':k');         %  0 dB level
            plot([0.0, f / 2.0], [sqrt(0.5), sqrt(0.5)], ':k');         % -3 dB level
            % dB scale
            subplot(2, 2, sub_fig + 2);
            hold on;
            for i = 1 : n
                plot([CutFreq(i), CutFreq(i)], [min_dB, 0.0], ':k');    % Cut frequencies
            end;
            plot([0.0, f / 2.0], [0.0,   0.0], ':k');                   %  0 dB level
            plot([0.0, f / 2.0], [-3.0, -3.0], ':k');                   % -3 dB level
        end;

        colors                   = 'rgbcmyk';
        N                        = 1024;                                % plot size
        freq_axis                = [0 : N - 1] / N * fs / 2.0;

        H_butterworth            = zeros(n + 1, N);
        sum_H_flat_butterworth   = zeros(1,     N);
        sum_H_gain_butterworth   = zeros(1,     N);
        HP_butterworth_cur_freqz = zeros(1,     N);

        fprintf(file, 'Butterworth biquads design : sampling freq = %1.0f Hz - biquad_order = %d\n', fs, biquad_order);
        [biquads_butterworth_LP, biquads_butterworth_HP] = mdrc_bands_butterworth(CutFreq, fs, biquad_order);

        %%%%%%%%%%%%%%%%%%%%%%%%%
        % biquads (Butterworth) %
        %%%%%%%%%%%%%%%%%%%%%%%%%
        for i = 1 : n + 1
            HP_butterworth_prev_freqz = HP_butterworth_cur_freqz;

            if i <= n
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                % combine <biquad_order> low-pass/high-pass biquads for each cut frequency %
                LP_butterworth_b = [1.0];
                LP_butterworth_a = [1.0];
                HP_butterworth_b = [1.0];
                HP_butterworth_a = [1.0];
                for j = 1 : biquad_order
                    LP_butterworth_b = conv(LP_butterworth_b, biquads_butterworth_LP(i, 6 * j - 5 : 6 * j - 3));
                    LP_butterworth_a = conv(LP_butterworth_a, biquads_butterworth_LP(i, 6 * j - 2 : 6 * j - 0));
                    HP_butterworth_b = conv(HP_butterworth_b, biquads_butterworth_HP(i, 6 * j - 5 : 6 * j - 3));
                    HP_butterworth_a = conv(HP_butterworth_a, biquads_butterworth_HP(i, 6 * j - 2 : 6 * j - 0));
                end;
                LP_butterworth_cur_freqz = freqz(LP_butterworth_b, LP_butterworth_a, N);
                HP_butterworth_cur_freqz = freqz(HP_butterworth_b, HP_butterworth_a, N);

                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                % display biquads filter coefficients %
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                fprintf(file, '\nCut freq(%d) = %1.0f Hz\n', i, CutFreq(i));
                for j = 1 : biquad_order
                    fprintf(file, 'LP_butterworth(%d,%d) : (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2) / (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2)\n', i, j, biquads_butterworth_LP(i, 6 * j - 5), biquads_butterworth_LP(i, 6 * j - 4), biquads_butterworth_LP(i, 6 * j - 3), biquads_butterworth_LP(i, 6 * j - 2), biquads_butterworth_LP(i, 6 * j - 1), biquads_butterworth_LP(i, 6 * j));
                end;
                for j = 1 : biquad_order
                    fprintf(file, 'HP_butterworth(%d,%d) : (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2) / (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2)\n', i, j, biquads_butterworth_HP(i, 6 * j - 5), biquads_butterworth_HP(i, 6 * j - 4), biquads_butterworth_HP(i, 6 * j - 3), biquads_butterworth_HP(i, 6 * j - 2), biquads_butterworth_HP(i, 6 * j - 1), biquads_butterworth_HP(i, 6 * j));
                end;

                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                % plot low-pass & high-pass biquad responses %
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                % linear scale
                subplot(2, 2, 1);
                hold on;
                plot(freq_axis, abs(LP_butterworth_cur_freqz), strcat('-', colors(i)));
                plot(freq_axis, abs(HP_butterworth_cur_freqz), strcat('-', colors(i)));
                % dB scale
                subplot(2, 2, 3);
                hold on;
                plot(freq_axis, max(min_dB, 20.0 * log10(abs(LP_butterworth_cur_freqz))), strcat('-', colors(i)));
                plot(freq_axis, max(min_dB, 20.0 * log10(abs(HP_butterworth_cur_freqz))), strcat('-', colors(i)));
            end;

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % combine low-pass & high-pass biquads for sub-bands separation %
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            if i == 1
                % first band : only low-pass filter
                H_butterworth(i, :) = LP_butterworth_cur_freqz;
            elseif i == n + 1
                % last band : only high-pass filter
                H_butterworth(i, :) = HP_butterworth_prev_freqz;
            else
                % other bands : current low-pass combined with previus high-pass filters
                H_butterworth(i, :) = LP_butterworth_cur_freqz .* HP_butterworth_prev_freqz;
            end;
            sum_H_flat_butterworth = sum_H_flat_butterworth + H_butterworth(i, :);
            sum_H_gain_butterworth = sum_H_gain_butterworth + H_butterworth(i, :) * gains(1, i);

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % plot sub-band biquad filters responses %
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % linear scale
            subplot(2, 2, 2);
            hold on;
            plot(freq_axis, abs(H_butterworth(i, :)), strcat('-', colors(i)));
            % dB scale
            subplot(2, 2, 4);
            hold on;
            plot(freq_axis, max(min_dB, 20.0 * log10(abs(H_butterworth(i, :)))), strcat('-', colors(i)));
        end;


        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % plot sum of Butterworth biquads response %
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % linear scale
        subplot(2, 2, 2);
        hold on;
        plot(freq_axis, abs(sum_H_flat_butterworth), '-k', 'LineWidth', 2);
        plot(freq_axis, abs(sum_H_gain_butterworth), '-b', 'LineWidth', 2);
        % dB scale
        subplot(2, 2, 4);
        hold on;
        plot(freq_axis, max(min_dB, 20.0 * log10(abs(sum_H_flat_butterworth))), '-k', 'LineWidth', 2);
        plot(freq_axis, max(min_dB, 20.0 * log10(abs(sum_H_gain_butterworth))), '-b', 'LineWidth', 2);

        fclose(file);
    end;

    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Butterworth biquads design %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [biquads_LP, biquads_HP] = mdrc_bands_butterworth(CutFreq, fs, biquad_order)
    n          = size(CutFreq, 2);
    biquads_LP = zeros(n, 6 * biquad_order);
    biquads_HP = zeros(n, 6 * biquad_order);
    for i = 1 : n
        assert((CutFreq(i) > 0.0) && (CutFreq(i) <  fs / 2.0));
        assert((i == 1) || (CutFreq(i - 1) < CutFreq(i)));
        [biquads_LP_tmp, biquads_HP_tmp] = butterworth_LPHP(CutFreq(i), fs, biquad_order);
        for j = 1 : biquad_order
            biquads_LP(i, 6 * j - 5 : 6 * j) = biquads_LP_tmp(j, :);
            biquads_HP(i, 6 * j - 5 : 6 * j) = biquads_HP_tmp(j, :);
        end;
    end;


function [biquads_LP, biquads_HP] = butterworth_LPHP(fc, fs, biquad_order)
    assert((fc / fs > 0.0001) && (fc / fs < 0.4999));
    c          = cotg(pi * fc / fs);
    c2         = c * c;
    biquads_LP = zeros(biquad_order, 6);
    biquads_HP = zeros(biquad_order, 6);
    for k = 1 : biquad_order
        ck                   = -2.0 * cos((2.0 * (k + biquad_order) - 1.0) / (4.0 * biquad_order) * pi);
        biquads_LP(k, 1 : 3) = [1.0, 2.0, 1.0];
        biquads_LP(k, 4 : 6) = [c2 + ck * c + 1.0, 2.0 * (1.0 - c2), c2 - ck * c + 1.0];
        biquads_HP(k, 1 : 3) = [1.0, -2.0, 1.0];
        biquads_HP(k, 4 : 6) = biquads_LP(k, 4 : 6) / c2;
        biquads_LP(k, :)     = biquads_LP(k, :) / biquads_LP(k, 4);
        biquads_HP(k, :)     = biquads_HP(k, :) / biquads_HP(k, 4);
    end;




function y = cotg(x)
    y = cos(x) / sin(x);

