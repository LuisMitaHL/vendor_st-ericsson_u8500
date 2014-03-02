#!/usr/bin/perl
#===============================================================================
#
#         FILE:  linux-bug-fill-register.pl
#
#        USAGE:  ./linux-bug-fill-register.pl dmesg.txt dmesg.cmm
#
#  DESCRIPTION:
#
#       AUTHOR:  Y. Gautier (YGA), yann PT gautier AT stericsson PT com
#      COMPANY:  ST-Ericsson Le Mans
#      VERSION:  1.0
#      CREATED:  06/10/2010 02:48:55 PM
#===============================================================================

use strict;
use warnings;


#==============================================================================
# Global variables
#==============================================================================
use vars qw($opt_v $opt_h);
use vars qw($input $output @split_line $no_coredump);


#==============================================================================
# Main
#==============================================================================
$input=shift;
$output=shift;

if ($#ARGV == 0) {
	$no_coredump=shift;
} else {
	$no_coredump="";
}

open(IFILE, "<$input") or die "Cannot open $input";
open(OFILE, ">$output") or die "Cannot open $output";

if($input =~ /dmesg/) {
	while(<IFILE>) {
		next if(($_ !~ /pc : \[/) && ($_ !~ /sp : /) && ($_ !~ /r10: /) && ($_ !~ /r7 : /) && ($_ !~ /r3 : /) && ($_ !~ /DAC: /) && ($_ !~ /pid: /)  && ($_ !~ /Pid: /)&& ($_ !~ /CPU: /));
		chomp($_);
		$_ =~ s/^\<[0-9]\>//;
		$_ =~ s/^\[[ 0-9]*\.[0-9]*\]//;
		@split_line = split(" ",$_);
		if($_ =~ /pc : \[/) {
			$split_line[2] =~ s/\[\<//;
			$split_line[2] =~ s/\>\]//;
			$split_line[5] =~ s/\[\<//;
			$split_line[5] =~ s/\>\]//;
			print OFILE "dialog.set cpsr \"0x$split_line[7]\"\n" if($split_line[7] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r15 \"0x$split_line[2]\"\n" if($split_line[2] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r14 \"0x$split_line[5]\"\n" if($split_line[5] =~ /[0-9a-fA-F]{8}/);
		}
		if($_ =~ /sp : /) {
			print OFILE "dialog.set r13 \"0x$split_line[2]\"\n" if($split_line[2] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r12 \"0x$split_line[5]\"\n" if($split_line[5] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r11 \"0x$split_line[8]\"\n" if($split_line[8] =~ /[0-9a-fA-F]{8}/);
		}
		if($_ =~ /r10: /) {
			print OFILE "dialog.set r10 \"0x$split_line[1]\"\n" if($split_line[1] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r9 \"0x$split_line[4]\"\n" if($split_line[4] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r8 \"0x$split_line[7]\"\n" if($split_line[7] =~ /[0-9a-fA-F]{8}/);
		}
		if($_ =~ /r7 : /) {
			print OFILE "dialog.set r7 \"0x$split_line[2]\"\n" if($split_line[2] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r6 \"0x$split_line[5]\"\n" if($split_line[5] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r5 \"0x$split_line[8]\"\n" if($split_line[8] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r4 \"0x$split_line[11]\"\n" if($split_line[11] =~ /[0-9a-fA-F]{8}/);
		}
		if($_ =~ /r3 : /) {
			print OFILE "dialog.set r3 \"0x$split_line[2]\"\n" if($split_line[2] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r2 \"0x$split_line[5]\"\n" if($split_line[5] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r1 \"0x$split_line[8]\"\n" if($split_line[8] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set r0 \"0x$split_line[11]\"\n" if($split_line[11] =~ /[0-9a-fA-F]{8}/);
		}
		if($_ =~ /DAC: /) {
			print OFILE "dialog.set ctrl \"0x$split_line[1]\"\n" if($split_line[1] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set table \"0x$split_line[3]\"\n" if($split_line[3] =~ /[0-9a-fA-F]{8}/);
			print OFILE "dialog.set dac \"0x$split_line[5]\"\n" if($split_line[5] =~ /[0-9a-fA-F]{8}/);
		}
		if($_ =~ /pid: /) {
			if ($no_coredump ne "nodump") {
				$split_line[3]  =~ s/,//;
				print OFILE "&space_id=\"$split_line[3].\"\n" if($split_line[3] =~ /[0-9]*/);
			} else {
				print OFILE "&space_id=\"0.\"\n";
			}
		}
		if($_ =~ /Pid: /) {
			if ($no_coredump ne "nodump") {
				$split_line[1]  =~ s/,//;
				print OFILE "&space_id=\"$split_line[1].\"\n" if($split_line[1] =~ /[0-9]*/);
			} else {
				print OFILE "&space_id=\"0.\"\n";
			}
		}
		if($_ =~ /CPU: [0-9]/) {
			$split_line[3]  =~ s/,//;
			print OFILE "&cpu_core=\"$split_line[1].\"\n";
		}
	}
} elsif($input =~ /logcat/) {
	my $offset=0;
	while(<IFILE>) {
		next if(($_ !~ /DEBUG/));
		next if(($_ !~ / sp /) && ($_ !~ / fp /) && ($_ !~ / r7 /) && ($_ !~ / r3 /) && ($_ !~ /DEBUG : pid: /));
		chomp($_);
		$_ =~ s/\|$//;
		$offset=5 if($_ =~ /\|/);
		@split_line = split(" ",$_);
		if($_ =~ / sp /) {
			print OFILE "dialog.set r12 \"0x$split_line[6+$offset]\"\n";
			print OFILE "dialog.set r13 \"0x$split_line[8+$offset]\"\n";
			print OFILE "dialog.set r14 \"0x$split_line[10+$offset]\"\n";
			print OFILE "dialog.set r15 \"0x$split_line[12+$offset]\"\n";
			print OFILE "dialog.set cpsr \"0x$split_line[14+$offset]\"\n";
		}
		if($_ =~ / r9 /) {
			print OFILE "dialog.set r8 \"0x$split_line[6+$offset]\"\n";
			print OFILE "dialog.set r9 \"0x$split_line[8+$offset]\"\n";
			print OFILE "dialog.set r10 \"0x$split_line[10+$offset]\"\n";
			print OFILE "dialog.set r11 \"0x$split_line[12+$offset]\"\n";
		}
		if($_ =~ / r7 /) {
			print OFILE "dialog.set r4 \"0x$split_line[6+$offset]\"\n";
			print OFILE "dialog.set r5 \"0x$split_line[8+$offset]\"\n";
			print OFILE "dialog.set r6 \"0x$split_line[10+$offset]\"\n";
			print OFILE "dialog.set r7 \"0x$split_line[12+$offset]\"\n";
		}
		if($_ =~ / r3 /) {
			print OFILE "dialog.set r0 \"0x$split_line[6+$offset]\"\n";
			print OFILE "dialog.set r1 \"0x$split_line[8+$offset]\"\n";
			print OFILE "dialog.set r2 \"0x$split_line[10+$offset]\"\n";
			print OFILE "dialog.set r3 \"0x$split_line[12+$offset]\"\n";
		}
		if($_ =~ /DEBUG : pid: /) {
			$split_line[6+$offset]  =~ s/,//;
			print OFILE "&space_id=\"$split_line[6+$offset].\"\n";
			print OFILE "&tid=\"$split_line[8+$offset].\"\n";
			$split_line[10+$offset]  =~ s/.*\///;
			my $trunc_proc_name = substr($split_line[10+$offset], -15, 15);
			print OFILE "&process_name=\"$trunc_proc_name\"\n";
		}
	}
}

close(OFILE);
close(IFILE);
exit 0;

