#!/bin/sh
eval 'exec perl -x $0 ${1+"$@"} ;'
  if 0;
#! -*- perl -*-
# vim:syntax=perl
#*****************************************************************************#
#
# Copyright (C) ST-Ericsson SA 2010. All rights reserved.
# This code is ST-Ericsson proprietary and confidential.
# Any use of the code for whatever purpose is subject to
# specific written permission of ST-Ericsson SA.
#
# \file    
# \brief   MMD Imaging report utility
# \author  ST-Ericsson
#
#*****************************************************************************#

# Add specific perl lib location
use strict;
use warnings;
use Colors;
use Getopt::Long;
use File::Basename;

use MMTEST::mmtest;

# Add path for executable
BEGIN {
    use Env qw(MMROOT);
}
# User specific libs

# My variables
my $f_help       = 0;
my $verbose      = 0;
my $Log_path     = "" ;
my $TestGroup    = "";
my $Tag          = "";

my $testname       = "";
my @test_descrip   = ();
my $test_assertion = "";
my $nb_tests = 0;
my %tab_assertion  = ();
my %tab_status     = ();
my %tab_details    = ();


# Filled with specfic options
my $ret = GetOptions (
    'verbose!'      => \$verbose,      
    'help!'         => \$f_help,
    'log=s'         => \$Log_path,
    'group=s'       => \$TestGroup,
    'tag=s'         => \$Tag,
    );

if (($f_help ) || (! $ret )) {
    displayHelp();
    exit(1);
}

#--------------------------------------------------------
# Check options
#--------------------------------------------------------
# MMROOT check
die "MMROOT is not set\n" unless $MMROOT ne "";

# TestGroup check
if (! $TestGroup ) {
    displayHelp();
    red "Error: Group is a mandatory parameter ";
    exit(2);
}

# Tag check
if (! $Tag ) {
    displayHelp();
    red "Error: Tag is a mandatory parameter ";
    exit(2);
}

# Log_path check
if (! $Log_path ) {
    displayHelp();
    red "Error: log path name is a mandatory parameter ";
    exit(2);
}
if (! -d $Log_path ) {
    red "Error: Unable to find $Log_path ";
    die;
}

#--------------------------------------------------------
# MAIN
#--------------------------------------------------------
# Report one single test, from one log dir content
clean_test_struct();

# Find testname, and declare test to report
$testname = get_testname($TestGroup, $Log_path);
add_entry_to_list ($TestGroup, $testname);

# List and read .out files 
my @list = glob("$Log_path/*.out");
foreach my $item ( @list ){
    if (( -f $item ) && ($item =~ /.*\.out$/)) {
	read_out_file ($item);
    }
}

# Create rpt for this directory
create_rpt();

exit 0;


#--------------------------------------------------------
# testname based on concatenation of sub_dirs name
#--------------------------------------------------------
sub get_testname {
    my $group = shift @_;
    my $sub_dir = shift @_;

    my $found = 0;
    foreach ( File::Spec->splitdir($sub_dir) ) {
	if ($_ eq $group) {
		$found=1;
		$testname = $_ ;
	}
	elsif ($found == 1) {
		$testname .= "_" .$_ ;
	}
    }

    blue "\tTEST NAME= $testname" if $verbose;
    return $testname;
}


#--------------------------------------------------------
# Add one entry in testlist 
#--------------------------------------------------------
sub add_entry_to_list {
    my $group = shift @_;
    my $tname = shift @_;

    open  TESTLIST , ">>$group.lst" or die "Unable to open $group.lst : $!";
    print TESTLIST "$tname\n"; 
    close TESTLIST;
}


#--------------------------------------------------------
# Read one file and fill table with datas
#--------------------------------------------------------
sub read_out_file {
    my $log_filename = shift @_;
    blue "...file to parse: $log_filename" ;

    open (LOG, "$log_filename")
        or die "Unable to open " . $log_filename . " : $!\n";
    my $lineNb = 0;
    my $tmp_str = "";
    my $tmp_status = "";

    # Get options from log file - dependant of the use case implementation
    while (my $line = <LOG>) {
        $lineNb ++;
        chomp($line);

        # Test Identification
        if($line =~ /^S:\w+:(\S+)/) {
	    $test_assertion = basename($1);
	    $nb_tests++ ;
	    $tab_assertion{$nb_tests} = $test_assertion;
	    &trace("\tASSERTION= $test_assertion") if $verbose;
        }
#        # Test Description
#        elsif($line =~ /^D:\w+:(.+)$/) { }
        # Test Detail
        elsif($line =~ /^N:\w+:\d+:(.+)$/) {
	    $tmp_str = $1;
	    $tmp_str =~ s/[\r\n]//;	# remove leading CR
        }
        # Detail Status
        elsif($line =~ /^R:\w+:\d+:(\S+):\w+/) {
	    $tmp_status = $1;
	    if (! ($tmp_status =~ m/PASSED/i )) {
		$tab_details{$nb_tests} .= $tmp_str ." - ". $tmp_status ."\n";
            }
	    $tmp_str = "";
        }
#        # Test Comment
#        elsif($line =~ /^C:\w+:(.+)$/) { }

        # Assertion Status
        elsif($line =~ /^E:\w+:(\S+):/) {
	    $tab_status{$nb_tests} = $1;
	    &trace("\t$test_assertion= $1") if $verbose;
	    $test_assertion = "";

        }
    }
    close LOG;
}


#--------------------------------------------------------
# Generate one test .rpt
#--------------------------------------------------------
sub create_rpt {
    # UNITARY TEST PLAN
    mmtest::OSplan( $nb_tests);
    mmtest::init($testname,$Tag);

    # Add property for reporting
    my $tmp_str = join ('<BR>', @test_descrip);
    if($tmp_str ne ""){
	mmtest::addProp("description", $tmp_str);
    }

    foreach my $ind ( keys %tab_assertion) {
	my $a_assert = $tab_assertion{$ind} ;
	my $a_res    = $tab_status{$ind} ;
	# Comment applied to following assertion for mmtest
	if (defined $tab_details{$ind}) {
            mmtest::comment($tab_details{$ind} );
	}
	mmtest::is($a_res, "PASSED", $a_assert);
    }

    mmtest::concl();
}

#--------------------------------------------------------
# Clean structure associated to .rpt
#--------------------------------------------------------
sub clean_test_struct {
    $testname      = "";
    @test_descrip  = ();
    %tab_assertion = ();
    %tab_status    = ();
    %tab_details   = ();

    $nb_tests =0 ;
}

#--------------------------------------------------------
sub displayHelp {
    print "Usage : $0 -tag <Tag> -group <Group> -log <Path> [-help] [-verbose]
    Create one single .rpt (usable by mmtest) from one dir with several .out files (issued from mmte) 
    <Path> is the directory (absolute) where are several .out resulting from mmte execution
    <Tag> is the tested version identification (sample: w912 or x.y.z)
    <Group> is the list where tests will be associated to
";
}
#--------------------------------------------------------

sub warning {
    my $lineNb = shift;
    my $comment = shift;
    magenta "L$lineNb : Warning: $comment";
}

sub trace {
    my $comment = shift;
    print "$comment\n";
}

