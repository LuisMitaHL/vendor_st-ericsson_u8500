#!/usr/bin/env perl

use strict;
use warnings;

BEGIN {
    use File::Spec;
    use File::Basename;
    use FindBin;

    use lib File::Spec->catfile($FindBin::Bin => qw(lib));
}

use Cwd;
use Cwd 'abs_path';
use Getopt::Long;
use File::Basename;
use File::Find;
use Term::ANSIColor qw(:constants);
use File::Temp qw/ tempfile tempdir /;

$Term::ANSIColor::AUTORESET = 1;

################################################################################
# Options

my $help = 0;
my $verbose = 0;
my $rec_offset = 0;
my $tkdiff = 0;
my $usegdb = 0;
my $dumpall = 0;
my $dump_line_info = 0;

################################################################################

my $symbols_dir="$ENV{ANDROID_PRODUCT_OUT}/symbols";

GetOptions ('help|h'           => \$help,
	    'versbose|v'       => \$verbose,
	    'all|a'            => \$dumpall,
	    'tkdiff|t'         => \$tkdiff,
	    #'gdb|g'            => \$usegdb,
	    'rec-offset|o=i'   => \$rec_offset,
	    'dump-line-info|d' => \$dump_line_info,
    ) or help(1);

help(0) if($help);
help(1) if(scalar(@ARGV) != 1);

sub help {
    my $exitval = shift;

    print "Usage: ". basename($0) . " [OPTION]* mediaserver-memstatus.txt\n";
    print "\t-h, --help                 print this help\n";
    print "\t-a, --all                  dump all differences, not only the extra allocations\n";
    print "\t-v, --verbose              enable verbose mode\n";
    print "\t-o, --rec-offset <offset>  give the offset of the baseline, always baseline +1 is compared to baseline\n";
    print "\t-t, --tkdiff               launch a tkdiff of the selected entries raw data\n";
    print "\t-d, --dump-line-info       dump to STDOUT the difference between the compared dataset with the line information\n";
    exit($exitval);
}

if (system("bash -c ' ( source build/envsetup.sh && gettop ) > /dev/null 2>&1 '") != 0) {
    print "Error: ` source build/envsetup.sh && gettop` failed - ". basename($0) .
        " must be executed from top directory of Android environment\n";
    exit (1);
}

sub dump_call_stack_addr2line {
    my $ref = shift;
    my $entries = shift;

    my $pwd = `pwd`;
    chomp $pwd;

    foreach my $entry (@{$entries}) {
        my @info = split(/ /, $entry);
        if(defined $info[2]) {
            chomp $info[2];
            print " DEBUG: info line $info[1] $info[2]\n" if ($verbose);
            if(-e "$symbols_dir$info[2]") {
                my $infoline = `prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-addr2line -e $symbols_dir$info[2] $info[1]`;
                $infoline =~ s|$pwd/||;
                printf "  %s %-40s %s", $info[1], $info[2], $infoline;
            } else {
                printf "  %s %-40s : $symbols_dir$info[2] not found\n", $info[1], $info[2];
            }
        } else {
            # same address maybe due to wrong unwind, just print it
            printf "  %s %-40s\n", $info[1], "";
        }
    }
}

sub dump_call_stack_gdb {
    my $ref = shift;
    my $entries = shift;

    my $top=`bash -c 'source build/envsetup.sh > /dev/null 2>&1 && gettop'`;
    chomp $top;
    my $gdb="$top" . "/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-gdb";

    my ($GDBCMDS, $gdb_cmds) = tempfile();
    close ($GDBCMDS);

    foreach my $entry (@{$entries}) {
        my @info = split(/ /, $entry);

        open ($GDBCMDS, "> $gdb_cmds") or die "Failed to open $gdb_cmds for writing";
        print $GDBCMDS "info line *$info[1]\n";
        print $GDBCMDS "quit\n";
        close($GDBCMDS);

        print " DEBUG: info line $info[1] $info[2]\n" if ($verbose);

        if(defined $info[2]) {
            chomp $info[2];
            if(-e "$symbols_dir$info[2]") {
                my $gbd_call = "$gdb -q -x $gdb_cmds $symbols_dir$info[2] </dev/null 2>/dev/null";
                print " DEBUG: $gbd_call\n" if ($verbose);
                my $gdbinfoline = `$gbd_call`;
                $gdbinfoline =~ s,\n, ,g;

                if($gdbinfoline =~ /No line number information available for address/) {
                    printf "  %s %-40s\n", $info[1], $info[2];
                } else {
                    my @lineinfo = split(/ /, $gdbinfoline);
                    $lineinfo[3] =~ s,",,g; #"
                    printf "  %s %-40s %s:%s\n", $info[1], $info[2], $lineinfo[3], $lineinfo[1];
                }
            } else {
                printf "  %s %-40s : $symbols_dir$info[2] not found\n", $info[1], $info[2];
            }
        } else {
            # same address maybe due to wrong unwind, just print it
            printf "  %s %-40s\n", $info[1], "";
        }
    }
    unlink $gdb_cmds;
}

open(MYINPUTFILE, "< $ARGV[0]") or die "Failed to open $ARGV[0]";
my @lines = <MYINPUTFILE>;
close(MYINPUTFILE);

my @dataset;
my $datasetindex = -1;
my $nbdataset = 0;

foreach my $line (@lines) {
    if($line =~ /^ Allocation count/) {
        $nbdataset += 1;
        if($rec_offset == 0) {
            $datasetindex += 1;
        } else {
            $rec_offset -= 1;
        }
    }
    next if (!($line =~ /^size/));
    if(($datasetindex == 0) || ($datasetindex == 1)) {
        push @{$dataset[$datasetindex]} => $line;
    }
}

print "File as $nbdataset entries\n";

if($tkdiff) {
    my ($fh0, $tmpname0) = tempfile();
    my ($fh1, $tmpname1) = tempfile();

    print $fh0 @{$dataset[0]};
    print $fh1 @{$dataset[1]};

    close($fh0);
    close($fh1);

    system("(tkdiff $tmpname0 $tmpname1 ; rm $tmpname0 $tmpname1) &");
}

## We create hash table to detect the 2 case:
#  - new allocation: entry in set 1 not present in set 0
#  - extra allocation: entry in set 1 present in set 0 but with lower dup

if($dump_line_info) {
    my $rec0 = { };
    my $rec1 = { };

    foreach my $ref0 (@{$dataset[0]}) {
        # Isolate number of allocation
        my $dup = (split(/,/, $ref0))[1];
        $dup =~ s/dup//g;
        $dup =~ s/ //g;

        # Entry record is : size + backtrace without number of allocation
        $ref0 =~ s/, dup[^,]*//g;

        #print " DEBUG: dup=$dup for $ref0" if ($verbose);
        if ( defined $rec0->{$ref0} ) {
            # Accumulate duplicate entries - don't really know why bionic has double entries ??
            print "WARNING: accumulating existing entry with dup=$rec0->{$ref0}->{-dup} + new dup=$dup for $ref0\n" if ($verbose);
            $rec0->{$ref0}->{-dup} += $dup;
        } else {
            $rec0->{$ref0} = { -dup => $dup };
        }
    }

    foreach my $ref1 (@{$dataset[1]}) {
        # Isolate number of allocation
        my $dup = (split(/,/, $ref1))[1];
        $dup =~ s/dup//g;
        $dup =~ s/ //g;

        # Entry record is : size + backtrace without number of allocation
        $ref1 =~ s/, dup[^,]*//g;

        #print " DEBUG: dup=$dup for $ref0" if ($verbose);
        if ( defined $rec1->{$ref1} ) {
            # Accumulate duplicate entries - don't really know why bionic has double entries ??
            print "WARNING: accumulating existing entry with dup=$rec1->{$ref1}->{-dup} + new dup=$dup for $ref1\n" if ($verbose);
            $rec1->{$ref1}->{-dup} += $dup;
        } else {
            $rec1->{$ref1} = { -dup => $dup };
        }
    }

    foreach my $ref (keys %{$rec1}) {
        # dup computing
        my $dup_diff;
        my $dup = $rec1->{$ref}->{-dup};

        if(! defined $rec0->{$ref}) {
            $dup_diff = 0;
            print " DEBUG: New entry : $ref\n" if ($verbose);
        } else {
            next if ($dup == $rec0->{$ref}->{-dup});
            $dup_diff = $dup - $rec0->{$ref}->{-dup};
            print " DEBUG: Existing entry : $ref\n" if ($verbose);
        }

        my @entries = split(/,/, $ref);
        my $size_entry = shift(@entries);
        # Drop dup entry
        shift(@entries);

        if($dumpall || $dup_diff) {
            print "\n$size_entry\n";
            if($dup_diff != 0) {
                printf "+dup %8d\n", $dup_diff;
            } else {
                printf " dup %8d\n", $dup;
            }

            chomp $ref;
            if($usegdb) {
                dump_call_stack_gdb($ref, \@entries);
            } else {
                dump_call_stack_addr2line($ref, \@entries);
            }
        }
    }
}
