function [gains global_response band_responses index error] = mdrc_gains(bands)

    sampling_freq = 48000;
    FreqCutoff    = [0 1 2 3 4 5 6 7 8 9 10 15 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95 100 110 120 130 140 150 160 170] * 100;

    %                  b2,    -b1/2,      b0,   -a1/2,      a2
    LPCoefTab = [[      0,       -0,       0, 8388607, 8388607],
                 [    356,     -356,     356, 8310966, 8234749],
                 [   1411,    -1411,    1411, 8233344, 8083724],
                 [   3146,    -3146,    3146, 8155760, 7935495],
                 [   5541,    -5541,    5541, 8078234, 7790021],
                 [   8577,    -8577,    8577, 8000780, 7647261],
                 [  12238,   -12238,   12238, 7923418, 7507178],
                 [  16504,   -16504,   16504, 7846162, 7369732],
                 [  21358,   -21358,   21358, 7769030, 7234882],
                 [  26783,   -26783,   26783, 7692034, 7102590],
                 [  32761,   -32761,   32761, 7615190, 6972818],
                 [  70399,   -70399,   70399, 7233707, 6360402],
                 [ 119556,  -119556,  119556, 6857840, 5805295],
                 [ 178504,  -178504,  178504, 6488789, 5302984],
                 [ 245707,  -245707,  245707, 6127486, 4849190],
                 [ 319807,  -319807,  319807, 5774638, 4439894],
                 [ 399607,  -399607,  399607, 5430758, 4071336],
                 [ 484057,  -484057,  484057, 5096202, 3740025],
                 [ 572239,  -572239,  572239, 4771190, 3442727],
                 [ 663351,  -663351,  663351, 4455832, 3176458],
                 [ 756697,  -756697,  756697, 4150147, 2938474],
                 [ 851674,  -851674,  851674, 3854084, 2726259],
                 [ 947762,  -947762,  947762, 3567532, 2537505],
                 [1044512, -1044512, 1044512, 3290333, 2370105],
                 [1141538, -1141538, 1141538, 3022295, 2222134],
                 [1238513, -1238513, 1238513, 2763196, 2091835],
                 [1335156, -1335156, 1335156, 2512796, 1977608],
                 [1431229, -1431229, 1431229, 2270842, 1877993],
                 [1526534, -1526534, 1526534, 2037068, 1791664],
                 [1714194, -1714194, 1714194, 1592982, 1654134],
                 [1897115, -1897115, 1897115, 1178372, 1556594],
                 [2074617, -2074617, 2074617,  791105, 1492071],
                 [2246282, -2246282, 2246282,  429132, 1454786],
                 [2411881, -2411881, 2411881,   90515, 1439948],
                 [2571327, -2571327, 2571327, -226558, 1443585],
                 [2724636, -2724636, 2724636, -523771, 1462394]] / power(2, 23);

    HPCoefTab = [[8388607,  8388607, 8388607, 8388607, 8388607],
                 [8311322,  8311322, 8311322, 8310966, 8234749],
                 [8234756,  8234756, 8234756, 8233344, 8083724],
                 [8158906,  8158906, 8158906, 8155760, 7935495],
                 [8083774,  8083774, 8083774, 8078234, 7790021],
                 [8009357,  8009357, 8009357, 8000780, 7647261],
                 [7935656,  7935656, 7935656, 7923418, 7507178],
                 [7862666,  7862666, 7862666, 7846162, 7369732],
                 [7790387,  7790387, 7790387, 7769030, 7234882],
                 [7718816,  7718816, 7718816, 7692034, 7102590],
                 [7647952,  7647952, 7647952, 7615190, 6972818],
                 [7304106,  7304106, 7304106, 7233707, 6360402],
                 [6977396,  6977396, 6977396, 6857840, 5805295],
                 [6667292,  6667292, 6667292, 6488789, 5302984],
                 [6373192,  6373192, 6373192, 6127486, 4849190],
                 [6094444,  6094444, 6094444, 5774638, 4439894],
                 [5830366,  5830366, 5830366, 5430758, 4071336],
                 [5580260,  5580260, 5580260, 5096202, 3740025],
                 [5343428,  5343428, 5343428, 4771190, 3442727],
                 [5119182,  5119182, 5119182, 4455832, 3176458],
                 [4906844,  4906844, 4906844, 4150147, 2938474],
                 [4705759,  4705759, 4705759, 3854084, 2726259],
                 [4515294,  4515294, 4515294, 3567532, 2537505],
                 [4334844,  4334844, 4334844, 3290333, 2370105],
                 [4163833,  4163833, 4163833, 3022295, 2222134],
                 [4001709,  4001709, 4001709, 2763196, 2091835],
                 [3847952,  3847952, 3847952, 2512796, 1977608],
                 [3702071,  3702071, 3702071, 2270842, 1877993],
                 [3563602,  3563602, 3563602, 2037068, 1791664],
                 [3307176,  3307176, 3307176, 1592982, 1654134],
                 [3075486,  3075486, 3075486, 1178372, 1556594],
                 [2865722,  2865722, 2865722,  791105, 1492071],
                 [2675414,  2675414, 2675414,  429132, 1454786],
                 [2502396,  2502396, 2502396,   90515, 1439948],
                 [2344769,  2344769, 2344769, -226558, 1443585],
                 [2200865,  2200865, 2200865, -523771, 1462394]] / power(2, 23);

    LPCoefTab = [LPCoefTab(:, 3), LPCoefTab(:, 2) * -2, LPCoefTab(:, 1), LPCoefTab(:, 4) * -2, LPCoefTab(:, 5)];
    HPCoefTab = [HPCoefTab(:, 3), HPCoefTab(:, 2) * -2, HPCoefTab(:, 1), HPCoefTab(:, 4) * -2, HPCoefTab(:, 5)];
    N         = 4096;
    weigthing = compute_weigthing(sampling_freq, N);
    nb_bands  = min(size(bands, 2), 5);

    if nb_bands > 1
        nb_filters = size(FreqCutoff, 2);
        index      = zeros(1, nb_bands);

        for i = 2 : nb_bands
            for j = 1 : nb_filters
                if bands(i) <= FreqCutoff(j)
                    index(i - 1) = j;

                    % select nearest frequency
                    if j > 1
                        if (FreqCutoff(j) - bands(i)) > (bands(i) - FreqCutoff(j - 1))
                            index(i - 1) = j - 1;
                        end;
                    end;
                    break;
                end;
            end;
        end;


        band_responses = zeros(N, nb_bands);
        for i = 1 : nb_bands
            if i == 1
                b_LP =    LPCoefTab(index(i),     1 : 3);
                a_LP = [1 LPCoefTab(index(i),     4 : 5)];
                b_HP = [1];
                a_HP = [1];
            elseif i < nb_bands
                b_LP =    LPCoefTab(index(i),     1 : 3);
                a_LP = [1 LPCoefTab(index(i),     4 : 5)];
                b_HP =    HPCoefTab(index(i - 1), 1 : 3);
                a_HP = [1 HPCoefTab(index(i - 1), 4 : 5)];
            else
                b_LP = [1];
                a_LP = [1];
                b_HP =    HPCoefTab(index(i - 1), 1 : 3);
                a_HP = [1 HPCoefTab(index(i - 1), 4 : 5)];
            end;
            b_band               = conv(b_LP, b_HP);
            a_band               = conv(a_LP, a_HP);
            b_band               = conv(b_band, b_band);
            a_band               = conv(a_band, a_band);
            band_responses(:, i) = freqz(b_band, a_band, N);
        end;

        gains       = 1 ./ max(abs(band_responses));
        error       = error_response(band_responses, gains, weigthing);
        best_gains  = gains;
        best_error  = error;
        dx          = 0.1;
        error_max   = 0.05;
        counter     = 1;
        counter_max = power(10, nb_bands);

        fprintf(1, 'counter = %d, best error = %f\n', counter, error);
        f_axis          = sampling_freq / 2 * [0 : N - 1] / N;
        response_module = abs(band_responses * gains');
        for i = 1 : nb_bands
            band_modules(:, i) = abs(band_responses(:, i)) * gains(1, i);
        end;
        figure(1);
        clf(1);
        subplot(2, 2, 1);
        plot(f_axis, band_modules);
        subplot(2, 2, 2);
        plot(f_axis, max(-100, 20 * log10(band_modules)));
        subplot(2, 2, 3);
        plot(f_axis, response_module);
        subplot(2, 2, 4);
        plot(f_axis, max(-100, 20 * log10(response_module)));
        drawnow; pause(0.1)

        % coarse gains search
        while (best_error > error_max) && (counter < counter_max)
            [counter best_gains best_error] = solve_gains(counter, dx, 5, nb_bands, best_gains, best_error, band_responses, weigthing, error_max, counter_max, f_axis);
            dx = dx * 0.5;
        end;

        % fine gains search
        counter_max = 2 * counter_max;
        error_max   = 0.2 * error_max;
        dx          = 0.2 * dx;
        [counter best_gains best_error] = solve_gains(counter, dx, 10, nb_bands, best_gains, best_error, band_responses, weigthing, error_max, counter_max, f_axis);

        gains = best_gains;
        error = best_error;
        fprintf(1, 'counter = %d, best error = %f, dx = %f\n', counter, error, dx);

    else

        index          = [0];
        gains          = [1];
        band_responses = freqz([1], [1], N);
        error          = error_response(band_responses, gains, weigthing);
        fprintf(1, 'no sub-band\n');

    end;

    global_response = band_responses * gains';
    for i = 1 : nb_bands
        fprintf(1, 'gain %d = %1.6f\n', i, gains(1, i));
    end;
    fprintf(1, 'error = %1.6f\n', error);


function [counter best_gains best_error] = solve_gains(counter, dx, n, nb_bands, best_gains, best_error, band_responses, weigthing, error_max, counter_max, f_axis)
    gains = best_gains;
    for i1 = -n : n
        x1    = gains;
        x1(1) = gains(1) * power(1 + dx, i1);
        if nb_bands >= 2
            x2 = x1;
            for i2 = -n : n
                x2(2) = x1(2) * power(1 + dx, i2);
                if nb_bands >= 3
                    x3 = x2;
                    for i3 = -n : n
                        x3(3) = x2(3) * power(1 + dx, i3);
                        if nb_bands >= 4
                            x4  = x3;
                            for i4 = -n : n
                                x4(4) = x3(4) * power(1 + dx, i4);
                                if nb_bands >= 5
                                    x5 = x4;
                                    for i5 = -n : n
                                        x5(5)   = x4(5) * power(1 + dx, i5);
                                        error   = error_response(band_responses, x5, weigthing);
                                        counter = counter + 1;
                                        if error < best_error
                                            best_gains = x5;
                                            best_error = error;
                                            display_result(counter, error, f_axis, band_responses, best_gains, dx);
                                        end;
                                    end;
                                    gains = best_gains;
                                    error = best_error;
                                    if (error < error_max) || (counter > counter_max)
                                        return;
                                    end;
                                else
                                    error   = error_response(band_responses, x4, weigthing);
                                    counter = counter + 1;
                                    if error < best_error
                                        best_gains = x4;
                                        best_error = error;
                                        display_result(counter, error, f_axis, band_responses, best_gains, dx);
                                    end;
                                end;
                            end;
                            gains = best_gains;
                            error = best_error;
                            if (error < error_max) || (counter > counter_max)
                                return;
                            end;
                        else
                            error   = error_response(band_responses, x3, weigthing);
                            counter = counter + 1;
                            if error < best_error
                                best_gains = x3;
                                best_error = error;
                                display_result(counter, error, f_axis, band_responses, best_gains, dx);
                            end;
                        end;
                    end;
                    gains = best_gains;
                    error = best_error;
                    if (error < error_max) || (counter > counter_max)
                        return;
                    end;
                else
                    error   = error_response(band_responses, x2, weigthing);
                    counter = counter + 1;
                    if error < best_error
                        best_gains = x2;
                        best_error = error;
                        display_result(counter, error, f_axis, band_responses, best_gains, dx);
                    end;
                end;
            end;
            gains = best_gains;
            error = best_error;
            if (error < error_max) || (counter > counter_max)
                return;
            end;
        else
            error   = error_response(band_responses, x1, weigthing);
            counter = counter + 1;
            if error < best_error
                best_gains = x1;
                best_error = error;
                display_result(counter, error, f_axis, band_responses, best_gains, dx);
            end;
        end;
    end;

function display_result(counter, error, f_axis, band_responses, gains, dx)
    fprintf(1, 'counter = %d, best error = %f, dx = %f\n', counter, error, dx);
    response_module = abs(band_responses * gains');
    subplot(2, 2, 3);
    plot(f_axis, response_module);
    subplot(2, 2, 4);
    plot(f_axis, max(-100, 20 * log10(response_module)));
    drawnow; pause(0.1)


function [weigthing] = compute_weigthing(sampling_freq, N)
    weigthing  = ones(1, N);
    ind_1000Hz = round(1000 / sampling_freq * 2 * N);
    for i = ind_1000Hz + 1 : N
        weigthing(i) = power(2, 1 - i / ind_1000Hz);
    end;
    weigthing = weigthing / sum(weigthing);


function [error] = error_response(band_responses, gains, weigthing)
    global_response = band_responses * gains';
    err_response    = abs(global_response) - ones(size(global_response));
    error           = sqrt(weigthing * (err_response .* err_response));
