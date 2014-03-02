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
use File::Basename;
use File::Copy;

my $f_help       = 0;
my $verbose      = 0;
my $Log_path     = "";
my $Tag          = "NoTag" ;
my $Platform     = "";
my $GenTestlist  = 0;
my $Html_only    = 0;
my $list_dir     = "";
my $f_tag;
my $Primarycam;
my $Secondarycam;

use File::Basename;
my ($scriptname,$scriptdir,$scripttype) = fileparse($0,qr/\..*/);

# Constants
my $Analys_F     = "$scriptdir/Omx_report_analysis.txt";
my $str_Primary  = "PrimaryCam";
my $str_Secondary = "SecondaryCam";

my $ret = GetOptions (
    'help!'        => \$f_help,        
    'verbose!'     => \$verbose,
    'tag=s'        => \$f_tag, 
    'path=s'       => \$Log_path,
    'platform=s'   => \$Platform,
    'gentestlist'  => \$GenTestlist,
    'html'  	   => \$Html_only,
    'testlist=s'   => \$list_dir,
    'primarycam'   => \$Primarycam,
    'secondarycam' => \$Secondarycam,
);

if (($f_help ) || (! $ret )) {
    displayHelp();
    exit(0);
}


#--------------------------------------------------------
# Check options
#--------------------------------------------------------
# Tag check
if (! $f_tag ) {
    displayHelp();
    red "Error: Tag is a mandatory parameter ";
    exit(2);
}
$Tag = $f_tag;

# Platform value
if (! $Platform ) {
    $Platform = $ENV{'PLATFORM'};
}
blue "Note: considering Platform = $Platform ";

# Log path check
if (! $Log_path ) {
    displayHelp();
    red "Error: log path is a mandatory parameter ";
    exit(2);
}
if (! -d $Log_path ) {
    displayHelp();
    red "Error: log path '$Log_path' does not exist ";
    exit(2);
}

# Test list options check
if (($list_dir) && ($GenTestlist)) {
    displayHelp();
    red "Error: both options -testlist and -gentestlist are exclusive ";
    exit(2);
}
if ((! $list_dir) && (! $GenTestlist)) {
    $GenTestlist =1;
    blue "Note: As no <testlist> option is passed, default to '-gentestlist' ";
}
if (($list_dir) && (! -d $list_dir)) {
    red "Error: directory '$list_dir' for list files not found ";
    exit(2);
}


#--------------------------------------------------------
# MAIN
#--------------------------------------------------------
# Clean existing report and lists
my @list1;

if ( $Html_only==0 ) {
@list1 = glob("*.lst");
    foreach my $a_f (@list1) { 
    	if ( $a_f =~ /[\w]+[^\_].lst/ ){
	    print "remove: $a_f \n"    if $verbose;
	    unlink ($a_f);
    	}
    }

unlink <*.rpt>; 	# remove existing rpt
## unlink <*.dat>; 	# remove existing .dat

# Found existing analysis
$Analys_F = "" if (! -f $Analys_F);

#parse log (generate .rpt and testlist.lst) 
blue "Step 1/ Parse directories and logs";

my $dft_group;
if ($Primarycam)
    { $dft_group = "${str_Primary}_Basic" ; }
elsif ($Secondarycam)
    { $dft_group = "${str_Secondary}_Basic" ; }
else
    { $dft_group = "Basic" ; }

my @loglists = glob("$Log_path/*");
foreach my $item ( @loglists ){
    if( ( -f $item ) && ($item =~ /.*\.out$/) ){
        blue "...file to report: $item";
        my $cmd = "perl $scriptdir/parse_ite_out.pl --log=$item ";
        $cmd .= " -tag $Tag" ;
        $cmd .= " -platform $Platform"  if $Platform;
        $cmd .= " -gentestlist"         if $GenTestlist;
        $cmd .= " -group $dft_group"    if $GenTestlist;
	$cmd .= " -analys $Analys_F"	if $Analys_F;
	# $cmd .= " -verbose"          if $verbose;
        
        my_system ($cmd);
    }
    if( -d $item ){
        my $dname = basename ($item);
        blue "...directory to group: $dname";
        my $cmd = "perl $scriptdir/parse_ite_group.pl --log=$item ";
        $cmd .= " -tag $Tag" ;
	# $cmd .= " -verbose";         #  if $verbose;
        
        my_system ($cmd);
    }
}
} ## ( $Html_only==0 ) 


# Retrieve all testlist.lst 
blue "Step 2/ Prepare report";
my @testlists = ();
# Either from command line
if ($list_dir){
    print "Get tests list from folder : $list_dir \n";
    @list1 = glob("$list_dir/*.lst");
    foreach my $a_f (@list1) {
	# When all tests missing, remove this group from global list
	my $find_a_result = 0;
	open (LIST, "$a_f");
	my @list_test = <LIST>;
	close LIST;	
	foreach my $a_t (@list_test) {
	    chomp $a_t;
	    $a_t =~ s/\s*$//;	#leading space
	    my @list_tmp = glob($a_t."_".$Tag."*.rpt");
	    $find_a_result++ if (@list_tmp);
	}
	if ( $find_a_result ) {
	    print "considering: $a_f \n"    if $verbose;
	    my $b_f = basename ($a_f);
	    copy($a_f, $b_f);
            push (@testlists, $b_f);
	}
    }
}
# Either from newly created list
elsif ($GenTestlist){
    if ($Primarycam)
        { @list1 = glob("${str_Primary}*.lst"); }
    elsif ($Secondarycam)
        { @list1 = glob("${str_Secondary}*.lst"); }
    else
        { @list1 = glob("*.lst"); }
    foreach my $a_f (@list1) {
    	if ( $a_f =~ /[\w]+[^\_].lst/ ){
	    print "considering: $a_f \n"    if $verbose;
            push (@testlists, $a_f);
    	}
    }
}
# Default impossible case
if ( $#testlists < 0 ) {
    red "Missing list to report";
    die ;
}

# Create database report by Group
foreach my $file ( @testlists ){
    print "...compute result : $file \n"	if ($verbose);
    if (-r "$file" ) {
	my $db_file = $file;
	$db_file =~ s/.lst//;
	$db_file .= "_".$Tag.".dat";	# postfixe DB with tag

        # create test database
	my_system("testdbcreate --tag=$Tag --testlist $file --details --ignore=DSPTOOLS --ignore=datetime  --ignore=runtime --ignore=OS --ignore=host -o $db_file \n");

        # create html output # html extension automatically added
	#my_system("testdbcreate --tag=$Tag --html --testlist $file --details --ignore=DSPTOOLS --ignore=datetime  --ignore=runtime --ignore=OS --ignore=host --comments-after -o $db_file \n");
    }
    else{
        red "Error: filename '$file' not readable";
        die;    
    }
}


# Merge database and generate html
# ... Following is accurate when we need to merge results from several group
# ... tested on a same Release
blue "Step 3/ Generate global report";

#    for ( my $i=0; $i <= $#testlists; $i++ ){ 
    foreach my $file ( @testlists ){
        $file =~ s/\.lst//;	# Remove extension
	$file .= "_".$Tag.".dat";	# postfixe DB with tag
    }

    my $link_opt = "";
    if ( -f "overall_notes.txt") {
	$link_opt = "-k overall_notes.txt";
    }
    
    my $report_name = "test_report_$Tag";
    $report_name .= "_${str_Primary}"	if ($Primarycam);
    $report_name .= "_${str_Secondary}"	if ($Secondarycam);

    my @tmp = map ( { "-i ".$_ } @testlists);
    my_system ("testdbmerge @tmp -o $report_name --details  --html --ignore=DSPTOOLS --ignore=datetime --ignore=runtime --ignore=OS --ignore=host --comments-after $link_opt \n");

    # Generate Xls file
    my_system ("testdbmerge @tmp -o $report_name --details  --csv --ignore=DSPTOOLS --ignore=datetime --ignore=runtime --ignore=OS --ignore=host --comments-after $link_opt \n");



#-------------------------------------------------
#  Sub function
#-------------------------------------------------
sub displayHelp {
    blue "Usage:\t$0 -tag <Tag> -path <path_logfiles> [-gentestlist | -testlist <list_dir>] ";
    blue "\t [-platform <ptf_ID>] [-primarycam|secondarycam] [-help] [-verbose] ";
    print "\tAim is to create mmtest Db and reports for MMTE outputs found under <path_logfiles>
\t<path_logfiles> path to considered files
\t<Tag> is the tested version identification (sample: w912 or x.y.z)
\t<ptf_ID> is usual ndk20-think used into mmtools environment (default to \$PLATFORM value)
\tgentestlist: the test list is generated automatically from the found files.out
\t<list_dir> is a path to restricted lists of considered outputs for databases
\tgentestlist & testlist are exclusives (when none, -gentestlist is used)
\tprimarycam or secondarycam allows to product dedicated html per camera for NMF level tests (make sense only -gentestlist is used)
";
}

sub my_system {
	my $cmd = shift;
    blue "$cmd" if $verbose;
    system "$cmd";
}

