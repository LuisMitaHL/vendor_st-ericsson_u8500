/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * ile   vsrc1_table2.c
 * rief  
 * uthor ST-Ericsson
 */
/*****************************************************************************/
#include "resample_local.h"

Float const YMEM vsrc1_table2[175] = {
	FORMAT_FLOAT(0.000012077796174,MAXVAL),  		// 0
	FORMAT_FLOAT(0.000010199008563,MAXVAL),  		// 1
	FORMAT_FLOAT(0.000014426353978,MAXVAL),  		// 2
	FORMAT_FLOAT(0.000019747451404,MAXVAL),  		// 3
	FORMAT_FLOAT(0.000026341662091,MAXVAL),  		// 4
	FORMAT_FLOAT(0.000034415560595,MAXVAL),  		// 5
	FORMAT_FLOAT(0.000044202961171,MAXVAL),  		// 6
	FORMAT_FLOAT(0.000055931402382,MAXVAL),  		// 7
	FORMAT_FLOAT(0.000069846033066,MAXVAL),  		// 8
	FORMAT_FLOAT(0.000086264416555,MAXVAL),  		// 9
	FORMAT_FLOAT(0.000105444085254,MAXVAL),  		// 10
	FORMAT_FLOAT(0.000127723974929,MAXVAL),  		// 11
	FORMAT_FLOAT(0.000153420858479,MAXVAL),  		// 12
	FORMAT_FLOAT(0.000182879424465,MAXVAL),  		// 13
	FORMAT_FLOAT(0.000216449112733,MAXVAL),  		// 14
	FORMAT_FLOAT(0.000254500657252,MAXVAL),  		// 15
	FORMAT_FLOAT(0.000297405323841,MAXVAL),  		// 16
	FORMAT_FLOAT(0.000345531056320,MAXVAL),  		// 17
	FORMAT_FLOAT(0.000399260591070,MAXVAL),  		// 18
	FORMAT_FLOAT(0.000458952139986,MAXVAL),  		// 19
	FORMAT_FLOAT(0.000524969034008,MAXVAL),  		// 20
	FORMAT_FLOAT(0.000597653527535,MAXVAL),  		// 21
	FORMAT_FLOAT(0.000677333044524,MAXVAL),  		// 22
	FORMAT_FLOAT(0.000764301629427,MAXVAL),  		// 23
	FORMAT_FLOAT(0.000858824593103,MAXVAL),  		// 24
	FORMAT_FLOAT(0.000961123996111,MAXVAL),  		// 25
	FORMAT_FLOAT(0.001071369069197,MAXVAL),  		// 26
	FORMAT_FLOAT(0.001189681026354,MAXVAL),  		// 27
	FORMAT_FLOAT(0.001316109666986,MAXVAL),  		// 28
	FORMAT_FLOAT(0.001450636056623,MAXVAL),  		// 29
	FORMAT_FLOAT(0.001593155060416,MAXVAL),  		// 30
	FORMAT_FLOAT(0.001743472899240,MAXVAL),  		// 31
	FORMAT_FLOAT(0.001901289475829,MAXVAL),  		// 32
	FORMAT_FLOAT(0.002066196946162,MAXVAL),  		// 33
	FORMAT_FLOAT(0.002237667407722,MAXVAL),  		// 34
	FORMAT_FLOAT(0.002415040256673,MAXVAL),  		// 35
	FORMAT_FLOAT(0.002597519788212,MAXVAL),  		// 36
	FORMAT_FLOAT(0.002784159230991,MAXVAL),  		// 37
	FORMAT_FLOAT(0.002973857132036,MAXVAL),  		// 38
	FORMAT_FLOAT(0.003165344867289,MAXVAL),  		// 39
	FORMAT_FLOAT(0.003357184696277,MAXVAL),  		// 40
	FORMAT_FLOAT(0.003547756378126,MAXVAL),  		// 41
	FORMAT_FLOAT(0.003735255714167,MAXVAL),  		// 42
	FORMAT_FLOAT(0.003917688748718,MAXVAL),  		// 43
	FORMAT_FLOAT(0.004092865376613,MAXVAL),  		// 44
	FORMAT_FLOAT(0.004258400592284,MAXVAL),  		// 45
	FORMAT_FLOAT(0.004411710136720,MAXVAL),  		// 46
	FORMAT_FLOAT(0.004550012986652,MAXVAL),  		// 47
	FORMAT_FLOAT(0.004670329205988,MAXVAL),  		// 48
	FORMAT_FLOAT(0.004769487802894,MAXVAL),  		// 49
	FORMAT_FLOAT(0.004844127192017,MAXVAL),  		// 50
	FORMAT_FLOAT(0.004890704913772,MAXVAL),  		// 51
	FORMAT_FLOAT(0.004905507039419,MAXVAL),  		// 52
	FORMAT_FLOAT(0.004884657716680,MAXVAL),  		// 53
	FORMAT_FLOAT(0.004824134059060,MAXVAL),  		// 54
	FORMAT_FLOAT(0.004719780168663,MAXVAL),  		// 55
	FORMAT_FLOAT(0.004567326639500,MAXVAL),  		// 56
	FORMAT_FLOAT(0.004362407369654,MAXVAL),  		// 57
	FORMAT_FLOAT(0.004100585783280,MAXVAL),  		// 58
	FORMAT_FLOAT(0.003777376436643,MAXVAL),  		// 59
	FORMAT_FLOAT(0.003388273150757,MAXVAL),  		// 60
	FORMAT_FLOAT(0.002928778540753,MAXVAL),  		// 61
	FORMAT_FLOAT(0.002394434785518,MAXVAL),  		// 62
	FORMAT_FLOAT(0.001780857751282,MAXVAL),  		// 63
	FORMAT_FLOAT(0.001083771919840,MAXVAL),  		// 64
	FORMAT_FLOAT(0.000299049667661,MAXVAL),  		// 65
	FORMAT_FLOAT(-0.000577252416302,MAXVAL),  		// 66
	FORMAT_FLOAT(-0.001548849900191,MAXVAL),  		// 67
	FORMAT_FLOAT(-0.002619190111789,MAXVAL),  		// 68
	FORMAT_FLOAT(-0.003791408090127,MAXVAL),  		// 69
	FORMAT_FLOAT(-0.005068281970604,MAXVAL),  		// 70
	FORMAT_FLOAT(-0.006452187644244,MAXVAL),  		// 71
	FORMAT_FLOAT(-0.007945053094322,MAXVAL),  		// 72
	FORMAT_FLOAT(-0.009548313269085,MAXVAL),  		// 73
	FORMAT_FLOAT(-0.011262862582326,MAXVAL),  		// 74
	FORMAT_FLOAT(-0.013089011953133,MAXVAL),  		// 75
	FORMAT_FLOAT(-0.015026442273441,MAXVAL),  		// 76
	FORMAT_FLOAT(-0.017074161924622,MAXVAL),  		// 77
	FORMAT_FLOAT(-0.019230464475703,MAXVAL),  		// 78
	FORMAT_FLOAT(-0.021492888977534,MAXVAL),  		// 79
	FORMAT_FLOAT(-0.023858181669809,MAXVAL),  		// 80
	FORMAT_FLOAT(-0.026322260798415,MAXVAL),  		// 81
	FORMAT_FLOAT(-0.028880185108662,MAXVAL),  		// 82
	FORMAT_FLOAT(-0.031526123122828,MAXVAL),  		// 83
	FORMAT_FLOAT(-0.034253329530083,MAXVAL),  		// 84
	FORMAT_FLOAT(-0.037054122745967,MAXVAL),  		// 85
	FORMAT_FLOAT(-0.039919868616300,MAXVAL),  		// 86
	FORMAT_FLOAT(-0.042840967821770,MAXVAL),  		// 87
	FORMAT_FLOAT(-0.045806849460857,MAXVAL),  		// 88
	FORMAT_FLOAT(-0.048805968783686,MAXVAL),  		// 89
	FORMAT_FLOAT(-0.051825811063139,MAXVAL),  		// 90
	FORMAT_FLOAT(-0.054852902443972,MAXVAL),  		// 91
	FORMAT_FLOAT(-0.057872824564121,MAXVAL),  		// 92
	FORMAT_FLOAT(-0.060870237919497,MAXVAL),  		// 93
	FORMAT_FLOAT(-0.063828909845497,MAXVAL),  		// 94
	FORMAT_FLOAT(-0.066731750143534,MAXVAL),  		// 95
	FORMAT_FLOAT(-0.069560852239872,MAXVAL),  		// 96
	FORMAT_FLOAT(-0.072297542531399,MAXVAL),  		// 97
	FORMAT_FLOAT(-0.074922435255395,MAXVAL),  		// 98
	FORMAT_FLOAT(-0.077415494091936,MAXVAL),  		// 99
	FORMAT_FLOAT(-0.079756101536151,MAXVAL),  		// 100
	FORMAT_FLOAT(-0.081923132761455,MAXVAL),  		// 101
	FORMAT_FLOAT(-0.083895037267588,MAXVAL),  		// 102
	FORMAT_FLOAT(-0.085649925488420,MAXVAL),  		// 103
	FORMAT_FLOAT(-0.087165662060640,MAXVAL),  		// 104
	FORMAT_FLOAT(-0.088419963103543,MAXVAL),  		// 105
	FORMAT_FLOAT(-0.089390499983962,MAXVAL),  		// 106
	FORMAT_FLOAT(-0.090055006761336,MAXVAL),  		// 107
	FORMAT_FLOAT(-0.090391391411529,MAXVAL),  		// 108
	FORMAT_FLOAT(-0.090377851843977,MAXVAL),  		// 109
	FORMAT_FLOAT(-0.089992993692806,MAXVAL),  		// 110
	FORMAT_FLOAT(-0.089215951406413,MAXVAL),  		// 111
	FORMAT_FLOAT(-0.088026510450860,MAXVAL),  		// 112
	FORMAT_FLOAT(-0.086405231724552,MAXVAL),  		// 113
	FORMAT_FLOAT(-0.084333574984041,MAXVAL),  		// 114
	FORMAT_FLOAT(-0.081794023359389,MAXVAL),  		// 115
	FORMAT_FLOAT(-0.078770206545309,MAXVAL),  		// 116
	FORMAT_FLOAT(-0.075247021750146,MAXVAL),  		// 117
	FORMAT_FLOAT(-0.071210753080733,MAXVAL),  		// 118
	FORMAT_FLOAT(-0.066649187017284,MAXVAL),  		// 119
	FORMAT_FLOAT(-0.061551724317563,MAXVAL),  		// 120
	FORMAT_FLOAT(-0.055909486509324,MAXVAL),  		// 121
	FORMAT_FLOAT(-0.049715418123579,MAXVAL),  		// 122
	FORMAT_FLOAT(-0.042964381321564,MAXVAL),  		// 123
	FORMAT_FLOAT(-0.035653244430617,MAXVAL),  		// 124
	FORMAT_FLOAT(-0.027780962830226,MAXVAL),  		// 125
	FORMAT_FLOAT(-0.019348650955520,MAXVAL),  		// 126
	FORMAT_FLOAT(-0.010359645841245,MAXVAL),  		// 127
	FORMAT_FLOAT(-0.000819560918727,MAXVAL),  		// 128
	FORMAT_FLOAT(0.009263669959407,MAXVAL),  		// 129
	FORMAT_FLOAT(0.019879759908787,MAXVAL),  		// 130
	FORMAT_FLOAT(0.031016046194780,MAXVAL),  		// 131
	FORMAT_FLOAT(0.042657479933509,MAXVAL),  		// 132
	FORMAT_FLOAT(0.054786627163439,MAXVAL),  		// 133
	FORMAT_FLOAT(0.067383681689845,MAXVAL),  		// 134
	FORMAT_FLOAT(0.080426490496077,MAXVAL),  		// 135
	FORMAT_FLOAT(0.093890591415805,MAXVAL),  		// 136
	FORMAT_FLOAT(0.107749263235592,MAXVAL),  		// 137
	FORMAT_FLOAT(0.121973587770090,MAXVAL),  		// 138
	FORMAT_FLOAT(0.136532525182412,MAXVAL),  		// 139
	FORMAT_FLOAT(0.151393000300776,MAXVAL),  		// 140
	FORMAT_FLOAT(0.166520001435463,MAXVAL),  		// 141
	FORMAT_FLOAT(0.181876690739774,MAXVAL),  		// 142
	FORMAT_FLOAT(0.197424525495512,MAXVAL),  		// 143
	FORMAT_FLOAT(0.213123390166063,MAXVAL),  		// 144
	FORMAT_FLOAT(0.228931738762883,MAXVAL),  		// 145
	FORMAT_FLOAT(0.244806746810425,MAXVAL),  		// 146
	FORMAT_FLOAT(0.260704471545684,MAXVAL),  		// 147
	FORMAT_FLOAT(0.276580021208006,MAXVAL),  		// 148
	FORMAT_FLOAT(0.292387730887177,MAXVAL),  		// 149
	FORMAT_FLOAT(0.308081345065658,MAXVAL),  		// 150
	FORMAT_FLOAT(0.323614205926752,MAXVAL),  		// 151
	FORMAT_FLOAT(0.338939446209850,MAXVAL),  		// 152
	FORMAT_FLOAT(0.354010185521551,MAXVAL),  		// 153
	FORMAT_FLOAT(0.368779729458258,MAXVAL),  		// 154
	FORMAT_FLOAT(0.383201770599982,MAXVAL),  		// 155
	FORMAT_FLOAT(0.397230589272809,MAXVAL),  		// 156
	FORMAT_FLOAT(0.410821254560544,MAXVAL),  		// 157
	FORMAT_FLOAT(0.423929823313370,MAXVAL),  		// 158
	FORMAT_FLOAT(0.436513536317139,MAXVAL),  		// 159
	FORMAT_FLOAT(0.448531010821819,MAXVAL),  		// 160
	FORMAT_FLOAT(0.459942428339388,MAXVAL),  		// 161
	FORMAT_FLOAT(0.470709716260120,MAXVAL),  		// 162
	FORMAT_FLOAT(0.480796722543857,MAXVAL),  		// 163
	FORMAT_FLOAT(0.490169383016229,MAXVAL),  		// 164
	FORMAT_FLOAT(0.498795879029676,MAXVAL),  		// 165
	FORMAT_FLOAT(0.506646785838559,MAXVAL),  		// 166
	FORMAT_FLOAT(0.513695210386784,MAXVAL),  		// 167
	FORMAT_FLOAT(0.519916917485526,MAXVAL),  		// 168
	FORMAT_FLOAT(0.525290443890060,MAXVAL),  		// 169
	FORMAT_FLOAT(0.529797199865321,MAXVAL),  		// 170
	FORMAT_FLOAT(0.533421557204587,MAXVAL),  		// 171
	FORMAT_FLOAT(0.536150923059821,MAXVAL),  		// 172
	FORMAT_FLOAT(0.537975800146972,MAXVAL),  		// 173
	FORMAT_FLOAT(0.538889831712763,MAXVAL)   		// 174
};  
