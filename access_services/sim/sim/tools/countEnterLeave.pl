#!/usr/bin/perl -w
######################### -*- Mode: Perl -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : countEnterLeave.pl 
## Description     : Helper script for logging function calls in threads 
## 
## Author          : Steve Critchlow <steve.critchlow@stericsson.com> 
## 
###############################################################################

use strict;

my %count;
my %threadNames;

# !:state:process:thread:mSecTime:this_fn:call_site
# @:Enter:simd:(NoThread):1616322:08049fc1:0804a2c4
# 0:1    :2   :3         :4      :5       :6
while ( <> ) {
    chomp;
    my @fields = split /:/;
    if ( $fields[0] eq "@" ) {
        $threadNames{$fields[3]} = 1;
        my $t = "$fields[1]:$fields[3]";
        $count{$t}++;
    }
}

for my $i ( sort keys %threadNames ) {
    my $e = $count{"Enter:$i"};
    my $l = $count{"Leave:$i"};
    my $diff = $e - $l;
    if ( $diff > 0 ) {
        my $c = $diff == 1 ? "call" : "calls";
        print "Thread $i has $diff more Enter than Leave $c\n";
    }
    elsif ( $diff < 0 ) {
        $diff = -$diff;
        my $c = $diff == 1 ? "call" : "calls";
        print "Thread $i has $diff less Enter than Leave $c\n";
    }
}
