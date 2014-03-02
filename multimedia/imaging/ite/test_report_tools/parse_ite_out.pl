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
my $Log_filename = "" ;
my $Tag          = "" ;
my $Platform     = "" ;
my $GenTestlist  = 0;
my $TestGroup    = "" ;
my $Analys_f     = "" ;

my $testname       = "";
my $teststatus     = "";
my @test_descrip   = ();
my $test_assertion = "";
my $nb_tests = 0;
my %tab_assertion  = ();
my %tab_status     = ();
my %tab_details    = ();
my @assertions = (); # empty assertion hash containing status, details and assertion


# Filled with specfic options
my $ret = GetOptions (
    'verbose!'      => \$verbose,      
    'help!'         => \$f_help,
    'log=s'         => \$Log_filename,
    'tag=s'         => \$Tag,
    'platform=s'    => \$Platform,
    'group=s'       => \$TestGroup,
    'gentestlist'   => \$GenTestlist,
    'analys=s'      => \$Analys_f
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

# Tag check
if (! $Tag ) {
    displayHelp();
    red "Error: Tag is a mandatory parameter ";
    exit(2);
}

# Logfile check
if (! $Log_filename ) {
    displayHelp();
    red "Error: log file name is a mandatory parameter ";
    exit(2);
}
if (! -f $Log_filename ) {
    red "Error: Unable to find $Log_filename ";
    die;
}

# Group check or set, used only when gentestlist
if (! $TestGroup ) {
    $TestGroup = $Tag ;
}
blue "\tTESTGROUP= $TestGroup" if $GenTestlist;

if ($Analys_f ne "") {
    if (-f $Analys_f) {
    	blue  "... Find analysis file : $Analys_f ";
    } else {
    	red  "Error: Unable to find $Analys_f ";
	#die;
    }
}

#--------------------------------------------------------
# MAIN
#--------------------------------------------------------
open (LOG, "$Log_filename")
    or die "Unable to open " . $Log_filename . " : $!\n";

# Get informations from log file - dependant on the use case implementation
my $lineNb = 0;
my $tmp_str;
my %current_assertion;
my $is_first = 1; # first assertion
while (my $line = <LOG>) {
    $lineNb ++;
    chomp $line;
    $line =~ s/\r//g;
    $line =~ s/\n//g;

    # Test Identification
    if($line =~ /^S:\w+:(\S+)/) {
	my $tempname = basename ($1);
	$tempname =~ s/_$Platform//  if(($Platform) && ($tempname =~ /$Platform/)) ;
	$testname = $tempname;
	$nb_tests =0 ;
	blue "\tTESTNAME= $testname" if $verbose;
    }
    # Test Description
    elsif($line =~ /^D:\w+:(.+)$/) {
	$tmp_str = $1;
	$tmp_str =~ s/"//g ;
	push(@test_descrip, $tmp_str);
	blue "\tTESTCOMMENT= $tmp_str" if $verbose;
    }
    # Test Assertion
    elsif($line =~ /^N:\w+:\d+:(.+)/) {
	$nb_tests++ ;
        # new test start, let's push previous in array
        if (!$is_first) {
            my %persistent_assertion = %current_assertion;
            push @assertions,\%persistent_assertion;
            delete $current_assertion{STATUS} ;
            delete $current_assertion{DETAILS} ;
        }else {
            print ("First occurence of N: in out file\n") if $verbose ;
            $is_first = 0;
        }
	$tab_assertion{$nb_tests} = $1;
        $current_assertion{ASSERTION} = $1;
	$test_assertion = $1;
	blue "\tASSERTION= $test_assertion" if $verbose;
    }
    # Result Assertion
    elsif($line =~ /^R:\w+:\d+:(\S+):\w+/) {
        $current_assertion{STATUS} = $1;
	$tab_status{$nb_tests} = $1;
	blue "\t$test_assertion= $1" if $verbose;
	$test_assertion = "";
	if ($current_assertion{STATUS} eq "SKIPPED") {
            $current_assertion{DETAILS} .= $current_assertion{STATUS} ;
	}
    }
    # Test Comment
    elsif($line =~ /^C:\w+:(.+)$/) {
	$tmp_str = $1;
	$tmp_str =~ s/"//g ;
	if ($tmp_str =~ /OMX_StateExecuting/i) {next;}

	if (defined $current_assertion{DETAILS} && $current_assertion{DETAILS} =~ /$tmp_str/i ) {next;} #prevent repetitions
	if ($nb_tests == 0) {
	    push(@test_descrip, $tmp_str);
	    blue "\tTESTCOMMENT= $tmp_str" if $verbose;
	} else {
            $current_assertion{DETAILS} .= $tmp_str ."\n";
	    $tab_details{$nb_tests} .= $tmp_str ."\n";
	    blue "\t$test_assertion >> $tmp_str" if $verbose;
	}
    }

    # Test Status
    elsif($line =~ /^E:\w+:(\S+):/) {
        my $sanction = $1;  
        if($teststatus ne ""){
            warning ("TEST STATUS already set");
        }        
        $teststatus = $sanction;
        blue "\tTest status = $sanction" if $verbose;

        my %persistent_assertion = %current_assertion;
        push @assertions,\%persistent_assertion ;
        report($TestGroup,$testname,$GenTestlist);
        clean_test_struct ();
    }
}
close LOG;

# In case of test not ending (testname not reset), report fail test
if ($testname) {
    red "!!! Test status = Not ended" ;
    $nb_tests++ ;
    $teststatus       = "UNCOMPLETE";

    my %persistent_assertion = %current_assertion;
    push @assertions,\%persistent_assertion ;
    report($TestGroup,$testname,$GenTestlist);
    clean_test_struct ();
}


#--------------------------------------------------------
# Add one entry in testlist 
#--------------------------------------------------------
sub add_entry_to_list {
    my $group = shift @_;
    my $tname = shift @_;

    open  TESTLIST , ">>$group.lst" or die "Unable to open $group.lst : $!";
    print TESTLIST "$testname\n"; 
    close TESTLIST;
}


#--------------------------------------------------------
# Look for analysis
#--------------------------------------------------------
sub search_analysis {
    my $tname = shift @_;
    my $cause = "";
    my @tmp_str = "";

    if (-f $Analys_f) {
	@tmp_str = `grep ^$tname: $Analys_f `;
    }
    foreach (@tmp_str) {
	chomp($_);
	$_ =~ s/$tname:\s*// ;
	$_ =~ s/\r$// ;
    }
    $cause = join('; ', @tmp_str) ;
    #print ">>> Analyse cause: $cause \n";
    return $cause;
}

#--------------------------------------------------------
# Generate one test .rpt
#--------------------------------------------------------
sub report {
    # UNITARY TEST PLAN
    my $TestGroup = shift;
    my $testname = shift;
    my $shouldGenTestList = shift;
	add_entry_to_list ($TestGroup, $testname) if ($shouldGenTestList);
    mmtest::OSplan( $nb_tests);
    mmtest::init($testname,$Tag); # report file name (.rpt) depends on the argument for that method

    # Add property for reporting
    if($teststatus ne "PASSED"){
	my $str = search_analysis($testname);
	if($teststatus eq "UNCOMPLETE"){
            $str .= "; " if($str ne "") ;
            $str .= "Some tests not executed";
        }
        mmtest::addProp("Analysis", $str);
    }
    my $tmp_str = join ('<BR>', @test_descrip);
    if($tmp_str ne ""){
	mmtest::addProp("description", $tmp_str);
    }
#    my $fname = basename ($Log_filename);
#    mmtest::addLink($fname, "Out file");



    if(0){ # "old way of managing assertion through 3 maps.
        foreach my $ind (sort keys %tab_assertion) {
            my $a_assert = $tab_assertion{$ind} ;
            my $a_res    = $tab_status{$ind} ;
            if (defined $tab_details{$ind}) {
                mmtest::comment($tab_details{$ind} );
            }
            mmtest::is($a_res, "PASSED", $a_assert);
        }
    }else { # 1 assertion = 1 map, all pushed in an array

        foreach my $assertion_ref (@assertions) {
            my %assertion = %$assertion_ref;
            mmtest::comment($assertion{DETAILS}) if $assertion{DETAILS};
            blue "DETAILS : $assertion{DETAILS}" if $verbose and  $assertion{DETAILS};
            blue "ASSERTION : $assertion{ASSERTION}"    if $verbose;
            blue "STATUS : $assertion{STATUS}"          if $verbose and  $assertion{STATUS};
	    #PASSED or SKIPPED -> green report
            mmtest::like($assertion{STATUS},"/(PASS|SKIPP)ED/",$assertion{ASSERTION});
        }
    }
    mmtest::concl();
}

#--------------------------------------------------------
# Clean structure associated to .rpt
#--------------------------------------------------------
sub clean_test_struct {
    $testname      = "";
    $teststatus    = "";
    @test_descrip  = ();
    %tab_assertion = ();
    %tab_status    = ();
    %tab_details   = ();
}

#--------------------------------------------------------
sub displayHelp {
    print "Usage : $0 -log <file.out> -tag <Tag> [-group <Group>] [-genTestList] [-platform <ptf_ID>] 
    			[-analys <File>] [-help] [-verbose]
    Purpose is to transform mmte .out file into .rpt file usable by mmtest
    <file.out> is the one resulting from mmte test execution
    <Tag> is the tested version identification (sample: w912 or x.y.z)
    <ptf_ID> is usual ndk20-think used into mmtools environment (no default)
    <Group> is the list where tests will be associated to, used if gentestlist only (default to <Tag>)
    genTestList: allows to add this result to this group (= tag) test list
    <Analys file>: should contain previous analysis for failure tests
";
}

sub warning {
    my $comment = shift;
    magenta "L$lineNb : Warning: $comment";
}

