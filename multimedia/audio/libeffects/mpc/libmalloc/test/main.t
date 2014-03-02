#!/bin/env perl
# vim:syntax=perl
use File::Basename;
use Getopt::Long;
use MMTEST::mmtest;
use Cwd;
use strict;
use warnings;

my $noclean = 0;
GetOptions (
    'noclean'      => \$noclean
);
my $tag  = shift;

my ($script,$dir,$ext) = fileparse($0);
chdir $dir;

my $nb_assertion = 0;
#compilation
$nb_assertion++;
#simulation
$nb_assertion++;

$nb_assertion = $nb_assertion;
mmtest::OSplan($nb_assertion);

mmtest::init($0,$tag);

my $clean = "clean";
$clean = "" if $noclean ;
my $compilation = mmtest::system("make $clean all",0, "compilation");

if ($compilation == 0){

    run_test();
}

mmtest::concl();


##################################################

sub run_test
{
    open(CMDFILE, ">cmd") 
        or die "Unable to open cmd: $!\n";

    print CMDFILE "file build/test.elf\n";
    print CMDFILE "b exit\n";
    print CMDFILE "enable profile\n";
    print CMDFILE "profile -r\n";
    print CMDFILE "run \n";
    print CMDFILE "info stacklimit \n";
    print CMDFILE "profile -o testtx_prof\n";
    print CMDFILE "quit \n";

    close (CMDFILE);

    mmtest::system("mmdspsim -c cmd > malloc_test.log", 0, "simulation :configuration");
}
