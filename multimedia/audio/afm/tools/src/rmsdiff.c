/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   rmsdiff.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define BITS_IN_BYTE 8
#define STRICT_ONE_BIT

static char     programName[] = "rmsdiff";

void
usage()
{
	fprintf(stderr,
			"\nusage: %s [-C] [-b] [-f] [-g] [-i n1 [-i n2]] [-w w1 [-w w2]] [-v]\n"
			" \t [-n n1 [-n n2]] [-t t1 [-t t2]] \n"
			" \t [-e] [-s] [-c] [-r] [-p] file_1 file_2\n"
			"Find the maximum absolute difference between signed 16-24 or 32 bit words in file_1 and\n"
			"file_2. Options are:\n\n"
			"        -m          specify max sample-wise difference \n"
			"        -b          byte swap both files before comparing\n"
            "        -f          byte swap file_1 before comparing\n"
            "        -g          byte swap file_2 before comparing\n"
			"        -i n        ignore n bytes at the start of both file_1 and file_2\n"
			"        -i n1 -i n2 ignore n1 bytes at the start of file 1, and n2 bytes\n"
			"                    at the start of file2\n"
			"        -w w1 -w w2 sets number of bytes per words in file_1 and file_2\n"
			"        -n n1 -n n2 sets number of tracks to handle in multichannel files, assumes interleaved data \n"
			"        -t t1 -t t2 sets tracks to handle in multichannel files, assumes interleaved data \n"
			"        -v          lists non-zero differences\n"
			"        -d          lists differences > 1 \n"
			"        -e          lists every difference\n"
			"        -s          print signed difference\n"
			"        -r          print RMS error level\n"
			"        -p          discard RMS error level\n"
			"        -c          count samples different and samples compared\n"
			"        -z          positive vs negative error distribution \n"
			"        -S          strictly one-bit difference allowed for bit and rms difference \n"
            "        -C          check if file_1 and file_2 has the same size",


			programName);
	exit(1);
}

int
main(int argc, char *argv[])
{
	FILE           *inputFile1;
	FILE           *inputFile2;

	double          maxDifference;
	double          minDifference;
	long            sample1;
	long            sample2;
	double          sample1_float;
	double          sample2_float;
	unsigned char   byte_sample1;
	unsigned char   byte_sample2;
	double          err_energy = 0.0;
	double          rms_error;

	long            ignore1 = -1;
	long            ignore2 = -1;
	long            wordsize1 = -1;
	long            wordsize2 = -1;

	long            track1 = -1;
	long            track2 = -1;
	long            num_track1 = -1;
	long            num_track2 = -1;

	int             i,j;
	int             done_1 = 0;
	int             done_2 = 0;
	int             verbose = 0;
	int             display_above = 0;
	int             every = 0;
	int             sign = 0;
	int             count = 0;
	int             num_comp = 0;
	int             num_diff = 0;
    int             byte_swap_1 = 0;
    int             byte_swap_2 = 0;
	int             check_size = 0;
	int             rms = 0;
	int             no_rms = 0;
	int             N1 = 16;
	int             N2 = 16;
	int             maxN;
	double          maxAllowedDiff;
	double          maxAllowedRMS;
	double          oneBitDifference;
	double          maxAllowedBitsDiff = 0.0;
	long            count_positive=0;
	long            count_negative=0;
	int             err_dist=0;
	int				strict=0;

	while (argc > 1 && argv[1][0] == '-') {
		long            arg;

		switch (argv[1][1]) {
		case 'i':
			/*
			 * number of bytes to ignore at the start of each file
			 */
			if (argv[1][2] != '\0')
				arg = atol(&argv[1][2]);
			else if (argc > 2) {
				arg = atol(argv[2]);
				argv++;
				argc--;
			} else
				usage();

			if (ignore1 < 0)
				ignore1 = arg;	/* first */
			else
				ignore2 = arg;	/* second */

			break;

		case 'm':
			/*
			 * maximum sample-wise difference
			 */
			if (argv[1][2] != '\0')
				arg = atol(&argv[1][2]);
			else if (argc > 2) {
				arg = atol(argv[2]);
				argv++;
				argc--;
			} else
				usage();

			maxAllowedBitsDiff = (double) arg;

			break;

		case 'n':
			/*
			 * number of tracks to handle
			 */
			if (argv[1][2] != '\0')
				arg = atol(&argv[1][2]);
			else if (argc > 2) {
				arg = atol(argv[2]);
				argv++;
				argc--;
			} else
				usage();
			
			if (num_track1 < 0)
				num_track1 = arg;	/* first */
			else
				num_track2 = arg;	/* second */
			
			break;

		case 't':
			/*
			 * track to handle
			 */
			if (argv[1][2] != '\0')
				arg = atol(&argv[1][2]);
			else if (argc > 2) {
				arg = atol(argv[2]);
				argv++;
				argc--;
			} else
				usage();
			
			if (track1 < 0)
				track1 = arg;	/* first */
			else
				track2 = arg;	/* second */
			
			break;

		case 'w':
			/*
			 * number of bytes per words in file 
			 */
			if (argv[1][2] != '\0')
				arg = atol(&argv[1][2]);
			else if (argc > 2) {
				arg = atol(argv[2]);
				argv++;
				argc--;
			} else
				usage();
			
			if (wordsize1 < 0)
				wordsize1 = arg;	/* first */
			else
				wordsize2 = arg;	/* second */
			
			break;

		case 'e':
			every = 1;
			
		case 'v':
			verbose = 1;
			break;

		case 'd':
			display_above = 1;
			break;
			
		case 's':
			sign = 1;
			break;

		case 'c':
			count = 1;
			break;

		case 'b':
			byte_swap_1 = 1;
			byte_swap_2 = 1;
			break;

        case 'f':
            byte_swap_1 = 1;
            break;
			
        case 'g':
            byte_swap_2 = 1;
            break;

		case 'r':
			rms = 1;
			break;
	
        case 'C':
			check_size = 1;
			break;

		case 'p':
			no_rms = 1;
			break;
			
		case 'z':
			err_dist = 1;
			break;

		case 'S':
			strict = 1;
			break;

			
		default:
			usage();
		}

		argv++;
		argc--;
	}

	if (argc < 3)
		usage();
	
	/*
	 * set up the number of bytes to ignore from the defaults
	 */
	if (ignore1 < 0)
		ignore1 = 0;
	if (ignore2 < 0)
		ignore2 = ignore1;
	
	/*
	 * set up the number of bytes per word from the defaults
	 */
	if (wordsize1 < 0)
		wordsize1 = 2;
	if (wordsize2 < 0)
		wordsize2 = wordsize1;

	/* set up number of tracks in multichannel files */
	if (num_track1 < 0)
		num_track1 = 1;
	if (num_track2 < 0)
		num_track2 = num_track1;

	/* setup tracks to test */
	if (track1 < 0)
		track1 = 0;
	if (track2 < 0)
		track2 = track1;

	assert( num_track1>0);assert( num_track2>0);
	assert( track1>=0);assert( track2>=0);
	assert( track1 < num_track1); assert( track2 < num_track2);
	
	/* setup N1 and N2 */
	N1 = wordsize1 * BITS_IN_BYTE;
	N2 = wordsize2 * BITS_IN_BYTE;
	maxN = (N1 > N2) ? N1 : N2;

	if (strict==0)
		maxAllowedRMS =  pow(2.0, (double) (16 - 15)) / sqrt(12.0);
	else
		maxAllowedRMS =  pow(2.0, (double) (16 - 16)) / sqrt(12.0);
	oneBitDifference = pow(2.0, (double) (maxN - 16));
	

	if (strict==0)
		maxAllowedDiff = pow(2.0, (double) (maxN - 15));
	else
		maxAllowedDiff = oneBitDifference;

	if( maxAllowedBitsDiff > 0.0 ) {
		maxAllowedDiff = pow(2.0, (double) (maxN - 16 + maxAllowedBitsDiff)) - 1.0;
	}
	
	/*
	 * open the input files
	 */
	inputFile1 = fopen(argv[1], "rb");
	if (inputFile1 == NULL) {
		fprintf(stderr,
				"%s: cannot open input file %s\n", programName, argv[1]);
		exit(1);
	}
	/*
	 * skip over bytes at the beginning of inputFile1
	 */
	while (ignore1-- > 0 &&
		   fread(&byte_sample1, sizeof (byte_sample1), 1,
				 inputFile1) == 1);

	inputFile2 = fopen(argv[2], "rb");
	if (inputFile2 == NULL) {
		fprintf(stderr,
				"%s: cannot open input file %s\n", programName, argv[2]);
		exit(1);
	}
	/*
	 * skip over bytes at the beginning of inputFile2
	 */
	while (ignore2-- > 0 &&
		   fread(&byte_sample2, sizeof (byte_sample2), 1,
				 inputFile2) == 1);

	/*
	 * read a sample for each channel and make sure all are available before
	 * writing.
	 */
	maxDifference = 0.0;
	minDifference = 0.0;
	while (!(done_1 || done_2)) {
		sample1 = 0;
		sample2 = 0;

		/* skip useless data */
		for(j=0;j<track1;j++) {
			for (i = 0; i < wordsize1; i++) {
				done_1 = done_1 ||
					!(fread(&byte_sample1, sizeof (byte_sample1), 1, inputFile1)
					  == 1);
			}
		}

		for (i = 0; i < wordsize1; i++) {
			done_1 = done_1 ||
				!(fread(&byte_sample1, sizeof (byte_sample1), 1, inputFile1)
				  == 1);
			sample1 = (sample1 << BITS_IN_BYTE) + byte_sample1;
		}

		/* skip other channels */
		for(j=track1+1;j<num_track1;j++) {
			for (i = 0; i < wordsize1; i++) {
				done_1 = done_1 ||
					!(fread(&byte_sample1, sizeof (byte_sample1), 1, inputFile1)
					  == 1);
			}
		}

		/* skip useless data */
		for(j=0;j<track2;j++) {
			for (i = 0; i < wordsize2; i++) {
				done_2 = done_2 ||
					!(fread(&byte_sample2, sizeof (byte_sample2), 1, inputFile2)
					  == 1);
			}
		}
		if (!done_2) {
			for (i = 0; i < wordsize2; i++) {
				done_2 = done_2 ||
					!(fread(&byte_sample2, sizeof (byte_sample2), 1,
							inputFile2) == 1);
				sample2 = (sample2 << BITS_IN_BYTE) + byte_sample2;
			}
		}
		/* skip other channels */
		for(j=track2+1;j<num_track2;j++) {
			for (i = 0; i < wordsize2; i++) {
				done_2 = done_2 ||
					!(fread(&byte_sample2, sizeof (byte_sample2), 1, inputFile2)
					  == 1);
			}
		}

		if (!(done_1 || done_2)) {

			double difference;
            
            if (byte_swap_1) {
                unsigned long   u_s1 = (unsigned long) sample1;

                /* swap bytes */
                sample1 = (long) (((u_s1 & 0xff000000) >> 24) |
                                  ((u_s1 & 0x00ff0000) >> 8) |
                                  ((u_s1 & 0x0000ff00) << 8) |
                                  ((u_s1 & 0x000000ff) << 24));

                /* shift right, sign extend */
                sample1 = (sample1 >> (32 - N1));
            }

            if (byte_swap_2) {
                unsigned long   u_s2 = (unsigned long) sample2;

                /* swap bytes */
                sample2 = (long) (((u_s2 & 0xff000000) >> 24) |
                                  ((u_s2 & 0x00ff0000) >> 8) |
                                  ((u_s2 & 0x0000ff00) << 8) |
                                  ((u_s2 & 0x000000ff) << 24));

                /* shift right, sign extend */
                sample2 = (sample2 >> (32 - N2));
            }

			/* Sign extend data words */
			sample1 = (sample1 << (32 - N1));
			sample2 = (sample2 << (32 - N2));

            if(sample1 & 0x80000000L)
            {
                sample1_float = (double) sample1;
                if(sample1_float > 0.0)
                {
                    sample1_float -= pow(2.0, 32.0);
                }
                sample1_float /= pow(2.0, (double) maxN);
            }
            else
            {
                sample1_float = ((double) sample1) / pow(2.0, (double) maxN);
            }
            if(sample2 & 0x80000000L)
            {
                sample2_float = (double) sample2;
                if(sample2_float > 0.0)
                {
                    sample2_float -= pow(2.0, 32.0);
                }
                sample2_float /= pow(2.0, (double) maxN);
            }
            else
            {
                sample2_float = ((double) sample2) / pow(2.0, (double) maxN);
            }
            
			difference = (sign ? (sample1_float - sample2_float)
						  : fabs(sample1_float - sample2_float));

			err_energy += difference * difference;

			num_comp++;

			if (difference != 0.0) {
				num_diff++;
			}
			if (difference > maxDifference)
				maxDifference = difference;

			if (difference < minDifference)
				minDifference = difference;

			if(err_dist)
			{
				if( (abs(sample2)-abs(sample1) ) > 0)
				{
					count_positive++;
				}
				else if ( (abs(sample2)-abs(sample1)) < 0)
				{
					count_negative++;
				}	
			}	
			
			if (verbose) {
				if (every || (difference != 0.0))
					printf("Sample %6d : %6.0lf %6.0lf %6.0lf\n",
						   num_comp, sample1_float, sample2_float, difference);
			} else {
				if (display_above) {
					if (every || (difference > maxAllowedDiff)) {
			  					printf("Sample %6d : %6.0lf %6.0lf %6.0lf\n",
							   num_comp, sample1_float, sample2_float, difference);
					}
				}
			}
		}
	}

	/*
	 * clean up
	 */
	fclose(inputFile1);
	fclose(inputFile2);

	/* Don't print */
	/*
	 * if (sign) { printf("Max : %lf, Min : %lf\n", maxDifference,
	 * minDifference);          maxDifference =
	 * (maxDifference>fabs(minDifference)) ? maxDifference :
	 * fabs(minDifference); } else { printf("Max:%d\n", maxDifference); }
	 */

	if (count) {
		printf("Samples compared : %6d, Samples different : %6d\n",
			   num_comp, num_diff);
	}
	rms_error = sqrt(err_energy / (double) num_comp) / pow(2.0, (double) (maxN - 16));
	if (rms) {
		printf("   RMS level: %lf   Allowed RMS level:%lf\n", rms_error,
			   maxAllowedRMS);
	}

	if(err_dist)
	{
		printf("total samples with positive error :%6d\n",count_positive);
		printf("total samples with negative error :%6d\n",count_negative);
	}	
	
	if (no_rms == 0) {
		/* Check for maximum difference and rms error requirements */
		if ((maxDifference > maxAllowedDiff) ||
			(rms_error > maxAllowedRMS)) {
			printf("RMS Verification Test Failed.\n");
			printf("   Max differenceB: %lf   Allowed max difference:%lf\n",
				   maxDifference, maxAllowedDiff);
			printf("   RMS level: %lf   Allowed RMS level:%lf\n", rms_error,
				   maxAllowedRMS);
			return (1);
		}

		else {
			/* printf("RMS Verification Test Passed.\n"); */
			/* Don't print anything if it passes. */
		}
	} else {
		/* Check for maximum difference and rms error requirements */
		if ((maxDifference > maxAllowedDiff)) {
			printf("   Max difference: %lf   Allowed max difference:%lf\n",
				   maxDifference, maxAllowedDiff);
			return (1);
		}

		else {
			/* printf("RMS Verification Test Passed.\n"); */
			/* Don't print anything if it passes. */
		}
	}
    
    if((!(done_1 && done_2)) && check_size){
        if(done_1){
            printf("EOF file_1.\n");
        }    
        if(done_2){
            printf("EOF file_2.\n");
        }
        return (1);
    }

	return (0);
}
