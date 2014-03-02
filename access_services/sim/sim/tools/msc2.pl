#!/usr/bin/perl -w
######################### -*- Mode: Perl -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : msc2.pl 
## Description     : A trace / msc generation tool 
## 
## Author          : Steve Critchlow <steve.critchlow@stericsson.com> 
## 
###############################################################################

use strict;

# A trace / msc generation tool.

my %programSymbols = ();        # a cache of program symbols
my %enterLeaveMatches = ();     # for tracking mis-matched enter/leave calls
my %enterLeaveMatchesCount = ();

sub annotateLogFile;
sub lookup;

if ( $#ARGV != 1 ) {
    print STDERR "Usage:\n";
    exit 1;
}
if ( ! -x $ARGV[0] ) {
    print STDERR "Arg1 should be the executable\n";
}

annotateLogFile $ARGV[0], $ARGV[1];

for my $i ( keys %enterLeaveMatches ) {
    print STDERR "Address is mis-matched $enterLeaveMatches{$i} ($enterLeaveMatchesCount{$i})\n";
}

# !:state:process:thread:mSecTime:this_fn:call_site
# @:Enter:simd:(NoThread):1616322:08049fc1:0804a2c4
# 0:1    :2   :3         :4      :5       :6
sub annotateLogFile {
    my ($e,$f) = @_;
    local *FH;
    local *OF;
    my $count = 0;
    ( my $logfile = $f ) =~ s/_func_ee//;

    open FH,$f or die "Unable to open file $f:$!";
    open OF,">$logfile" or die "Unable to open $logfile:$!";

    while ( <FH> ) {
        chomp;
        my @fields = split /:/;
        if ( $#fields >= 4 && $fields[0] eq "@" ) {
            my $this_fn = $fields[5];
            my $call_site = $fields[6];
            my $threadName  = $fields[3];

            if ( ! exists $programSymbols{$call_site} ) {
                $programSymbols{$call_site} = lookup $e,$call_site;
            }
            if ( ! exists $programSymbols{$this_fn} ) {
                $programSymbols{$this_fn} = lookup $e,$this_fn;
            }
            my $note = "$programSymbols{$call_site} -> $programSymbols{$this_fn}";

            if ( $fields[1] eq "Leave" ) {
                $count--;
                delete $enterLeaveMatches{"$threadName:$call_site"};
                $enterLeaveMatchesCount{"$threadName:$call_site"}--;
            }
            my $pad = " " x $count;
            print OF "$pad$_:$note\n";
            if ( $fields[1] eq "Enter" ) {
                $count++;
                $enterLeaveMatches{"$threadName:$call_site"} = "$.:$_:$note";
                $enterLeaveMatchesCount{"$threadName:$call_site"}++;
            }
        } elsif ( $fields[0] eq "!" ) {
            print OF "$_:file:line:function calls(->) file:line:function\n";
        } else {
            print STDERR "Bad line:$_\n";
        }
    }

    close OF;
    close FH;
}

# The addr2line utility takes an executable file and an address and returns file:line:function
# information.
sub lookup {
    my ($e,$a) = @_;
    my $cmd = "addr2line --exe=$e -s -f $a |";
    local *FH;
    open FH,$cmd or die "Unable to $cmd:$!";
    my $func = <FH>; chomp $func;
    my $fileLine = <FH>; chomp $fileLine;
    close FH;
    return "$fileLine:$func";
}
