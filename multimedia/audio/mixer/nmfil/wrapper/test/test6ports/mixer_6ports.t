#!/bin/env perl
# vim:syntax=perl
use File::Basename;
use Getopt::Long;
use mmaudio;
use Cwd;
use strict;
use warnings;

my $noclean = 0;
my $nocompil = 0;

GetOptions (
    'noclean'      => \$noclean,
    'nocompil'      => \$nocompil
);
my $tag  = shift;

my ($script,$dir,$ext) = fileparse($0);
chdir $dir;

my @NbChannel = (1);
my @InputName = ("casta_8m.pcm", "BassISO_8m.pcm", "frer07_1_8m.pcm", "gspi35_1_8m.pcm", "harp40_1_8m.pcm", "spff51_1_8m.pcm");
my $NbInputToMix = (6);
my $NbSampleToMix = (58000);
my $cmp       = $ENV{MMROOT} . "/audio/afm/tools/bin/Linux/comp";

my $nb_assertion = 0;
#compilation
$nb_assertion++;
#simulation
$nb_assertion++;

my $clean = "clean ";
$clean = "" if $noclean ;
my $compilation = 0;
if($nocompil)
{
    $nb_assertion--;
    $nb_assertion = $nb_assertion + (scalar(@NbChannel))*3;
    mmtest::OSplan($nb_assertion);
    
    mmtest::init($0,$tag);
}
else
{
    $nb_assertion = $nb_assertion + (scalar(@NbChannel))*3;
    mmtest::OSplan($nb_assertion);
    mmtest::init($0,$tag);
    $compilation = mmtest::system("make $clean all",0, "compilation");
}

if ($compilation == 0){

     foreach my $NBCHAN (@NbChannel) {

         my $extension;
         my @InputLocalName = @InputName;
         my $OutputLocalName = "mix6ports.outnmf";
         my $OutputRefLocalName = "mix6ports_mono_ref";
        
         run_test(128, $NBCHAN, $NbInputToMix, $NbSampleToMix, $OutputLocalName, $OutputRefLocalName, @InputLocalName);
     }
}

mmtest::concl();


#################################################

sub run_test
{
    my ($blockSize, $nbChan, $nbInputToMix, $nbSampleToMix, $outputFileName, $referenceName, @inputFileName) = @_;

    open(CMDFILE, ">cmd") 
        or die "Unable to open cmd: $!\n";

    my $inputfilename1 = $inputFileName[0];
    my $inputfilename2 = $inputFileName[1];
    my $inputfilename3 = $inputFileName[2];
    my $inputfilename4 = $inputFileName[3];    
    my $inputfilename5 = $inputFileName[4];        
    my $inputfilename6 = $inputFileName[5];            

    print CMDFILE "file build/test.elf\n";
    print CMDFILE "set args $blockSize $nbChan $nbInputToMix $nbSampleToMix ../../../../../streams/mixer/streams/$inputfilename1 ../../../../../streams/mixer/streams/$inputfilename2 ../../../../../streams/mixer/streams/$inputfilename3 ../../../../../streams/mixer/streams/$inputfilename4 ../../../../../streams/mixer/streams/$inputfilename5 ../../../../../streams/mixer/streams/$inputfilename6 $outputFileName\n";
    print CMDFILE "run\n";

    close (CMDFILE);
    my $suffix = MMVariables::getMMDSPPROCESSOR($ENV{"PLATFORM"});
    
    mmtest::system("mmdspsim -c cmd > mixer_6ports_test.log",0, "simulation :configuration $blockSize $nbChan");
    my $ref_name = $referenceName.".outnmf";
    mmtest::system("$cmp $outputFileName $ref_name", 0, "Comparing outputfile :$outputFileName $ref_name");
    my $output_name = $outputFileName."2";
    $ref_name = $referenceName.".outnmf2";
    mmtest::system("$cmp $output_name $ref_name", 0, "Comparing outputfile :$output_name $ref_name");
    $output_name = $outputFileName."22";
    $ref_name = $referenceName.".outnmf22";
    #EOS test -> check with "cmp" instead of "$cmp"
    mmtest::system("cmp $output_name $ref_name", 0, "Comparing outputfile :$output_name $ref_name");
}
