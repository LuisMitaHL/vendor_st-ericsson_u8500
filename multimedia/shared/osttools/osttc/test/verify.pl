#!/bin/env perl
# vim:syntax=perl
#--------------------------------------------------------------------
## @file
#/*****************************************************************************/
#
#  Copyright (C) ST-Ericsson 2011 - All rights reserved.
#  This code is ST-Ericsson proprietary and confidential.
#  Any use of the code for whatever purpose is subject to
#  specific written permission of ST-Ericsson SA.
#  
#---------------------------------------------------------------------

use Carp;
use strict;
use warnings;
use Getopt::Long;
use Term::ANSIColor;

my @action;
my %defines;

if (defined ($defines{strict}))
{
	$SIG{__WARN__} = \&Carp::confess;
	$SIG{__DIE__} = \&Carp::confess;
}

GetOptions (
	'help'    		=> \&help,
	"defines:s"		=> \%defines,	
	"action=s"		=> \@action,
);	  

if ( not defined ($action[0]))
{
	printf "Missing Action: Do not know what to do!\n";
	help();
	exit 1;
}
if ($action[0] eq "verify")
{

	if ( not defined ($defines{refdir}))
	{
		printf "Reference directory missing!\n";
		help();
		exit 1;
	}
	die "Reference directory does not exist: $defines{refdir} \n" if (!-d $defines{refdir});

	printf "refdir: %s\n", $defines{refdir};

	my $header = "osttc_test_tmp_st_osttc_test1Traces.h";
	my $ret = my_diff($header,$defines{refdir});

	if ($defines{refdir} !~ /test1/)
	{
		$header = "osttc_test_tmp_st_osttc_test2Traces.h";
		$ret += my_diff($header,$defines{refdir});
	}
	$ret += my_diff("ST_TU_OSTTC.xml",$defines{refdir});
	$ret += my_diff("ST_TU_OSTTC_top.h",$defines{refdir});
	#$ret += my_diff("ST_TU_OSTTC_top.txt",$defines{refdir});
	$ret += my_diff("ST_TU_OSTTC_top.xml",$defines{refdir});
	$ret += my_diff("toto_top.xml",$defines{refdir});
	$ret += my_diff("toto_top.h",$defines{refdir});
	#$ret += my_diff("toto_top.txt",$defines{refdir});

	exit $ret;
}


if ($action[0] eq "patch")
{
	if ( not defined ($defines{input}))
	{
		printf "Input file missing!\n";
		help();
		exit 1;
	}

	if ( not defined ($defines{output}))
	{
		printf "Output file missing!\n";
		help();
		exit 1;
	}

	if ( not defined ($defines{line}))
	{
		printf "Line to be added missing!\n";
		help();
		exit 1;
	}

	if ( not defined ($defines{lpos}))
	{
		printf "Line position to be added missing!\n";
		help();
		exit 1;
	}

	my $ret = patch($defines{input}, $defines{output}, $defines{line}, $defines{lpos});
	exit $ret;
}

sub my_diff 
{
	my $file = $_[0];
	my $refdir = $_[1];
	my $rval;

	my $ref = "$refdir/".$file; 
	my $outputfile = "$ENV{'TARGET_OUT_INTERMEDIATES'}" . "/shared/osttools/osttc/test/tmp/autogentraces/".$file; 
	
	die "File not found: $ref \n" if (!-e $ref);
	die "File not found: $outputfile \n" if (!-e $outputfile);

	$rval = system("diff $ref $outputfile");

	if ($rval != 0)
	{
		die "Found difference between $ref and $outputfile\n";
		return 1;
	}
	return 0;
}

sub my_patch
{
	my $input = $_[0];
	my $output = $_[1];
	my $position = $_[2];
	my $refline = $_[3];
	my $linec = 0;
	my $match = 0;

	open IN, $input or die "Could not open file [$input]: $!\n";		
	open OUT, $output or die "Could not open file [$output]: $!\n";		

	while (my $line = <IN>)
	{
		$linec++;
		print OUT $line;

		if ($position == $linec)
		{
			$match = 1;
			print OUT $refline;
		}
	}
	if ($match == 0)
	{
		printf "Cannot find line number: %d in file %s\n", $position, $input;
		return 1;
	}
	return 0;
}

sub help
{
	print "verify\n";
	&usage();
	print "\n== Available Options are:
	--help							: prints this help and quit	
	--action=<ACTION>		: Action to perform
		--action=patch
		--action=verify
	--defines 					: Configure some internal options
		For ACTION: patch
		--defines=input=<input file>: provide input file
		--defines=output=<input file>: provide output file
		--defines=line<line>: line to be added in output file
		--defines=lpos<number>: line position in input file after which we need to add the line
		For ACTION: verify
			--defines=refdir=<directory>: reference directory to sue for comparison
	";
	exit(0);
}

sub usage
{
	print "\n== Usage :  verify [OPTIONS]\n";
	exit(0);	
}


		
