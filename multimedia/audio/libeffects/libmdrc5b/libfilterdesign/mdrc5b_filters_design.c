/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: none.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

//#define DEBUG_MDRC_FILTER_DESIGN

#ifdef DEBUG_MDRC_FILTER_DESIGN
#include <stdio.h>
#endif // DEBUG_MDRC_FILTER_DESIGN

#include "mdrc5b_filters_design.h"


static int butterworth_LPHP_filter_design(double    fc,
                                          double    fs,
                                          int       biquad_order,
                                          t_biquad *p_biquads_LP,
                                          t_biquad *p_biquads_HP);

static int FIR_filter_design             (
#ifdef SEARCH_MAX_GAIN_FIR
                                          double f_start,
                                          double f_end,
                                          double *pMaxGain,
                                          double *pMaxFreq,
#endif // SEARCH_MAX_GAIN_FIR
                                          double *p_gains,
                                          int    FIR_order,
                                          int    N,
                                          double *b,
                                          double *cos_table);

static int optimize_gains_FIR            (double *p_CutFreq,
                                          double fs,
                                          int    nb_bands,
                                          double **p_p_FIR,
                                          int    FIR_size,
                                          double *p_gains_in,
                                          double *p_gains_out);

static int solve_linear_system           (double **p_p_a,
                                          double *p_c,
                                          int    n,
                                          double *p_b);


#define PI          3.1415926535897932384626433832795
#define SQRT_2      1.4142135623730950488016887242097
#define SQRT_0_5    0.70710678118654752440084436210485
#define SQRT_SQRT_2 1.1892071150027210667174999705605
#define LN_2        0.69314718055994530941723212145818
#define P9000MB     31622.776601683793319988935444327
#define M9000MB     0.000031622776601683793319988935444327

#ifdef USE_LIB_MATH
#define cotg(x) (cos(x) / sin(x))
#else // USE_LIB_MATH
#define cos(x)  cosine(x)
#define sqrt(x) square_root(x)
static double cosine     (double x);
static double cotg       (double x);
static double square_root(double x);
#endif // USE_LIB_MATH



int mdrc_bands_butterworth(double   *pCutFreq,
                           int      nbCutFreq,
                           double   fs,
                           int      biquad_order,
                           t_biquad **p_p_biquads_LP,
                           t_biquad **p_p_biquads_HP)
{
    int i;

    for(i = 0; i < nbCutFreq; i++)
    {
        if((pCutFreq[i] <= 0.0) || (pCutFreq[i] >= fs / 2.0))
        {
            return -1;
        }
        if((i > 0) && (pCutFreq[i - 1] >= pCutFreq[i]))
        {
            return -1;
        }

        if(butterworth_LPHP_filter_design(pCutFreq[i], fs, biquad_order, *(p_p_biquads_LP++), *(p_p_biquads_HP++)) != 0)
        {
            return -1;
        }
    }

    return 0;
}


static int butterworth_LPHP_filter_design(double   fc,
                                          double   fs,
                                          int      biquad_order,
                                          t_biquad *p_biquads_LP,
                                          t_biquad *p_biquads_HP)
{
    double c, c2, ck;
    double a0, a1, a2;
    int    k;

    if((fc / fs < 0.0001) || (fc / fs > 0.4999))
    {
        return -1;
    }

    c  = cotg(PI * fc / fs);
    c2 = c * c;
    for(k = 1; k <= biquad_order; k++)
    {
        ck               = -2.0 * cos(((double) (2 * (k + biquad_order) - 1)) / ((double) (4 * biquad_order)) * PI);

        a0               = c2 + ck * c + 1.0;
        a1               = 2.0 * (1.0 - c2);
        a2               = c2 - ck * c + 1.0;

        p_biquads_LP->b0 = 1.0 / a0;
        p_biquads_LP->b1 = 2.0 * p_biquads_LP->b0;
        p_biquads_LP->b2 = p_biquads_LP->b0;
        p_biquads_LP->a1 = a1 / a0;
        p_biquads_LP->a2 = a2 / a0;

        p_biquads_HP->b0 = p_biquads_LP->b0 * c2;
        p_biquads_HP->b1 = -2.0 * p_biquads_HP->b0;
        p_biquads_HP->b2 = p_biquads_HP->b0;
        p_biquads_HP->a1 = p_biquads_LP->a1;
        p_biquads_HP->a2 = p_biquads_LP->a2;

        p_biquads_LP++;
        p_biquads_HP++;
    }

    return 0;
}


int mdrc_bands_FIR(int    free_cos_table,
#ifdef SEARCH_MAX_GAIN_FIR
                   double *pMaxGain,
                   double *pMaxFreq,
#endif // SEARCH_MAX_GAIN_FIR
                   double *pCutFreq,
                   int    nbCutFreq,
                   double fs,
                   int    FIR_order,
                   double **p_p_FIR)
{
    static const int    N          = 1024;  // size of iDFT (number of points of filters response curve)
    static const double slope      = 10.0;  // slope of FIRs
    static double       *cos_table = NULL;
#ifdef SEARCH_MAX_GAIN_FIR
    double              f_start    = 0.0, f_end;
#endif // SEARCH_MAX_GAIN_FIR
    double              *p_gains, *p_gains_HP_prev, gains_LP_cur;
    double              df1, df2, f, tmp;
    int                 i, j;

#ifdef DEBUG_MDRC_FILTER_DESIGN
    printf("*** mdrc_bands_FIR ***\n");
    printf("free_cos_table = %d\n", free_cos_table);
    printf("cos_table = 0x%08X\n", (int) cos_table);
#endif // DEBUG_MDRC_FILTER_DESIGN

    if(free_cos_table)
    {
        if(cos_table != NULL)
        {
            free(cos_table);
            cos_table = NULL;
        }
        return 0;
    }

#ifdef DEBUG_MDRC_FILTER_DESIGN
    printf("fs = %1.0lf\n", fs);
    for(i = 0; i < nbCutFreq; i++)
    {
        printf("pCutFreq[%d] = %1.0lf\n", i , pCutFreq[i]);
    }
    for(i = 0; i <= nbCutFreq; i++)
    {
        printf("p_p_FIR[%d] = 0x%08X\n", i , (int) p_p_FIR[i]);
    }
    printf("FIR_order = %d\n", FIR_order);
#endif // DEBUG_MDRC_FILTER_DESIGN

    if(cos_table == NULL)
    {
        cos_table = (double *) malloc(2 * N * sizeof(double));
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("cos_table = 0x%08X\n", (int) cos_table);
#endif // DEBUG_MDRC_FILTER_DESIGN
        if(cos_table == NULL)
        {
#ifdef DEBUG_MDRC_FILTER_DESIGN
            printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
            return -1;
        }
        for(i = 0; i < 2 * N; i++)
        {
            cos_table[i] = cos(PI * ((double) i) / ((double) N));
        }
    }

    p_gains = (double *) malloc(N * sizeof(double));
#ifdef DEBUG_MDRC_FILTER_DESIGN
    printf("p_gains = 0x%08X\n", (int) p_gains);
#endif // DEBUG_MDRC_FILTER_DESIGN
    if(p_gains == NULL)
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        free(cos_table);
        cos_table = NULL;
        return -1;
    }

    p_gains_HP_prev = (double *) malloc(N * sizeof(double));
#ifdef DEBUG_MDRC_FILTER_DESIGN
    printf("p_gains_HP_prev = 0x%08X\n", (int) p_gains_HP_prev);
#endif // DEBUG_MDRC_FILTER_DESIGN
    if(p_gains_HP_prev == NULL)
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        free(cos_table);
        cos_table = NULL;
        free(p_gains);
        return -1;
    }

    for(i = 0; i < N; i++)
    {
        p_gains_HP_prev[i] = 1.0;
    }

    // band separation filters response curve construction
    df1 = 0.5 * fs / ((double) N);
    for(i = 0; i < nbCutFreq; i++)
    {
        if(   ((pCutFreq[i] <= 0.0) && (pCutFreq[i] >= fs / 2.0))
           || ((i > 0) && (pCutFreq[i - 1] >= pCutFreq[i])))
        {
#ifdef DEBUG_MDRC_FILTER_DESIGN
            printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
            free(cos_table);
            cos_table = NULL;
            free(p_gains);
            free(p_gains_HP_prev);
            return -1;
        }

        df2 = df1 / pCutFreq[i];
        f   = 0.0;
        for(j = 0; j < N; j++)
        {
            tmp                = slope * (f - 1.0);
            f                  = f + df2;
            gains_LP_cur       = max(0.0, min(1.0, 0.5 - tmp));
            p_gains[j]         = gains_LP_cur * p_gains_HP_prev[j];
            p_gains_HP_prev[j] = max(0.0, min(1.0, 0.5 + tmp));
        }

#ifdef SEARCH_MAX_GAIN_FIR
        f_end = pCutFreq[i] / fs;
        if(FIR_filter_design(f_start,
                             f_end,
                             pMaxGain,
                             pMaxFreq,
                             p_gains,
                             FIR_order,
                             N,
                             *(p_p_FIR++),
                             cos_table) != 0)
        {
#ifdef DEBUG_MDRC_FILTER_DESIGN
            printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
            free(cos_table);
            cos_table = NULL;
            free(p_gains);
            free(p_gains_HP_prev);
            return -1;
        }
        pMaxGain++;
        *pMaxFreq++ *= fs;
        f_start      = f_end;
#else // SEARCH_MAX_GAIN_FIR
        if(FIR_filter_design(p_gains,
                             FIR_order,
                             N,
                             *(p_p_FIR++),
                             cos_table) != 0)
        {
#ifdef DEBUG_MDRC_FILTER_DESIGN
            printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
            free(cos_table);
            cos_table = NULL;
            free(p_gains);
            free(p_gains_HP_prev);
            return -1;
        }
#endif // SEARCH_MAX_GAIN_FIR

#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("FIR %d : %1.0lf to %1.0lf Hz", i, i == 0 ? 0.0 : pCutFreq[i - 1], pCutFreq[i]);
        for(j = 0; j < FIR_order; j++)
        {
            if((j % 8) == 0)
            {
                printf("\n");
            }
            printf("%+1.8lf ", *(*(p_p_FIR - 1) + j));
        }
        printf("\n");
#endif // DEBUG_MDRC_FILTER_DESIGN
    }

#ifdef SEARCH_MAX_GAIN_FIR
    if(FIR_filter_design(f_start,
                         0.5,
                         pMaxGain,
                         pMaxFreq,
                         p_gains_HP_prev,
                         FIR_order,
                         N,
                         *p_p_FIR,
                         cos_table) != 0)
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        free(cos_table);
        cos_table = NULL;
        free(p_gains);
        free(p_gains_HP_prev);
        return -1;
    }
    *pMaxFreq *= fs;
#else // SEARCH_MAX_GAIN_FIR
    if(FIR_filter_design(p_gains_HP_prev,
                         FIR_order,
                         N,
                         *p_p_FIR,
                         cos_table) != 0)
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        free(cos_table);
        cos_table = NULL;
        free(p_gains);
        free(p_gains_HP_prev);
        return -1;
    }
#endif // SEARCH_MAX_GAIN_FIR

#ifdef DEBUG_MDRC_FILTER_DESIGN
    printf("FIR %d : %1.0lf to %1.0lf Hz", i, pCutFreq[nbCutFreq - 1], fs / 2.0);
    for(j = 0; j < FIR_order; j++)
    {
        if((j % 8) == 0)
        {
            printf("\n");
        }
        printf("%+1.8lf ", *(*p_p_FIR + j));
    }
    printf("\n");

    printf("Sum of %d FIR :", nbCutFreq + 1);
    p_p_FIR -= nbCutFreq;
    for(j = 0; j < FIR_order; j++)
    {
        double coef = 0.0;

        for(i = 0; i <= nbCutFreq; i++)
        {
            coef += p_p_FIR[i][j];
        }
        if((j % 8) == 0)
        {
            printf("\n");
        }
        printf("%+1.8lf ", coef);
    }
    printf("\n");
#endif // DEBUG_MDRC_FILTER_DESIGN

    free(p_gains);
    free(p_gains_HP_prev);

    return 0;
}


static int FIR_filter_design(
#ifdef SEARCH_MAX_GAIN_FIR
                             double f_start,
                             double f_end,
                             double *pMaxGain,
                             double *pMaxFreq,
#endif // SEARCH_MAX_GAIN_FIR
                             double *p_gains,
                             int    FIR_order,
                             int    N,
                             double *b,
                             double *cos_table)
{
#ifdef SEARCH_MAX_GAIN_FIR
    double gain_re, gain_im, gain2, max_gain2;
    int    k1, k2, i_max, i_start, i_end;
#endif // SEARCH_MAX_GAIN_FIR
    double sign, b_tmp;
    int    p, i, j, k;

    // if FIR_order is even, decrease it by 1 (to have an odd order)
    // and set the last coefficient to 0 to obtain the required order
    p = FIR_order / 2;
    if(FIR_order == 2 * p)
    {
        b[FIR_order - 1] = 0.0;
        FIR_order--;
    }

    if(N <= p)      // N = size of iDFT (must be > p)
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        return -1;
    }

    // Hamming weighted iDFT
    sign = 1.0;
    for(i = 0; i <= p; i++)
    {
        b_tmp = (p_gains[0] + sign * p_gains[N - 1]) / 2.0;
        sign  = -sign;
        k     = 0;
        for(j = 1; j < N; j++)
        {
            k += i;
            if(k >= 2 * N)
            {
                k -= 2 * N;
            }
            b_tmp += cos_table[k] * p_gains[j];
        }
        b[p + i] = b_tmp / ((double) N) * (0.54 + 0.46 * cos(PI * ((double) i) / ((double) p)));
        b[p - i] = b[p + i];                        // symetrize filter
    }

#ifdef SEARCH_MAX_GAIN_FIR
    // search max gain between f_start and f_end (pass-band zone)
    max_gain2 = -1.0;
    i_max     = 0;
    i_start   = (int) floor(f_start * ((double) (2 * N)));
    i_end     = (int) floor(f_end   * ((double) (2 * N)));
    for(i = i_start; i <= i_end; i++)
    {
        gain_re = 0.0;
        gain_im = 0.0;
        k1      = 0;
        k2      = N / 2;
        for(j = 0; j < FIR_order; j++)
        {
            gain_re = gain_re + b[j] * cos_table[k1 + 1];
            gain_im = gain_im + b[j] * cos_table[k2 + 1];
            k1 += i;
            k2 -= i;
            if(k1 >= 2 * N)
            {
                k1 -= 2 * N;
            }
            if(k2 < 0)
            {
                k2 += 2 * N;
            }
        }
        gain2 = gain_re * gain_re + gain_im * gain_im;
        if(gain2 > max_gain2)
        {
            max_gain2 = gain2;
            i_max     = i;
        }
    }
    *pMaxGain = sqrt(max_gain2);
    *pMaxFreq = ((double) i_max) / ((double) (2 * N));
#endif // SEARCH_MAX_GAIN_FIR

    return 0;
}


#ifndef USE_LIB_MATH

static double cosine(double x)
{
    int    n;
    double sign, x2, cosinus;

    // cos period = 2.PI
    x = x - 2.0 * PI * floor(x / 2.0 / PI);
    // x is now between 0 and 2.PI
    // if x >= PI, 2.PI-x is between 0 and PI and cos(2.PI-x) = cos(x)
    if(x >= PI)
    {
        x    = 2.0 * PI - x;
    }
    // x is now between 0 and PI
    // if x >= PI/2, PI-x is between 0 and PI/2 and cos(PI-x) = -cos(x)
    if(x >= PI / 2.0)
    {
        x    = PI - x;
        sign = -1.0;
    }
    else
    {
        sign = 1.0;
    }
    // x is now between 0 and PI/2
    // divide x by 2 n times while x > 0.01
    n = 0;
    while(x > 0.01)
    {
        x = x * 0.5;
        n++;
    }
    x2      = x * x;
    cosinus = 1.0 - x2 / 2.0 * (1.0 - x2 / 12.0 * (1.0 - x2 / 30.0 * (1.0 - x2 / 56.0)));
    while(n > 0)
    {
        cosinus = 2.0 * cosinus * cosinus - 1.0;
        n--;
    }

    return sign * cosinus;
}


static double cotg(double x)
{
    int    n;
    double sign, x2, sinus, cosinus;

    // cotg period = PI
    x = x - PI * floor(x / PI);
    // x is now between 0 and PI
    // if x >= PI/2, PI-x is between 0 and PI/2 and cotg(PI-x) = -cotg(x)
    if(x >= PI / 2.0)
    {
        x    = PI - x;
        sign = -1.0;
    }
    else
    {
        sign = 1.0;
    }
    n = 0;
    // x is now between 0 and PI/2
    // divide x by 2 n times while x > 0.01
    while(x > 0.01)
    {
        x = x * 0.5;
        n++;
    }
    x2      = x * x;
    sinus   = x * (1.0 - x2 / 6.0 * (1.0 - x2 / 20.0 * (1.0 - x2 / 42.0 * (1.0 - x2 / 72.0))));
    cosinus =      1.0 - x2 / 2.0 * (1.0 - x2 / 12.0 * (1.0 - x2 / 30.0 * (1.0 - x2 / 56.0)));
    while(n > 0)
    {
        sinus   = 2.0 * cosinus * sinus;
        cosinus = 2.0 * cosinus * cosinus - 1.0;
        n--;
    }

    return sign * cosinus / sinus;
}


static double square_root(double x)
{
    double y, y1, yk, ck;
    int    n, flag, k;

    if(x < 0.0)
    {
        return -1.0;
    }
    if(x == 0.0)
    {
        return 0.0;
    }

    // sqrt(x) = x^0.5 = (1 + y)^0.5 * 2^(n/2) with x = (1 + y) * 2^n
    y = x;
    n = 0;
    while(y > 1.0)
    {
        y *= 0.5;
        n++;
    }
    while(y < 0.5)
    {
        y *= 2.0;
        n--;
    }

    // y is now between 0.5 and 1.0
    if(y > SQRT_0_5)
    {
        // y is now between sqrt(0.5) and 1.0
        y   *= SQRT_0_5;  // multiply by sqrt(0.5) => y is now between 0.5 and sqrt(0.5)
        flag = 1;
    }
    else
    {
        // y is now between 0.5 and sqrt(0.5)
        flag = 0;
    }
    y1 = 2.0 * y - 1.0;  // y1 is now between 0 and 2.0*(sqrt(0.5)-0.5) (=sqrt(2.0)-1.0)
    n--;

    // sqrt(1 + y1) = 1 +       1/2^1 .y1^1
    //                  -       1/2^3 .y1^2
    //                  +       1/2^4 .y1^3
    //                  -       5/2^7 .y1^4
    //                  +       7/2^8 .y1^5
    //                  -      21/2^10.y1^6
    //                  +      33/2^11.y1^7
    //                  -     429/2^15.y1^8
    //                  +     715/2^16.y1^9
    //                  -    2431/2^18.y1^10
    //                  +    4199/2^19.y1^11
    //                  -   29393/2^22.y1^12
    //                  +   52003/2^23.y1^13
    //                  -  185725/2^25.y1^14
    //                  +  334305/2^26.y1^15
    //                  - 9694845/2^31.y1^16
    //                  + ..................
    //                  - ..................
    //                  + ..................
    //                  + [(-1)^(k+1).(2.k-3)!]/[k!.(k-2)!.2^(2.k-2)].y1^k  for k > 2
    yk = y1 * y1;
    ck = -0.125;
    y  = 1.0 + 0.5 * y1 + ck * yk;
    for(k = 3; k < 20; k++)
    {
        // c(k) = [(-1)^(k+1).(2.k-3)!]/[k!.(k-2)!.2^(2.k-2)].y1^k = -(2.k-3).(2.k-4)/k/(k-2)/2^2.c(k-1) = -(2.k-3)/k/2.c(k-1)
        ck *= ((double) (3 - 2 * k)) / ((double) (2 * k));
        yk *= y1;
        y  += ck * yk;
    }

    if(flag)
    {
        y *= SQRT_SQRT_2;
    }
    if(n & 1)
    {
        y *= SQRT_2;
        n--;
    }
    n >>= 1;
    while(n > 0)
    {
        y *= 2.0;
        n--;
    }
    while(n < 0)
    {
        y *= 0.5;
        n++;
    }

    return y;
}

#endif // !USE_LIB_MATH



int convert_biquad_int32(t_biquad          *p_biquads,
                         t_biquad_int32    *p_biquads_int32,
                         int               biquad_order,
                         t_biquad_sub_kind biquad_sub_kind)
{
    int    i;
    double b0, b1, b2, a1, a2, min_coef, max_coef, gain, exponent;

    for(i = 0; i < biquad_order; i++)
    {
        b0       = p_biquads->b0;
        b1       = p_biquads->b1 / 2.0;
        b2       = p_biquads->b2;
        a1       = p_biquads->a1 / 2.0;
        a2       = p_biquads->a2;

        min_coef = 0.0;
        min_coef = min(min_coef, b0);
        min_coef = min(min_coef, b1);
        min_coef = min(min_coef, b2);
        max_coef = 0.0;
        max_coef = max(max_coef, b0);
        max_coef = max(max_coef, b1);
        max_coef = max(max_coef, b2);
        gain     = 0.0;
        if(min_coef < 0.0)
        {
            gain = -2147483648.0 / min_coef;
        }
        if(max_coef > 0.0)
        {
            if(gain == 0.0)
            {
                gain = 2147483647.0 / max_coef;
            }
            else
            {
                gain = min(gain, 2147483647.0 / max_coef);
            }
        }
        if(gain > 0.0)
        {
            exponent = floor(log(gain) / LN_2);
            gain     = pow(2.0, exponent);
        }
        else
        {
            exponent = 0.0;
            gain     = 1.0;
        }

        if((a1 < -1.0) || (a1 > 1.0))
        {
            return -1;
        }
        if((a2 < -1.0) || (a2 > 1.0))
        {
            return -1;
        }

        b0 = floor(b0 * gain         + 0.5);
        b1 = floor(b1 * gain         + 0.5);
        b2 = floor(b2 * gain         + 0.5);
        a1 = floor(a1 * 2147483648.0 + 0.5);
        a2 = floor(a2 * 2147483648.0 + 0.5);
        a1 = min(2147483647.0, a1);
        a2 = min(2147483647.0, a2);

        p_biquads_int32->kind     = biquad_sub_kind;
        p_biquads_int32->gain_exp = 31 - ((int) exponent);
        p_biquads_int32->b0       = (int) b0;
        p_biquads_int32->b1       = (int) b1;
        p_biquads_int32->b2       = (int) b2;
        p_biquads_int32->a1       = (int) a1;
        p_biquads_int32->a2       = (int) a2;

        switch(biquad_sub_kind)
        {
            case BUTTERWORTH_LOW_PASS_BIQUAD:
                if(p_biquads_int32->b0 != p_biquads_int32->b2)
                {
                    return -1;
                }
                if(p_biquads_int32->b0 != p_biquads_int32->b1)
                {
                    return -1;
                }
                break;

            case BUTTERWORTH_HIGH_PASS_BIQUAD:
                if(p_biquads_int32->b0 !=  p_biquads_int32->b2)
                {
                    return -1;
                }
                if(p_biquads_int32->b0 != -p_biquads_int32->b1)
                {
                    return -1;
                }
                break;

            default:
                break;
        }

        p_biquads++;
        p_biquads_int32++;
    }

    return 0;
}


int convert_FIR_int32(double      *p_FIR,
                      t_FIR_int32 *p_FIR_int32,
                      int         FIR_order,
                      t_FIR_kind  FIR_kind)
{
    int    i, p;
    double min_coef, max_coef, gain, exponent;

#ifdef DEBUG_MDRC_FILTER_DESIGN
    printf("*** convert_biquad_int32 ***\n");
    printf("FIR_kind  = %d\n", FIR_kind);
    printf("FIR_order = %d\n", FIR_order);
#endif // DEBUG_MDRC_FILTER_DESIGN
    if(FIR_kind == SYMMETRIC_FIR)
    {
        p = FIR_order / 2;
        if(FIR_order == 2 * p)
        {
            if(p_FIR[FIR_order - 1] != 0.0)
            {
#ifdef DEBUG_MDRC_FILTER_DESIGN
                printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
                return -1;
            }
        }
        for(i = 0; i < p; i++)
        {
            if(p_FIR[i] != p_FIR[2 * p - i])
            {
#ifdef DEBUG_MDRC_FILTER_DESIGN
                printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
                return -1;
            }
        }
        FIR_order = p + 1;
        p_FIR    += p;
    }
    if(FIR_order > MDRC_FIR_ORDER_MAX)
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        return -1;
    }

    min_coef = max_coef = 0.0;
    for(i = 0; i < FIR_order; i++)
    {
        min_coef = min(min_coef, p_FIR[i]);
        max_coef = max(max_coef, p_FIR[i]);
    }
    gain = 0.0;
    if(min_coef < 0.0)
    {
        gain = -2147483648.0 / min_coef;
    }
    if(max_coef > 0.0)
    {
        if(gain == 0.0)
        {
            gain = 2147483647.0 / max_coef;
        }
        else
        {
            gain = min(gain, 2147483647.0 / max_coef);
        }
    }
    if(gain > 0.0)
    {
        exponent = floor(log(gain) / LN_2);
        gain     = exp(LN_2 * exponent);
    }
    else
    {
        exponent = 0.0;
        gain     = 1.0;
    }

    for(i = 0; i < FIR_order; i++)
    {
        p_FIR_int32->coef[i] = (int) floor(p_FIR[i] * gain + 0.5);
    }

    p_FIR_int32->gain_exp  = 31 - ((int) exponent);
    p_FIR_int32->kind      = FIR_kind;
    p_FIR_int32->order     = FIR_order;

#ifdef DEBUG_MDRC_FILTER_DESIGN
    for(i = 0; i < FIR_order; i++)
    {
        if((i % 8) == 0)
        {
            printf("\n");
        }
        printf("%+11d ", p_FIR_int32->coef[i]);
    }
    printf("\n");
    printf("gain_exp = %d\n", p_FIR_int32->gain_exp);
#endif // DEBUG_MDRC_FILTER_DESIGN

    return 0;
}



int convert_biquad_hst2mpc(t_biquad_int32 *p_biquad_hst, t_biquad_cell_int16 *p_biquad_mpc)
{
    p_biquad_mpc->b_exp   = (t_uint16)  p_biquad_hst->gain_exp;
    p_biquad_mpc->b0_low  = (t_uint16)  (p_biquad_hst->b0        & 0xFFFF);
    p_biquad_mpc->b0_high = (t_uint16) ((p_biquad_hst->b0 >> 16) & 0xFFFF);
    p_biquad_mpc->b1_low  = (t_uint16)  (p_biquad_hst->b1        & 0xFFFF);
    p_biquad_mpc->b1_high = (t_uint16) ((p_biquad_hst->b1 >> 16) & 0xFFFF);
    p_biquad_mpc->b2_low  = (t_uint16)  (p_biquad_hst->b2        & 0xFFFF);
    p_biquad_mpc->b2_high = (t_uint16) ((p_biquad_hst->b2 >> 16) & 0xFFFF);
    p_biquad_mpc->a1_low  = (t_uint16)  (p_biquad_hst->a1        & 0xFFFF);
    p_biquad_mpc->a1_high = (t_uint16) ((p_biquad_hst->a1 >> 16) & 0xFFFF);
    p_biquad_mpc->a2_low  = (t_uint16)  (p_biquad_hst->a2        & 0xFFFF);
    p_biquad_mpc->a2_high = (t_uint16) ((p_biquad_hst->a2 >> 16) & 0xFFFF);

    return 0;
}


int convert_FIR_hst2mpc(t_FIR_int32 *p_FIR_hst, t_FIR_int16 *p_FIR_mpc)
{
    int i;

#ifdef DEBUG_MDRC_FILTER_DESIGN
    printf("*** convert_FIR_hst2mpc ***\n");
#endif // DEBUG_MDRC_FILTER_DESIGN
    p_FIR_mpc->order    = (t_uint16) p_FIR_hst->order;
    p_FIR_mpc->gain_exp = (t_uint16) p_FIR_hst->gain_exp;
    if(p_FIR_mpc->order > sizeof(p_FIR_mpc->coef) / sizeof(p_FIR_mpc->coef[0]))
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        return -1;
    }
    for(i = 0; i < p_FIR_hst->order; i++)
    {
        p_FIR_mpc->coef[i].low  = (t_uint16)  (p_FIR_hst->coef[i]        & 0xFFFF);
        p_FIR_mpc->coef[i].high = (t_uint16) ((p_FIR_hst->coef[i] >> 16) & 0xFFFF);
    }

    return 0;
}



int optimize_gains_FIR_mB(double *p_CutFreq,
                          double fs,
                          int    nb_bands,
                          double **p_p_FIR,
                          int    FIR_size,
                          int    *p_gains_in_mB,
                          int    *p_gains_out_mB)
{
    double *p_gains_in, *p_gains_out;
    int    i, ret;


#ifdef DEBUG_MDRC_FILTER_DESIGN
    printf("*** optimize_gains_FIR_mB ***\n");
    for(i = 0; i < nb_bands; i++)
    {
        printf("p_gains_in_mB[%d] = %d\n", i, p_gains_in_mB[i]);
    }
#endif // DEBUG_MDRC_FILTER_DESIGN

    p_gains_in = (double *) malloc(nb_bands * sizeof(double));
    if(p_gains_in == NULL)
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        return -1;
    }
    p_gains_out = (double *) malloc(nb_bands * sizeof(double));
    if(p_gains_out == NULL)
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        free(p_gains_in);
        return -1;
    }

    for(i = 0; i < nb_bands; i++)
    {
        p_gains_in[i] = pow(10.0, ((double) min(9000, max(-9000, p_gains_in_mB[i]))) / 2000.0); // limited to -9000 ... +9000 mB range
    }

#ifdef DEBUG_MDRC_FILTER_DESIGN
    for(i = 0; i < nb_bands; i++)
    {
        printf("p_gains_in[%d] = %1.8lf\n", i, p_gains_in[i]);
    }
#endif // DEBUG_MDRC_FILTER_DESIGN

    ret = optimize_gains_FIR(p_CutFreq,
                             fs,
                             nb_bands,
                             p_p_FIR,
                             FIR_size,
                             p_gains_in,
                             p_gains_out);

#ifdef DEBUG_MDRC_FILTER_DESIGN
    for(i = 0; i < nb_bands; i++)
    {
        printf("p_gains_out[%d] = %1.8lf\n", i, p_gains_out[i]);
    }
#endif // DEBUG_MDRC_FILTER_DESIGN

    if(ret == 0)
    {
        for(i = 0; i < nb_bands; i++)
        {
            p_gains_out_mB[i] = (int) floor(2000.0 * log10(p_gains_out[i]) + 0.5);
        }
#ifdef DEBUG_MDRC_FILTER_DESIGN
        for(i = 0; i < nb_bands; i++)
        {
            printf("p_gains_out_mB[%d] = %d\n", i, p_gains_out_mB[i]);
        }
#endif // DEBUG_MDRC_FILTER_DESIGN
    }
#ifdef DEBUG_MDRC_FILTER_DESIGN
    else
    {
        printf("error in file %s line %d\n", __FILE__, __LINE__);
    }
#endif // DEBUG_MDRC_FILTER_DESIGN

    free(p_gains_in);
    free(p_gains_out);
    return ret;
}


static int optimize_gains_FIR(double *p_CutFreq,
                              double fs,
                              int    nb_bands,
                              double **p_p_FIR,
                              int    FIR_size,
                              double *p_gains_in,
                              double *p_gains_out)
{
    // since all sub-band FIRs have the same size and are linear phase, they all have the same phase;
    // so the module of a linear combination of their response is equal to the same linear combination of the module of their response
    //
    // so if we want to obtain a certain gain for a certain frequency for a linear combination of these filters, it will be identical
    // to the same linear combination of the gains of each filters for this frequency
    //
    // so if we want to obtain a certain gain for the center of each sub-band (let's call them center_freq(0) ... center_freq(n-1)),
    // we have to solve the following linear system :
    //
    // | response(0,0) ..... response(0,n-1)   |   | g_optimized(0)   |   | g_requested(0)   |
    // | ..................................... | x | ................ | = | ................ |
    // | response(n-1,0) ... response(n-1,n-1) |   | g_optimized(n-1) |   | g_requested(n-1) |
    //
    // with :
    // - g_requested the requested gains (i.e. p_gains_in),
    // - g_optimized the gains to search (i.e. p_gains_out),
    // - response(i,j) the gain (module of response) of FIR(j) for center_freq(i)

    int    i, j, k, ret;
    double **p_p_response;
    double theta, theta1, theta2, resp_re, resp_im, error1_dB, error2_dB;


#ifdef DEBUG_MDRC_FILTER_DESIGN
    printf("*** optimize_gains_FIR ***\n");
#endif // DEBUG_MDRC_FILTER_DESIGN
    // allocate response matrix
    p_p_response = (double **) malloc(nb_bands * sizeof(double *));
    if(p_p_response == NULL)
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        return -1;
    }
    for(i = 0; i < nb_bands; i++)
    {
        p_p_response[i] = (double *) malloc(nb_bands * sizeof(double));
        if(p_p_response[i] == NULL)
        {
#ifdef DEBUG_MDRC_FILTER_DESIGN
            printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
            for(j = 0; j < i; j++)
            {
                free(p_p_response[j]);
            }
            free(p_p_response);
            return -1;
        }
    }

    // fill response matrix
    for(i = 0; i < nb_bands; i++)
    {
        // normalized frequency for center_freq(i)
        if(i == 0)
        {
            theta = p_CutFreq[0] / fs * PI;
        }
        else if(i == nb_bands - 1)
        {
            theta = (p_CutFreq[nb_bands - 2] / fs + 0.5) * PI;
        }
        else
        {
            theta = (p_CutFreq[i - 1] + p_CutFreq[i]) / fs * PI;
        }

        for(j = 0; j < nb_bands; j++)
        {
            // compute filter response of FIR(j) for center_freq(i)
            // Re(response(i,j)) = Sum[FIR(j,k) * cos(k*theta)] for k=0...FIR_size-1
            // Im(response(i,j)) = Sum[FIR(j,k) * sin(k*theta)] for k=0...FIR_size-1
            resp_re = 0.0;
            resp_im = 0.0;
            theta1  = 0.0;          // theta1 used for cos(x)
            theta2  = PI / 2.0;     // theta2 used for sin(x) = cos(PI/2-x)
            for(k = 0; k < FIR_size; k++)
            {
                resp_re += p_p_FIR[j][k] * cos(theta1);
                resp_im -= p_p_FIR[j][k] * cos(theta2);
                theta1  += theta;
                theta2  -= theta;
                if(theta1 > PI)
                {
                    theta1 -= 2.0 * PI;
                }
                if(theta2 < -PI)
                {
                    theta2 += 2.0 * PI;
                }
            }
            p_p_response[i][j] = sqrt(resp_re * resp_re + resp_im * resp_im);   // module of response
        }
    }

    // solve linear system
    ret = solve_linear_system(p_p_response, p_gains_in, nb_bands, p_gains_out);

    // negative gains are rejected (because they will converted to mB in calling routine)
    for(i = 0; i < nb_bands; i++)
    {
        if(p_gains_out[i] < M9000MB)
        {
            p_gains_out[i] = M9000MB;
        }
        else if(p_gains_out[i] > P9000MB)
        {
            p_gains_out[i] = P9000MB;
        }
    }

    // since negative gains have been rejected, response may be non-perfect for center frequencies
    // thus we compare error with p_gains_in and p_gains_out and keep the best ones
    error1_dB = error2_dB = 0.0;
    for(i = 0; i < nb_bands; i++)
    {
        double gain1 = 0.0, gain2 = 0.0;

        for(j = 0; j < nb_bands; j++)
        {
            gain1 += p_p_response[i][j] * p_gains_in [j];
            gain2 += p_p_response[i][j] * p_gains_out[j];
        }
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("global response for band %d center frequency : with input gain = %1.8lf, with optimized gain = %1.8lf\n", i, gain1, gain2);
#endif // DEBUG_MDRC_FILTER_DESIGN
        error1_dB += fabs(20.0 * log10(gain1 / p_gains_in[i]));
        error2_dB += fabs(20.0 * log10(gain2 / p_gains_in[i]));
    }
#ifdef DEBUG_MDRC_FILTER_DESIGN
    printf("global error with input     gains = %1.2lf dB\n", error1_dB);
    printf("global error with optimized gains = %1.2lf dB\n", error2_dB);
#endif // DEBUG_MDRC_FILTER_DESIGN
    if(error1_dB < error2_dB)
    {
        for(i = 0; i < nb_bands; i++)
        {
            p_gains_out[i] = p_gains_in[i];
        }
    }

    // free response matrix
    for(i = 0; i < nb_bands; i++)
    {
        free(p_p_response[i]);
    }
    free(p_p_response);

    return ret;
}


static int solve_linear_system(double **p_p_a, double *p_c, int n, double *p_b)
{
    // solve linear system : a x b = c
    // with a : n x n matrix
    //      b : n x 1 matrix
    //      c : n x 1 matrix
    // using Gauss elimination method

    int    i, j, k, i_max;
    double abs_a, a_max, swap, coef;
    double **p_p_a_copy, *p_c_copy;

#ifdef DEBUG_MDRC_FILTER_DESIGN
    printf("*** solve_linear_system ***\n");
    for(i = 0; i < n; i++)
    {
        printf("| ");
        for(j = 0; j < n; j++)
        {
            printf("%+1.8lf ", p_p_a[i][j]);
        }
        if(i == n / 2)
        {
            printf("| x | b%d | = ", i);
        }
        else
        {
            printf("|   | b%d |   ", i);
        }
        printf("| %+1.8lf |\n", p_c[i]);
    }
#endif // DEBUG_MDRC_FILTER_DESIGN

    // copy a and c tables because they mustn't be modified for calling routine
    p_p_a_copy = (double **) malloc(n * sizeof(double *));
    if(p_p_a_copy == NULL)
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        return -1;
    }
    for(i = 0; i < n; i++)
    {
        p_p_a_copy[i] = (double *) malloc(n * sizeof(double));
        if(p_p_a_copy[i] == NULL)
        {
#ifdef DEBUG_MDRC_FILTER_DESIGN
            printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
            for(j = 0; j < i; j++)
            {
                free(p_p_a_copy[j]);
            }
            free(p_p_a_copy);
            return -1;
        }
    }
    p_c_copy = (double *) malloc(n * sizeof(double));
    if(p_c_copy == NULL)
    {
#ifdef DEBUG_MDRC_FILTER_DESIGN
        printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
        for(i = 0; i < n; i++)
        {
            free(p_p_a_copy[i]);
        }
        free(p_p_a_copy);
        free(p_c_copy);
        return -1;
    }
    for(i = 0; i < n; i++)
    {
        for(j = 0; j < n; j++)
        {
            p_p_a_copy[i][j] = p_p_a[i][j];
        }
        p_c_copy[i] = p_c[i];
    }
    p_p_a = p_p_a_copy;
    p_c   = p_c_copy;

    for(i = 0; i < n; i++)
    {
        // search line j with max(abs(a(j,i))) for j=i...n-1
        a_max = fabs(p_p_a[i][i]);
        i_max = i;
        for(j = i + 1; j < n; j++)
        {
            abs_a = fabs(p_p_a[j][i]);
            if(abs_a > a_max)
            {
                a_max = abs_a;
                i_max = j;
            }
        }
        if(a_max < 0.000001)
        {
#ifdef DEBUG_MDRC_FILTER_DESIGN
            printf("error in file %s line %d\n", __FILE__, __LINE__);
#endif // DEBUG_MDRC_FILTER_DESIGN
            return -1;
        }
        // swap lines i and i_max of matrixes a and c
        if(i_max > i)
        {
            for(j = i; j < n; j++)
            {
                swap            = p_p_a[i]    [j];
                p_p_a[i]    [j] = p_p_a[i_max][j];
                p_p_a[i_max][j] = swap;
            }
            swap       = p_c[i];
            p_c[i]     = p_c[i_max];
            p_c[i_max] = swap;
        }
        // remove coef*line(i) to line(j) of matrix a such as a(j,i)=0 for j=i+1...n; update c matrix in the same manner
        for(j = i + 1; j < n; j++)
        {
            coef = p_p_a[j][i] / p_p_a[i][i];
            for(k = i + 1; k < n; k++)
            {
                p_p_a[j][k] -= coef * p_p_a[i][k];
            }
            p_c[j] -= coef * p_c[i];
        }
    }

    // a matrix is now triangular : a(i,j)=0 for i>j
    // Remark : it's not true with current implementation but only because of optimization;
    //          a(i,j) for i>j is not computed because we know that it will be 0
    // Thus it's easy to compute the last b(n-1), to replace its value in previous lines, then to compute b(n-2), and so on...
    for(i = n - 1; i >= 0; i--)
    {
        p_b[i] = p_c[i] / p_p_a[i][i];
        for(j = 0; j < i; j++)
        {
            p_c[j] -= p_p_a[j][i] * p_b[i];
        }
    }

#ifdef DEBUG_MDRC_FILTER_DESIGN
    for(i = 0; i < n; i++)
    {
        printf("b%d = %1.8lf\n", i, p_b[i]);
    }
#endif // DEBUG_MDRC_FILTER_DESIGN

    // free a and c tables copies
    for(i = 0; i < n; i++)
    {
        free(p_p_a_copy[i]);
    }
    free(p_p_a_copy);
    free(p_c_copy);

    return 0;
}
