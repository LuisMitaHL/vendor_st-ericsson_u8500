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

# Add path for executable
BEGIN {
    use Env qw(MMROOT);
}
# User specific libs

# My variables
my $f_help       = 0;
my $verbose      = 0;
my $Log_pathname = "" ;
my $Tag          = "" ;


# Filled with specfic options
my $ret = GetOptions (
    'verbose!'      => \$verbose,      
    'help!'         => \$f_help,
    'log=s'         => \$Log_pathname,
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

# Tag check
if (! $Tag ) {
    displayHelp();
    red "Error: Tag is a mandatory parameter ";
    exit(2);
}

# Logfile check
if (! $Log_pathname ) {
    displayHelp();
    red "Error: log path name is a mandatory parameter ";
    exit(2);
}
if (! -d $Log_pathname ) {
    die "Unable to find $Log_pathname \n";
}

#--------------------------------------------------------
# MAIN
#--------------------------------------------------------
# Global group
my $TestGroup = get_groupname($Log_pathname);

# go through directory recursively
&go_into_dir ($Log_pathname);

exit 0;


#--------------------------------------------------------
# Read group Name
#--------------------------------------------------------
sub get_groupname {
    my $gdir = shift @_;
    my $group = basename ($gdir);
    blue "\tTEST GROUP= $group" if $verbose;

    # Initial clean of residual rpt
    unlink <*$group.lst>;
    unlink <*$group*.rpt>;

    return $group;
}


#--------------------------------------------------------
# Go into a log directory, recursive function
#--------------------------------------------------------
sub go_into_dir {
    my $sub_dir = shift @_;

    my @d_list = ();
    my @out_list = ();
    my $nb_out = 0;
    my $nb_dir = 0;
    my @list = glob("$sub_dir/*");

    # Separate dirs from files
    foreach my $item ( @list ){
        if (( -f $item ) && ($item =~ /.*\.out$/)) {
            $nb_out++ ;
	    push (@out_list, $item);
	}
        if( -d $item ) {
            $nb_dir++ ;
	    push (@d_list, $item);
	}
    }

    # When no sub-dirs under the group
    my $loc_dir = basename ($sub_dir);
    if (($loc_dir eq $TestGroup) && ($nb_dir == 0)) {
        foreach my $_f ( @out_list ){
                one_file_report($TestGroup, $_f);
	}
	return;
    }

    # Report from current directory if necessary
    if ($nb_out > 0) {
        one_dir_report($TestGroup, $sub_dir);
    }
    foreach my $_d ( @d_list ){
        &trace("...directory to go through: $_d") if $verbose;
        go_into_dir ($_d);
    }
}


#--------------------------------------------------------
# Report one single test, from one log dir content
#--------------------------------------------------------
sub one_dir_report {
    my $group = shift @_;
    my $sub_dir = shift @_;
    blue "...directory to report: $sub_dir" ;

    my $cmd = "perl read_one_rep.pl -log $sub_dir ";
        $cmd .= " -group $group" ;
        $cmd .= " -tag $Tag";
	# $cmd .= " -verbose"          if $verbose;
        
    my_system ($cmd);
}


#--------------------------------------------------------
# Report each .out as a single test
#--------------------------------------------------------
sub one_file_report {
    my $group = shift @_;
    my $item = shift @_;
    blue "...file to report: $item";

    # dissociate PrimaryCam & SecondaryCam groups
    my $file = basename ($item);
    my ($cam) = split('_', $file);
    if ($cam =~ /.*Cam$/ )
    	{ $cam .= "_";}
    else 
    	{ $cam ="";}

    my $cmd = "perl parse_ite_out.pl --log=$item ";
        $cmd .= " -tag $Tag" ;
        $cmd .= " -gentestlist" ;
        $cmd .= " -group $cam$group" ;
	# $cmd .= " -verbose"          if $verbose;

    my_system ($cmd);
}


#--------------------------------------------------------
# Usage
#--------------------------------------------------------
sub displayHelp {
    print "Usage : $0 -log <Path> -tag <Tag> [-help] [-verbose]
    Go through a test result major directory, considered as a group, 
    and parse the .out files from its sub-folders as as many tests
    <Path> is the major directory (absolute) under which sub-tree contains .out files
    <Tag> is the tested version identification (sample: w912 or x.y.z)
";
}

#--------------------------------------------------------
# Utilities
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

sub my_system {
	my $cmd = shift;
    blue "$cmd" if $verbose;
    system "$cmd";
}

