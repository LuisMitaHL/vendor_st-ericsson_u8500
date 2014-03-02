/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   basic_math.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/

//#include <assert.h>
#define assert(a)

#include "basic_math.h"

#ifdef USE_MY_MATH

#define SQRT_2      1.4142135623730950488016887242097
#define SQRT_0_5    0.70710678118654752440084436210485
#define SQRT_SQRT_2 1.1892071150027210667174999705605
#define LOG_10      2.3025850929940456840179914546844
#define EXP_1       2.7182818284590452353602874713527
#define EXP_2       7.389056098930650227230427460575
#define EXP_4       54.598150033144239078110261202861
#define EXP_8       2980.9579870417282747435920994529
#define EXP_16      8886110.5205078726367630237407815
#define EXP_32      78962960182680.695160978022635108


t_float my_floor(t_float x)
{
    int n = (int) x;

    return (t_float) n;
}


t_float my_ceil(t_float x)
{
    int n = (int) x;

    if(x > (t_float) n)
    {
        n++;
    }
    return (t_float) n;
}


t_float my_fabs(t_float x)
{
    if(x < 0.0)
    {
        return -x;
    }
    else
    {
        return x;
    }
}


t_float my_sqrt(t_float x)
{
    int     shift, x_greater_sqrt_0_5;
    t_float xn, y;

    assert(x >= 0.0);
    if(x == 0.0)
    {
        return 0.0;
    }
    shift = 0;
    while(x > 1.0)
    {
        x *= 0.5;
        shift--;
    }
    while(x < 0.5)
    {
        x *= 2.0;
        shift++;
    }
    // x is now between 0.5 and 1
    x_greater_sqrt_0_5 = (x > SQRT_0_5);
    if(x_greater_sqrt_0_5)
    {
        x *= SQRT_0_5;
    }
    // x is now between 0.5 and sqrt(0.5)
    x = 2.0 * x - 1.0;
    shift++;
    // x is now between 0 and 2.0*sqrt(0.5)-1.0 (=sqrt(2.0)-1.0)

    // sqrt(1 + x) = 1 +       1/2^1 .x^1
    //                 -       1/2^3 .x^2
    //                 +       1/2^4 .x^3
    //                 -       5/2^7 .x^4
    //                 +       7/2^8 .x^5
    //                 -      21/2^10.x^6
    //                 +      33/2^11.x^7
    //                 -     429/2^15.x^8
    //                 +     715/2^16.x^9
    //                 -    2431/2^18.x^10
    //                 +    4199/2^19.x^11
    //                 -   29393/2^22.x^12
    //                 +   52003/2^23.x^13
    //                 -  185725/2^25.x^14
    //                 +  334305/2^26.x^15
    //                 - 9694845/2^31.x^16
    //                 + .................
    y   = 1.0;                                      //                  1
    xn  = x;                                        //
    y  += xn * 0.5;                                 // +       1/2^1 .x^1
    xn *= x;                                        //
    y  -= xn * 0.125;                               // -       1/2^3 .x^2
    xn *= x;                                        //
    y  += xn * 0.0625;                              // +       1/2^4 .x^3
    xn *= x;                                        //
    y  -= xn * 0.0390625;                           // -       5/2^7 .x^4
    xn *= x;                                        //
    y  += xn * 0.02734375;                          // +       7/2^8 .x^5
    xn *= x;                                        //
    y  -= xn * 0.0205078125;                        // -      21/2^10.x^6
    xn *= x;                                        //
    y  += xn * 0.01611328125;                       // +      33/2^11.x^7
    xn *= x;                                        //
    y  -= xn * 0.013092041015625;                   // -     429/2^15.x^8
    xn *= x;                                        //
    y  += xn * 0.0109100341796875;                  // +     715/2^16.x^9
    xn *= x;                                        //
    y  -= xn * 0.009273529052734375;                // -    2431/2^18.x^10
    xn *= x;                                        //
    y  += xn * 0.0080089569091796875;               // +    4199/2^19.x^11
    xn *= x;                                        //
    y  -= xn * 0.0070078372955322265625;            // -   29393/2^22.x^12
    xn *= x;                                        //
    y  += xn * 0.00619924068450927734375;           // +   52003/2^23.x^13
    xn *= x;                                        //
    y  -= xn * 0.0055350363254547119140625;         // -  185725/2^25.x^14
    xn *= x;                                        //
    y  += xn * 0.00498153269290924072265625;        // +  334305/2^26.x^15
    xn *= x;                                        //
    y  -= xn * 0.0045145140029489994049072265625;   // - 9694845/2^31.x^16

    if(x_greater_sqrt_0_5)
    {
        y *= SQRT_SQRT_2;
    }
    if(shift & 1)
    {
        y *= SQRT_0_5;
    }
    shift >>= 1;
    while(shift > 0)
    {
        y *= 0.5;
        shift--;
    }
    while(shift < 0)
    {
        y *= 2.0;
        shift++;
    }

    return y;
}


t_float my_log(t_float x)
{
    int     shift;
    t_float xn, y;

    assert(x > 0.0);
    shift = 0;
    while(x > SQRT_2)
    {
        x *= 0.5;
        shift--;
    }
    while(x < SQRT_0_5)
    {
        x *= 2.0;
        shift++;
    }
    // x is now between sqrt(0.5) and sqrt(2)
    x -= 1.0;
    // log(1 + x) =       x^1
    //            -  1/2 .x^2
    //            +  1/3 .x^3
    //            -  1/4 .x^4
    //            +  1/5 .x^5
    //            -  1/6 .x^6
    //            +  1/7 .x^7
    //            -  1/8 .x^8
    //            +  1/9 .x^9
    //            + 1/10 .x^10
    //            - 1/11 .x^11
    //            + 1/12 .x^12
    //            - 1/13 .x^13
    //            + 1/14 .x^14
    //            - 1/15 .x^15
    //            + 1/16 .x^16
    //            + ..........
    xn  = x;
    y   = xn;
    xn *= x;
    y  -= xn / 2.0;
    xn *= x;
    y  += xn / 3.0;
    xn *= x;
    y  -= xn / 4.0;
    xn *= x;
    y  += xn / 5.0;
    xn *= x;
    y  -= xn / 6.0;
    xn *= x;
    y  += xn / 7.0;
    xn *= x;
    y  -= xn / 8.0;
    xn *= x;
    y  += xn / 9.0;
    xn *= x;
    y  -= xn / 10.0;
    xn *= x;
    y  += xn / 11.0;
    xn *= x;
    y  -= xn / 12.0;
    xn *= x;
    y  += xn / 13.0;
    xn *= x;
    y  -= xn / 14.0;
    xn *= x;
    y  += xn / 15.0;
    xn *= x;
    y  -= xn / 16.0;
    xn *= x;
    y  += xn / 17.0;
    xn *= x;
    y  -= xn / 18.0;
    xn *= x;
    y  += xn / 19.0;
    xn *= x;
    y  -= xn / 20.0;
    xn *= x;
    y  += xn / 21.0;
    xn *= x;
    y  -= xn / 22.0;
    xn *= x;
    y  += xn / 23.0;
    xn *= x;
    y  -= xn / 24.0;
    xn *= x;
    y  += xn / 25.0;
    xn *= x;
    y  -= xn / 26.0;
    xn *= x;
    y  += xn / 27.0;
    xn *= x;
    y  -= xn / 28.0;
    xn *= x;
    y  += xn / 29.0;
    xn *= x;
    y  -= xn / 30.0;

    y  -= ((t_float) shift) * LOG_2;

    return y;
}


t_float my_log2(t_float x)
{
    return my_log(x) / LOG_2;
}


t_float my_log10(t_float x)
{
    return my_log(x) / LOG_10;
}


t_float my_exp(t_float x)
{
    int     sign;
    t_float coef, xn, y;

    if(x >= 0.0)
    {
        sign = 0;
    }
    else
    {
        sign = 1;
        x    = -x;
    }
    coef = 1.0;
    while(x >= 32.0)
    {
        x    -= 32.0;
        coef *= EXP_32;
    }
    // x is now between 0 and 32
    if(x >= 16.0)
    {
        x    -= 16.0;
        coef *= EXP_16;
    }
    // x is now between 0 and 16
    if(x >= 8.0)
    {
        x    -= 8.0;
        coef *= EXP_8;
    }
    // x is now between 0 and 8
    if(x >= 4.0)
    {
        x    -= 4.0;
        coef *= EXP_4;
    }
    // x is now between 0 and 4
    if(x >= 2.0)
    {
        x    -= 2.0;
        coef *= EXP_2;
    }
    // x is now between 0 and 2
    if(x >= 1.0)
    {
        x    -= 1.0;
        coef *= EXP_1;
    }
    // x is now between 0 and 1
    if(x >= 0.5)
    {
        x    -= 1.0;
        coef *= EXP_1;
    }
    // x is now between -0.5 and 0.5
    // exp(x) =                 1
    //        +                 x^1
    //        + 1/2            .x^2
    //        + 1/6            .x^3
    //        + 1/24           .x^4
    //        + 1/120          .x^5
    //        + 1/720          .x^6
    //        + 1/5040         .x^7
    //        + 1/40320        .x^8
    //        + 1/362880       .x^9
    //        + 1/3628800      .x^10
    //        + 1/39916800     .x^11
    //        + 1/479001600    .x^12
    //        + 1/6227020800   .x^13
    //        + 1/87178291200  .x^14
    //        + 1/1307674368000.x^15
    //        + ....................
    y   = 1.0;
    xn  = x;
    y  += xn;
    xn *= x;
    y  += xn / 2.0;
    xn *= x;
    y  += xn / 6.0;
    xn *= x;
    y  += xn / 24.0;
    xn *= x;
    y  += xn / 120.0;
    xn *= x;
    y  += xn / 720.0;
    xn *= x;
    y  += xn / 5040.0;
    xn *= x;
    y  += xn / 40320.0;
    xn *= x;
    y  += xn / 362880.0;
    xn *= x;
    y  += xn / 3628800.0;
    xn *= x;
    y  += xn / 39916800.0;
    xn *= x;
    y  += xn / 479001600.0;
    xn *= x;
    y  += xn / 6227020800.0;
    xn *= x;
    y  += xn / 87178291200.0;
    xn *= x;
    y  += xn / 1307674368000.0;

    y  *= coef;
    if(sign)
    {
        y = 1.0 / y;
    }

    return y;
}


t_float my_pow2(t_float x)
{
    return my_exp(x * LOG_2);
}


t_float my_pow10(t_float x)
{
    return my_exp(x * LOG_10);
}


t_float my_pow(t_float x, t_float y)
{
    return my_exp(y * my_log(x));
}


t_float my_cos(t_float x)
{
    int     n;
    t_float x2, xn, y;

    x -= 2.0 * PI * my_floor(x / 2.0 / PI);
    if(x > PI)
    {
        x = 2.0 * PI - x;
    }
    for(n = 0; x > 0.1; n++)
    {
        x *= 0.5;
    }
    // x is now between 0 and 0.1
    // cos(x) = 1
    //        - 1/2                     .x^2
    //        + 1/24                    .x^4
    //        - 1/720                   .x^6
    //        + 1/40320                 .x^8
    //        - 1/3628800               .x^10
    //        + 1/479001600             .x^12
    //        - 1/87178291200           .x^14
    //        + 1/20922789888000        .x^16
    //        - 1/6402373705728000      .x^18
    //        + 1/2432902008176640000   .x^20
    //        - 1/1124000727777607680000.x^22
    //        + .............................
    y   = 1.0;
    x2  = x * x;
    xn  = x2;
    y  -= xn / 2.0;
    xn *= x2;
    y  += xn / 24.0;
    xn *= x2;
    y  -= xn / 720.0;
    xn *= x2;
    y  += xn / 40320.0;
    xn *= x2;
    y  -= xn / 3628800.0;
    xn *= x2;
    y  += xn / 479001600.0;
    xn *= x2;
    y  -= xn / 87178291200.0;
    xn *= x2;
    y  += xn / 20922789888000.0;
    xn *= x2;
    y  -= xn / 6402373705728000.0;
    xn *= x2;
    y  += xn / 2432902008176640000.0;
    xn *= x2;
    y  -= xn / 1124000727777607680000.0;
    while(n > 0)
    {
        y = 2.0 * y * y - 1.0;
        n--;
    }
    return y;
}


t_float my_sin(t_float x)
{
    return my_cos(0.5 * PI - x);
}


t_float my_cosh(t_float x)
{
    t_float y;

    y = my_exp(x);
    y = (y + 1.0 / y) * 0.5;
    return y;
}


t_float my_sinh(t_float x)
{
    t_float y;

    y = my_exp(x);
    y = (y - 1.0 / y) * 0.5;
    return y;
}

#endif // USE_MY_MATH
