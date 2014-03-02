#!/usr/bin/perl -w
######################### -*- Mode: Perl -*- ########################
## Copyright (C) 2010, ST-Ericsson
## 
## File name       : msc.pl 
## Description     : A trace / msc generation tool 
## 
## Author          : Steve Critchlow <steve.critchlow@stericsson.com> 
## 
###############################################################################

use strict;


my %programSymbols = ();
my %enterLeaveMatches = ();

sub getSymbolsFromProgram;
sub annotateLogFile;

if ( $#ARGV != 1 ) {
    print STDERR "Usage:\n";
    exit 1;
}
if ( ! -x $ARGV[0] ) {
    print STDERR "Arg1 should be the executable\n";
}

getSymbolsFromProgram $ARGV[0];
annotateLogFile $ARGV[1];

for my $i ( keys %enterLeaveMatches ) {
    if ( $enterLeaveMatches{$i} != 0 ) {
        print "Address is mis-matched $i\n";
    }
}

# @:Enter:simd:(NoThread):1616322:08049fc1:0804a2c4
# 0:1    :2   :3         :4      :5       :6
sub annotateLogFile {
    my $f = shift;
    local *FH;
    my $count = 0;

    open FH,$f or die "Unable to open file $f:$!";
    while ( <FH> ) {
        chomp;
        my @fields = split /:/;
        if ( $#fields >= 4 && $fields[0] eq "@" ) {
            my $prog = $fields[2];
            my $addr = $fields[5];
            if ( $fields[1] eq "Leave" ) {
                $count--;
                $enterLeaveMatches{$addr}--;
            }
            my $pad = " " x $count;
            if ( exists $programSymbols{$prog} && exists $programSymbols{$prog}{$addr} ) {
                print "$pad$_:$programSymbols{$prog}{$addr}\n";
            }
            if ( $fields[1] eq "Enter" ) {
                $count++;
                $enterLeaveMatches{$addr}++;
            }
        } else {
            print STDERR "Bad line:$_\n";
        }
    }
    close FH;
}

sub getSymbolsFromProgram {
    my $f = shift;
    my $cmd = "nm --defined-only $f |";
    local *FH;

    (my $prog = $f) =~ s/.*\///;    # trim leading paths
    open FH,$cmd or die "Unable to $cmd:$!";
    while ( <FH> ) {
        chomp;
        my @fields = split;
        if ( $fields[1] =~ /[Tt]/ ) {
            $programSymbols{$prog}{$fields[0]} = $fields[2];
#            print "$prog:$fields[0]:$fields[2]\n";
        }
    }
    close FH;
}
