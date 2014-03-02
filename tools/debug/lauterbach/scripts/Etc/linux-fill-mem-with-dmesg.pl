#!/usr/bin/perl
#===============================================================================
#
#         FILE:  linux-fill-mem-with-dmesg.pl
#
#        USAGE:  ./linux-fill-mem-with-dmesg.pl <dmesg_file> <cmm_script>
#
#  DESCRIPTION:  Create a cmm script to fill memory from the data given in dmesg
#
#       AUTHOR:  Yann Gautier (YG), yann.gautier@stericsson.com
#      COMPANY:  ST-Ericsson
#      VERSION:  1.0
#      CREATED:  06/13/2012 04:54:14 PM
#===============================================================================

use strict;
use warnings;

use vars qw($input $output @split_line);
use vars qw($line $use $reg_address $offset $stack $stack_line);

$input=shift;
$output=shift;

$line = -1;
$use = 0;
$stack = 0;
$stack_line = 0;

open(IFILE, "<$input") or die "Cannot open $input";
open(OFILE, ">$output") or die "Cannot open $output";

while(<IFILE>) {
	next if ($_ =~ /^$/);
	next if ($_ =~ /\[[ 0-9]*\.[0-9]*\]$/);
	$use = 1 if ($_ =~ /\[[ 0-9]*\.[0-9]*\] PC:/);
	$use = 0 if ($_ =~ / from \[\<[0-9a-fA-F]{8}\>\]/);

	next if ($use == 0);
	next if ($_ =~ /\[[ 0-9]*\.[0-9]*\] Process /);

	$_ =~ s/^\<[0-9]\>//;
	$_ =~ s/^\[[ 0-9]*\.[0-9]*\]//;

	next if ($_ =~ /^$/);
	next if ($_ =~ /^ $/);
	next if ($_ =~ /^\r$/);
	next if ($_ =~ /^ \r$/);

	@split_line = split(" ",$_);

	if ($_ =~ /Stack: /) {
		$reg_address = $split_line[1];
		$reg_address =~ s/\(//;
		$line = 0;
		$stack = 1;
		$stack_line = 1;
		next;
	} elsif (($_ =~ /: /)&&($stack != 1)) {
		$reg_address = $split_line[1];
		$reg_address =~ s/://;
		$line = 0;
		next;
	}
	for ( my $idx = 0; $idx < 8; $idx++ ) {
		if ($stack_line > 1) {
			$offset=32*$line+4*$idx-16;
		} else {
			$offset=32*$line+4*$idx;
		}
		next if (($stack_line == 1)&&($idx > 3));
		print OFILE "data.set $reg_address+$offset. %long 0x$split_line[$idx+1]\n" if($split_line[$idx+1] =~ /[0-9a-fA-F]{8}/);
	}

	$stack_line++ if ($stack == 1);
	$line++;
}

close(OFILE);
close(IFILE);
exit 0;
