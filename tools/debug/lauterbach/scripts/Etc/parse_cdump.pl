#!/usr/bin/perl
#===============================================================================
#
#         FILE:  parse_cdump.pl
#
#        USAGE:  ./parse_cdump.pl <coredump_file> <script_file>
#
#  DESCRIPTION:  Splits coredump files into binary files (one for each memory
#                area
#                And create a cmm file to load those binaries
#
#       AUTHOR:  Yann Gautier (YG), yann.gautier@stericsson.com
#      COMPANY:  ST-Ericsson
#      VERSION:  1.0
#      CREATED:  02/02/2012 03:22:10 PM
#===============================================================================

use strict;
use warnings;


use Getopt::Std;

#==============================================================================
# Global variables
#==============================================================================
use vars qw($opt_v $opt_h);
use vars qw($IFIL $dot $elf_type $bytes);
use vars qw($e_type $e_machine $e_version $e_entry $e_phoff $e_shoff $e_flags);
use vars qw($e_ehsize $e_phentsize $e_phnum $e_shentsize $e_shnum $e_shstrndx);
use vars qw(@p_type @p_offset @p_vaddr @p_paddr @p_filesz @p_memsz @p_flags @p_align);

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

my $coredumpFile = $ARGV[0];

open($IFIL, "<$coredumpFile") || die("Can't open $coredumpFile");
binmode $IFIL;
my $ret = read $IFIL, $bytes, 4;
if ((!defined $ret) || ($ret != 4)) {
	close $IFIL;
	die "ERROR: RAMDUMP File read error\n";
}

# Reads 4 first bytes, should be ".ELF"
($dot, $elf_type) = unpack "Ca3", $bytes;
seek($IFIL, 16, 0);
$ret = read $IFIL, $bytes, 40;

# Split header into each of its fields
($e_type, $e_machine, $e_version, $e_entry, $e_phoff, $e_shoff, $e_flags,
	$e_ehsize, $e_phentsize, $e_phnum, $e_shentsize, $e_shnum, $e_shstrndx) = unpack "SSLLLLLSSSSSS", $bytes;

unless ($dot == 0x7F && $elf_type eq 'ELF' && $e_machine == 0x28)
  { die "not an ARM ELF file\n" }

# Reads each section parameters
seek($IFIL, 52, 0);
for ( my $i=0; $i < $e_phnum; $i++ ) {
	$ret = read $IFIL, $bytes, 32;
	($p_type[$i], $p_offset[$i], $p_vaddr[$i], $p_paddr[$i], $p_filesz[$i], $p_memsz[$i], $p_flags[$i], $p_align[$i]) = unpack "L8", $bytes;
	seek($IFIL, 52+32*($i+1), 0);
}

# Create cmm script file, which will load the binary files
my $script_file = $ARGV[1];
open(my $SFIL, ">$script_file") || die("Can't open $script_file");
for ( my $i=0; $i < $e_phnum; $i++ ) {
	next if $p_type[$i] != 1;
	seek ($IFIL, $p_offset[$i], 0);
	$ret = read $IFIL, $bytes, $p_filesz[$i];
	open(my $OFIL, ">$coredumpFile.$i") || die("Can't open $coredumpFile.$i");
	binmode $OFIL;
	print $OFIL $bytes;
	close $OFIL;
	print $SFIL "data.load.binary $coredumpFile.$i a:$p_paddr[$i]. /NOSYMBOL /NOCLEAR /NOREG\n";
}
close $SFIL;
close $IFIL;

exit 0;

