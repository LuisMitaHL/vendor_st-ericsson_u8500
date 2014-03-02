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
use strict;
use warnings;
use Colors;
use Getopt::Long;

use File::Spec;
use File::Copy;
use File::Path;

my @List_group1 = ("lr_format", "hr_format");
my @List_group = ("still_framerate", "video_framerate", "still_buffer", "video_buffer");
my @List_path2 = ( "CIF", "CIFPLUS", "QCIF", "QCIFPLUS", "SUBQCIF", "SUBQCIFPLUS", 
		"QQVGA", "QVGA", "QVGAPLUS", "VGA", "VGAPLUS",
		);

my $f_help       = 0;
my $verbose      = 0;
my $Res_path     = "";

my $ret = GetOptions (
    'help!'        => \$f_help,        
    'verbose!'     => \$verbose,
    'path=s'       => \$Res_path,
);

if (($f_help ) || (! $ret )) {
    displayHelp();
    exit(0);
}


#--------------------------------------------------------
# Check options
#--------------------------------------------------------
# Log path check
if (! $Res_path ) {
    displayHelp();
    red "Error: Result global path is a mandatory parameter ";
    exit(2);
}

#--------------------------------------------------------
# MAIN
#--------------------------------------------------------
if (-d $Res_path ) {
    # rename existing directory
    my ($s, $mi, $h, $mday,$month,$year) = localtime();
    my $tmp_str = sprintf("%02d%02d%02d", $year, $month+1, $mday) ;
    my $new_resdir = $Res_path ."_". $tmp_str;

    red "Warning: rename existing result dir to '$new_resdir' ";
    move ($Res_path, $new_resdir);
}

blue "Create global directory '$Res_path' ";
mkpath ($Res_path);

my $d_name ;
foreach my $a_g (@List_group) { 
        blue "Create sub-tree '$a_g' ";
        foreach my $a_format (@List_path2) { 
	    $d_name = File::Spec->catfile($Res_path, $a_g, $a_format);
	    mkpath ($d_name);
        }
}
foreach my $a_g (@List_group1) { 
        blue "Create sub-tree '$a_g' ";
        $d_name = File::Spec->catfile($Res_path, $a_g);
	mkpath ($d_name);
       
}


#-------------------------------------------------
#  Sub function
#-------------------------------------------------
sub displayHelp {
    blue "Usage:\t$0 -path <path_result> [-help] [-verbose]";
    print "\tAim is to create result sub-tree structure
\t<path_result> root path, might be relativ
";
}

