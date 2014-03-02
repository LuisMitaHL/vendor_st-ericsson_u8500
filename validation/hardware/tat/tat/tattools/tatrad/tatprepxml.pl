#!/usr/local/bin/perl
# this program transforms an output XML file produced by dth9pserver -a to the corresponding input file.
# it replace the user_data="number" by their user_data="symbol:number" the best it can by parsing the TAT module sources files

use strict;
use warnings;
#use diagnostics; # enable this if you have a perl error you don't understand to get more information about it

use Getopt::Long;

my $opt_debug = 0;
my $opt_help = 0;
my $opt_verbose = 0;
my $opt_xml;
my $opt_enum;

GetOptions(
	"debug" => \$opt_debug,
	"help" => \$opt_help,
	"verbose" => \$opt_verbose
);

# program takes following arguments:
# input header enum output

sub help {
    print "Purpose: this program transforms an output XML file produced by dth9pserver -a to the corresponding input file.\n";
    print "\tsyntax: input header enum output -debug -help -verbose\n";
    print "\tinput\tXML input file to process.\n";
    print "\theader\tC header file where the DTH elements enumeration is located.\n";
    print "\tenum\tname of the C enum in header.\n";
    print "\toutput\tresult file to produce.\n";
    print "\t'-help' prints program help.\n";
    print "\t'-debug' prints debug information.\n";
    print "\t'-verbose' prints more information.\n";
}

# prints verbose information if enabled
sub verb {
    if ( $opt_verbose ) {
        print @_;
    }
}

# prints debug string if enabled
sub dbg {
    if ( $opt_debug ) {
        print @_;
    }
}

sub translate {
    my ($ln, $enums) = @_;
    
    $ln =~ /user_data="(\d*)"/;
    my $repl = $1;
    if ( exists $enums->{$1} )  
    {
        dbg "processing user_data=$repl\n";
        $ln =~ s/user_data="(\d*)"/user_data="$enums->{$1}"/;
    }
    else
    {
        print "warning: user_data=\"$repl\" cannot be identified.\n";
    }
    
    return $ln;
}

sub load_enum {
    my ($file, $enum, $enums) = @_;
    
    my $pos = 0; # 0=before enum, 1=into enum, 2=after enum
    my $num = 0; # number in enum
    
    while (<$file>)
    {   
        if ($pos == 1)
        {
            if ( $_ =~ /};/ )
            {
                # end of enum
                $pos = 2; # after enum
            }
            elsif ( $_ =~ /^\s*([A-Za-z]\w*)/ )
            {
                $enums->{$num} = $1;
                verb "$1 = $num\n";
                $num ++;
            }
        }
        elsif (($pos == 0) and ($_ =~ /enum\s+$enum\s*{/))
        {
            $pos = 1;
        }
    }
    
    if ( $opt_debug )
    {
        while ( my ($k, $v) = each (%$enums))
        {
            dbg "$v = $k\n";
        }
    }    
}

sub load_define {
}

# IMPORTANT: sub load_dth_struct is not complete and bugged
sub load_dth_struct {
    my ($file, $name, $struct) = @_;
    
    # open define file and load symbols
    open my $f, '<', $file or die "can't read file $file: $!";
    my $lc = 1;

    my $pos = 0; # 0=before struct, 1=into struct, 2=after struct
    while (<$f>)
    {   
        if ($pos == 1)
        {
            #print "$_\n";
            my @tk = split /\s+/, $_;
            my $index = $#tk;

            if ( ($index >= 0) and ($tk[$index] =~ /\s*};/ ) )
            {
                # end of struct
                print "structure $name ends at line $lc\n";
                $pos = 2; # after struct
            }
            else
            {
                # split by field
                
            }
        }
        # structure begins at struct dth_element $name [] = {
        elsif (($pos == 0) and ($_ =~ /struct dth_element\s+$name\s*\[\]\s*=\s*{/))
        {
            print "structure $name begins at line $lc\n";
            $pos = 1;
        }
        
        $lc++;
    }
    
    close $f;

    if ( $opt_debug )
    {
        while ( my ($k, $v) = each (%$struct))
        {
            dbg "$v = $k\n";
        }
    }    
}

#################
# Main program
#################

if ( $opt_help )
{
    help();
}
else
{
    if ( @ARGV == 4 )
    {
        my $input = $ARGV[0];
        my $header = $ARGV[1];
        my $enum = $ARGV[2];
        my $output = $ARGV[3];
        
        my %enums;
        
        # open define file and load symbols
        open my $defs, '<', $header or die "can't read file $header: $!";
        load_enum $defs, $enum, \%enums;
        
        # open input read only
        open my $in, '<', $input or die "can't read file $input: $!";
        
        # create output write only
        open my $out, '>', $output or die "can't create file $output: $!";
        
        # read input line by line
        my $ln = <$in>;
        while ($ln)
        {
            # replace occurrence of numbered user_data by their symbol counterpart
            if ( $ln =~ /user_data="\d+"/ )
            {
                $ln = translate $ln, \%enums;
            }

            print $out $ln;
            $ln = <$in>;
        }
        
        close $defs;
        close $in;
        close $out;
    }
    else
    {
        print "Try -help to get help\n";
    }
}

