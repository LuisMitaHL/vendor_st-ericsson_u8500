#!/bin/env perl
# vim:syntax=perl
use strict;
use warnings;
use Colors;
use mmaudio;
use Getopt::Long;

# Command line options
my $verbose    = 0;
my $dryrun     = 0;
my $script     = mmaudio::getMmteDefaultScriptName();

GetOptions (
    'help'          => \&usage,
    'script=s'      => \$script,
    'dryrun'        => \$dryrun,
    'verbose'       => \$verbose
);

my $tag = $ENV{PLATFORM};
$tag = shift if scalar(@ARGV);

my $MMROOT = $ENV{MMROOT};

mmtest::OSplan( 1 );

mmtest::init($0,$tag);

mmaudio::generateMmteInitScript($script);
mmtest::system("$MMROOT/shared/mmte/bin/x86/mmte.exe &> x86_log.txt",0,"MMTE execution");
mmaudio::genRptFromOut();

mmtest::concl();

############################################################################
#    END
############################################################################
sub usage { 
    print "Usage: $0 [--script <filename>]\n";
    print "--script: script name \n";
    exit; 
}
