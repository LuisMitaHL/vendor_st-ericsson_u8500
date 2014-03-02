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

my @NbChannel = (1, 2);
my @InputMonoName = ("casta_8m.pcm", "BassISO_8m.pcm");
my @InputStereoName = ("techno.pcm", "spmg54_1_44stereo.pcm");
my $NbInputToMix = (2);
my $NbSampleToMix = (58000);
my $cmp       = $ENV{MMROOT} . "/audio/afm/tools/bin/Linux/comp";

my $nb_assertion = 0;
#compilation
$nb_assertion++;



my $clean = "clean ";
$clean = "" if $noclean ;
my $compilation = 0;
if($nocompil)
{
    $nb_assertion--;
    $nb_assertion = $nb_assertion + (scalar(@NbChannel))*4;
    mmtest::OSplan($nb_assertion);
    
    mmtest::init($0,$tag);
}
else
{
    $nb_assertion = $nb_assertion + (scalar(@NbChannel))*4;
    mmtest::OSplan($nb_assertion);
    mmtest::init($0,$tag);
    $compilation = mmtest::system("make $clean all",0, "compilation");
}

if ($compilation == 0){

    foreach my $NBCHAN (@NbChannel) {

        my $extension;
        my @InputLocalName;
        my $OutputRefLocalName;
        if($NBCHAN == 1){
            @InputLocalName = @InputMonoName;
            $OutputRefLocalName = "mix2ports_mono_ref";
        }else{
            @InputLocalName = @InputStereoName;
            $OutputRefLocalName = "mix2ports_stereo_ref";
        }
        my $OutputLocalName = "mix2ports.outnmf";
        run_test(128, $NBCHAN, $NbInputToMix, $NbSampleToMix, $OutputLocalName, $OutputRefLocalName, @InputLocalName);
    }
}

mmtest::concl();


#################################################

sub run_test
{
    my ($blockSize, $nbChan, $nbInputToMix, $nbSampleToMix, $outputFileName, $referenceName, @inputFileName) = @_;

    open(CMDFILE, ">cmd$nbChan") 
        or die "Unable to open cmd: $!\n";

    my $inputfilename1 = $inputFileName[0];
    my $inputfilename2 = $inputFileName[1];

    print CMDFILE "file build/test.elf\n";
    print CMDFILE "set args $blockSize $nbChan $nbInputToMix $nbSampleToMix ../../../../../streams/mixer/streams/$inputfilename1 ../../../../../streams/mixer/streams/$inputfilename2 $outputFileName\n";
    print CMDFILE "run\n";

    close (CMDFILE);

    my $suffix = MMVariables::getMMDSPPROCESSOR($ENV{"PLATFORM"});

    mmtest::system("mmdspsim -c cmd$nbChan > mixer_2ports_test$nbChan.log",0, "simulation :configuration $blockSize $nbChan");
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
