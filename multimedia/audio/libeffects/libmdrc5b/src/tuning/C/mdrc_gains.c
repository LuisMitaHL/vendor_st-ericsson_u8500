/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NB_BANDS_MAX  5
#define N             4096
#define SAMPLING_FREQ 48000.0
#define PI            3.141592653589793236

typedef struct
{
    double re;
    double im;
} t_complex;

typedef struct
{
    int       index[NB_BANDS_MAX];
    double    gains[NB_BANDS_MAX];
    t_complex band_responses[NB_BANDS_MAX][N];
    t_complex global_response[N];
    double    error;
} t_mdrc_gains;

static int mdrc_gains(double *bands, int nb_bands, t_mdrc_gains **p_p_mdrc_gains);

static void compute_weigthing(double *weigthing);

static void response(double b0_LP, double b1_LP, double b2_LP1, double a1_LP, double a2_LP,
                     double b0_HP, double b1_HP, double b2_HP1, double a1_HP, double a2_HP,
                     t_complex *band_response);

static double error_response(t_mdrc_gains *p_mdrc_gains, double *p_gains, int nb_bands, double *p_weigthing);

static void solve_gains(int *p_counter, double dx, int n, int nb_bands, double *p_best_gains, double *p_best_error, t_mdrc_gains *p_mdrc_gains, double *p_weigthing, double error_max, int counter_max);

static void display_result(int counter, double error, double dx);


int main(int argc, char *argv[])
{
    double       bands[NB_BANDS_MAX];
    int          nb_bands;
    t_mdrc_gains *p_mdrc_gains;
    int          i;


    if(argc < 2)
    {
        printf("usage : mrdc_gains <freq_cut_off(1)>..<freq_cut_off(n)> with n=1..%d\n", NB_BANDS_MAX);
        return 1;
    }

    nb_bands = argc - 1;
    if(nb_bands > NB_BANDS_MAX)
    {
        nb_bands = NB_BANDS_MAX;
    }
    for(i = 0; i < nb_bands; i++)
    {
        bands[i] = atof(argv[i + 1]);
    }

    if(mdrc_gains(bands, nb_bands, &p_mdrc_gains) != 0)
    {
        fprintf(stderr, "Allocation error !\n");
        return -1;
    }

    for(i = 0; i < nb_bands; i++)
    {
        printf("gain %d = %1.6lf\n", i + 1, p_mdrc_gains->gains[i]);
    }
    printf("error = %1.6lf\n", p_mdrc_gains->error);

    free(p_mdrc_gains);

    return 0;
}


static int mdrc_gains(double *bands, int nb_bands, t_mdrc_gains **p_p_mdrc_gains)
// inputs :
//      bands           : band cut off frequencies (first band cut off frequency = 0)
//      nb_bands        : number of bands (1 to 5)
//
// outputs (into **p_p_mdrc_gains) :
//      index           : index of filter for each band
//      gains           : best found gains for each band
//      band_responses  : response of each band
//      global_response : global response
//      error           : error
{
    double FreqCutoff[]   = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 110, 120, 130, 140, 150, 160, 170};

    //                             b2,    -b1/2,      b0,   -a1/2,      a2
    double LPCoefTab[][5] = {{      0,       -0,       0, 8388607, 8388607},
                             {    356,     -356,     356, 8310966, 8234749},
                             {   1411,    -1411,    1411, 8233344, 8083724},
                             {   3146,    -3146,    3146, 8155760, 7935495},
                             {   5541,    -5541,    5541, 8078234, 7790021},
                             {   8577,    -8577,    8577, 8000780, 7647261},
                             {  12238,   -12238,   12238, 7923418, 7507178},
                             {  16504,   -16504,   16504, 7846162, 7369732},
                             {  21358,   -21358,   21358, 7769030, 7234882},
                             {  26783,   -26783,   26783, 7692034, 7102590},
                             {  32761,   -32761,   32761, 7615190, 6972818},
                             {  70399,   -70399,   70399, 7233707, 6360402},
                             { 119556,  -119556,  119556, 6857840, 5805295},
                             { 178504,  -178504,  178504, 6488789, 5302984},
                             { 245707,  -245707,  245707, 6127486, 4849190},
                             { 319807,  -319807,  319807, 5774638, 4439894},
                             { 399607,  -399607,  399607, 5430758, 4071336},
                             { 484057,  -484057,  484057, 5096202, 3740025},
                             { 572239,  -572239,  572239, 4771190, 3442727},
                             { 663351,  -663351,  663351, 4455832, 3176458},
                             { 756697,  -756697,  756697, 4150147, 2938474},
                             { 851674,  -851674,  851674, 3854084, 2726259},
                             { 947762,  -947762,  947762, 3567532, 2537505},
                             {1044512, -1044512, 1044512, 3290333, 2370105},
                             {1141538, -1141538, 1141538, 3022295, 2222134},
                             {1238513, -1238513, 1238513, 2763196, 2091835},
                             {1335156, -1335156, 1335156, 2512796, 1977608},
                             {1431229, -1431229, 1431229, 2270842, 1877993},
                             {1526534, -1526534, 1526534, 2037068, 1791664},
                             {1714194, -1714194, 1714194, 1592982, 1654134},
                             {1897115, -1897115, 1897115, 1178372, 1556594},
                             {2074617, -2074617, 2074617,  791105, 1492071},
                             {2246282, -2246282, 2246282,  429132, 1454786},
                             {2411881, -2411881, 2411881,   90515, 1439948},
                             {2571327, -2571327, 2571327, -226558, 1443585},
                             {2724636, -2724636, 2724636, -523771, 1462394}};

    double HPCoefTab[][5] = {{8388607,  8388607, 8388607, 8388607, 8388607},
                             {8311322,  8311322, 8311322, 8310966, 8234749},
                             {8234756,  8234756, 8234756, 8233344, 8083724},
                             {8158906,  8158906, 8158906, 8155760, 7935495},
                             {8083774,  8083774, 8083774, 8078234, 7790021},
                             {8009357,  8009357, 8009357, 8000780, 7647261},
                             {7935656,  7935656, 7935656, 7923418, 7507178},
                             {7862666,  7862666, 7862666, 7846162, 7369732},
                             {7790387,  7790387, 7790387, 7769030, 7234882},
                             {7718816,  7718816, 7718816, 7692034, 7102590},
                             {7647952,  7647952, 7647952, 7615190, 6972818},
                             {7304106,  7304106, 7304106, 7233707, 6360402},
                             {6977396,  6977396, 6977396, 6857840, 5805295},
                             {6667292,  6667292, 6667292, 6488789, 5302984},
                             {6373192,  6373192, 6373192, 6127486, 4849190},
                             {6094444,  6094444, 6094444, 5774638, 4439894},
                             {5830366,  5830366, 5830366, 5430758, 4071336},
                             {5580260,  5580260, 5580260, 5096202, 3740025},
                             {5343428,  5343428, 5343428, 4771190, 3442727},
                             {5119182,  5119182, 5119182, 4455832, 3176458},
                             {4906844,  4906844, 4906844, 4150147, 2938474},
                             {4705759,  4705759, 4705759, 3854084, 2726259},
                             {4515294,  4515294, 4515294, 3567532, 2537505},
                             {4334844,  4334844, 4334844, 3290333, 2370105},
                             {4163833,  4163833, 4163833, 3022295, 2222134},
                             {4001709,  4001709, 4001709, 2763196, 2091835},
                             {3847952,  3847952, 3847952, 2512796, 1977608},
                             {3702071,  3702071, 3702071, 2270842, 1877993},
                             {3563602,  3563602, 3563602, 2037068, 1791664},
                             {3307176,  3307176, 3307176, 1592982, 1654134},
                             {3075486,  3075486, 3075486, 1178372, 1556594},
                             {2865722,  2865722, 2865722,  791105, 1492071},
                             {2675414,  2675414, 2675414,  429132, 1454786},
                             {2502396,  2502396, 2502396,   90515, 1439948},
                             {2344769,  2344769, 2344769, -226558, 1443585},
                             {2200865,  2200865, 2200865, -523771, 1462394}};

    const int    nb_filters = sizeof(FreqCutoff) / sizeof(*FreqCutoff);
    double       coef = pow(2.0, -23.0);
    double       swap_tmp, max_response;
    double       error, best_error, error_max, dx;
    double       weigthing[N];
    double       best_gains[NB_BANDS_MAX];
    int          i, j, counter, counter_max;
    t_mdrc_gains *p_mdrc_gains;
    int          *p_index;
    double       *p_gains;
    t_complex    *p_band_response;
    t_complex    *p_global_response;


    nb_bands = (nb_bands > NB_BANDS_MAX ? NB_BANDS_MAX : nb_bands);

    p_mdrc_gains = (t_mdrc_gains *) malloc(sizeof(t_mdrc_gains));
    if(p_mdrc_gains == NULL)
    {
        return -1;
    }
    p_index           = p_mdrc_gains->index;
    p_gains           = p_mdrc_gains->gains;
    p_global_response = p_mdrc_gains->global_response;
    *p_p_mdrc_gains   = p_mdrc_gains;

    for(i = 0; i < sizeof(FreqCutoff) / sizeof(*FreqCutoff); i++)
    {
        FreqCutoff[i] *= 100.0;
    }
    for(i = 0; i < sizeof(LPCoefTab) / sizeof(*LPCoefTab); i++)
    {
        swap_tmp         = LPCoefTab[i][2] * coef;
        LPCoefTab[i][2]  = LPCoefTab[i][0] * coef;
        LPCoefTab[i][0]  = swap_tmp;
        LPCoefTab[i][1] *= coef * -2.0;
        LPCoefTab[i][3] *= coef * -2.0;
        LPCoefTab[i][4] *= coef;
    }
    for(i = 0; i < sizeof(HPCoefTab) / sizeof(*HPCoefTab); i++)
    {
        swap_tmp         = HPCoefTab[i][2] * coef;
        HPCoefTab[i][2]  = HPCoefTab[i][0] * coef;
        HPCoefTab[i][0]  = swap_tmp;
        HPCoefTab[i][1] *= coef * -2.0;
        HPCoefTab[i][3] *= coef * -2.0;
        HPCoefTab[i][4] *= coef;
    }
    compute_weigthing(weigthing);

    if(nb_bands > 1)
    {
        for(i = 1; i < nb_bands; i++)
        {
            for(j = 0; j < nb_filters; j++)
            {
                if(bands[i] <= FreqCutoff[j])
                {
                    p_index[i - 1] = j;

                    // select nearest frequency
                    if(j > 0)
                    {
                        if((FreqCutoff[j] - bands[i]) > (bands[i] - FreqCutoff[j - 1]))
                        {
                            p_index[i - 1] = j - 1;
                        }
                    }
                    break;
                }
            }
        }


        for(i = 0; i < nb_bands; i++)
        {
            double max_square;
            int    index_LP, index_HP;

            p_band_response = p_mdrc_gains->band_responses[i];
            if(i == 0)
            {
                index_LP = p_index[i];
                response(LPCoefTab[index_LP][0], LPCoefTab[index_LP][1], LPCoefTab[index_LP][2], LPCoefTab[index_LP][3], LPCoefTab[index_LP][4],
                         1.0,                    0.0,                    0.0,                    0.0,                    0.0,
                         p_band_response);
            }
            else if(i < nb_bands - 1)
            {
                index_LP = p_index[i];
                index_HP = p_index[i - 1];
                response(LPCoefTab[index_LP][0], LPCoefTab[index_LP][1], LPCoefTab[index_LP][2], LPCoefTab[index_LP][3], LPCoefTab[index_LP][4],
                         HPCoefTab[index_HP][0], HPCoefTab[index_HP][1], HPCoefTab[index_HP][2], HPCoefTab[index_HP][3], HPCoefTab[index_HP][4],
                         p_band_response);
            }
            else
            {
                index_HP = p_index[i - 1];
                response(1.0,                    0.0,                    0.0,                    0.0,                    0.0,
                         HPCoefTab[index_HP][0], HPCoefTab[index_HP][1], HPCoefTab[index_HP][2], HPCoefTab[index_HP][3], HPCoefTab[index_HP][4],
                         p_band_response);
            }

            max_square = 0.0;
            for(j = 0; j < N; j++)
            {
                double square = p_band_response[j].re * p_band_response[j].re + p_band_response[j].im * p_band_response[j].im;

                if(square > max_square)
                {
                    max_square = square;
                }
            }
            p_gains[i] = 1.0 / sqrt(max_square);
        }

        error = error_response(p_mdrc_gains, p_gains, nb_bands, weigthing);
        memcpy(best_gains, p_gains, nb_bands * sizeof(double));
        best_error  = error;
        dx          = 0.1;
        error_max   = 0.05;
        counter     = 1;
        counter_max = 1000000;
        display_result(counter, error, dx);

        // coarse gains search
        while((best_error > error_max) && (counter < counter_max))
        {
            solve_gains(&counter, dx, 5, nb_bands, best_gains, &best_error, p_mdrc_gains, weigthing, error_max, counter_max);
            dx *= 0.5;
        }

        // fine gains search
        counter_max *= 2;
        error_max   *= 0.2;
        dx          *= 0.2;
        solve_gains(&counter, dx, 10, nb_bands, best_gains, &best_error, p_mdrc_gains, weigthing, error_max, counter_max);

        memcpy(p_gains, best_gains, nb_bands * sizeof(double));
        error = best_error;
        display_result(counter, error, dx);
    }
    else
    {
        p_index[0] = 0;
        p_gains[0] = 1.0;
        response(1.0, 0.0, 0.0, 0.0, 0.0,
                 1.0, 0.0, 0.0, 0.0, 0.0,
                 p_mdrc_gains->band_responses[0]);
        error = error_response(p_mdrc_gains, p_gains, nb_bands, weigthing);
        printf("no sub-band\n");
    }

    for(i = 0; i < N; i++)
    {
        p_global_response[i].re = 0.0;
        p_global_response[i].im = 0.0;
        for(j = 0; j < nb_bands; j++)
        {
            p_global_response[i].re += p_gains[j] * p_mdrc_gains->band_responses[j][i].re;
            p_global_response[i].im += p_gains[j] * p_mdrc_gains->band_responses[j][i].im;
        }
    }

    p_mdrc_gains->error = error;

    return 0;
}


static void compute_weigthing(double *p_weigthing)
{
    double sum_weigthing;
    int    i, ind_1000Hz = round(1000.0 / SAMPLING_FREQ * 2.0 * ((double) N));

    for(i = 0; i < ind_1000Hz; i++)
    {
        p_weigthing[i] = 1.0;
    }
    sum_weigthing = (double) ind_1000Hz;
    for(; i < N; i++)
    {
        p_weigthing[i] = pow(2.0, 1.0 - ((double) i) / ((double) ind_1000Hz));
        sum_weigthing += p_weigthing[i];
    }
    for(i = 0; i < N; i++)
    {
        p_weigthing[i] /= sum_weigthing;
    }
}


static void response(double b0_LP, double b1_LP, double b2_LP, double a1_LP, double a2_LP,
                     double b0_HP, double b1_HP, double b2_HP, double a1_HP, double a2_HP,
                     t_complex *p_band_response)
{
    double theta, cos1, sin1, cos2, sin2, tmp;
    double re_b_LP, im_b_LP, re_a_LP, im_a_LP, re_b_HP, im_b_HP, re_a_HP, im_a_HP, re_LP, im_LP, re_HP, im_HP;
    int    i;

    // b_LP(z)^2 / a_LP(z)^2 * b_HP(z)^2 / a_HP(z)^2 with z = exp(-j*2*pi*f/sampling_freq)
    for(i = 0; i < N; i++)
    {
        theta   = -PI * ((double) i) / ((double) N);
        cos1    = cos(theta);
        sin1    = sin(theta);
        cos2    = cos1 * cos1 - sin1 * sin1;
        sin2    = 2.0 * sin1 * cos1;

        re_b_LP = b0_LP + b1_LP * cos1 + b2_LP * cos2;              // b_LP(z)
        im_b_LP =         b1_LP * sin1 + b2_LP * sin2;              //

        re_a_LP = 1.0   + a1_LP * cos1 + a2_LP * cos2;              // a_LP(z)
        im_a_LP =         a1_LP * sin1 + a2_LP * sin2;              //

        re_b_HP = b0_HP + b1_HP * cos1 + b2_HP * cos2;              // b_HP(z)
        im_b_HP =         b1_HP * sin1 + b2_HP * sin2;              //

        re_a_HP = 1.0   + a1_HP * cos1 + a2_HP * cos2;              // a_HP(z)
        im_a_HP =         a1_HP * sin1 + a2_HP * sin2;              //

        tmp     = re_b_LP * re_b_LP - im_b_LP * im_b_LP;            // b_LP(z)^2
        im_b_LP = 2.0 * re_b_LP * im_b_LP;                          //
        re_b_LP = tmp;                                              //

        tmp     = re_a_LP * re_a_LP - im_a_LP * im_a_LP;            // a_LP(z)^2
        im_a_LP = 2.0 * re_a_LP * im_a_LP;                          //
        re_a_LP = tmp;                                              //

        tmp     = re_b_HP * re_b_HP - im_b_HP * im_b_HP;            // b_HP(z)^2
        im_b_HP = 2.0 * re_b_HP * im_b_HP;                          //
        re_b_HP = tmp;                                              //

        tmp     = re_a_HP * re_a_HP - im_a_HP * im_a_HP;            // a_HP(z)^2
        im_a_HP = 2.0 * re_a_HP * im_a_HP;                          //
        re_a_HP = tmp;                                              //

        tmp     =  re_a_LP * re_a_LP + im_a_LP * im_a_LP;           // b_LP(z)^2 / a_LP(z)^2
        re_LP   = (re_b_LP * re_a_LP + im_b_LP * im_a_LP) / tmp;    //
        im_LP   = (im_b_LP * re_a_LP - re_b_LP * im_a_LP) / tmp;    //

        tmp     =  re_a_HP * re_a_HP + im_a_HP * im_a_HP;           // b_HP(z)^2 / a_HP(z)^2
        re_HP   = (re_b_HP * re_a_HP + im_b_HP * im_a_HP) / tmp;    //
        im_HP   = (im_b_HP * re_a_HP - re_b_HP * im_a_HP) / tmp;    //

        p_band_response[i].re = re_LP * re_HP - im_LP * im_HP;      // b_LP(z)^2 / a_LP(z)^2 * b_HP(z)^2 / a_HP(z)^2
        p_band_response[i].im = re_LP * im_HP + im_LP * re_HP;      //
    }
}


double error_response(t_mdrc_gains *p_mdrc_gains, double *p_gains, int nb_bands, double *p_weigthing)
{
    int       i, j;
    t_complex global_response;
    double    err_response, error;

    error = 0.0;
    for(i = 0; i < N; i++)
    {
        global_response.re = 0.0;
        global_response.im = 0.0;
        for(j = 0; j < nb_bands; j++)
        {
            global_response.re += p_gains[j] * p_mdrc_gains->band_responses[j][i].re;
            global_response.im += p_gains[j] * p_mdrc_gains->band_responses[j][i].im;
        }
        err_response = sqrt(global_response.re * global_response.re + global_response.im * global_response.im) - 1.0;
        error       += err_response * err_response * p_weigthing[i];
    }
    error = sqrt(error);

    return error;
}


static void solve_gains(int *p_counter, double dx, int n, int nb_bands, double *p_best_gains, double *p_best_error, t_mdrc_gains *p_mdrc_gains, double *p_weigthing, double error_max, int counter_max)
{
    double gains[NB_BANDS_MAX], x1[NB_BANDS_MAX], x2[NB_BANDS_MAX], x3[NB_BANDS_MAX], x4[NB_BANDS_MAX], x5[NB_BANDS_MAX], error;
    int    i1, i2, i3, i4, i5;

    memcpy(gains, p_best_gains, nb_bands * sizeof(double));
    for(i1 = -n; i1 <= n; i1++)
    {
        memcpy(x1, gains, nb_bands * sizeof(double));
        x1[0] = gains[0] * pow(1.0 + dx, i1);
        if(nb_bands >= 2)
        {
            memcpy(x2, x1, nb_bands * sizeof(double));
            for(i2 = -n; i2 <= n; i2++)
            {
                x2[1] = x1[1] * pow(1.0 + dx, i2);
                if(nb_bands >= 3)
                {
                    memcpy(x3, x2, nb_bands * sizeof(double));
                    for(i3 = -n; i3 <= n; i3++)
                    {
                        x3[2] = x2[2] * pow(1.0 + dx, i3);
                        if(nb_bands >= 4)
                        {
                            memcpy(x4, x3, nb_bands * sizeof(double));
                            for(i4 = -n; i4 <= n; i4++)
                            {
                                x4[3] = x3[3] * pow(1.0 + dx, i4);
                                if(nb_bands >= 5)
                                {
                                    memcpy(x5, x4, nb_bands * sizeof(double));
                                    for(i5 = -n; i5 <= n; i5++)
                                    {
                                        x5[4] = x4[4] * pow(1.0 + dx, i5);
                                        error = error_response(p_mdrc_gains, x5, nb_bands, p_weigthing);
                                        (*p_counter)++;
                                        if(error < *p_best_error)
                                        {
                                            memcpy(p_best_gains, x5, nb_bands * sizeof(double));
                                            *p_best_error = error;
                                            display_result(*p_counter, error, dx);
                                        }
                                    }
                                    memcpy(gains, p_best_gains, nb_bands * sizeof(double));
                                    error = *p_best_error;
                                    if((error < error_max) || (*p_counter > counter_max))
                                    {
                                        return;
                                    }
                                }
                                else
                                {
                                    error = error_response(p_mdrc_gains, x4, nb_bands, p_weigthing);
                                    (*p_counter)++;
                                    if(error < *p_best_error)
                                    {
                                        memcpy(p_best_gains, x4, nb_bands * sizeof(double));
                                        *p_best_error = error;
                                        display_result(*p_counter, error, dx);
                                    }
                                }
                            }
                            memcpy(gains, p_best_gains, nb_bands * sizeof(double));
                            error = *p_best_error;
                            if((error < error_max) || (*p_counter > counter_max))
                            {
                                return;
                            }
                        }
                        else
                        {
                            error = error_response(p_mdrc_gains, x3, nb_bands, p_weigthing);
                            (*p_counter)++;
                            if(error < *p_best_error)
                            {
                                memcpy(p_best_gains, x3, nb_bands * sizeof(double));
                                *p_best_error = error;
                                display_result(*p_counter, error, dx);
                            }
                        }
                    }
                    memcpy(gains, p_best_gains, nb_bands * sizeof(double));
                    error = *p_best_error;
                    if((error < error_max) || (*p_counter > counter_max))
                    {
                        return;
                    }
                }
                else
                {
                    error = error_response(p_mdrc_gains, x2, nb_bands, p_weigthing);
                    (*p_counter)++;
                    if(error < *p_best_error)
                    {
                        memcpy(p_best_gains, x2, nb_bands * sizeof(double));
                        *p_best_error = error;
                        display_result(*p_counter, error, dx);
                    }
                }
            }
            memcpy(gains, p_best_gains, nb_bands * sizeof(double));
            error = *p_best_error;
            if((error < error_max) || (*p_counter > counter_max))
            {
                return;
            }
        }
        else
        {
            error = error_response(p_mdrc_gains, x1, nb_bands, p_weigthing);
            (*p_counter)++;
            if(error < *p_best_error)
            {
                memcpy(p_best_gains, x1, nb_bands * sizeof(double));
                *p_best_error = error;
                display_result(*p_counter, error, dx);
            }
        }
    }
}


static void display_result(int counter, double error, double dx)
{
    printf("counter = %d, best error = %lf, dx = %lf\n", counter, error, dx);
}
