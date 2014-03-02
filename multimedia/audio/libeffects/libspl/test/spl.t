#!/bin/env perl
# vim:syntax=perl
use mmaudio;
use strict;
use warnings;

my $tag  = shift;

my $PROCESSOR =  MMVariables::getMMPROCESSOR($ENV{PLATFORM});
my $useMmdspsim   = mmaudio::useSimulator($PROCESSOR);

my $SPL_STREAMS = "../../../streams/spl/streams";


my @input = (
    "$SPL_STREAMS/SimulatedProgramNoise48kHz.pcm",
    "$SPL_STREAMS/simprog_noise_2cr2_48k.pcm",
    "$SPL_STREAMS/carre_10s_48khz.pcm",
    "$SPL_STREAMS/SimulatedProgramNoise48kHz.pcm",
    "$SPL_STREAMS/b48_32khz_stereo_15s.pcm",
    "$SPL_STREAMS/sandstorm48000mono.pcm",
    "$SPL_STREAMS/sandstorm48000stereo.pcm",
    "$SPL_STREAMS/white_noise.pcm",
    "$SPL_STREAMS/white_noise.pcm"
);

my @static_param = (
	     "spl_static_param_1.txt",
	     "spl_static_param_1.txt",
	     "spl_static_param_2.txt",
	     "spl_static_param_2.txt",
	     "spl_static_param_3.txt",
	     "spl_static_param_pl.txt",
	     "spl_static_param_pl_tuning2.txt",
	     "spl_static_param_pl.txt",
	     "spl_static_param_pl_tuning3.txt"
);

my @dynamic_param = (
	     "spl_dynamic_param_1.txt",
	     "spl_dynamic_param_1.txt",
	     "spl_dynamic_param_1.txt",
	     "spl_dynamic_param_2.txt",
	     "spl_dynamic_param_3.txt",
	     "spl_dynamic_param_tc1.txt",
	     "spl_dynamic_param_tc1.txt",
	     "spl_dynamic_param_tc2.txt",
	     "spl_dynamic_param_tc3.txt"
);

my @stream_param = (
	     "48000 1",
	     "48000 2",
	     "48000 1",
	     "48000 1",
	     "32000 2",
	     "48000 1",
	     "48000 2",
	     "48000 1",
	     "48000 1"
);

my @output = (
    "./out/SimulatedProgramNoise48kHz_output.pcm",
    "./out/simprog_noise_2cr2_48k_output.pcm",
    "./out/carre_10s_48khz_output.pcm",
    "./out/SimulatedProgramNoise48kHz_2_output.pcm",
    "./out/b48_32khz_stereo_15s_output.pcm",
    "./out/sandstorm48000mono_output.pcm",
    "./out/sandstorm48000stereo_output.pcm",
    "./out/white_noise_tc2_output.pcm",
    "./out/white_noise_tc3_output.pcm"
);

my @ref = (
    "$SPL_STREAMS/SimulatedProgramNoise48kHz_ref.pcm",
    "$SPL_STREAMS/simprog_noise_2cr2_48k_ref.pcm",
    "$SPL_STREAMS/carre_10s_48khz_ref.pcm",
    "$SPL_STREAMS/SimulatedProgramNoise48kHz_2_ref.pcm",
    "$SPL_STREAMS/b48_32khz_stereo_15s_ref.pcm",
    "$SPL_STREAMS/sandstorm48000mono_ref.pcm",
    "$SPL_STREAMS/sandstorm48000stereo_ref.pcm",
    "$SPL_STREAMS/white_noise_tc2_ref.pcm",
    "$SPL_STREAMS/white_noise_tc3_ref.pcm"
);

my $simcmd   = "simcmd_$PROCESSOR";
my $exe="$ENV{MMBUILDOUT}/bin/mmdsp-x/SPL";

system ("mkdir -p ./out/$PROCESSOR");

mmtest::OSplan( scalar(@output) * 2 ); # 1 test for execution another for comparison
mmtest::init($0,$tag);


my $i;
for ($i =0; $i <= $#output; $i++) {
    if($useMmdspsim){
        mmaudio::genSimpleSimCmdfile($simcmd , $exe, "$static_param[$i] $dynamic_param[$i] $input[$i] $output[$i] $stream_param[$i]");
        mmtest::system("mmdspsim -c $simcmd",0, "execution: $static_param[$i] $dynamic_param[$i] $input[$i] $output[$i] $stream_param[$i]");
    }
    else{
        mmtest::system("$exe.exe $static_param[$i] $dynamic_param[$i] $input[$i] $output[$i] $stream_param[$i]",0, "execution:  $static_param[$i] $dynamic_param[$i] $input[$i] $output[$i] $stream_param[$i]");
    }

    mmtest::system("$ENV{MMROOT}/audio/afm/tools/bin/Linux/comp $output[$i] $ref[$i]", 0, "Comparing $output[$i] with $ref[$i]");
}

mmtest::concl();

