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

my @NbChannel = (1);
my $InputName = "danse_of_furie.pcm";
my $NbInputToMix = (1);
my $NbSampleToMix = (58000);
my $cmp       = $ENV{MMROOT} . "/audio/afm/tools/bin/Linux/comp";

my $nb_assertion = 0;
#compilation
$nb_assertion++;
#simulation
$nb_assertion++;

$nb_assertion = $nb_assertion + (scalar(@NbChannel))*3;
mmtest::OSplan($nb_assertion);

mmtest::init($0,$tag);

my $clean = "clean ";
$clean = "" if $noclean ;
my $compilation = mmtest::system("make $clean all",0, "compilation");


if ($compilation == 0){

     foreach my $NBCHAN (@NbChannel) {

         my $extension;
         my $InputLocalName = $InputName;
         my $OutputLocalName = $InputName."outnmf";
         my $OutputRefLocalName = $InputName;
        
         run_test(128, $NBCHAN, $NbInputToMix, $NbSampleToMix, $InputLocalName, $OutputLocalName, $OutputRefLocalName);
     }
}

mmtest::concl();


##################################################

sub run_test
{
    my ($blockSize, $nbChan, $nbInputToMix, $nbSampleToMix, $inputFileName, $outputFileName, $referenceName) = @_;

    open(CMDFILE, ">cmd") 
        or die "Unable to open cmd: $!\n";

    print CMDFILE "file build/test.elf\n";
    print CMDFILE "set args $blockSize $nbChan $nbInputToMix $nbSampleToMix ../../../../../../streams/mixer/streams/$inputFileName $outputFileName\n";
    print CMDFILE "run\n";

    close (CMDFILE);
    
    mmtest::system("mmdspsim -c cmd > mixer_1port_port8_test.log",0, "simulation :configuration $blockSize $nbChan");
    mmtest::system("$cmp $outputFileName ../../../../../../streams/mixer/streams/$referenceName", 0, "Comparing outputfile :$outputFileName $referenceName");
    my $output_name = $outputFileName."2";
    mmtest::system("$cmp $output_name ../../../../../../streams/mixer/streams/$referenceName", 0, "Comparing outputfile :$output_name $referenceName");
    $output_name = $outputFileName."22";
    mmtest::system("$cmp $output_name ../../../../../../streams/mixer/streams/$referenceName", 0, "Comparing outputfile :$output_name $referenceName");
}
