#!/bin/env perl

&main();

sub main {
    my @frequencies = (8, 11, 12, 16, 22, 24, 32, 44, 48);
    my $base_inputfile = "Portal";
    my $nb_channels = 1;

    # generate input scripts (one per frequency)
    foreach my $freq_kHz (@frequencies) {
        my $script_name = "pcm_input_".$freq_kHz."k_";
        if ($nb_channels == 1) {
            $script_name .= "mono";
        } else {
            $script_name .= "stereo";
        }
        $script_name .= ".ate";

        my $inputfile = $base_inputfile."_".$freq_kHz;
        $inputfile .= "m" if $nb_channels == 1;
        $inputfile .= ".pcm";

        open SCRIPT, ">$script_name" or die "Cannot create output file '$script_name': $!\n";

        my $freq_hertz = $freq_kHz * 1000;
        if ($freq_hertz == 11000) { $freq_hertz = 11025; }
        if ($freq_hertz == 22000) { $freq_hertz = 22050; }
        if ($freq_hertz == 44000) { $freq_hertz = 44100; }

        print SCRIPT "OMX_SetParameter    \$1 OMX_IndexParamAudioPcm nPortIndex=\$2 nChannels=$nb_channels nSamplingRate=$freq_hertz eEndian=OMX_EndianBig\n\n";

        print SCRIPT "if \$3 == input_reference\n";
        print SCRIPT "ate_attach_io       \$1 \$2 MEM \$(NOMADIK)/samplerateconv/streams/$inputfile 0 0 0\n";
        print SCRIPT "endif\n";

        close SCRIPT;
    }

    # generate output scripts (one for each combination of frequencies)
    foreach my $in_freq_kHz (@frequencies) {
        foreach my $out_freq_kHz (@frequencies) {
            my $suffix = $in_freq_kHz."k_";
            if ($nb_channels == 1) {
                $suffix .= "mono";
            } else {
                $suffix .= "stereo";
            }
            $suffix .= "_".$out_freq_kHz."k_";
            if ($nb_channels == 1) {
                $suffix .= "mono";
            } else {
                $suffix .= "stereo";
            }

            my $script_name = "pcm_output_".$suffix.".ate";

            my $outputfile = "src_pcm_".$suffix.".out";

            my $reffile = $base_inputfile."_".$suffix.".ref.out";

            my $refdigest = $base_inputfile."_".$suffix."_digest.hash";

            open SCRIPT, ">$script_name" or die "Cannot create output file '$script_name': $!\n";

            my $freq_hertz = $out_freq_kHz * 1000;
            if ($freq_hertz == 11000) { $freq_hertz = 11025; }
            if ($freq_hertz == 22000) { $freq_hertz = 22050; }
            if ($freq_hertz == 44000) { $freq_hertz = 44100; }

            print SCRIPT "OMX_SetParameter    \$1 OMX_IndexParamAudioPcm nPortIndex=0 nChannels=$nb_channels nSamplingRate=$freq_hertz eEndian=OMX_EndianBig\n\n";

            print SCRIPT "if \$2 == output_reference\n";
            print SCRIPT "alias OUTPUT_FILE   \$(OUTPUT_DIR)/$outputfile\n";
            print SCRIPT "ate_attach_io       \$1 0 MEM \$(OUTPUT_FILE) 2000000 0 0\n";
            print SCRIPT "alias REF_FILE      \$(NOMADIK)/samplerateconv/streams/$reffile\n";
            print SCRIPT "alias REF_DIGEST    \$(NOMADIK)/samplerateconv/streams/$refdigest\n";
            print SCRIPT "endif\n";

            close SCRIPT;
        }
    }
}
