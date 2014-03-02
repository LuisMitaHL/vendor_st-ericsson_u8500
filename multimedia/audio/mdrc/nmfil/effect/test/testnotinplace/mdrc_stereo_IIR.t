#!/bin/env perl
# vim:syntax=perl
use File::Basename;
use Getopt::Long;
use MMTEST::mmtest;
use MMVariables;
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

my $fileSize         = 1584396;
my $blockSizeIn      = 480;
my $blockSizeOut     = 480;
my $inputFileName    = "../../../../../streams/mdrc/streams/stereo_48k.pcm";
my $outputFileName   = "output_IIR_stereo_48k.pcm.out";
my $referenceName   = "../../../../../streams/mdrc/streams/ref_op_IIR_stereo_48k_nmf.pcm";
my $config           = 2;
my $freq_in          = 48000;
my $nChannels        = 2;
my $paramFile        = "../../../../../streams/mdrc/streams/params_IIR_stereo_48k";




my $cmp  = $ENV{MMROOT} . "/audio/afm/tools/bin/Linux/comp";
my $rmsdiff  = $ENV{MMROOT} . "/audio/afm/tools/bin/Linux/rmsdiff";
my $mcproject = MMVariables::getMCPROJECT( $ENV{PLATFORM} );


my $nb_assertion = 0;
#compilation
$nb_assertion++;
#comparisons
$nb_assertion += 5;

mmtest::OSplan($nb_assertion);

mmtest::init($0,$tag);

my $clean = "clean ";
$clean = "" if $noclean ;
my $compilation = mmtest::system("make $clean all", 0, "compilation");
#my $compilation = mmtest::system("make CFLAGS=-g", 0, "compilation");

if ($compilation == 0) {
    run_test($fileSize, $blockSizeIn, $blockSizeOut, $inputFileName, $outputFileName, $referenceName, $config, $freq_in, $nChannels, $paramFile);
}

mmtest::concl();

##################################################

sub run_test
{
    my ($fileSize, $blockSizeIn, $blockSizeOut, $inputFileName, $outputFileName, $referenceName, $config, $freq_in, $nChannels, $paramFile) = @_;

    open(CMDFILE, ">cmd")
        or die "Unable to open cmd: $!\n";

    print CMDFILE "file build/test.elf\n";
    print CMDFILE "set args $fileSize $blockSizeIn $blockSizeOut $inputFileName $outputFileName $config $freq_in $nChannels $paramFile\n";
    print CMDFILE "break exit\n";
    print CMDFILE "run\n";
    print CMDFILE "info stacklimit\n";
    print CMDFILE "quit\n";

    close (CMDFILE);

    mmtest::system("mmdspsim --project $mcproject -c cmd",0, "simulation :configuration $fileSize");

    mmtest::system("$cmp $outputFileName $referenceName", 0, "Comparing outputfile: $outputFileName to $referenceName");
    mmtest::system("$cmp $outputFileName"."2 $referenceName", 0, "Comparing outputfile :Idle -> Pause -> Executing -> Pause -> Idle");
    mmtest::system("$cmp $outputFileName"."22 $referenceName", 0, "Comparing file1 :Idle -> Executing -> EOS (file1) -> newStream -> Idle (file2)");
    mmtest::system("$cmp $outputFileName"."222 $referenceName", 0, "Comparing file2 :Idle -> Executing -> EOS (file1) -> newStream -> Idle (file2)");
}
