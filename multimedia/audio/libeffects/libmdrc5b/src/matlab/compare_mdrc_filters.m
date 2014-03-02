function compare_mdrc_filters(CutFreq)
    FIR_order           = 41;                  % odd value (linear phase FIR filter)
    biquad_order        = 2;
    fs                  = 48000;
    biquad_order_legacy = 2;                   % 2 is mandatory for legacy biquads, any value for Butterworth biquads
    fs_legacy           = 48000;               % 48000 is mandatory for legacy biquads, any value for Butterworth biquads or FIR
    n                   = size(CutFreq, 2);

    file = fopen('mdrc_filters.txt', 'w');

    if n >= 1
        figure(1);
        clf(1);
        subplot(2, 2, 1);
        hold on;
        title('Legacy biquads (linear)');
        subplot(2, 2, 2);
        hold on;
        title('Legacy sub-bands (linear)');
        subplot(2, 2, 3);
        hold on;
        title('Legacy biquads (dB)');
        subplot(2, 2, 4);
        hold on;
        title('Legacy sub-bands (dB)');

        figure(2);
        clf(2);
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

        figure(3);
        clf(3);
        subplot(2, 1, 1);
        hold on;
        title('FIR (linear)');
        subplot(2, 1, 2);
        hold on;
        title('FIR (dB)');

        min_dB = -30.0;

        % biquads
        for fig = 1 : 2
            figure(fig);
            if fig == 1
                f = fs_legacy;
            else
                f = fs;
            end;
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
        end;

        % FIR
        for fig = 3 : 3
            figure(fig);
            % linear scale
            subplot(2, 1, 1);
            hold on;
            for i = 1 : n
                plot([CutFreq(i), CutFreq(i)], [0.0, 1.0], ':k');       % Cut frequencies
            end;
            plot([0.0, fs / 2.0], [1.0, 1.0], ':k');                    %  0 dB level
            plot([0.0, fs / 2.0], [0.5, 0.5], ':k');                    % -6 dB level
            % dB scale
            subplot(2, 1, 2);
            hold on;
            for i = 1 : n
                plot([CutFreq(i), CutFreq(i)], [min_dB, 0.0], ':k');    % Cut frequencies
            end;
            plot([0.0, fs / 2.0], [0.0,   0.0], ':k');                  %  0 dB level
            plot([0.0, fs / 2.0], [-6.0, -6.0], ':k');                  % -6 dB level
        end;

        colors                   = 'rgbcmyk';
        N                        = 1024;                                % plot size
        freq_axis_legacy         = [0 : N - 1] / N * fs_legacy / 2.0;
        freq_axis                = [0 : N - 1] / N * fs        / 2.0;

        H_legacy                 = zeros(n + 1, N);
        sum_H_legacy             = zeros(1,     N);
        HP_legacy_cur_freqz      = zeros(1,     N);
        H_butterworth            = zeros(n + 1, N);
        sum_H_butterworth        = zeros(1,     N);
        HP_butterworth_cur_freqz = zeros(1,     N);

        fprintf(file, 'legacy      biquads design : sampling freq = %1.0f Hz - biquad_order = %d\n', fs_legacy, biquad_order_legacy);
        fprintf(file, 'Butterworth biquads design : sampling freq = %1.0f Hz - biquad_order = %d\n', fs,        biquad_order);
        [biquads_legacy_LP,      biquads_legacy_HP]      = mdrc_bands_legacy     (CutFreq, fs_legacy, biquad_order_legacy);
        [biquads_butterworth_LP, biquads_butterworth_HP] = mdrc_bands_butterworth(CutFreq, fs,        biquad_order);

        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % biquads (legacy & Butterworth) %
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        for i = 1 : n + 1
            HP_legacy_prev_freqz      = HP_legacy_cur_freqz;
            HP_butterworth_prev_freqz = HP_butterworth_cur_freqz;

            if i <= n
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                % combine <biquad_order> low-pass/high-pass biquads for each cut frequency %
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                LP_legacy_b      = [1.0];
                LP_legacy_a      = [1.0];
                HP_legacy_b      = [1.0];
                HP_legacy_a      = [1.0];
                LP_butterworth_b = [1.0];
                LP_butterworth_a = [1.0];
                HP_butterworth_b = [1.0];
                HP_butterworth_a = [1.0];
                for j = 1 : biquad_order
                    LP_legacy_b      = conv(LP_legacy_b,      biquads_legacy_LP     (i, 6 * j - 5 : 6 * j - 3));
                    LP_legacy_a      = conv(LP_legacy_a,      biquads_legacy_LP     (i, 6 * j - 2 : 6 * j - 0));
                    HP_legacy_b      = conv(HP_legacy_b,      biquads_legacy_HP     (i, 6 * j - 5 : 6 * j - 3));
                    HP_legacy_a      = conv(HP_legacy_a,      biquads_legacy_HP     (i, 6 * j - 2 : 6 * j - 0));
                    LP_butterworth_b = conv(LP_butterworth_b, biquads_butterworth_LP(i, 6 * j - 5 : 6 * j - 3));
                    LP_butterworth_a = conv(LP_butterworth_a, biquads_butterworth_LP(i, 6 * j - 2 : 6 * j - 0));
                    HP_butterworth_b = conv(HP_butterworth_b, biquads_butterworth_HP(i, 6 * j - 5 : 6 * j - 3));
                    HP_butterworth_a = conv(HP_butterworth_a, biquads_butterworth_HP(i, 6 * j - 2 : 6 * j - 0));
                end;
                LP_legacy_cur_freqz      = freqz(LP_legacy_b,      LP_legacy_a,      N);
                HP_legacy_cur_freqz      = freqz(HP_legacy_b,      HP_legacy_a,      N);
                LP_butterworth_cur_freqz = freqz(LP_butterworth_b, LP_butterworth_a, N);
                HP_butterworth_cur_freqz = freqz(HP_butterworth_b, HP_butterworth_a, N);

                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                % display biquads filter coefficients %
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                fprintf(file, '\nCut freq(%d) = %1.0f Hz\n', i, CutFreq(i));
                for j = 1 : biquad_order_legacy
                    fprintf(file, 'LP_legacy(%d,%d)      : (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2) / (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2)\n', i, j, biquads_legacy_LP(i, 6 * j - 5), biquads_legacy_LP(i, 6 * j - 4), biquads_legacy_LP(i, 6 * j - 3), biquads_legacy_LP(i, 6 * j - 2), biquads_legacy_LP(i, 6 * j - 1), biquads_legacy_LP(i, 6 * j));
                end;
                for j = 1 : biquad_order_legacy
                    fprintf(file, 'HP_legacy(%d,%d)      : (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2) / (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2)\n', i, j, biquads_legacy_HP(i, 6 * j - 5), biquads_legacy_HP(i, 6 * j - 4), biquads_legacy_HP(i, 6 * j - 3), biquads_legacy_HP(i, 6 * j - 2), biquads_legacy_HP(i, 6 * j - 1), biquads_legacy_HP(i, 6 * j));
                end;
                for j = 1 : biquad_order
                    fprintf(file, 'LP_butterworth(%d,%d) : (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2) / (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2)\n', i, j, biquads_butterworth_LP(i, 6 * j - 5), biquads_butterworth_LP(i, 6 * j - 4), biquads_butterworth_LP(i, 6 * j - 3), biquads_butterworth_LP(i, 6 * j - 2), biquads_butterworth_LP(i, 6 * j - 1), biquads_butterworth_LP(i, 6 * j));
                end;
                for j = 1 : biquad_order
                    fprintf(file, 'HP_butterworth(%d,%d) : (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2) / (%+1.10f %+1.10f.z^-1 %+1.10f.z^-2)\n', i, j, biquads_butterworth_HP(i, 6 * j - 5), biquads_butterworth_HP(i, 6 * j - 4), biquads_butterworth_HP(i, 6 * j - 3), biquads_butterworth_HP(i, 6 * j - 2), biquads_butterworth_HP(i, 6 * j - 1), biquads_butterworth_HP(i, 6 * j));
                end;

                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                % plot low-pass & high-pass biquad responses %
                %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                figure(1);
                % linear scale
                subplot(2, 2, 1);
                hold on;
                plot(freq_axis_legacy, abs(LP_legacy_cur_freqz), strcat('-', colors(i)));
                plot(freq_axis_legacy, abs(HP_legacy_cur_freqz), strcat('-', colors(i)));
                % dB scale
                subplot(2, 2, 3);
                hold on;
                plot(freq_axis_legacy, max(min_dB, 20.0 * log10(abs(LP_legacy_cur_freqz))), strcat('-', colors(i)));
                plot(freq_axis_legacy, max(min_dB, 20.0 * log10(abs(HP_legacy_cur_freqz))), strcat('-', colors(i)));

                figure(2);
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
                H_legacy(i, :)      = LP_legacy_cur_freqz;
                H_butterworth(i, :) = LP_butterworth_cur_freqz;
            elseif i == n + 1
                % last band : only high-pass filter
                H_legacy(i, :)      = HP_legacy_prev_freqz;
                H_butterworth(i, :) = HP_butterworth_prev_freqz;
            else
                % other bands : current low-pass combined with previus high-pass filters
                H_legacy(i, :)      = LP_legacy_cur_freqz      .* HP_legacy_prev_freqz;
                H_butterworth(i, :) = LP_butterworth_cur_freqz .* HP_butterworth_prev_freqz;
            end;
            sum_H_legacy      = sum_H_legacy      + H_legacy(i, :);
            sum_H_butterworth = sum_H_butterworth + H_butterworth(i, :);

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % plot sub-band biquad filters responses %
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            figure(1);
            % linear scale
            subplot(2, 2, 2);
            hold on;
            plot(freq_axis_legacy, abs(H_legacy(i, :)), strcat('-', colors(i)));
            % dB scale
            subplot(2, 2, 4);
            hold on;
            plot(freq_axis_legacy, max(min_dB, 20.0 * log10(abs(H_legacy(i, :)))), strcat('-', colors(i)));

            figure(2);
            % linear scale
            subplot(2, 2, 2);
            hold on;
            plot(freq_axis, abs(H_butterworth(i, :)), strcat('-', colors(i)));
            % dB scale
            subplot(2, 2, 4);
            hold on;
            plot(freq_axis, max(min_dB, 20.0 * log10(abs(H_butterworth(i, :)))), strcat('-', colors(i)));
        end;

        %%%%%%%%
        % FIRs %
        %%%%%%%%
        fprintf(file, '\n\nFIR design : sampling freq = %1.0f Hz - FIR_order = %d\n', fs, FIR_order);
        [FIR max_gain max_freq] = mdrc_bands_FIR(CutFreq, fs, FIR_order);
        H_FIR                   = zeros(n + 1, N);
        sum_H_FIR               = zeros(1,     N);
        sum_FIR                 = zeros(1, FIR_order);
        bands                   = [0.0, CutFreq, fs / 2.0];
        for i = 1 : n + 1
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % display FIRs filter coefficients %
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            fprintf(file, '\nFIR%d (%1.0f to %1.0f Hz): max gain = %1.2f dB at %1.0f Hz\n', i, bands(i), bands(i + 1), 20.0 * log10(max_gain(i)), max_freq(i));
            for j = 1 : FIR_order
                fprintf(file, ' %+1.10f.z-%2d', FIR(i, j), j - 1);
                if (mod(j, 8) == 0) && (j < FIR_order)
                    fprintf(file, '\n');
                end;
            end;
            fprintf(file, '\n');

            H_FIR(i, :) = freqz(FIR(i, :), [1], N);
            sum_H_FIR   = sum_H_FIR + H_FIR(i, :);
            sum_FIR     = sum_FIR   + FIR(i, :);

            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            % plot sub-band FIR filters responses %
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            figure(3);
            % linear scale
            subplot(2, 1, 1);
            hold on;
            plot(freq_axis, abs(H_FIR(i, :)), strcat('-', colors(i)));
            % dB scale
            subplot(2, 1, 2);
            hold on;
            plot(freq_axis, max(min_dB, 20.0 * log10(abs(H_FIR(i, :)))), strcat('-', colors(i)));
        end;

        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % display sum of FIRs filter coefficients %
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        fprintf(file, '\nsum of %d FIRs :\n', n + 1);
        for j = 1 : FIR_order
            fprintf(file, ' %+1.10f.z-%2d', sum_FIR(j), j - 1);
            if (mod(j, 8) == 0) && (j < FIR_order)
                fprintf(file, '\n');
            end;
        end;
        fprintf(file, '\n');

        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % plot sum of legacy biquads response %
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        figure(1);
        % linear scale
        subplot(2, 2, 2);
        hold on;
        plot(freq_axis_legacy, abs(sum_H_legacy), '-k', 'LineWidth', 2);
        % dB scale
        subplot(2, 2, 4);
        hold on;
        plot(freq_axis_legacy, max(min_dB, 20.0 * log10(abs(sum_H_legacy))), '-k', 'LineWidth', 2);

        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % plot sum of Butterworth biquads response %
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        figure(2);
        % linear scale
        subplot(2, 2, 2);
        hold on;
        plot(freq_axis, abs(sum_H_butterworth), '-k', 'LineWidth', 2);
        % dB scale
        subplot(2, 2, 4);
        hold on;
        plot(freq_axis, max(min_dB, 20.0 * log10(abs(sum_H_butterworth))), '-k', 'LineWidth', 2);

        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        % plot sum of FIRs response %
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        figure(3);
        % linear scale
        subplot(2, 1, 1);
        hold on;
        plot(freq_axis, abs(sum_H_FIR), '-k', 'LineWidth', 2);
        % dB scale
        subplot(2, 1, 2);
        hold on;
        plot(freq_axis, max(min_dB, 20.0 * log10(abs(sum_H_FIR))), '-k', 'LineWidth', 2);

        fclose(file);
    end;



%%%%%%%%%%%%%%%%%%%%%%%%%
% legacy biquads design %
%%%%%%%%%%%%%%%%%%%%%%%%%
function [biquads_LP, biquads_HP] = mdrc_bands_legacy(CutFreq, fs, biquad_order)
    assert(fs           == 48000);  % sampling frequency must be 48000 Hz
    assert(biquad_order == 2);      % biquad order must be 2 (2 identical biquad cells)

    n          = size(CutFreq, 2);
    biquads_LP = zeros(n, 6 * biquad_order);
    biquads_HP = zeros(n, 6 * biquad_order);
    for i = 1 : n
        assert((CutFreq(i) > 0.0) && (CutFreq(i) <  fs / 2.0));
        assert((i == 1) || (CutFreq(i - 1) < CutFreq(i)));
        [FilterL FilterH] = mdrc5b_compute_filter_coef(CutFreq(i));
        for j = 1 : biquad_order
            % FilterL & FilterH coefficients order : b2, -b1/2, b0, -a1/2, a2 (Q23)
            biquads_LP(i, 6 * j - 5) = FilterL(3);
            biquads_LP(i, 6 * j - 4) = FilterL(2) * -2;
            biquads_LP(i, 6 * j - 3) = FilterL(1);
            biquads_LP(i, 6 * j - 2) = 2^23;
            biquads_LP(i, 6 * j - 1) = FilterL(4) * -2;
            biquads_LP(i, 6 * j - 0) = FilterL(5);
            biquads_HP(i, 6 * j - 5) = FilterH(3);
            biquads_HP(i, 6 * j - 4) = FilterH(2) * -2;
            biquads_HP(i, 6 * j - 3) = FilterH(1);
            biquads_HP(i, 6 * j - 2) = 2^23;
            biquads_HP(i, 6 * j - 1) = FilterH(4) * -2;
            biquads_HP(i, 6 * j - 0) = FilterH(5);
        end;
    end;
    biquads_LP = biquads_LP * 2.0^-23.0;
    biquads_HP = biquads_HP * 2.0^-23.0;


function [FilterL FilterH] = mdrc5b_compute_filter_coef(FreqCutoff)
    % For Sample Freq == 48000Hz only (1/48000)                             %    // For Sample Freq == 48000Hz only (1/48000)
    t_man = 5726623; % 0x57619F                                             %    t_man = 0x57619F;
    t_exp = -38;                                                            %    t_exp = -38;

    % tsq (1/48000^2)                                                       %    // tsq (1/48000^2)
    tsq_man = 7818749; % 0x774DFD                                           %    tsq_man = 0x774DFD;
    tsq_exp = -54;                                                          %    tsq_exp = -54;

    % PI = 3.141592653589793236                                             %    // PI = 3.141592653589793236
    pi_man = 6588397; % 0x6487ED                                            %    pi_man = 0x6487ED;
    pi_exp = -21;                                                           %    pi_exp = -21;

    % TWOSQRT = 1.4142135623730950488016887242097                           %    // TWOSQRT = 1.4142135623730950488016887242097
    Sqrt2_man = 5931641; % 0x5A8279                                         %    Sqrt2_man = 0x5A8279;
    Sqrt2_exp = -22;                                                        %    Sqrt2_exp = -22;

    fc = FreqCutoff; %4780; %230; %FreqCutoff;                              %    fc = FreqCutoff; //4780; //230; //FreqCutoff;

    fc_exp = wedge(fc);                                                     %    fc_exp = wedge(fc);
    fc_man = fc * 2^fc_exp;                                                 %    fc_man = fc << fc_exp; //wmsl(fc,fc_exp);
    fc_exp = -fc_exp;                                                       %    fc_exp = -fc_exp;

    % a = 2.0*PI*FreqCutOff                                                 %    // a = 2.0*PI*FreqCutOff
    lltemp1 = pi_man * fc_man;                                              %    lltemp1 = (Word48)pi_man * (Word48)fc_man;
    a_man = floor(lltemp1 / 2^23);                                          %    a_man = lltemp1 >> 23;
    a_exp = pi_exp + fc_exp + 23 + 1;                                       %    a_exp = pi_exp + fc_exp + 23 + 1;
                                                                            %
    % asq                                                                   %    // asq
    asq_man = floor(a_man * a_man / 2^23);                                  %    asq_man = ((Word48)a_man * (Word48)a_man) >> 23;
    asq_exp = a_exp * 2 + 23;                                               %    asq_exp = a_exp * 2 + 23;

    % term1 = asq*tsq                                                       %    // term1 = asq*tsq
    term1_ll_man = asq_man * tsq_man;                                       %    term1_ll_man = ((Word48)asq_man * (Word48)tsq_man);
    term1_ll_exp = asq_exp + tsq_exp;                                       %    term1_ll_exp = asq_exp + tsq_exp;
    term1_man = floor(term1_ll_man / 2^23);                                 %    term1_man = term1_ll_man >> 23;
    term1_exp = term1_ll_exp + 23;                                          %    term1_exp = term1_ll_exp + 23;

    % term2 = 2.0*TWOSQRT*a*t                                               %    // term2 = 2.0*TWOSQRT*a*t
    term2_man = floor(a_man * t_man / 2^23);                                %    term2_man = ((Word48)a_man * (Word48)t_man) >> 23;
    term2_exp = a_exp + t_exp + 23;                                         %    term2_exp = a_exp + t_exp + 23;
    term2_ll_man = Sqrt2_man * term2_man;                                   %    term2_ll_man = ((Word48)Sqrt2_man * (Word48)term2_man);
    term2_ll_exp = Sqrt2_exp + term2_exp + 1;                               %    term2_ll_exp = Sqrt2_exp + term2_exp + 1;
    term2_man = floor(term2_ll_man / 2^23);                                 %    term2_man = term2_ll_man >> 23;
    term2_exp = term2_ll_exp + 23;                                          %    term2_exp = term2_ll_exp + 23;

    if (term1_exp<-20) && (term2_exp<-20)                                   %    if(term1_exp<-20 && term2_exp<-20)
        den_man = 2^22; % 0x400000;                                         %    {
        den_man = den_man + floor(term1_man / 2^(-20-term1_exp));           %        den_man = 0x400000
        den_man = den_man + floor(term2_man / 2^(-20-term2_exp));           %                + ((term1_man)>>(-20-term1_exp))
        den_exp = -20;                                                      %                + ((term2_man)>>(-20-term2_exp));
    else                                                                    %        den_exp = -20;
        if term1_exp >= term2_exp                                           %    }
            den_exp = term1_exp;                                            %    else
        else                                                                %    {
            den_exp = term2_exp;                                            %        den_exp = (term1_exp>=term2_exp)?term1_exp:term2_exp;
        end;                                                                %        den_man = (0x4LL<<(-den_exp))
        den_man = 4 * 2^(-den_exp);                                         %                + ((term1_man)>>(den_exp-term1_exp))
        den_man = den_man + floor(term1_man / 2^(den_exp-term1_exp));       %                + ((term2_man)>>(den_exp-term2_exp));
        den_man = den_man + floor(term2_man / 2^(den_exp-term2_exp));       %    }
    end;

    lltemp1 = floor(term1_ll_man / 2^(-28-term1_ll_exp));                   %    lltemp1 = wL_msr(term1_ll_man,(-28-term1_ll_exp)); //((term1_ll_man)>>(-28-term1_ll_exp));
    lltemp2 = floor(term2_ll_man / 2^(-28-term2_ll_exp));                   %    lltemp2 = wL_msr(term2_ll_man,(-28-term2_ll_exp)); //((term2_ll_man)>>(-28-term2_ll_exp));

    gl_man = floor(term1_ll_man / den_man);                                 %    gl_man = term1_ll_man / (Word48)den_man;
    gl_exp = term1_ll_exp - den_exp;                                        %    gl_exp = term1_ll_exp - den_exp;

    ltemp = floor(gl_man / 2^(-23-gl_exp));                                 %    ltemp = (Word24)wL_msr(gl_man,(-23-gl_exp)); //(gl_man>>(-23-gl_exp));
    FilterL(1) =  ltemp; %+1;                                               %    FilterL[0] = FilterL[2] = ltemp; //+1;
    FilterL(3) =  ltemp;                                                    %    FilterL[1] = -FilterL[0];
    FilterL(2) = -ltemp;

    lltemp1 = 2^45; % 0x200000000000 : 4.0 in Q43                           %    lltemp1 = ((Word48)0x200000000000LL); // 4.0 in Q43
    lltemp1 = floor(term1_ll_man / 2^(-43-term1_ll_exp)) - lltemp1;         %    lltemp1 = (wL_msr(term1_ll_man,(-43-term1_ll_exp))/*(term1_ll_man>>(-43-term1_ll_exp))*/ - lltemp1);
    a1_man = floor(lltemp1 / den_man);                                      %    a1_man = lltemp1 / (Word48)den_man;
    a1_exp = (-43+1) - (den_exp) - 1;                                       %    a1_exp = (-43+1) - (den_exp) - 1;

    ltemp = -floor(a1_man / 2^(-23-a1_exp)); %-(a1_man>>(-0x17-a1_exp));    %    ltemp = (Word24)-wL_msr(a1_man,(-23-a1_exp)); //-(a1_man>>(-0x17-a1_exp));
    FilterL(4) = ltemp; %-2;                                                %    FilterL[3] = FilterH[3] = ltemp; //-2;
    FilterH(4) = ltemp;

    lltemp1 = 2^44; % 0x100000000000 : 4.0 in Q42                           %    lltemp1 = ((Word48)0x100000000000LL); // 4.0 in Q42
    lltemp1 = lltemp1 - floor(term2_ll_man / 2^(-42-term2_ll_exp));         %    lltemp1 = lltemp1 - wL_msr(term2_ll_man,(-42-term2_ll_exp)); //(term2_ll_man>>(-42-term2_ll_exp));
    lltemp1 = lltemp1 + floor(term1_ll_man / 2^(-42-term1_ll_exp));         %    lltemp1 = lltemp1 + wL_msr(term1_ll_man,(-42-term1_ll_exp)); //(term1_ll_man>>(-42-term1_ll_exp));
    a2_man = floor(lltemp1 / den_man);                                      %    a2_man = lltemp1 / (Word48)den_man;
    a2_exp = -42 - den_exp;                                                 %    a2_exp = -42 - den_exp;

    ltemp = floor(a2_man * 2^(a2_exp-(-23)));                               %    ltemp = (Word24)wL_msl(a2_man,(a2_exp-(-23))); //(a2_man<<(a2_exp-(-23)));
    FilterL(5) = ltemp; %-2;                                                %    FilterL[4] = FilterH[4] = ltemp; //-2;
    FilterH(5) = ltemp;

    lltemp1 = 2^46; % 0x400000000000 : 4.0 in Q44                           %    lltemp1 = ((Word48)0x400000000000LL); // 4.0 in Q44
    gh_man = floor(lltemp1 / den_man);                                      %    gh_man = lltemp1 / (Word48)den_man;
    gh_exp = -44 - den_exp;                                                 %    gh_exp = -44 - den_exp;

    ltemp = floor(gh_man / 2^(-23-gh_exp));                                 %    ltemp = (Word24)wL_msr(gh_man,(-23-gh_exp)); //(gh_man>>(-23-gh_exp));
    FilterH(1) = ltemp; %-2;                                                %    FilterH[0] = FilterH[1] = FilterH[2] = ltemp; //-2;
    FilterH(2) = ltemp;
    FilterH(3) = ltemp;


function y = wedge(x)
    max_x =  2^23 - 1;
    min_x = -2^23;
    y     = 0;
    if x ~= 0
        while (x < max_x / 2.0) && (x > min_x / 2.0)
            x = x + x;
            y = y + 1;
        end;
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
        ck                   = -2.0 * cosine((2.0 * (k + biquad_order) - 1.0) / (4.0 * biquad_order) * pi);
        biquads_LP(k, 1 : 3) = [1.0, 2.0, 1.0];
        biquads_LP(k, 4 : 6) = [c2 + ck * c + 1.0, 2.0 * (1.0 - c2), c2 - ck * c + 1.0];
        biquads_HP(k, 1 : 3) = [1.0, -2.0, 1.0];
        biquads_HP(k, 4 : 6) = biquads_LP(k, 4 : 6) / c2;
        biquads_LP(k, :)     = biquads_LP(k, :) / biquads_LP(k, 4);
        biquads_HP(k, :)     = biquads_HP(k, :) / biquads_HP(k, 4);
    end;


function y = cosine(x)
    % cos period = 2.pi
    x = x - 2.0 * pi * floor(x / 2.0 / pi);
    % x is now between 0 and 2.pi
    % if x >= pi, 2.pi-x is between 0 and pi and cos(2.pi-x) = cos(x)
    if x >= pi
        x    = 2.0 * pi - x;
    end;
    % x is now between 0 and pi
    % if x >= pi/2, pi-x is between 0 and pi/2 and cos(pi-x) = -cos(x)
    if x >= pi / 2.0
        x    = pi - x;
        sign = -1.0;
    else
        sign = 1.0;
    end;
    % x is now between 0 and pi/2
    % divide x by 2 n times while x > 0.01
    n = 0;
    while x > 0.01
        x = x * 0.5;
        n = n + 1;
    end;
    x2      = x * x;
    cosinus = 1.0 - x2 / 2.0 * (1.0 - x2 / 12.0 * (1.0 - x2 / 30.0 * (1.0 - x2 / 56.0)));
    while n > 0
        cosinus = 2.0 * cosinus * cosinus - 1.0;
        n       = n - 1;
    end;
    y = sign * cosinus;


function y = cotg(x)
    % cotg period = pi
    x = x - pi * floor(x / pi);
    % x is now between 0 and pi
    % if x >= pi/2, pi-x is between 0 and pi/2 and cotg(pi-x) = -cotg(x)
    if x >= pi / 2.0
        x    = pi - x;
        sign = -1.0;
    else
        sign = 1.0;
    end;
    n = 0;
    % x is now between 0 and pi/2
    % divide x by 2 n times while x > 0.01
    while x > 0.01
        x = x * 0.5;
        n = n + 1;
    end;
    x2      = x * x;
    sinus   = x * (1.0 - x2 / 6.0 * (1.0 - x2 / 20.0 * (1.0 - x2 / 42.0 * (1.0 - x2 / 72.0))));
    cosinus =      1.0 - x2 / 2.0 * (1.0 - x2 / 12.0 * (1.0 - x2 / 30.0 * (1.0 - x2 / 56.0)));
    while n > 0
        sinus   = 2.0 * cosinus * sinus;
        cosinus = 2.0 * cosinus * cosinus - 1.0;
        n       = n - 1;
    end;
    y = sign * cosinus / sinus;



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% linear phase FIR design (symetric filter) %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [FIR max_gain max_freq] = mdrc_bands_FIR(CutFreq, fs, FIR_order)
    n             = size(CutFreq, 2);
    N             = 1024;                       % size of iDFT (number of points of filters response curve)
    gains_HP_prev = ones(1, N);
    FIR           = zeros(n + 1, FIR_order);
    max_gain      = zeros(n + 1, 1);
    max_freq      = zeros(n + 1, 1);
    df1           = 0.5 * fs / N;
    slope         = 10.0;                       % slope of FIRs

    % band separation filters response curve construction
    f_start = 0.0;
    for i = 1 : n
        assert((CutFreq(i) > 0.0) && (CutFreq(i) <  fs / 2.0));
        assert((i == 1) || (CutFreq(i - 1) < CutFreq(i)));
        df2 = df1 / CutFreq(i);
        f   = 0.0;
        for j = 1 : N
            tmp              = slope * (f - 1.0);
            f                = f + df2;
            gains_LP_cur     = max(0.0, min(1.0, 0.5 - tmp));
            gains(j)         = gains_LP_cur * gains_HP_prev(j);
            gains_HP_prev(j) = max(0.0, min(1.0, 0.5 + tmp));
        end;
        f_end = CutFreq(i) / fs;
        [FIR(i, :) max_gain(i) max_freq(i)] = compute_FIR(f_start, f_end, gains, FIR_order, N);
        max_freq(i) = max_freq(i) * fs;
        f_start     = f_end;
    end;
    [FIR(n + 1, :) max_gain(n + 1) max_freq(n + 1)] = compute_FIR(f_start, 0.5, gains_HP_prev, FIR_order, N);
    max_freq(n + 1) = max_freq(n + 1) * fs;


function [b max_gain max_freq] = compute_FIR(f_start, f_end, gains, FIR_order, N)
    % if FIR_order is even, decrease it by 1 (to have an odd order)
    % and set the last coefficient to 0 to obtain the required order
    b = zeros(1, FIR_order);
    p = floor(FIR_order / 2);
    if FIR_order == 2 * p
        b(FIR_order) = 0.0;
        FIR_order    = FIR_order - 1;
    end;

    assert(N > p);                                          % N = size of iDFT (must be > p)
    cos_table = cos(pi * [0 : 2 * N - 1] / N);

    % Hamming weighted iDFT
    sign = 1.0;
    for i = 0 : p
        b_tmp = (gains(1) + sign * gains(N)) / 2.0;
        sign  = -sign;
        k     = 0;
        for j = 1 : N - 1
            k = k + i;
            if k >= 2 * N
                k = k - 2 * N;
            end;
            b_tmp = b_tmp + cos_table(k + 1) * gains(j + 1);
        end;
        b(p + 1 + i) = b_tmp / N * (0.54 + 0.46 * cosine(pi * i / p));
        b(p + 1 - i) = b(p + 1 + i);                        % symetrize filter
    end;

    % search max gain between f_start and f_end (pass-band zone)
    max_gain2 = -1.0;
    i_max     = 0;
    i_start   = floor(f_start * 2 * N);
    i_end     = floor(f_end   * 2 * N);
    for i = i_start : i_end
        gain_re = 0.0;
        gain_im = 0.0;
        k1      = 0;
        k2      = N / 2;
        for j = 1 : FIR_order
            gain_re = gain_re + b(j) * cos_table(k1 + 1);
            gain_im = gain_im + b(j) * cos_table(k2 + 1);
            k1 = k1 + i;
            k2 = k2 - i;
            if k1 >= 2 * N
                k1 = k1 - 2 * N;
            end;
            if k2 < 0
                k2 = k2 + 2 * N;
            end;
        end;
        gain2 = gain_re * gain_re + gain_im * gain_im;
        if gain2 > max_gain2
            max_gain2 = gain2;
            i_max     = i;
        end;
    end;
    max_gain = sqrt(max_gain2);
    max_freq = i_max / 2.0 / N;
