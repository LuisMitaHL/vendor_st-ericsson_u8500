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
use File::Basename qw(dirname basename fileparse);

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


if ($action[0] eq "diff")
{
	my $ret = 1;

	if ( not defined ($defines{input1}))
	{
		printf "Input file missing!\n";
		help();
		exit 1;
	}
	if ( not defined ($defines{input2}))
	{
		printf "Input file missing!\n";
		help();
		exit 1;
	}


	if ($defines{diff2})
	{
		$ret = my_diff_2($defines{input1}, $defines{input2});
		$ret += my_diff_2($defines{input2}, $defines{input1});
	}
	else
	{
		$ret = my_diff($defines{input1}, $defines{input2});
	}
	exit $ret;
}


sub my_diff
{
	my $input1 = $_[0];
	my $input2 = $_[1];
	my $linec = 0;
	my $error = 0;
	my $line1;
	my $line2;
  
  # split between filename and directory name of the generated header file
  my ($output1_file, $output1_dir, $output1_fext) = fileparse($input1);
  my ($output2_file, $output2_dir, $output2_fext) = fileparse($input2);

  if ( defined ($defines{outputdir}))
  {
    $output1_dir=$defines{outputdir};
    $output2_dir=$defines{outputdir};
  }

	my $output1 = $output1_dir."/".$output1_file."_strip.txt";
	my $output2 = $output2_dir."/".$output2_file."_strip.txt";



	open IN1, $input1 or die "Could not open file [$input1]: $!\n";		
	open IN2, $input2 or die "Could not open file [$input2]: $!\n";		

	open OUT1, '>', $output1 or die "Could not open file [$output1]: $!\n";		
	open OUT2, '>', $output2 or die "Could not open file [$output2]: $!\n";		
	

	while (1)
	{

		$line1 = <IN1>;
		$line2 = <IN2>;
		
		last if (not defined($line1));
		last if (not defined($line2));

		$linec++;
		if ($line1 =~ /(.*)\s?OMX handle (.*): ID:? -?(\w+):\s?(.*)/)
		{
			$line1 =~ s/(.*)\s?OMX handle (.*): ID:? -?(\w+):\s?(.*)/$1 OMX handle: ID: $4/
		}
		if ($line1 =~ /(.*) (\w+): Hello World (.*)/)
		{
			$line1 =~ s/(.*) (\w+): Hello World (.*)/$1 Hello World $3/
		}

		if ($line2 =~ /(.*)\s?OMX handle (.*): ID:? -?(\w+):\s?(.*)/)
		{
			$line2 =~ s/(.*)\s?OMX handle (.*): ID:? -?(\w+):\s?(.*)/$1 OMX handle: ID: $4/
		}
		if ($line2 =~ /(.*) (\w+): Hello World (.*)/)
		{
			$line2 =~ s/(.*) (\w+): Hello World (.*)/$1 Hello World $3/
		}

		printf OUT1 "$line1";
		printf OUT2 "$line2";

		if ($line1 ne $line2)
		{
			$error++;
			printf "$line1\n";
			printf "--\n";
			printf "$line2\n";			
		}
	}
	close IN1;
	close IN2;
	close OUT1;
	close OUT2;
	return $error;
}


sub my_diff_2
{
	my $input1 = $_[0];
	my $input2 = $_[1];
	my $linec = 0;
	my $error = 0;
	my $line1;
	my $line2;
  # split between filename and directory name of the generated header file
  my ($output1_file, $output1_dir, $output1_fext) = fileparse($input1);
  my ($output2_file, $output2_dir, $output2_fext) = fileparse($input2);

  if ( defined ($defines{outputdir}))
  {
    $output1_dir=$defines{outputdir};
    $output2_dir=$defines{outputdir};
  }

	my $output1 = $output1_dir."/".$output1_file."_strip.txt";
	my $output2 = $output2_dir."/".$output2_file."_strip.txt";
	my $match = 0;



	open IN1, $input1 or die "Could not open file [$input1]: $!\n";

	open OUT1, '>', $output1 or die "Could not open file [$output1]: $!\n";
	open OUT2, '>', $output2 or die "Could not open file [$output2]: $!\n";
	

	while ($line1 = <IN1>)
	{
		if ($line1 =~ /^\[.*:0x..\]/)
		{
			$line1 =~ s/^\[(.{1,5}:)(0x..)\](.*)/\[$1\]$3/
		}

		if ($line1 =~ /(.*) OMX handle (.*): ID:? (\w+): (.*)/)
		{
			$line1 =~ s/(.*) OMX handle (.*): ID:? (\w+): (.*)/$1 OMX handle: ID: $4/
		}

		if ($line1 =~ /(.*) (\w+): Hello World (.*)/)
		{
			$line1 =~ s/(.*) (\w+): Hello World (.*)/$1 Hello World $3/
		}

		if ($line1 =~ /(.*) at time: (.*)/)
		{
			$line1 =~ s/(.*) at time: (.*)/$1 at time:/
		}

		printf OUT1 "$line1";

		open IN2, $input2 or die "Could not open file [$input2]: $!\n";
		$match = 0;

		while ($line2 = <IN2>)
		{
			if ($line2 =~ /^\[.*:0x..\]/)
			{
				$line2 =~ s/^\[(.{1,5}:)(0x..)\](.*)/\[$1\]$3/
			}
			
			if ($line2 =~ /(.*) OMX handle (.*): ID:? (\w+): (.*)/)
			{
				$line2 =~ s/(.*) OMX handle (.*): ID:? (\w+): (.*)/$1 OMX handle: ID: $4/
			}

			if ($line2 =~ /(.*) (\w+): Hello World (.*)/)
			{
				$line2 =~ s/(.*) (\w+): Hello World (.*)/$1 Hello World $3/
			}

			if ($line2 =~ /(.*) at time: (.*)/)
			{
				$line2 =~ s/(.*) at time: (.*)/$1 at time:/
			}

			if ($line1 eq $line2)
			{
				printf OUT2 "$line2";
				$match = 1;
				last;		
			}
		}
		if ($match != 1)
		{
			$error++;		
			printf "line: $line1 not found in $output2\n";
		}
		close IN2;
	}
	close IN1;
	close OUT1;
	close OUT2;
	return $error;
}

sub help
{
	print "verify\n";
	&usage();
	print "\n== Available Options are:
	--help							: prints this help and quit	
	--action=<ACTION>		: Action to perform
		--action=diff
	--defines 					: Configure some internal options
		For ACTION: diff
		--defines=input1=<input file>: provide left input file for comparison
		--defines=input2=<input file>: provide left input file for comparison
	";
	exit(0);
}

sub usage
{
	print "\n== Usage :  verify [OPTIONS]\n";
	exit(0);	
}

