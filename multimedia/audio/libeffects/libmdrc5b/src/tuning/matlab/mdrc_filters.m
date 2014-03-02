clear;

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

nb_filters = size(FreqCutoff, 2);
N          = 4096;
LPCoefTab  = [LPCoefTab(:, 3), LPCoefTab(:, 2) * -2, LPCoefTab(:, 1), LPCoefTab(:, 4) * -2, LPCoefTab(:, 5)];
HPCoefTab  = [HPCoefTab(:, 3), HPCoefTab(:, 2) * -2, HPCoefTab(:, 1), HPCoefTab(:, 4) * -2, HPCoefTab(:, 5)];

LP_response = zeros(nb_filters, N);
HP_response = zeros(nb_filters, N);
for i = 1 : nb_filters
    b_LP2 = conv(     LPCoefTab(i, 1 : 3),       LPCoefTab(i, 1 : 3));
    a_LP2 = conv([1.0 LPCoefTab(i, 4 : 5)], [1.0 LPCoefTab(i, 4 : 5)]);
    b_HP2 = conv(     HPCoefTab(i, 1 : 3),       HPCoefTab(i, 1 : 3));
    a_HP2 = conv([1.0 HPCoefTab(i, 4 : 5)], [1.0 HPCoefTab(i, 4 : 5)]);
    LP_response(i, :) = freqz(b_LP2, a_LP2, N);
    HP_response(i, :) = freqz(b_HP2, a_HP2, N);
end;
HP_response(1, :) = zeros(1, N);
LP_response_dB = 20.0 * log10(abs(LP_response));
HP_response_dB = 20.0 * log10(abs(HP_response));
freq_axis      = [0 : N - 1] * sampling_freq / N / 2;

figure(1);
for i = 1 : nb_filters
    clf(1);
    subplot(2, 1, 1);
    plot(freq_axis, abs(LP_response(i, :)), 'r-');
    hold on;
    plot(freq_axis, abs(HP_response(i, :)), 'b-');
    hold on;
    plot([FreqCutoff(i) FreqCutoff(i)], [0.0 1.0], 'g--');
    hold on;
    subplot(2, 1, 2);
    plot(freq_axis, max(-60.0, LP_response_dB(i, :)), 'r-');
    hold on;
    plot(freq_axis, max(-60.0, HP_response_dB(i, :)), 'b-');
    hold on;
    plot([FreqCutoff(i) FreqCutoff(i)], [-60.0 0.0], 'g--');
    hold off;
    fprintf(1, 'FreqCutoff(%d) : %f\n', i, FreqCutoff(i));
    pause(0.2);
end;

figure(2);
clf(2);
for i = 1 : nb_filters
    subplot(2, 1, 1);
    plot(freq_axis, abs(LP_response(i, :)), 'r-');
    hold on;
    plot(freq_axis, abs(HP_response(i, :)), 'b-');
    hold on;
    plot([FreqCutoff(i) FreqCutoff(i)], [0.0 1.0], 'g--');
    hold on;
    subplot(2, 1, 2);
    plot(freq_axis, max(-60.0, LP_response_dB(i, :)), 'r-');
    hold on;
    plot(freq_axis, max(-60.0, HP_response_dB(i, :)), 'b-');
    hold on;
    plot([FreqCutoff(i) FreqCutoff(i)], [-60.0 0.0], 'g--');
    hold on;
end;
hold off;

figure(3);
clf(3);
ind_LP = 25;
ind_HP = 20;
BP_response = LP_response(ind_LP, :) .* HP_response(ind_HP, :);
BP_response_dB = 20.0 * log10(abs(BP_response));
subplot(2, 1, 1);
plot(freq_axis, abs(LP_response(ind_LP, :)), 'r-');
hold on;
plot(freq_axis, abs(HP_response(ind_HP, :)), 'b-');
hold on;
plot(freq_axis, abs(BP_response), 'g-');
hold on;
subplot(2, 1, 2);
plot(freq_axis, max(-60.0, LP_response_dB(ind_LP, :)), 'r-');
hold on;
plot(freq_axis, max(-60.0, HP_response_dB(ind_HP, :)), 'b-');
hold on;
plot(freq_axis, max(-60.0, BP_response_dB), 'g-');
hold off;

figure(4);
clf(4);
ind1_LP = 15;
ind2_LP = 20;
ind2_HP = ind1_LP;
ind3_LP = 30;
ind3_HP = ind2_LP;
ind4_HP = ind3_LP;
BP1_response = LP_response(ind1_LP, :);
BP2_response = LP_response(ind2_LP, :) .* HP_response(ind2_HP, :);
BP3_response = LP_response(ind3_LP, :) .* HP_response(ind3_HP, :);
BP4_response =                            HP_response(ind4_HP, :);
BP_response  = BP1_response + BP2_response + BP3_response + BP4_response;
BP1_response_dB = 20.0 * log10(abs(BP1_response));
BP2_response_dB = 20.0 * log10(abs(BP2_response));
BP3_response_dB = 20.0 * log10(abs(BP3_response));
BP4_response_dB = 20.0 * log10(abs(BP4_response));
BP_response_dB  = 20.0 * log10(abs(BP_response));
subplot(2, 1, 1);
plot(freq_axis, abs(LP_response(ind1_LP, :)), 'r-');
hold on;
plot(freq_axis, abs(BP1_response), 'g-');
hold on;
plot(freq_axis, abs(LP_response(ind2_LP, :)), 'r-');
hold on;
plot(freq_axis, abs(HP_response(ind2_HP, :)), 'b-');
hold on;
plot(freq_axis, abs(BP2_response), 'g-');
hold on;
plot(freq_axis, abs(LP_response(ind3_LP, :)), 'r-');
hold on;
plot(freq_axis, abs(HP_response(ind3_HP, :)), 'b-');
hold on;
plot(freq_axis, abs(BP3_response), 'g-');
hold on;
plot(freq_axis, abs(HP_response(ind4_HP, :)), 'b-');
hold on;
plot(freq_axis, abs(BP4_response), 'g-');
hold on;
plot(freq_axis, abs(BP_response), 'c-');
hold on;
subplot(2, 1, 2);
plot(freq_axis, max(-60.0, LP_response_dB(ind1_LP, :)), 'r-');
hold on;
plot(freq_axis, max(-60.0, BP1_response_dB), 'g-');
hold on;
plot(freq_axis, max(-60.0, LP_response_dB(ind2_LP, :)), 'r-');
hold on;
plot(freq_axis, max(-60.0, HP_response_dB(ind2_HP, :)), 'b-');
hold on;
plot(freq_axis, max(-60.0, BP2_response_dB), 'g-');
hold on;
plot(freq_axis, max(-60.0, LP_response_dB(ind3_LP, :)), 'r-');
hold on;
plot(freq_axis, max(-60.0, HP_response_dB(ind3_HP, :)), 'b-');
hold on;
plot(freq_axis, max(-60.0, BP3_response_dB), 'g-');
hold on;
plot(freq_axis, max(-60.0, HP_response_dB(ind4_HP, :)), 'b-');
hold on;
plot(freq_axis, max(-60.0, BP4_response_dB), 'g-');
hold on;
plot(freq_axis, max(-60.0, BP_response_dB), 'c-');
hold off;


[gains global_response band_responses index error] = mdrc_gains([0 500 1500 5000]);
figure(5);
clf(5);
ind1_LP                 = index(1);
ind2_LP                 = index(2);
ind2_HP                 = index(1);
ind3_LP                 = index(3);
ind3_HP                 = index(2);
ind4_HP                 = index(3);
BP1_response            = LP_response(ind1_LP, :);
BP2_response            = LP_response(ind2_LP, :) .* HP_response(ind2_HP, :);
BP3_response            = LP_response(ind3_LP, :) .* HP_response(ind3_HP, :);
BP4_response            =                            HP_response(ind4_HP, :);
BP_wo_gains_response    = BP1_response + BP2_response + BP3_response + BP4_response;
BP_w_gains_response     = gains(1) * BP1_response + gains(2) * BP2_response + gains(3) * BP3_response + gains(4) * BP4_response;
BP1_response_dB         = 20.0 * log10(abs(BP1_response));
BP2_response_dB         = 20.0 * log10(abs(BP2_response));
BP3_response_dB         = 20.0 * log10(abs(BP3_response));
BP4_response_dB         = 20.0 * log10(abs(BP4_response));
BP_wo_gains_response_dB = 20.0 * log10(abs(BP_wo_gains_response));
BP_w_gains_response_dB  = 20.0 * log10(abs(BP_w_gains_response));
subplot(2, 1, 1);
plot(freq_axis, abs(BP1_response), 'g-');
hold on;
plot(freq_axis, abs(BP2_response), 'g-');
hold on;
plot(freq_axis, abs(BP3_response), 'g-');
hold on;
plot(freq_axis, abs(BP4_response), 'g-');
hold on;
plot(freq_axis, abs(BP_wo_gains_response), 'c-');
hold on;
plot(freq_axis, abs(BP_w_gains_response), 'm-');
hold on;
subplot(2, 1, 2);
plot(freq_axis, max(-60.0, BP1_response_dB), 'g-');
hold on;
plot(freq_axis, max(-60.0, BP2_response_dB), 'g-');
hold on;
plot(freq_axis, max(-60.0, BP3_response_dB), 'g-');
hold on;
plot(freq_axis, max(-60.0, BP4_response_dB), 'g-');
hold on;
plot(freq_axis, max(-60.0, BP_wo_gains_response_dB), 'c-');
hold on;
plot(freq_axis, max(-60.0, BP_w_gains_response_dB), 'm-');
hold off;
