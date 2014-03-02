#!/usr/local/bin/perl

use strict;
use warnings;
#use diagnostics; # enable this if you have a perl error you don't understand to get more information about it

use Getopt::Long;

my $opt_debug = 0;
my $opt_help = 0;
my $opt_diff = "adr"; # "adr" for added (into file2), difference, removed (from file2)
my $opt_verbose = 0;

GetOptions(
	"debug" => \$opt_debug,
	"help" => \$opt_help,
	"opt=s" => \$opt_diff,
	"verbose" => \$opt_verbose
);

sub exec_diff;

#################
# Main program
#################

sub help {
    print "Purpose: compare two DTH RF parameter files.\n";
    print "\tsyntax: dthrfdiff.pl file1 file2 -opt=adr -verbose\n";
    print "\t-opt\tsets reporting options. default is -mode=adr.\n";
    print "\t\t'd' prints parameters which are present in both files but having different definitions.\n";
    print "\t\t'a' reports parameters found in file1 but not in file2\n";
    print "\t\t'r' reports parameters found in file2 but not in file1\n";
    print "\t'-verbose' prints much more information.\n";
}

if ( $opt_help )
{
    help();
}
else
{
    if ( @ARGV == 2 )
    {
        {
            my @files = ( $ARGV[0], $ARGV[1] );
            
            if ( $opt_verbose ) {
                print "running diff for the following RF parameter files: ";
                my $fnum = 1;
                foreach (@files)
                {
                    print "[$fnum]: $_ ";
                    $fnum ++;
                }
                print "\n\n";
            }
                        
            exec_diff(\@files, 0);
            exec_diff(\@files, 1);
        }
    }
    else
    {
        print "Try -help to get help\n";
    }
}

sub seek_param {
    my ($p, $spec, $file, $ref) = @_;
    my $lc = 1;
    
    open my $in, '<', $file or die "can't read file $file: $!";
    my $found = 0;
    my $ln = <$in>;
    while ($ln and !$found)
    {
        chomp $ln;
        if ( ($lc >2) and ($ln =~ /,$p,/) )
        {
            #print "$p found line $lc\n";
            $found = 1;
        }
        else
        {
            $lc++;
            $ln = <$in>;
        }
    }
    
    if ( !$found )
    {
        # miss in file
        #print "$p $file $ref\n";
        if ( ($opt_diff=~/r/ and $ref==2) or ($opt_diff=~/a/ and $ref==1) ) {
            print "only in [$ref]: $p\n";
        }
    }
    elsif ( ($ref == 1) and !($ln eq $spec) )
    {
        # diff in records
        if ( $opt_diff =~ /d/ ) {
            print "difference in $p";
            if ( $opt_verbose ) {
                print ":\n1=$spec\n2=$ln\n";
            } else {
                print "\n";
            }
        }
    }
    
    close $in;
}

#################
# Sub program exec_diff
# Purpose: run diff of two dthrf_param_element.csv files
#################

sub exec_diff {
    my ($f, $num) = @_;    
    my $lc = 0;
    
    my $dual = 1;
    if ( $num == 1 ) { $dual = 0; }
    
    open (INPUT, @$f[$num]) or die "cannot open file @$f[$num]";
    while (<INPUT>)
    {
        chomp;
        if ( ($lc>2) and !($_=~ /^#/) and (length $_) )
        {
            $_ =~ /\w*,(\w*),/;
            if ( length $1 )
            {
                seek_param $1, $_, @$f[$dual], $num+1;
            }
            # else found an empty record which can occur if exporting from excel for example
        }
        $lc++;
    }
    close(INPUT);
}

