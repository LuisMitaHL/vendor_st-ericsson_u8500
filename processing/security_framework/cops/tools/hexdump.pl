#!/usr/bin/perl
# Copyright (C) ST-Ericsson SA 2011. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
# Author: joakim.xx.bech@stericsson.com
#
use strict;
use warnings;

my $file;
# Read the complete file into a variable.
open IN, "<$ARGV[0]" or die "Cannot open $ARGV[0]\n";
binmode IN;
$file = join('', <IN>);
close IN;

my $remain = length($file);
my $line_ctr = 1;
my $column = 16;
for (my $i = 0; $i < length($file); $i++) {
	my @ascii = ();
	my $j = 0;
	for (; $j < $column; $j++) {
		my $hex_val = "0x" . sprintf("%02X,",
                                             ord(substr($file, $i+$j, 1)));

		# Save the output as ascii for later print.
		push(@ascii, substr($file, $i+$j, 1));

		print $hex_val;
		print " " if ($line_ctr % 4) == 0;
		$line_ctr++;
	}

	print " /* ";
	foreach my $byte (@ascii) {
		# Special handling since :print: prints a newline in windows.
		if ($byte ne '*' and $byte =~ m/[[:print:]]/ and
                    ord($byte) > 0x1F) {
			print $byte;
		} else {
			print ".";
		}
	}
	print " */\n";

	$i = $i+$j-1;
	$remain -= $j;
	$column = $remain if ($remain < 16);
}
