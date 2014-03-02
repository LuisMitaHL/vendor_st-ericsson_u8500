#!/usr/bin/perl
#===============================================================================
#
#         FILE:  mod_elf_header_ddr1.pl
#
#        USAGE:  ./mod_elf_header_ddr1.pl <file.elf>
#
#  DESCRIPTION:  Decrease the section number by 1 in the elf header file
#                This avoids the loading of DDR1 which can cause MMU conflicts
#
#       AUTHOR:  Yann Gautier (YG), yann.gautier@stericsson.com
#      COMPANY:  ST-Ericsson
#      VERSION:  1.0
#      CREATED:  02/23/2012 04:44:42 PM
#===============================================================================

use strict;
use warnings;


use Getopt::Std;

#==============================================================================
# Global variables
#==============================================================================
use vars qw($opt_v $opt_h);

#==============================================================================
# Sub-routines
#==============================================================================

## Debug print
sub dprint
{
	print STDERR @_ if ($opt_v);
}

## system avec print
sub dsystem
{
	print "@_\n" if ($opt_v);
	return (system @_) / 256;
}


#==============================================================================
# Main
#==============================================================================
# Input parameters management
if ( !Getopt::Std::getopts('vh') || $#ARGV < 0 || defined($opt_h))
{
    die("Usage: $0 [-v]erbose file\n");
}

my $elf_file=shift;

open  my $IFILE, '+<', $elf_file	or die  "$0 : failed to open  input file '$elf_file' : $!\n";
binmode $IFILE;

my $byte;
my $new_byte;
my $new_byte2;

seek($IFILE, 44, 0);
read($IFILE, $byte, 1);
$new_byte = unpack("C", $byte) - 1;
$new_byte2 = pack("C", $new_byte);
seek($IFILE, 44, 0);
print $IFILE $new_byte2;
close  $IFILE or warn "$0 : failed to close input file '$elf_file' : $!\n";


exit 0;

