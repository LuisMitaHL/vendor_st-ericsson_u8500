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

if($ENV{PLATFORM} =~ /svp/ ){
    mmaudio::launchMmteOnSVP($script);
    mmaudio::genRptFromOut();
}
else{
    mmaudio::launchMmteOnBoard($script);
}


############################################################################
#    END
############################################################################
sub usage { 
    print "Usage: $0 [--scipt <filename>]\n";
    print "--script: script name \n";
    exit; 
}

